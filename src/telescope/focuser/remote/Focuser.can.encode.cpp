//--------------------------------------------------------------------------------------------------
// remote telescope focuser control, using the CANbus interface

#include "Focuser.h"

#if defined(FOCUSER_CAN_CLIENT_PRESENT)

#include "../../../lib/convert/Convert.h"
#include "../../../lib/canTransport/CanPayload.h"

// Request:  [ tidop, ctrl, args... ]
// Response: [ tidop, status, payload... ]

bool Focuser::encodeRequest(uint8_t &opcode, uint8_t &tidop,
                            uint8_t requestPayload[8], uint8_t &requestLen,
                            char *command, char *parameter) {
  if (!command || !parameter) return false;

  // rolling 3-bit TID
  static uint8_t tid = 0;
  tid = (uint8_t)((tid + 1) & 0x07);

  CanPayload p(requestPayload, 8);

  auto begin = [&](uint8_t op, uint8_t ctrl) -> bool {
    opcode = op;
    tidop  = packTidOp(tid, opcode);
    requestPayload[0] = tidop;
    requestPayload[1] = ctrl;

    // reserve byte 0 for tidop, byte 1 for ctrl
    if (!p.seek(2)) return false;
    requestLen = 2;
    return true;
  };

  auto finish = [&]() -> bool {
    requestLen = p.offset();
    return true;
  };

  auto noParamAllowed = [&](char c) -> bool {
    return strchr("aTpIMtuQF1234+-GZHh", c) != nullptr;
  };

  // --------------------------------------------------------------------------
  // Determine target focuser number 1..6 (or default active)
  // --------------------------------------------------------------------------
  // Local semantics:
  //   :F[...]#   uses selected focuser (active)
  //   :F1[...]#  focuser #1
  // and the local code rewrites command[1] + shifts parameter; we do NOT mutate
  // strings here; we just compute focuserNum + effective subcommand.
  uint8_t focuserNum = 0; // 1..6; 0 means "use active" (we'll map to active below)
  char subcmd = 0;

  // This encoder assumes the router already split command vs parameter, so:
  // command is like "F", "FA", "F1", etc. In your code it's a 2-char buffer
  // where command[0] is the leading letter and command[1] is the subcommand.
  //
  // For the :F1[...]# special-case, your local parser used command[1] = '1' and
  // then consumed parameter[0] as the true subcommand. We'll implement the same:
  //
  // If command[0]=='F' and command[1] in '1'..'6' and parameter[0]!=0 then:
  //   focuserNum = command[1]-'0'
  //   subcmd = parameter[0]
  //   args come from parameter+1
  // Else:
  //   focuserNum = active+1
  //   subcmd = command[1]
  //   args come from parameter (as-is)

  const bool explicitFocuser = (command[0] == 'F' &&
                               command[1] >= '1' && command[1] <= '6' &&
                               parameter[0] != 0);

  const char *argStr = parameter;

  if (explicitFocuser) {
    focuserNum = (uint8_t)(command[1] - '0');  // 1..6
    subcmd     = parameter[0];                 // real command
    argStr     = &parameter[1];                // rest of parameter (may be empty)
  } else {
    // Use current active focuser (1..6). If none, fail.
    if (active < 0 || active >= 6) return false;
    focuserNum = (uint8_t)(active + 1);
    subcmd     = command[1];
    argStr     = parameter;
  }

  // Heartbeat gating rejects commands to missing nodes
  if (!heartbeatFresh(focuserNum - 1)) return false;

  // --------------------------------------------------------------------------
  // ctrl byte: focuser number + units bit
  // --------------------------------------------------------------------------
  // Units bit: 1=microns, 0=steps.
  // For commands that have upper/lower variants (I/M/G/R/S/B/D/H/h/Z), use case.
  // For others, the bit is ignored by the server anyway (floats/status).
  bool useMicrons = false;
  if (strchr("IMGRSBD", (char)toupper(subcmd)) != nullptr) {
    useMicrons = (subcmd >= 'A' && subcmd <= 'Z'); // uppercase -> microns form
  }
  uint8_t ctrl = (uint8_t)(focuserNum & FOC_CTRL_FOCUSER_MASK);
  if (useMicrons) ctrl |= FOC_CTRL_UNIT_MICRONS;

  // Reject parameters where they are not allowed (based on effective subcmd).
  if (noParamAllowed(subcmd) && argStr[0] != 0) return false;

  // :FT#
  if (subcmd == 'T' && argStr[0] == 0) {
    if (!begin(FOC_OP_GET_STATUS_T, ctrl)) return false;
    return finish();
  }

  // :Fp#
  if (subcmd == 'p' && argStr[0] == 0) {
    if (!begin(FOC_OP_GET_MODE_p, ctrl)) return false;
    return finish();
  }

  // :FI# / :Fi# (min)  (same opcode; units via ctrl)
  if (toupper(subcmd) == 'I' && argStr[0] == 0) {
    if (!begin(FOC_OP_GET_MIN_I, ctrl)) return false;
    return finish();
  }

  // :FM# / :Fm# (max)
  if (toupper(subcmd) == 'M' && argStr[0] == 0) {
    if (!begin(FOC_OP_GET_MAX_M, ctrl)) return false;
    return finish();
  }

  // :Fe#
  if (subcmd == 'e' && argStr[0] == 0) {
    if (!begin(FOC_OP_GET_TEMPDIFF_e, ctrl)) return false;
    return finish();
  }

  // :Ft#
  if (subcmd == 't' && argStr[0] == 0) {
    if (!begin(FOC_OP_GET_TEMP_t, ctrl)) return false;
    return finish();
  }

  // :Fu#
  if (subcmd == 'u' && argStr[0] == 0) {
    if (!begin(FOC_OP_GET_UM_PER_STEP_u, ctrl)) return false;
    return finish();
  }

  // :FB# (get) or :FB[n]# (set)  (units via ctrl)
  if (toupper(subcmd) == 'B') {
    if (!begin(FOC_OP_BACKLASH_B, ctrl)) return false;
    if (argStr[0] != 0) {
      const long v = atol(argStr);
      if (!p.writeI32LE((int32_t)v)) return false;
    }
    return finish();
  }

  // :FC# (get f32) or :FC[sn.n]# (set f32)
  if (subcmd == 'C') {
    if (!begin(FOC_OP_TCF_COEF_C, ctrl)) return false;
    if (argStr[0] != 0) {
      const float v = (float)atof(argStr);
      if (!p.writeF32LE(v)) return false;
    }
    return finish();
  }

  // :Fc# (get) or :Fc[n]# (set u8)
  if (subcmd == 'c') {
    if (!begin(FOC_OP_TCF_ENABLE_c, ctrl)) return false;
    if (argStr[0] != 0) {
      if (argStr[1] != 0) return false;
      const uint8_t en = (argStr[0] != '0') ? 1 : 0;
      if (!p.writeU8(en)) return false;
    }
    return finish();
  }

  // :FD# (get) or :FD[n]# (set i32)  (units via ctrl)
  if (toupper(subcmd) == 'D') {
    if (!begin(FOC_OP_TCF_DEADBAND_D, ctrl)) return false;
    if (argStr[0] != 0) {
      const long v = atol(argStr);
      if (!p.writeI32LE((int32_t)v)) return false;
    }
    return finish();
  }

  // :FP# (get) or :FP[n]# (set u8)
  if (subcmd == 'P') {
    if (!begin(FOC_OP_DC_POWER_P, ctrl)) return false;
    if (argStr[0] != 0) {
      const long v = atol(argStr);
      if (v < 0 || v > 100) return false;
      if (!p.writeU8((uint8_t)v)) return false;
    }
    return finish();
  }

  // :FQ#
  if (subcmd == 'Q' && argStr[0] == 0) {
    if (!begin(FOC_OP_STOP_Q, ctrl)) return false;
    return finish();
  }

  // :F[1..9]#  (rate digit; no param)
  if (subcmd >= '1' && subcmd <= '9') {
    if (argStr[0] != 0) return false;
    const uint8_t digit = (uint8_t)(subcmd - '0'); // 1..9
    if (!begin(FOC_OP_SET_RATE_1_9, ctrl)) return false;
    if (!p.writeU8(digit)) return false;
    return finish();
  }

  // :FW#
  if (subcmd == 'W' && argStr[0] == 0) {
    if (!begin(FOC_OP_GET_WORKRATE_W, ctrl)) return false;
    return finish();
  }

  // :F+#
  if (subcmd == '+' && argStr[0] == 0) {
    if (!begin(FOC_OP_MOVE_IN_PLUS, ctrl)) return false;
    return finish();
  }

  // :F-#
  if (subcmd == '-' && argStr[0] == 0) {
    if (!begin(FOC_OP_MOVE_OUT_MINUS, ctrl)) return false;
    return finish();
  }

  // :FG# / :Fg#  (get pos i32 units)
  if (toupper(subcmd) == 'G' && argStr[0] == 0) {
    if (!begin(FOC_OP_GET_POS_G, ctrl)) return false;
    return finish();
  }

  // :FR[sn]# / :Fr[sn]# (relative i32 units)
  if (toupper(subcmd) == 'R') {
    if (argStr[0] == 0) return false;
    if (!begin(FOC_OP_GOTO_REL_R, ctrl)) return false;
    if (!p.writeI32LE((int32_t)atol(argStr))) return false;
    return finish();
  }

  // :FS[n]# / :Fs[n]# (absolute i32 units)
  if (toupper(subcmd) == 'S') {
    if (argStr[0] == 0) return false;
    if (!begin(FOC_OP_GOTO_ABS_S, ctrl)) return false;
    if (!p.writeI32LE((int32_t)atol(argStr))) return false;
    return finish();
  }

  // :FZ#  (zero)
  if (subcmd == 'Z' && argStr[0] == 0) {
    if (!begin(FOC_OP_ZERO_Z, ctrl)) return false;
    return finish();
  }

  // :FH#  (set home)
  if (subcmd == 'H' && argStr[0] == 0) {
    if (!begin(FOC_OP_SET_HOME_H, ctrl)) return false;
    return finish();
  }

  // :Fh#  (goto home)
  if (subcmd == 'h' && argStr[0] == 0) {
    if (!begin(FOC_OP_GOTO_HOME_h, ctrl)) return false;
    return finish();
  }

  return false;
}

#endif
