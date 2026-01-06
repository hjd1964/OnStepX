//--------------------------------------------------------------------------------------------------
// remote telescope focuser control, using the CANbus interface (CAN client decode)

#include "Focuser.h"

#if defined(FOCUSER_CAN_CLIENT_PRESENT)

#include "../../../lib/convert/Convert.h"
#include "../../../lib/canTransport/CanPayload.h"

// Request:  [ tidop, ctrl, args... ]
// Response: [ tidop, status, payload... ]

bool Focuser::decodeResponse(char *reply,
                             uint8_t opcode,
                             const uint8_t responsePayload[8], uint8_t responseLen,
                             bool &supressFrame, bool &numericReply) {
  if (!reply) return false;
  reply[0] = 0;

  if (responseLen < 2) return false;

  uint8_t dataLen = (uint8_t)(responseLen - 2);
  const uint8_t *data   = &responsePayload[2];

  CanPayload p(data, dataLen);

  switch (opcode) {

    // :FT# -> RX: ASCII bytes "M"/"S" + rateDigit
    case FOC_OP_GET_STATUS_T: {
      if (dataLen > 2) dataLen = 2;
      for (uint8_t i = 0; i < dataLen; i++) reply[i] = (char)data[i];
      reply[dataLen] = 0;
      return true;
    }

    // :FI# / :FM# -> RX: i32LE (units already selected by ctrl on request)
    case FOC_OP_GET_MIN_I:
    case FOC_OP_GET_MAX_M: {
      int32_t v = 0;
      if (!p.readI32LE(v)) return false;
      sprintf(reply, "%ld", (long)v);
      return true;
    }

    // :Fe# / :Ft# -> RX: f32LE temperature values
    case FOC_OP_GET_TEMPDIFF_e:
    case FOC_OP_GET_TEMP_t: {
      float t = 0.0f;
      if (!p.readF32LE(t)) return false;
      // local code uses "%3.1f"
      sprintF(reply, "%3.1f", t);
      return true;
    }

    // :Fu# -> RX: f32LE microns per step
    case FOC_OP_GET_UM_PER_STEP_u: {
      float ups = 0.0f;
      if (!p.readF32LE(ups)) return false;
      // local code uses "%7.5f"
      sprintF(reply, "%7.5f", ups);
      return true;
    }

    // :FB# (get) -> RX: i32LE backlash (units)
    // :FB[n]# (set) -> typically no payload (status-only)
    case FOC_OP_BACKLASH_B: {
      if (dataLen == 0) { reply[0] = 0; return true; }
      int32_t v = 0;
      if (!p.readI32LE(v)) return false;
      sprintf(reply, "%ld", (long)v);
      return true;
    }

    // :FC# (get) -> RX: f32LE
    // :FC[sn.n]# (set) -> usually no payload
    case FOC_OP_TCF_COEF_C: {
      if (dataLen == 0) { reply[0] = 0; return true; }
      float c = 0.0f;
      if (!p.readF32LE(c)) return false;
      // local code uses "%7.5f"
      sprintF(reply, "%7.5f", c);
      return true;
    }

    // :Fc# (get) -> RX: u8 0/1 (if server returns payload) OR status-only
    // :Fc[n]# (set) -> no payload
    case FOC_OP_TCF_ENABLE_c: {
      if (dataLen == 0) { reply[0] = 0; return true; }
      uint8_t en = 0;
      if (!p.readU8(en)) return false;
      sprintf(reply, "%u", (unsigned)(en ? 1 : 0));
      return true;
    }

    // :FD# (get) -> RX: i32LE (units)
    // :FD[n]# (set) -> no payload
    case FOC_OP_TCF_DEADBAND_D: {
      if (dataLen == 0) { reply[0] = 0; return true; }
      int32_t v = 0;
      if (!p.readI32LE(v)) return false;
      sprintf(reply, "%ld", (long)v);
      return true;
    }

    // :FP# (get) -> RX: u8 percent (or i32 if you choose; server I gave uses u8)
    // :FP[n]# (set) -> no payload
    case FOC_OP_DC_POWER_P: {
      if (dataLen == 0) { reply[0] = 0; return true; }
      uint8_t v = 0;
      if (!p.readU8(v)) return false;
      sprintf(reply, "%u", (unsigned)v);
      return true;
    }

    // :FW# -> RX: i32LE work rate (um/s or whatever your server returns)
    case FOC_OP_GET_WORKRATE_W: {
      if (dataLen == 0) { reply[0] = 0; return true; }
      int32_t v = 0;
      if (!p.readI32LE(v)) return false;
      sprintf(reply, "%ld", (long)v);
      return true;
    }

    // :FG# -> RX: i32LE position (units)
    case FOC_OP_GET_POS_G: {
      int32_t v = 0;
      if (!p.readI32LE(v)) return false;
      sprintf(reply, "%ld", (long)v);
      return true;
    }

    default:
      // Most ops are "no reply payload"; empty reply is fine.
      reply[0] = 0;
      return true;
  }
}

#endif
