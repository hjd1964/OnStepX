//--------------------------------------------------------------------------------------------------
// remote telescope rotator control, using the CANbus interface

#include "Rotator.h"

#if defined(ROTATOR_CAN_CLIENT_PRESENT)

#include "../../../lib/convert/Convert.h"

bool Rotator::encodeRequest(char *command, char *parameter) {
  // Contract: both pointers are always provided by the command router.
  if (!command || !parameter) return false;

  // --------------------------------------------------------------------------
  // h - park/unpark
  // --------------------------------------------------------------------------
  if (command[0] == 'h') {

    // :hP#
    if (command[1] == 'P' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_PARK_HP)) return false;
    } else

    // :hR#
    if (command[1] == 'R' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_UNPARK_HR)) return false;
    } else
      return false;

  } else

  // --------------------------------------------------------------------------
  // r - rotator commands (keep same order as Rotator.command.cpp)
  // --------------------------------------------------------------------------
  if (command[0] == 'r') {

    // :rA#
    if (command[1] == 'A' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_ACTIVE_RA)) return false;
    } else

    // :rT#
    if (command[1] == 'T' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_GET_STATUS_T)) return false;
    } else

    // :rI#
    if (command[1] == 'I' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_GET_MIN_I)) return false;
    } else

    // :rM#
    if (command[1] == 'M' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_GET_MAX_M)) return false;
    } else

    // :rD#
    if (command[1] == 'D' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_GET_DEG_PERSTEP_D)) return false;
    } else

    // :rb# (get)
    if (command[1] == 'b' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_BACKLASH_b)) return false;
    } else

    // :rb[n]# (set)
    if (command[1] == 'b' && parameter[0] != 0) {
      if (!beginNewRequest(ROT_OP_BACKLASH_b)) return false;
      if (!writeI16((int16_t)atol(parameter))) return false;
    } else

    // :rQ#
    if (command[1] == 'Q' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_STOP_Q)) return false;
    }

    // :r[1..9]#  (no parameter allowed)
    if (command[1] >= '1' && command[1] <= '9') {
      if (parameter[0] != 0) return false;
      if (!beginNewRequest(ROT_OP_SET_RATE_1_9)) return false;
      if (!writeU8((uint8_t)(command[1] - '0'))) return false;
    } else

    // :rW#
    if (command[1] == 'W' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_GET_WORKRATE_W)) return false;
    } else

    // :rc#
    if (command[1] == 'c' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_CONTINUOUS_c)) return false;
    } else

    // :r>#
    if (command[1] == '>' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_MOVE_CW_GT)) return false;
    } else

    // :r<#
    if (command[1] == '<' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_MOVE_CCW_LT)) return false;
    } else

    // :rG#
    if (command[1] == 'G' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_GET_ANGLE_G)) return false;
    } else

    // :rr[sDDD*MM, etc.]#
    if (command[1] == 'r' && parameter[0] != 0) {
      double r = 0.0;
      if (!convert.dmsToDouble(&r, parameter, true)) return false;
      if (!beginNewRequest(ROT_OP_GOTO_REL_rr)) return false;
      if (!writeF32((float)r)) return false;
    } else

    // :rS[sDDD*MM, etc.]#
    if (command[1] == 'S' && parameter[0] != 0) {
      int i = 0;
      double t = 0.0, s = 1.0;
      if (parameter[0] == '-') s = -1.0;
      if (parameter[0] == '+' || parameter[0] == '-') i = 1;
      if (!convert.dmsToDouble(&t, &parameter[i], false)) return false;

      if (!beginNewRequest(ROT_OP_GOTO_S)) return false;
      if (!writeF32((float)(s * t))) return false;
    } else

    // :rZ#
    if (command[1] == 'Z' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_ZERO_Z)) return false;
    } else

    // :rF#
    if (command[1] == 'F' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_HALFTRAVEL_F)) return false;
    } else

    // :rC#
    if (command[1] == 'C' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_HALFTARGET_C)) return false;
    } else

    // :r+#
    if (command[1] == '+' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_DEROT_EN_PLUS)) return false;
    } else

    // :r-#
    if (command[1] == '-' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_DEROT_DIS_MINUS)) return false;
    } else

    // :rP#
    if (command[1] == 'P' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_GOTO_PAR_P)) return false;
    } else

    // :rR#
    if (command[1] == 'R' && parameter[0] == 0) {
      if (!beginNewRequest(ROT_OP_DEROT_REV_R)) return false;
    } else
      return false;

    return true;
  } else

  // --------------------------------------------------------------------------
  // :GX98#  Get rotator availability
  // --------------------------------------------------------------------------
  if (command[0] == 'G' && command[1] == 'X') {
    if (parameter[0] == '9' && parameter[1] == '8' && parameter[2] == 0) {
      if (!beginNewRequest(ROT_OP_AVAIL_GX98)) return false;
    } else
      return false;
    
    return true;
  }

  return false;
}

#endif
