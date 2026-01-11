//--------------------------------------------------------------------------------------------------
// remote telescope focuser control, using the CANbus interface

#include "Focuser.h"

#if defined(FOCUSER_CAN_CLIENT_PRESENT)

#include "../../../lib/convert/Convert.h"

bool Focuser::encodeRequest(char *command, char *parameter) {

  // on explicit focuser override the active focuser and reform the command/parameter
  uint8_t selectedFocuser = (uint8_t)(active + 1); // 1..6

  if (command[1] >= '1' && command[1] <= '6' && parameter[0] != 0) {
    selectedFocuser = (uint8_t)(command[1] - '0'); // 1..6
    command[1] = parameter[0];
    int len = strlen(parameter);
    for (int i = 0; i <= len; i++) { parameter[i] = parameter[i + 1]; }
  }

  // check to see if the selected focuser is present
  if (!heartbeatFresh(selectedFocuser - 1)) return false;

  // check for commands that shouldn't have a parameter
  if (strchr("aTpIMtuQF1234+-GZHh", command[1]) && parameter[0] != 0) return false;

  // flag if this command is using microns
  bool useMicrons = strchr("bdgimrs", command[1]);

  // ctrl byte: focuser number + units bit (1=microns, 0=steps)
  uint8_t ctrl = (uint8_t)(selectedFocuser & FOC_CTRL_FOCUSER_MASK);
  if (useMicrons) ctrl |= FOC_CTRL_UNIT_MICRONS;

  // :FT#
  if (command[1] == 'T') {
    if (!beginNewRequest(FOC_OP_GET_STATUS_T)) return false; else writeU8(ctrl);
  } else

  // :Fp#
  if (command[1] == 'p') {
    if (!beginNewRequest(FOC_OP_GET_MODE_p)) return false; else writeU8(ctrl);
  } else

  // :FI# / :Fi# (min)  (units via ctrl)
  if (toupper(command[1]) == 'I') {
    if (!beginNewRequest(FOC_OP_GET_MIN_I)) return false; else writeU8(ctrl);
  } else

  // :FM# / :Fm# (max)  (units via ctrl)
  if (toupper(command[1]) == 'M') {
    if (!beginNewRequest(FOC_OP_GET_MAX_M)) return false; else writeU8(ctrl);
  } else

  // :Fe#
  if (command[1] == 'e') {
    if (!beginNewRequest(FOC_OP_GET_TEMPDIFF_e)) return false; else writeU8(ctrl);
  } else

  // :Ft#
  if (command[1] == 't') {
    if (!beginNewRequest(FOC_OP_GET_TEMP_t)) return false; else writeU8(ctrl);
  } else

  // :Fu#
  if (command[1] == 'u') {
    if (!beginNewRequest(FOC_OP_GET_UM_PER_STEP_u)) return false; else writeU8(ctrl);
  } else

  // :FB# (get) or :FB[n]# (set)  (units via ctrl)
  if (toupper(command[1]) == 'B') {
    if (!beginNewRequest(FOC_OP_BACKLASH_B)) return false; else writeU8(ctrl);
    if (parameter[0] != 0) {
      const long v = atol(parameter);
      if (!writeI32((int32_t)v)) return false;
    }
  } else

  // :FC# (get f32) or :FC[sn.n]# (set f32)
  if (command[1] == 'C') {
    if (!beginNewRequest(FOC_OP_TCF_COEF_C)) return false; else writeU8(ctrl);
    if (parameter[0] != 0) {
      const float v = (float)atof(parameter);
      if (!writeF32(v)) return false;
    }
  } else

  // :Fc# (get) or :Fc[n]# (set u8)
  if (command[1] == 'c') {
    if (!beginNewRequest(FOC_OP_TCF_ENABLE_c)) return false; else writeU8(ctrl);
    if (parameter[0] != 0) {
      if (parameter[1] != 0) return false;
      const uint8_t en = (parameter[0] != '0') ? 1 : 0;
      if (!writeU8(en)) return false;
    }
  } else

  // :FD# (get) or :FD[n]# (set i32)
  if (toupper(command[1]) == 'D') {
    if (!beginNewRequest(FOC_OP_TCF_DEADBAND_D)) return false; else writeU8(ctrl);
    if (parameter[0] != 0) {
      const long v = atol(parameter);
      if (!writeI32((int32_t)v)) return false;
    }
  } else

  // :FP# (get) or :FP[n]# (set u8)
  if (command[1] == 'P') {
    if (!beginNewRequest(FOC_OP_DC_POWER_P)) return false; else writeU8(ctrl);
    if (parameter[0] != 0) {
      const long v = atol(parameter);
      if (v < 0 || v > 100) return false;
      if (!writeU8((uint8_t)v)) return false;
    }
  } else

  // :FQ#
  if (command[1] == 'Q') {
    if (!beginNewRequest(FOC_OP_STOP_Q)) return false; else writeU8(ctrl);
  } else

  // :F[1..9]#  (rate digit; no param)
  if (command[1] >= '1' && command[1] <= '9') {
    if (parameter[0] != 0) return false;
    const uint8_t digit = (uint8_t)(command[1] - '0'); // 1..9
    if (!beginNewRequest(FOC_OP_SET_RATE_1_9)) return false; else writeU8(ctrl);
    if (!writeU8(digit)) return false;
  } else

  // :FW#
  if (command[1] == 'W') {
    if (!beginNewRequest(FOC_OP_GET_WORKRATE_W)) return false; else writeU8(ctrl);
  } else

  // :F+#
  if (command[1] == '+') {
    if (!beginNewRequest(FOC_OP_MOVE_IN_PLUS)) return false; else writeU8(ctrl);
  } else

  // :F-#
  if (command[1] == '-') {
    if (!beginNewRequest(FOC_OP_MOVE_OUT_MINUS)) return false; else writeU8(ctrl);
  } else

  // :FG# / :Fg#  (get pos i32 units via ctrl)
  if (toupper(command[1]) == 'G') {
    if (!beginNewRequest(FOC_OP_GET_POS_G)) return false; else writeU8(ctrl);
  } else

  // :FR[sn]# / :Fr[sn]# (relative i32 units via ctrl)
  if (toupper(command[1]) == 'R') {
    if (parameter[0] == 0) return false;
    if (!beginNewRequest(FOC_OP_GOTO_REL_R)) return false; else writeU8(ctrl);
    if (!writeI32((int32_t)atol(parameter))) return false;
  } else

  // :FS[n]# / :Fs[n]# (absolute i32 units via ctrl)
  if (toupper(command[1]) == 'S') {
    if (parameter[0] == 0) return false;
    if (!beginNewRequest(FOC_OP_GOTO_ABS_S)) return false; else writeU8(ctrl);
    if (!writeI32((int32_t)atol(parameter))) return false;
  } else

  // :FZ#  (zero)
  if (command[1] == 'Z') {
    if (!beginNewRequest(FOC_OP_ZERO_Z)) return false; else writeU8(ctrl);
  } else

  // :FH#  (set home)
  if (command[1] == 'H') {
    if (!beginNewRequest(FOC_OP_SET_HOME_H)) return false; else writeU8(ctrl);
  } else

  // :Fh#  (goto home)
  if (command[1] == 'h') {
    if (!beginNewRequest(FOC_OP_GOTO_HOME_h)) return false; else writeU8(ctrl);
  } else
    return false;
  
  return true;
}

#endif
