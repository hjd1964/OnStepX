//--------------------------------------------------------------------------------------------------
// remote telescope rotator control, using the CANbus interface (CAN client decode)

#include "Rotator.h"

#if defined(ROTATOR_CAN_CLIENT_PRESENT)

#include "../../../lib/convert/Convert.h"
#include "../../../lib/canTransport/CanPayload.h"

bool Rotator::decodeResponse(char *reply,
                             uint8_t opcode,
                             const uint8_t responsePayload[8], uint8_t responseLen,
                             bool &supressFrame, bool &numericReply) {
  if (!reply) return false;
  reply[0] = 0;

  // Must have the 2-byte header (payload[0]=tidop, payload[1]=status)
  if (responseLen < 2) return false;

  uint8_t dataLen = (uint8_t)(responseLen - 2);
  const uint8_t *data   = &responsePayload[2];

  // Cursor-based reader over the data bytes only (post-header)
  CanPayload p(data, dataLen);

  switch (opcode) {

    // :rT# -> RX: ASCII bytes "M"/"S"["D"]["R"] + rateDigit
    // Accept variable length and copy as-is.
    case ROT_OP_GET_STATUS_T: {
      if (dataLen > 4) dataLen = 4;
      for (uint8_t i = 0; i < dataLen; i++) reply[i] = (char)data[i];
      reply[dataLen] = 0;
      return true;
    }

    // :rG# -> RX: f32LE degrees; convert to DMS string for LX200 output
    case ROT_OP_GET_ANGLE_G: {
      float deg = 0.0f;
      if (!p.readF32LE(deg)) return false;
      convert.doubleToDms(reply, (double)deg, true, true, PM_LOW);
      return true;
    }

    // :rI# -> RX: i32LE degrees (rounded)
    case ROT_OP_GET_MIN_I: {
      int32_t deg = 0;
      if (!p.readI32LE(deg)) return false;
      sprintf(reply, "%ld", (long)deg);
      return true;
    }

    // :rM# -> RX: i32LE degrees (rounded)
    case ROT_OP_GET_MAX_M: {
      int32_t deg = 0;
      if (!p.readI32LE(deg)) return false;
      sprintf(reply, "%ld", (long)deg);
      return true;
    }

    // :rD# -> RX: f32LE deg_per_step
    case ROT_OP_GET_DEG_PERSTEP_D: {
      float dps = 0.0f;
      if (!p.readF32LE(dps)) return false;
      sprintF(reply, "%7.5f", dps);
      return true;
    }

    // :rb# -> RX: i16LE steps (read mode) OR no payload (write mode)
    case ROT_OP_BACKLASH_b: {
      if (dataLen == 0) { reply[0] = 0; return true; }
      int16_t steps = 0;
      if (!p.readI16LE(steps)) return false;
      sprintf(reply, "%ld", (long)steps);
      return true;
    }

    // :rW# -> RX: f32LE deg_per_sec (settings.gotoRate)
    case ROT_OP_GET_WORKRATE_W: {
      float dps = 0.0f;
      if (!p.readF32LE(dps)) return false;
      sprintF(reply, "%0.1f", dps);
      return true;
    }

    // :GX98# -> RX: u8 ASCII 'D'/'R'/'N'
    case ROT_OP_AVAIL_GX98: {
      uint8_t c = 0;
      if (!p.readU8(c)) return false;
      reply[0] = (char)c;
      reply[1] = 0;
      return true;
    }

    default:
      // Most ops are "no reply payload"; empty reply is fine.
      reply[0] = 0;
      return true;
  }
}

#endif
