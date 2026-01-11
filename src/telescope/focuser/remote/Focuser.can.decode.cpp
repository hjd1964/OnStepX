//--------------------------------------------------------------------------------------------------
// remote telescope focuser control, using the CANbus interface (CAN client decode)

#include "Focuser.h"

#if defined(FOCUSER_CAN_CLIENT_PRESENT)

#include "../../../lib/convert/Convert.h"

bool Focuser::decodeResponse(char *reply) {
  if (!reply) return false;
  reply[0] = 0;

  // scratchpad variables
  float f;
  int32_t i;
  uint8_t u;

  switch (opCode()) {

    // :FT# -> RX: ASCII bytes "M"/"S" + rateDigit
    case FOC_OP_GET_STATUS_T:
      u = readRemainingBytes((uint8_t*)reply);
      reply[u] = 0;
    break;

    // :FI# / :FM# -> RX: i32LE (units already selected by ctrl on request)
    case FOC_OP_GET_MIN_I:
    case FOC_OP_GET_MAX_M:
      if (!readI32(i)) return false;
      sprintf(reply, "%ld", (long)i);
    break;

    // :Fe# / :Ft# -> RX: f32LE temperature values
    case FOC_OP_GET_TEMPDIFF_e:
    case FOC_OP_GET_TEMP_t:
      if (!readF32(f)) return false;
      // local code uses "%3.1f"
      sprintF(reply, "%3.1f", f);
    break;

    // :Fu# -> RX: f32LE microns per step
    case FOC_OP_GET_UM_PER_STEP_u:
      if (!readF32(f)) return false;
      // local code uses "%7.5f"
      sprintF(reply, "%7.5f", f);
    break;

    // :FB# (get) -> RX: i32LE backlash (units)
    // :FB[n]# (set) -> typically no payload (status-only)
    case FOC_OP_BACKLASH_B:
      if (remaining() == 0) return true;
      if (!readI32(i)) return false;
      sprintf(reply, "%ld", (long)i);
    break;

    // :FC# (get) -> RX: f32LE
    // :FC[sn.n]# (set) -> usually no payload
    case FOC_OP_TCF_COEF_C:
      if (remaining() == 0) return true;
      if (!readF32(f)) return false;
      // local code uses "%7.5f"
      sprintF(reply, "%7.5f", f);
    break;

    // :Fc# (get) -> RX: u8 0/1 (if server returns payload) OR status-only
    // :Fc[n]# (set) -> no payload
    case FOC_OP_TCF_ENABLE_c:
      if (remaining() == 0) return true;
      if (!readU8(u)) return false;
      sprintf(reply, "%u", (unsigned)(u ? 1 : 0));
    break;

    // :FD# (get) -> RX: i32LE (units)
    // :FD[n]# (set) -> no payload
    case FOC_OP_TCF_DEADBAND_D:
      if (remaining() == 0) return true;
      if (!readI32(i)) return false;
      sprintf(reply, "%ld", (long)i);
    break;

    // :FP# (get) -> RX: u8 percent (or i32 if you choose; server I gave uses u8)
    // :FP[n]# (set) -> no payload
    case FOC_OP_DC_POWER_P:
      if (remaining() == 0) return true;
      if (!readU8(u)) return false;
      sprintf(reply, "%u", (unsigned)u);
    break;

    // :FW# -> RX: i32LE work rate (um/s or whatever your server returns)
    case FOC_OP_GET_WORKRATE_W:
      if (remaining() == 0) return true;
      if (!readI32(i)) return false;
      sprintf(reply, "%ld", (long)i);
    break;

    // :FG# -> RX: i32LE position (units)
    case FOC_OP_GET_POS_G:
      if (!readI32(i)) return false;
      sprintf(reply, "%ld", (long)i);
    break;

    default:
      // Most ops are "no reply payload"; empty reply is fine.
      reply[0] = 0;
    break;
  }

  return true;
}

#endif
