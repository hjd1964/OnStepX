//--------------------------------------------------------------------------------------------------
// local telescope focuser control, CAN server (remote node)

#include "Focuser.h"

#ifdef FOCUSER_CAN_SERVER_PRESENT

#include "../../../lib/canTransport/CanPayload.h"
#include "../../../lib/axis/Axis.h"

extern Axis axis4;
#define FOCUSER_CAN_INDEX 0 // for the first focuser at axis 4

void Focuser::processCommand(const uint8_t data[8], uint8_t len) {
  if (len < 2) return; // need tidop + ctrl

  const uint8_t tidop = data[0];
  const uint8_t op    = (uint8_t)(tidop & 0x1F);

  const uint8_t ctrl  = data[1];
  const uint8_t focuserNumber = (uint8_t)(ctrl & FOC_CTRL_FOCUSER_MASK);     // 1..6
  const bool useMicrons = (ctrl & FOC_CTRL_UNIT_MICRONS) != 0;

  // Gate: only answer requests meant for this node.
  if (focuserNumber != (uint8_t)CAN_FOCUSER_NUMBER) return;

  const uint8_t argLen = (len > 2) ? (uint8_t)(len - 2) : 0;
  CanPayload args(&data[2], argLen);

  uint8_t payload[6] = {0};
  uint8_t payloadLen = 0;

  bool handled        = true;
  bool numericReply   = true;   // default: boolean 1/0 behavior
  bool suppressFrame  = true;   // not used on CAN wire, but keep contract
  CommandError commandError = CE_NONE;

  // Unit conversions (apply ONLY to I/M/G/R/S/B/D)
  const float micronsToSteps = axis4.getStepsPerMeasure();      // steps per micron
  const float stepsToMicrons = (micronsToSteps != 0.0f) ? (1.0f / micronsToSteps) : 0.0f;

  auto unitsToSteps_i32 = [&](int32_t vUnits) -> int32_t {
    if (useMicrons) return (int32_t)lround((double)vUnits * (double)micronsToSteps);
    return vUnits;
  };

  auto stepsToUnits_i32 = [&](int32_t vSteps) -> int32_t {
    if (useMicrons) return (int32_t)lround((double)vSteps * (double)stepsToMicrons);
    return vSteps;
  };

  auto micronsToUnits_i32 = [&](double vMicrons) -> int32_t {
    if (useMicrons) return (int32_t)lround(vMicrons);
    return (int32_t)lround(vMicrons * (double)micronsToSteps);
  };

  switch (op) {

    // :hP#  -> numeric 1/0
    case FOC_OP_PARK_HP: {
      CommandError e = park(FOCUSER_CAN_INDEX);
      commandError = (e == CE_NONE) ? CE_1 : e;
    } break;

    // :hR#  -> numeric 1/0
    case FOC_OP_UNPARK_HR: {
      CommandError e = unpark(FOCUSER_CAN_INDEX);
      commandError = (e == CE_NONE) ? CE_1 : e;
    } break;

    // :FT# -> "M"/"S" + rateDigit (ASCII)
    case FOC_OP_GET_STATUS_T: {
      payload[payloadLen++] = axis4.isSlewing() ? (uint8_t)'M' : (uint8_t)'S';
      payload[payloadLen++] = (uint8_t)('0' + getGotoRate(FOCUSER_CAN_INDEX)); // 1..5 in your local code
      numericReply  = false;
      suppressFrame = false;
    } break;

    // :Fp# -> status-only boolean (matches local: CE_0 if not DC)
    case FOC_OP_GET_MODE_p: {
      if (!isDC(FOCUSER_CAN_INDEX)) commandError = CE_0; // numeric 0/1 synthesized by caller
      // numericReply stays true, no payload
    } break;

    // :FI# -> i32 units
    case FOC_OP_GET_MIN_I: {
      const double limMinMicrons = (double)axis4.getLimitMin();
      const int32_t out = micronsToUnits_i32(limMinMicrons);
      CanPayload outp(payload, sizeof(payload));
      outp.writeI32LE(out);
      payloadLen = outp.offset();
      numericReply  = false;
      suppressFrame = false;
    } break;

    // :FM# -> i32 units
    case FOC_OP_GET_MAX_M: {
      const double limMaxMicrons = (double)axis4.getLimitMax();
      const int32_t out = micronsToUnits_i32(limMaxMicrons);
      CanPayload outp(payload, sizeof(payload));
      outp.writeI32LE(out);
      payloadLen = outp.offset();
      numericReply  = false;
      suppressFrame = false;
    } break;

    // :Fe# -> f32 (deg C)
    case FOC_OP_GET_TEMPDIFF_e: {
      // Local behavior: if TCF disabled, diff=0
      const float diff = getTcfEnable(FOCUSER_CAN_INDEX) ? (getTemperature() - getTcfT0(FOCUSER_CAN_INDEX)) : 0.0f;
      CanPayload outp(payload, sizeof(payload));
      outp.writeF32LE(diff);
      payloadLen = outp.offset();
      numericReply  = false;
      suppressFrame = false;
    } break;

    // :Ft# -> f32 (deg C)
    case FOC_OP_GET_TEMP_t: {
      const float t = getTemperature();
      CanPayload outp(payload, sizeof(payload));
      outp.writeF32LE(t);
      payloadLen = outp.offset();
      numericReply  = false;
      suppressFrame = false;
    } break;

    // :Fu# -> f32 (microns per step)
    case FOC_OP_GET_UM_PER_STEP_u: {
      const float umPerStep = (micronsToSteps != 0.0f) ? (1.0f / micronsToSteps) : 0.0f;
      CanPayload outp(payload, sizeof(payload));
      outp.writeF32LE(umPerStep);
      payloadLen = outp.offset();
      numericReply  = false;
      suppressFrame = false;
    } break;

    // :FB# (get) or :FB[n]# (set) -> values in units (microns or steps)
    case FOC_OP_BACKLASH_B: {
      if (argLen == 0) {
        const int32_t vUnits = stepsToUnits_i32((int32_t)getBacklashSteps(FOCUSER_CAN_INDEX));
        CanPayload outp(payload, sizeof(payload));
        outp.writeI32LE(vUnits);
        payloadLen = outp.offset();
        numericReply  = false;
        suppressFrame = false;
      } else {
        int32_t vUnits = 0;
        if (!args.readI32LE(vUnits)) { commandError = CE_PARAM_FORM; break; }
        const long vSteps = (long)unitsToSteps_i32(vUnits);
        CommandError e = setBacklashSteps(FOCUSER_CAN_INDEX, vSteps);
        commandError = (e == CE_NONE) ? CE_1 : e;
      }
    } break;

    // :FC# (get f32) or :FC[sn.n]# (set f32)
    case FOC_OP_TCF_COEF_C: {
      if (argLen == 0) {
        const float c = getTcfCoef(FOCUSER_CAN_INDEX);
        CanPayload outp(payload, sizeof(payload));
        outp.writeF32LE(c);
        payloadLen = outp.offset();
        numericReply  = false;
        suppressFrame = false;
      } else {
        float v = 0.0f;
        if (!args.readF32LE(v)) { commandError = CE_PARAM_FORM; break; }
        if (!setTcfCoef(FOCUSER_CAN_INDEX, v)) commandError = CE_PARAM_RANGE;
      }
    } break;

    // :Fc# (get) or :Fc[n]# (set) -> status-only boolean (matches local)
    case FOC_OP_TCF_ENABLE_c: {
      if (argLen == 0) {
        if (!getTcfEnable(FOCUSER_CAN_INDEX)) commandError = CE_0; // disabled => "0"
      } else {
        uint8_t en = 0;
        if (!args.readU8(en)) { commandError = CE_PARAM_FORM; break; }
        commandError = setTcfEnable(FOCUSER_CAN_INDEX, en != 0);
      }
      // numericReply stays true; no payload
    } break;

    // :FD# (get) or :FD[n]# (set) -> values in units
    case FOC_OP_TCF_DEADBAND_D: {
      if (argLen == 0) {
        const int32_t vUnits = stepsToUnits_i32((int32_t)getTcfDeadband(FOCUSER_CAN_INDEX));
        CanPayload outp(payload, sizeof(payload));
        outp.writeI32LE(vUnits);
        payloadLen = outp.offset();
        numericReply  = false;
        suppressFrame = false;
      } else {
        int32_t vUnits = 0;
        if (!args.readI32LE(vUnits)) { commandError = CE_PARAM_FORM; break; }
        const long vSteps = (long)unitsToSteps_i32(vUnits);
        if (!setTcfDeadband(FOCUSER_CAN_INDEX, vSteps)) commandError = CE_PARAM_RANGE;
      }
    } break;

    // :FP# (get) or :FP[n]# (set) -> u8 percent (no unit-bit conversion)
    case FOC_OP_DC_POWER_P: {
      if (!isDC(FOCUSER_CAN_INDEX)) { commandError = CE_CMD_UNKNOWN; break; }
      if (argLen == 0) {
        const uint8_t pwr = (uint8_t)getDcPower(FOCUSER_CAN_INDEX);
        CanPayload outp(payload, sizeof(payload));
        outp.writeU8(pwr);
        payloadLen = outp.offset();
        numericReply  = false;
        suppressFrame = false;
      } else {
        uint8_t pwr = 0;
        if (!args.readU8(pwr)) { commandError = CE_PARAM_FORM; break; }
        if (!setDcPower(FOCUSER_CAN_INDEX, pwr)) commandError = CE_PARAM_RANGE;
      }
    } break;

    // :FQ# -> no reply text
    case FOC_OP_STOP_Q: {
      if (axis4.isHoming()) { axis4.autoSlewAbort(); homing[FOCUSER_CAN_INDEX] = true; }
      else axis4.autoSlewStop();
      numericReply = false;
    } break;

    // :F[1..9]# -> no reply text
    case FOC_OP_SET_RATE_1_9: {
      uint8_t digit = 0;
      if (!args.readU8(digit)) { commandError = CE_PARAM_FORM; break; }
      if (digit < 1 || digit > 9) { commandError = CE_PARAM_RANGE; break; }
      if (digit < 5) setMoveRate(FOCUSER_CAN_INDEX, (int)digit); else setGotoRate(FOCUSER_CAN_INDEX, (int)digit - 4);
      numericReply = false;
    } break;

    // :FW# -> i32 µm/s (NO unit-bit conversion per your clarification)
    case FOC_OP_GET_WORKRATE_W: {
      const int32_t umps = (int32_t)settings[FOCUSER_CAN_INDEX].gotoRate;
      CanPayload outp(payload, sizeof(payload));
      outp.writeI32LE(umps);
      payloadLen = outp.offset();
      numericReply  = false;
      suppressFrame = false;
    } break;

    // :F+# / :F-# -> no reply text
    case FOC_OP_MOVE_IN_PLUS:
      commandError = move(FOCUSER_CAN_INDEX, DIR_FORWARD);
      numericReply = false;
    break;

    case FOC_OP_MOVE_OUT_MINUS:
      commandError = move(FOCUSER_CAN_INDEX, DIR_REVERSE);
      numericReply = false;
    break;

    // :FG# -> i32 units (position)
    case FOC_OP_GET_POS_G: {
      // Mirror local: (instrumentSteps - tcfSteps) converted to units
      const int32_t posSteps = (int32_t)(axis4.getInstrumentCoordinateSteps() - tcfSteps[FOCUSER_CAN_INDEX]);
      const int32_t posUnits = stepsToUnits_i32(posSteps);
      CanPayload outp(payload, sizeof(payload));
      outp.writeI32LE(posUnits);
      payloadLen = outp.offset();
      numericReply  = false;
      suppressFrame = false;
    } break;

    // :FR[sn]# -> no reply text (relative)
    case FOC_OP_GOTO_REL_R: {
      int32_t dUnits = 0;
      if (!args.readI32LE(dUnits)) { commandError = CE_PARAM_FORM; break; }
      const long dSteps = (long)unitsToSteps_i32(dUnits);
      commandError = gotoTarget(FOCUSER_CAN_INDEX, (long)target[FOCUSER_CAN_INDEX] + dSteps);
      numericReply = false;
    } break;

    // :FS[n]# -> numeric 1/0 (absolute)
    case FOC_OP_GOTO_ABS_S: {
      int32_t tUnits = 0;
      if (!args.readI32LE(tUnits)) { commandError = CE_PARAM_FORM; break; }
      const long tSteps = (long)unitsToSteps_i32(tUnits);
      commandError = gotoTarget(FOCUSER_CAN_INDEX, tSteps);
      // numericReply stays true to match local :FS behavior
    } break;

    // :FZ# -> no reply text
    case FOC_OP_ZERO_Z:
      commandError = resetTarget(FOCUSER_CAN_INDEX, 0);
      numericReply = false;
    break;

    // :FH# -> no reply text (set home)
    case FOC_OP_SET_HOME_H:
      commandError = resetTarget(FOCUSER_CAN_INDEX, (long)(int32_t)lround((double)getHomePosition(FOCUSER_CAN_INDEX) * (double)micronsToSteps));
      numericReply = false;
    break;

    // :Fh# -> no reply text (goto home)
    case FOC_OP_GOTO_HOME_h: {
      if (axis4.hasHomeSense()) commandError = moveHome(FOCUSER_CAN_INDEX);
      else commandError = gotoTarget(FOCUSER_CAN_INDEX, (long)(int32_t)lround((double)getHomePosition(FOCUSER_CAN_INDEX) * (double)micronsToSteps));
      numericReply = false;
    } break;

    default:
      handled = false;
      commandError = CE_CMD_UNKNOWN;
    break;
  }

  const uint8_t status = packStatus(handled, numericReply, suppressFrame, commandError);
  sendResponse(tidop, status, payloadLen ? payload : nullptr, payloadLen);
}

#endif
