//--------------------------------------------------------------------------------------------------
// remote telescope features control, using the CANbus interface (CAN client decode)

#include "Features.h"

#if defined(FEATURES_CAN_CLIENT_PRESENT)

#include "../FeaturesBase.h"
#include "../../../lib/convert/Convert.h"

// ---------- helpers ----------

// Dew heater: -5.0..+20.0C encoded as 0..250 in 0.1C steps (one byte).
static inline float unpackDew01C(uint8_t enc) {
  return ((float)enc / 10.0f) - 5.0f;
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

bool Features::decodeResponse(char *reply) {
  if (!reply) return false;
  reply[0] = 0;

  switch (opCode()) {

    // :GXY0# -> "01010101" (8 chars, no terminator transmitted on CAN; we add it locally)
    case FEAT_OP_GET_ACTIVE_Y0: {
      uint8_t bitmap = 0;
      if (!readU8(bitmap)) return false;

      for (uint8_t i = 0; i < 8; i++) { reply[i] = (bitmap & (1U << i)) ? '1' : '0'; }
      reply[8] = 0;
    } break;

    // :GXYn# -> "NAME,PURPOSE" (NAME up to 10 chars; PURPOSE numeric)
    // Wire:
    //   frame0 data: [purpose][name0..name4]
    //   frame1 data: [name5..name10] (up to 6 bytes)
    case FEAT_OP_GET_INFO_Yn: {
      uint8_t purpose = 0;
      if (!readU8(purpose)) return false;

      char name[14];
      int len = readRemainingBytes((uint8_t*)name);
      name[len] = 0;

      sprintf(reply, "%s,%u", name, (unsigned)purpose);
    } break;

    // :GXXn# -> per-purpose value reply, rendered as original comma-separated ASCII,
    // with optional power monitor telemetry appended (matches Features::strCatPower()).
    //
    // Wire (frame0 data is always 6 bytes; purpose is included):
    //   frame0 data: [purpose][value bytes...]
    //   frame1 data: optional [intervalometerCount] + optional [V i16 0.1][I i16 0.1][flags]
    //
    // Voltage/current fixed-point: enc = round(x * 10), int16 LE; 0x8000 => NAN.
    // Flags: FEAT_POWER_FLAGS_PRESENT + FEAT_POWER_FAULT_* (see FeaturesBase.h).
    case FEAT_OP_GET_VALUE_Xn: {
      uint8_t purpose = 0;
      if (!readU8(purpose)) return false;

      // -------- value decode --------
      if (purpose == SWITCH || purpose == MOMENTARY_SWITCH || purpose == COVER_SWITCH) {
        uint8_t v = 0;
        if (!readU8(v)) return false;
        sprintf(reply, "%u", (unsigned)v);
      } else

      if (purpose == ANALOG_OUTPUT) {
        uint8_t v = 0;
        if (!readU8(v)) return false;
        sprintf(reply, "%u", (unsigned)v);
      } else

      if (purpose == DEW_HEATER) {
        uint8_t en = 0;
        uint8_t zEnc = 0;
        uint8_t sEnc = 0;
        float delta = 0;
        if (!readU8(en) || !readU8(zEnc) || !readU8(sEnc) || !readFixedI16(delta, 10)) return false;

        const float zero = unpackDew01C(zEnc);
        const float span = unpackDew01C(sEnc);

        char s1[16], s2[16], s3[16];
        dtostrf(zero, 0, 1, s1);
        dtostrf(span, 0, 1, s2);
        if (isnan(delta)) strcpy(s3, "NAN"); else dtostrf(delta, 0, 1, s3);
        sprintf(reply, "%u,%s,%s,%s", (unsigned)en, s1, s2, s3);
      } else

      if (purpose == INTERVALOMETER) {
        uint8_t cur = 0;
        uint8_t expB = 0;
        uint8_t delB = 0;
        uint8_t count = 0;

        if (!readU8(cur) || !readU8(expB) || !readU8(delB) || !readU8(count)) return false;

        const float exposure = convert.unpackSeconds(expB);
        const float delay    = convert.unpackSeconds(delB);

        char sExp[20], sDel[20];
        fmtSeconds(sExp, exposure, true);
        fmtSeconds(sDel, delay, false);
        sprintf(reply, "%u,%s,%s,%u", (unsigned)cur, sExp, sDel, (unsigned)count);
      } else {
        // Unknown purpose; return first value byte as integer
        uint8_t v = 0;
        if (!readU8(v)) return false;
        sprintf(reply, "%u", (unsigned)v);
      }

      // optional power telemetry append
      if (remaining() == 5) {
        float volts = NAN;
        float amps  = NAN;
        uint8_t flags = 0;

        if (!readFixedI16(volts, 10) || !readFixedI16(amps, 10) || !readU8(flags)) return false;

        const bool present = ((flags & FEAT_POWER_FLAGS_PRESENT) != 0);

        if (!present) { strcat(reply, ",NAN,NAN,!!!!!"); return true; }

        if (isnan(volts)) { strcat(reply, ",NAN"); } else { char s[24]; sprintF(s, ",%1.1f", volts); strcat(reply, s); }

        if (isnan(amps)) { strcat(reply, ",NAN"); } else { char s[24]; sprintF(s, ",%1.1f", amps); strcat(reply, s); }

        // Flags string: ",P" + (C/v/V/T or !)
        char fs[8];
        fs[0] = ',';
        fs[1] = 'P';
        fs[2] = (flags & FEAT_POWER_FAULT_OC) ? '!' : 'C';
        fs[3] = (flags & FEAT_POWER_FAULT_UV) ? '!' : 'v';
        fs[4] = (flags & FEAT_POWER_FAULT_OV) ? '!' : 'V';
        fs[5] = (flags & FEAT_POWER_FAULT_OT) ? '!' : 'T';
        fs[6] = 0;
        strcat(reply, fs);
      }

    } break;

    // SET op is normally numericReply=true (so decodeResponse won't be called),
    // but keep it harmless if it is.
    case FEAT_OP_SET_VALUE_Xn:
    default:
      reply[0] = 0;
      return true;
    break;
  }

  return true;
}

#endif
