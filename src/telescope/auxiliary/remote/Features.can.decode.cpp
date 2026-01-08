//--------------------------------------------------------------------------------------------------
// remote telescope features control, using the CANbus interface (CAN client decode)

#include "Features.h"

#if defined(FEATURES_CAN_CLIENT_PRESENT)

#include "../FeaturesBase.h"
#include "../../../lib/convert/Convert.h"
#include "../../../lib/canTransport/CanPayload.h"

// ---------- helpers ----------

static inline uint16_t readU16LE(const uint8_t *p) {
  return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

// Dew heater: -5.0..+20.0C encoded as 0..250 in 0.1C steps (one byte).
static inline float unpackDew01C(uint8_t enc) {
  return ((float)enc / 10.0f) - 5.0f;
}

// Intervalometer: seconds encoded as 0.1s units in u16.
static inline float unpack01s(uint16_t enc) {
  return (float)enc / 10.0f;
}

static inline void fmtSeconds(char *out, float v, bool isExposure) {
  // Match the formatting intent from Features.command.cpp:
  // exposure: <1 => 3dp, <10 => 2dp, <30 => 1dp, else 0dp
  // delay:    <10 => 2dp, <30 => 1dp, else 0dp
  int d = 0;
  if (isExposure) {
    if (v < 1.0f) d = 3;
    else if (v < 10.0f) d = 2;
    else if (v < 30.0f) d = 1;
    else d = 0;
  } else {
    if (v < 10.0f) d = 2;
    else if (v < 30.0f) d = 1;
    else d = 0;
  }
  dtostrf(v, 0, d, out);
}

bool Features::decodeResponse(char *reply,
                             uint8_t opcode,
                             const uint8_t responsePayload[8], uint8_t responseLen,
                             bool &supressFrame, bool &numericReply) {
  (void)supressFrame;
  (void)numericReply;

  if (!reply) return false;
  reply[0] = 0;

  // Must have at least [tidop,status]
  if (responseLen < 2) return false;

  // In transact2(), the first 8 bytes are the first CAN frame,
  // and any bytes beyond 8 are the *data bytes only* from frame 2 (status/tidop stripped).
  const bool hasSecond = (responseLen > 8);
  const uint8_t firstFrameLen = (responseLen >= 8) ? 8 : responseLen;

  const uint8_t dataLen0 = (firstFrameLen >= 2) ? (uint8_t)(firstFrameLen - 2) : 0;
  const uint8_t *data0   = (dataLen0 ? &responsePayload[2] : nullptr);

  const uint8_t dataLen1 = hasSecond ? (uint8_t)(responseLen - 8) : 0;
  const uint8_t *data1   = hasSecond ? (const uint8_t*)(&responsePayload[8]) : nullptr;

  switch (opcode) {

    // :GXY0# -> "01010101" (8 chars, no terminator transmitted on CAN; we add it locally)
    case FEAT_OP_GET_ACTIVE_Y0: {
      if (dataLen0 < 1) return false;
      const uint8_t bitmap = data0[0];

      for (uint8_t i = 0; i < 8; i++) {
        reply[i] = (bitmap & (1U << i)) ? '1' : '0';
      }
      reply[8] = 0;
      return true;
    }

    // :GXYn# -> "NAME,PURPOSE" (NAME up to 10 chars; PURPOSE numeric)
    // Wire:
    //   frame0 data: [purpose][name0..name4]
    //   frame1 data: [name5..name10] (up to 6 bytes)
    case FEAT_OP_GET_INFO_Yn: {
      if (dataLen0 < 1) return false;

      const uint8_t purpose = data0[0];

      char name[11];
      uint8_t n = 0;

      // Take up to 5 bytes from frame0 after purpose
      for (uint8_t i = 1; i < dataLen0 && n < 10; i++) {
        name[n++] = (char)data0[i];
      }

      // Then up to 6 bytes from frame1 (already data-only)
      for (uint8_t i = 0; i < dataLen1 && n < 10; i++) {
        name[n++] = (char)data1[i];
      }

      name[n] = 0;

      // Build "name,purpose"
      char pbuf[8];
      snprintf(pbuf, sizeof(pbuf), "%u", (unsigned)purpose);

      strcpy(reply, name);
      strcat(reply, ",");
      strcat(reply, pbuf);
      return true;
    }

    // :GXXn# -> per-purpose value reply, rendered as original comma-separated ASCII where applicable.
    //
    // We decode by payload size (since purpose is not carried on-wire here):
    //   1 byte  => switch or analog output: "v"
    //   4 bytes => dew heater: "en,zero,span,delta"
    //   6 bytes => intervalometer: "cur,exp,delay,count"
    case FEAT_OP_GET_VALUE_Xn: {
      if (dataLen0 == 0) return false;

      if (dataLen0 == 1) {
        // SWITCH / ANALOG_OUTPUT
        char s[8];
        snprintf(s, sizeof(s), "%u", (unsigned)data0[0]);
        strcpy(reply, s);
        return true;
      }

      if (dataLen0 == 4) {
        // DEW_HEATER
        const uint8_t en   = data0[0];
        const float zero   = unpackDew01C(data0[1]);
        const float span   = unpackDew01C(data0[2]);
        const int8_t denc  = (int8_t)data0[3];
        const float delta  = ((float)denc) / 2.0f; // 0.5C steps

        char s0[8], s1[16], s2[16], s3[16];
        snprintf(s0, sizeof(s0), "%u", (unsigned)en);
        dtostrf(zero, 0, 1, s1);
        dtostrf(span, 0, 1, s2);
        dtostrf(delta, 0, 1, s3);

        strcpy(reply, s0);
        strcat(reply, ",");
        strcat(reply, s1);
        strcat(reply, ",");
        strcat(reply, s2);
        strcat(reply, ",");
        strcat(reply, s3);
        return true;
      }

      if (dataLen0 == 6) {
        // INTERVALOMETER (fixed 6 bytes)
        const uint8_t cur = data0[0];
        const float exposure = unpack01s(readU16LE(&data0[1]));
        const float delay    = unpack01s(readU16LE(&data0[3]));
        const uint8_t count  = data0[5];

        char sCur[8], sExp[20], sDel[20], sCnt[8];
        snprintf(sCur, sizeof(sCur), "%u", (unsigned)cur);
        fmtSeconds(sExp, exposure, true);
        fmtSeconds(sDel, delay, false);
        snprintf(sCnt, sizeof(sCnt), "%u", (unsigned)count);

        strcpy(reply, sCur);
        strcat(reply, ",");
        strcat(reply, sExp);
        strcat(reply, ",");
        strcat(reply, sDel);
        strcat(reply, ",");
        strcat(reply, sCnt);
        return true;
      }

      // Fallback: return first byte as integer
      char s[8];
      snprintf(s, sizeof(s), "%u", (unsigned)data0[0]);
      strcpy(reply, s);
      return true;
    }

    // SET op is normally numericReply=true (so decodeResponse won't be called),
    // but keep it harmless if it is.
    case FEAT_OP_SET_VALUE_Xn:
    default:
      reply[0] = 0;
      return true;
  }
}

#endif
