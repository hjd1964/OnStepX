//--------------------------------------------------------------------------------------------------
// local telescope focuser control, CAN server (remote node)

#include "Focuser.h"

#ifdef FOCUSER_CAN_SERVER_PRESENT

#include "../../../lib/axis/Axis.h"

extern Axis axis4;
#define FOCUSER_CAN_INDEX 0 // for the first focuser at axis 4

// default command semantics (mirrors the common command contract):
// handled=true, numericReply=true, suppressFrame=false, commandError=CE_NONE
// numericReply=true means boolean/numeric-style responses (e.g., CE_1/CE_0/errors) rather than a payload
void Focuser::processCommand() {
  if (!ready) return;

  bool handled        = true;
  bool numericReply   = true;
  bool suppressFrame  = false;
  CommandError commandError = CE_NONE;

  // scratchpad variables
  CommandError e = CE_NONE;
  float f;
  int32_t i;
  uint8_t u;

  // ctrl byte: focuser number + units bit (1=microns, 0=steps)
  uint8_t ctrl;
  if (!readU8(ctrl)) ctrl = 0;
  const uint8_t FocuserNumber = (uint8_t)(ctrl & FOC_CTRL_FOCUSER_MASK); // 1..6
  const bool UseMicrons = (ctrl & FOC_CTRL_UNIT_MICRONS) != 0;

  // Gate: make sure we're ready and only answer requests meant for this node
  if (!ready || FocuserNumber != (uint8_t)CAN_FOCUSER_NUMBER || FocuserNumber == 0) {
    sendResponse(handled, suppressFrame, numericReply, commandError);
  }

  // Unit conversions (apply ONLY to I/M/G/R/S/B/D)
  const float MicronsToSteps = axis4.getStepsPerMeasure(); // steps per micron
  const float StepsToMicrons = MicronsToSteps != 0.0F ? (1.0F / MicronsToSteps) : 0.0F;
  float MicronsToUnits = 1.0F;
  float StepsToUnits  = StepsToMicrons;
  float UnitsToSteps  = MicronsToSteps;
  if (UseMicrons) {
    MicronsToUnits = MicronsToSteps;
    StepsToUnits = 1.0F;
    UnitsToSteps = 1.0F;
  }

  switch (opCode()) {

    // :hP#  -> numeric 1/0
    case FOC_OP_PARK_HP:
      e = park(FOCUSER_CAN_INDEX);
      commandError = (e == CE_NONE) ? CE_1 : e;
    break;

    // :hR#  -> numeric 1/0
    case FOC_OP_UNPARK_HR:
      e = unpark(FOCUSER_CAN_INDEX);
      commandError = (e == CE_NONE) ? CE_1 : e;
    break;

    // :FT# -> "M"/"S" + rateDigit (ASCII)
    case FOC_OP_GET_STATUS_T:
      writeU8(axis4.isSlewing() ? (uint8_t)'M' : (uint8_t)'S');
      writeU8((uint8_t)('0' + getGotoRate(FOCUSER_CAN_INDEX)));
      numericReply = false;
    break;

    // :Fp# -> status-only boolean (matches local: CE_0 if not DC)
    case FOC_OP_GET_MODE_p:
      if (!isDC(FOCUSER_CAN_INDEX)) commandError = CE_0; // numeric 0/1 synthesized by caller
      // numericReply stays true, no payload
    break;

    // :FI# -> i32 units
    case FOC_OP_GET_MIN_I:
      writeI32(axis4.getLimitMin()*MicronsToUnits);
      numericReply = false;
    break;

    // :FM# -> i32 units
    case FOC_OP_GET_MAX_M:
      writeI32(axis4.getLimitMax()*MicronsToUnits);
      numericReply = false;
    break;

    // :Fe# -> f32 (deg C)
    case FOC_OP_GET_TEMPDIFF_e:
      // Local behavior: if TCF disabled, diff = 0
      writeF32(getTcfEnable(FOCUSER_CAN_INDEX) ? (getTemperature() - getTcfT0(FOCUSER_CAN_INDEX)) : 0.0F);
      numericReply = false;
    break;

    // :Ft# -> f32 (deg C)
    case FOC_OP_GET_TEMP_t:
      writeF32(getTemperature());
      numericReply = false;
    break;

    // :Fu# -> f32 (microns per step)
    case FOC_OP_GET_UM_PER_STEP_u:
      writeF32((MicronsToSteps != 0.0F) ? (1.0F/MicronsToSteps) : 0.0F);
      numericReply = false;
    break;

    // :FB# (get) or :FB[n]# (set) -> values in units (microns or steps)
    case FOC_OP_BACKLASH_B:
      if (remaining() == 0) {
        writeI32(getBacklashSteps(FOCUSER_CAN_INDEX)*StepsToUnits);
        numericReply = false;
      } else {
        if (!readI32(i)) { commandError = CE_PARAM_FORM; break; }
        const long vSteps = (long)i*UnitsToSteps;
        CommandError e = setBacklashSteps(FOCUSER_CAN_INDEX, vSteps);
        commandError = (e == CE_NONE) ? CE_1 : e;
      }
    break;

    // :FC# (get f32) or :FC[sn.n]# (set f32)
    case FOC_OP_TCF_COEF_C:
      if (remaining() == 0) {
        writeF32(getTcfCoef(FOCUSER_CAN_INDEX));
        numericReply  = false;
      } else {
        if (!readF32(f)) { commandError = CE_PARAM_FORM; break; }
        if (!setTcfCoef(FOCUSER_CAN_INDEX, f)) commandError = CE_PARAM_RANGE;
      }
    break;

    // :Fc# (get) or :Fc[n]# (set) -> status-only boolean (matches local)
    case FOC_OP_TCF_ENABLE_c:
      if (remaining() == 0) {
        if (!getTcfEnable(FOCUSER_CAN_INDEX)) commandError = CE_0; // disabled => "0"
      } else {
        if (!readU8(u)) { commandError = CE_PARAM_FORM; break; }
        commandError = setTcfEnable(FOCUSER_CAN_INDEX, u != 0);
      }
    break;

    // :FD# (get) or :FD[n]# (set) -> values in units
    case FOC_OP_TCF_DEADBAND_D:
      if (remaining() == 0) {
        writeI32(getTcfDeadband(FOCUSER_CAN_INDEX)*StepsToUnits);
        numericReply = false;
      } else {
        if (!readI32(i)) { commandError = CE_PARAM_FORM; break; }
        if (!setTcfDeadband(FOCUSER_CAN_INDEX, i*UnitsToSteps)) commandError = CE_PARAM_RANGE;
      }
    break;

    // :FP# (get) or :FP[n]# (set) -> u8 percent (no unit-bit conversion)
    case FOC_OP_DC_POWER_P:
      if (!isDC(FOCUSER_CAN_INDEX)) { commandError = CE_CMD_UNKNOWN; break; }
      if (remaining() == 0) {
        writeU8(getDcPower(FOCUSER_CAN_INDEX));
        numericReply = false;
      } else {
        if (!readU8(u)) { commandError = CE_PARAM_FORM; break; }
        if (!setDcPower(FOCUSER_CAN_INDEX, u)) commandError = CE_PARAM_RANGE;
      }
    break;

    // :FQ# -> no reply text
    case FOC_OP_STOP_Q:
      if (axis4.isHoming()) {
        axis4.autoSlewAbort();
        homing[FOCUSER_CAN_INDEX] = true;
      } else
        axis4.autoSlewStop();
      numericReply = false;
    break;

    // :F[1..9]# -> no reply text
    case FOC_OP_SET_RATE_1_9:
      if (!readU8(u)) { commandError = CE_PARAM_FORM; break; }
      if (u < 1 || u > 9) { commandError = CE_PARAM_RANGE; break; }
      if (u < 5) setMoveRate(FOCUSER_CAN_INDEX, (int)u); else setGotoRate(FOCUSER_CAN_INDEX, (int)u - 4);
      numericReply = false;
    break;

    // :FW# -> i32 µm/s (NO unit-bit conversion per your clarification)
    case FOC_OP_GET_WORKRATE_W:
      writeI32(settings[FOCUSER_CAN_INDEX].gotoRate);
      numericReply = false;
    break;

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
    case FOC_OP_GET_POS_G:
      // Mirror local: (instrumentSteps - tcfSteps) converted to units
      i = (int32_t)(axis4.getInstrumentCoordinateSteps() - tcfSteps[FOCUSER_CAN_INDEX]);
      writeI32(i*StepsToUnits);
      numericReply = false;
    break;

    // :FR[sn]# -> no reply text (relative)
    case FOC_OP_GOTO_REL_R:
      if (!readI32(i)) { commandError = CE_PARAM_FORM; break; }
      commandError = gotoTarget(FOCUSER_CAN_INDEX, target[FOCUSER_CAN_INDEX] + i*UnitsToSteps);
      numericReply = false;
    break;

    // :FS[n]# -> numeric 1/0 (absolute)
    case FOC_OP_GOTO_ABS_S:
      if (!readI32(i)) { commandError = CE_PARAM_FORM; break; }
      commandError = gotoTarget(FOCUSER_CAN_INDEX, i*UnitsToSteps);
      // numericReply stays true to match local :FS behavior
    break;

    // :FZ# -> no reply text
    case FOC_OP_ZERO_Z:
      commandError = resetTarget(FOCUSER_CAN_INDEX, 0);
      numericReply = false;
    break;

    // :FH# -> no reply text (set home)
    case FOC_OP_SET_HOME_H:
      commandError = resetTarget(FOCUSER_CAN_INDEX, lround(getHomePosition(FOCUSER_CAN_INDEX)*MicronsToSteps));
      numericReply = false;
    break;

    // :Fh# -> no reply text (goto home)
    case FOC_OP_GOTO_HOME_h:
      if (axis4.hasHomeSense())
        commandError = moveHome(FOCUSER_CAN_INDEX);
      else
        commandError = gotoTarget(FOCUSER_CAN_INDEX, lround(getHomePosition(FOCUSER_CAN_INDEX)*MicronsToSteps));
      numericReply = false;
    break;

    // :GXU[n]#
    case FOC_OP_DRIVER_STATUS: {
      DriverStatus status = axis4.motor->getDriverStatus();

      if (status.active) {
        uint8_t packedStatus = 0;
        packedStatus |= (status.standstill ? 1: 0);
        packedStatus |= (status.outputA.openLoad ? 1 : 0) << 1;
        packedStatus |= (status.outputB.openLoad ? 1 : 0) << 2;
        packedStatus |= (status.outputA.shortToGround ? 1 : 0) << 3;
        packedStatus |= (status.outputB.shortToGround ? 1 : 0) << 4;
        packedStatus |= (status.overTemperature ? 1 : 0) << 5;
        packedStatus |= (status.overTemperatureWarning ? 1 : 0) << 6;
        packedStatus |= (status.fault ? 1 : 0) << 7;
        writeU8(packedStatus);
        numericReply = false;
      } else commandError = CE_0;
    } break;

    default:
      handled = false;
      commandError = CE_CMD_UNKNOWN;
    break;
  }

  sendResponse(handled, suppressFrame, numericReply, commandError);
}

#endif
