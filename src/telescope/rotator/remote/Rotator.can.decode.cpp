//--------------------------------------------------------------------------------------------------
// remote telescope rotator control, using the CANbus interface (CAN client decode)

#include "Rotator.h"

#if defined(ROTATOR_CAN_CLIENT_PRESENT)

#include "../../../lib/convert/Convert.h"

bool Rotator::decodeResponse(char *reply) {
  if (!reply) return false;
  reply[0] = 0;

  switch (opCode()) {

    // :rT# -> RX: ASCII bytes "M"/"S"["D"]["R"] + rateDigit
    // Accept variable length and copy as-is.
    case ROT_OP_GET_STATUS_T: {
      int16_t dataLen = readRemainingBytes((uint8_t*)reply);
      reply[dataLen] = 0;
    } break;

    // :rG# -> RX: f32 degrees; convert to DMS string for LX200 output
    case ROT_OP_GET_ANGLE_G: {
      float deg = 0.0f;
      if (!readF32(deg)) return false;
      convert.doubleToDms(reply, (double)deg, true, true, PM_LOW);
    } break;

    // :rI# -> RX: i32 degrees (rounded)
    case ROT_OP_GET_MIN_I: {
      int32_t deg = 0;
      if (!readI32(deg)) return false;
      sprintf(reply, "%ld", (long)deg);
    } break;

    // :rM# -> RX: i32 degrees (rounded)
    case ROT_OP_GET_MAX_M: {
      int32_t deg = 0;
      if (!readI32(deg)) return false;
      sprintf(reply, "%ld", (long)deg);
    } break;

    // :rD# -> RX: f32 deg_per_step
    case ROT_OP_GET_DEG_PERSTEP_D: {
      float dps = 0.0f;
      if (!readF32(dps)) return false;
      sprintF(reply, "%7.5f", dps);
    } break;

    // :rb# -> RX: i16 steps (read mode) OR no payload (write mode)
    case ROT_OP_BACKLASH_b: {
      int16_t steps = 0;
      if (!readI16(steps)) return false;
      sprintf(reply, "%ld", (long)steps);
    } break;

    // :rW# -> RX: f32 deg_per_sec (settings.gotoRate)
    case ROT_OP_GET_WORKRATE_W: {
      float dps = 0.0f;
      if (!readF32(dps)) return false;
      sprintF(reply, "%0.1f", dps);
    } break;

    // :GX98# -> RX: u8 ASCII 'D'/'R'/'N'
    case ROT_OP_AVAIL_GX98: {
      uint8_t c = 0;
      if (!readU8(c)) return false;
      reply[0] = (char)c;
      reply[1] = 0;
    } break;

    // :GXU3# -> RX u8 packed status
    case ROT_OP_DRIVER_STATUS: {
      uint8_t packedStatus;
      readU8(packedStatus);
      strcat(reply, ( packedStatus       & 1) ? "ST," : ",");
      strcat(reply, ((packedStatus << 1) & 1) ? "OA," : ",");
      strcat(reply, ((packedStatus << 2) & 1) ? "OB," : ",");
      strcat(reply, ((packedStatus << 3) & 1) ? "GA," : ",");
      strcat(reply, ((packedStatus << 4) & 1) ? "GB," : ",");
      strcat(reply, ((packedStatus << 5) & 1) ? "OT," : ","); // > 150C
      strcat(reply, ((packedStatus << 6) & 1) ? "PW," : ","); // > 120C
      strcat(reply, ((packedStatus << 7) & 1) ? "GF" : "");
    } break;

    default:
      // Most ops are "no reply payload"; empty reply is fine
      reply[0] = 0;
  }

  return true;
}

#endif
