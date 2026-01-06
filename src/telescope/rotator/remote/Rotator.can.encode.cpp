//--------------------------------------------------------------------------------------------------
// remote telescope rotator control, using the CANbus interface

#include "Rotator.h"

#if defined(ROTATOR_CAN_CLIENT_PRESENT)

#include "../../../lib/convert/Convert.h"
#include "../../../lib/canTransport/CanPayload.h"

bool Rotator::encodeRequest(uint8_t &opcode, uint8_t &tidop,
                            uint8_t requestPayload[8], uint8_t &requestLen,
                            char *command, char *parameter) {
  // Contract: both pointers are always provided by the command router.
  if (!command || !parameter) return false;

  // rolling 3-bit TID
  static uint8_t tid = 0;
  tid = (uint8_t)((tid + 1) & 0x07);

  // Always build the payload with CanPayload.
  // Byte 0 = tidop, bytes 1..7 = optional args.
  CanPayload p(requestPayload, 8);

  auto begin = [&](uint8_t op) -> bool {
    opcode = op;
    tidop = packTidOp(tid, opcode);
    requestPayload[0] = tidop;

    // reserve byte 0, start writing args at byte 1
    if (!p.seek(1)) return false;
    requestLen = 1;           // updated later if args written
    return true;
  };

  auto finish = [&]() -> bool {
    requestLen = p.offset();  // 1 + arg bytes
    return true;
  };

  // --------------------------------------------------------------------------
  // h - park/unpark
  // --------------------------------------------------------------------------
  if (command[0] == 'h') {

    // :hP#
    if (command[1] == 'P' && parameter[0] == 0) {
      if (!begin(ROT_OP_PARK_HP)) return false;
      return finish(); // len=1
    }

    // :hR#
    if (command[1] == 'R' && parameter[0] == 0) {
      if (!begin(ROT_OP_UNPARK_HR)) return false;
      return finish(); // len=1
    }

    return false;
  } else

  // --------------------------------------------------------------------------
  // r - rotator commands (keep same order as Rotator.command.cpp)
  // --------------------------------------------------------------------------
  if (command[0] == 'r') {

    // :rA#
    if (command[1] == 'A' && parameter[0] == 0) {
      if (!begin(ROT_OP_ACTIVE_RA)) return false;
      return finish();
    }

    // :rT#
    if (command[1] == 'T' && parameter[0] == 0) {
      if (!begin(ROT_OP_GET_STATUS_T)) return false;
      return finish();
    }

    // :rI#
    if (command[1] == 'I' && parameter[0] == 0) {
      if (!begin(ROT_OP_GET_MIN_I)) return false;
      return finish();
    }

    // :rM#
    if (command[1] == 'M' && parameter[0] == 0) {
      if (!begin(ROT_OP_GET_MAX_M)) return false;
      return finish();
    }

    // :rD#
    if (command[1] == 'D' && parameter[0] == 0) {
      if (!begin(ROT_OP_GET_DEG_PERSTEP_D)) return false;
      return finish();
    }

    // :rb# (get)
    if (command[1] == 'b' && parameter[0] == 0) {
      if (!begin(ROT_OP_BACKLASH_b)) return false;
      return finish();
    }

    // :rb[n]# (set)
    if (command[1] == 'b' && parameter[0] != 0) {
      long v = atol(parameter);
      if (v < -32768L || v > 32767L) return false; // client-side sanity
      if (!begin(ROT_OP_BACKLASH_b)) return false;
      if (!p.writeI16LE((int16_t)v)) return false;
      return finish(); // len=3
    }

    // :rQ#
    if (command[1] == 'Q' && parameter[0] == 0) {
      if (!begin(ROT_OP_STOP_Q)) return false;
      return finish();
    }

    // :r[1..9]#  (no parameter allowed)
    if (command[1] >= '1' && command[1] <= '9') {
      if (parameter[0] != 0) return false;
      const uint8_t digit = (uint8_t)(command[1] - '0'); // 1..9
      if (!begin(ROT_OP_SET_RATE_1_9)) return false;
      if (!p.writeU8(digit)) return false;
      return finish(); // len=2
    }

    // :rW#
    if (command[1] == 'W' && parameter[0] == 0) {
      if (!begin(ROT_OP_GET_WORKRATE_W)) return false;
      return finish();
    }

    // :rc#
    if (command[1] == 'c' && parameter[0] == 0) {
      if (!begin(ROT_OP_CONTINUOUS_c)) return false;
      return finish();
    }

    // :r>#
    if (command[1] == '>' && parameter[0] == 0) {
      if (!begin(ROT_OP_MOVE_CW_GT)) return false;
      return finish();
    }

    // :r<#
    if (command[1] == '<' && parameter[0] == 0) {
      if (!begin(ROT_OP_MOVE_CCW_LT)) return false;
      return finish();
    }

    // :rG#
    if (command[1] == 'G' && parameter[0] == 0) {
      if (!begin(ROT_OP_GET_ANGLE_G)) return false;
      return finish();
    }

    // :rr[sDDD*MM, etc.]#
    if (command[1] == 'r' && parameter[0] != 0) {
      double r = 0.0;
      if (!convert.dmsToDouble(&r, parameter, true)) return false;
      if (!begin(ROT_OP_GOTO_REL_rr)) return false;
      if (!p.writeF32LE((float)r)) return false;
      return finish(); // len=5
    }

    // :rS[sDDD*MM, etc.]#
    if (command[1] == 'S' && parameter[0] != 0) {
      int i = 0;
      double t = 0.0, s = 1.0;
      if (parameter[0] == '-') s = -1.0;
      if (parameter[0] == '+' || parameter[0] == '-') i = 1;
      if (!convert.dmsToDouble(&t, &parameter[i], false)) return false;

      if (!begin(ROT_OP_GOTO_S)) return false;
      if (!p.writeF32LE((float)(s * t))) return false;
      return finish(); // len=5
    }

    // :rZ#
    if (command[1] == 'Z' && parameter[0] == 0) {
      if (!begin(ROT_OP_ZERO_Z)) return false;
      return finish();
    }

    // :rF#
    if (command[1] == 'F' && parameter[0] == 0) {
      if (!begin(ROT_OP_HALFTRAVEL_F)) return false;
      return finish();
    }

    // :rC#
    if (command[1] == 'C' && parameter[0] == 0) {
      if (!begin(ROT_OP_HALFTARGET_C)) return false;
      return finish();
    }

    // :r+#
    if (command[1] == '+' && parameter[0] == 0) {
      if (!begin(ROT_OP_DEROT_EN_PLUS)) return false;
      return finish();
    }

    // :r-#
    if (command[1] == '-' && parameter[0] == 0) {
      if (!begin(ROT_OP_DEROT_DIS_MINUS)) return false;
      return finish();
    }

    // :rP#
    if (command[1] == 'P' && parameter[0] == 0) {
      if (!begin(ROT_OP_GOTO_PAR_P)) return false;
      return finish();
    }

    // :rR#
    if (command[1] == 'R' && parameter[0] == 0) {
      if (!begin(ROT_OP_DEROT_REV_R)) return false;
      return finish();
    }

    return false;
  } else

  // --------------------------------------------------------------------------
  // :GX98#  Get rotator availability
  // --------------------------------------------------------------------------
  if (command[0] == 'G' && command[1] == 'X') {
    if (parameter[0] == '9' && parameter[1] == '8' && parameter[2] == 0) {
      if (!begin(ROT_OP_AVAIL_GX98)) return false;
      return finish();
    }
    return false;
  }

  return false;
}

#endif
