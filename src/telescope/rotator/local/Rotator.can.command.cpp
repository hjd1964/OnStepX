//--------------------------------------------------------------------------------------------------
// local telescope rotator control, using the CANbus interface

#include "Rotator.h"

#ifdef ROTATOR_CAN_SERVER_PRESENT

#include "../../../lib/convert/Convert.h"
#include "../../../lib/canTransport/CanPayload.h"
#include "../../../lib/axis/Axis.h"

#include "../../mount/Mount.h"

extern Axis axis3;

void Rotator::processCommand(const uint8_t data[8], uint8_t len) {
  const uint8_t tidop = data[0];
  const uint8_t op    = (uint8_t)(tidop & 0x1F);

  uint8_t payload[6] = {0};
  uint8_t payloadLen = 0;

  const uint8_t argLen = (len > 1) ? (uint8_t)(len - 1) : 0;
  CanPayload args(&data[1], argLen);

  bool handled        = true;
  bool numericReply   = true;
  bool suppressFrame  = true;
  CommandError commandError = CE_NONE;

  switch (op) {

    // :hP#
    case ROT_OP_PARK_HP: {
      CommandError e = park();
      commandError = (e == CE_NONE) ? CE_1 : e;
      // numericReply=true, suppressFrame=true (defaults)
    } break;

    // :hR#
    case ROT_OP_UNPARK_HR: {
      CommandError e = unpark();
      commandError = (e == CE_NONE) ? CE_1 : e;
      // numericReply=true, suppressFrame=true (defaults)
    } break;

    // :rA#
    case ROT_OP_ACTIVE_RA: {
      commandError = CE_1;
      // numericReply=true, suppressFrame=true (defaults)
    } break;

    // :rT#
    case ROT_OP_GET_STATUS_T: {
      // "M" or "S" + optional "D" + optional "R" + rateDigit
      payload[payloadLen++] = axis3.isSlewing() ? (uint8_t)'M' : (uint8_t)'S';
      if (!axis3.isSlewing()) {
        if (derotatorEnabled) payload[payloadLen++] = (uint8_t)'D';
        if (derotatorReverse) payload[payloadLen++] = (uint8_t)'R';
      }
      payload[payloadLen++] = (uint8_t)('0' + getGotoRate());

      numericReply  = false;
      suppressFrame = false;
    } break;

    // :rI#
    case ROT_OP_GET_MIN_I: {
      const int32_t deg = (int32_t)lround(axis3.getLimitMin());
      CanPayload out(payload, sizeof(payload));
      out.writeI32LE(deg);
      payloadLen = out.offset(); // 4
      numericReply  = false;
      suppressFrame = false;
    } break;

    // :rM#
    case ROT_OP_GET_MAX_M: {
      const int32_t deg = (int32_t)lround(axis3.getLimitMax());
      CanPayload out(payload, sizeof(payload));
      out.writeI32LE(deg);
      payloadLen = out.offset(); // 4
      numericReply  = false;
      suppressFrame = false;
    } break;

    // :rD#
    case ROT_OP_GET_DEG_PERSTEP_D: {
      const float degPerStep = (float)(1.0 / axis3.getStepsPerMeasure());
      CanPayload out(payload, sizeof(payload));
      out.writeF32LE(degPerStep);
      payloadLen = out.offset(); // 4
      numericReply  = false;
      suppressFrame = false;
    } break;

    // :rb# or :rb[n]#
    case ROT_OP_BACKLASH_b: {
      // If a payload is present, this is a set; otherwise it's a get.
      if (argLen == 0) {
        const int16_t steps = (int16_t)lround(getBacklash());
        CanPayload out(payload, sizeof(payload));
        out.writeI16LE(steps);
        payloadLen = out.offset(); // 2
        numericReply  = false;
        suppressFrame = false;
      } else {
        int16_t steps = 0;
        if (!args.readI16LE(steps)) { commandError = CE_PARAM_FORM; break; }
        CommandError e = setBacklash((long)steps);
        commandError = (e == CE_NONE) ? CE_1 : e;
        // numericReply=true, suppressFrame=true (defaults)
      }
    } break;

    // :rQ#
    case ROT_OP_STOP_Q:
      if (axis3.isHoming()) { axis3.autoSlewAbort(); homing = false; }
      else axis3.autoSlewStop();
      numericReply = false;
    break;

    // :r[1..9]#
    case ROT_OP_SET_RATE_1_9: {
      uint8_t digit = 0;
      if (!args.readU8(digit)) { commandError = CE_PARAM_FORM; break; }
      if (digit < 1 || digit > 9) { commandError = CE_PARAM_RANGE; break; }
      if (digit < 5) setMoveRate((int)digit);
      else setGotoRate((int)digit - 4);
      numericReply = false;
    } break;

    // :rW#
    case ROT_OP_GET_WORKRATE_W: {
      const float dps = (float)settings.gotoRate;
      CanPayload out(payload, sizeof(payload));
      out.writeF32LE(dps);
      payloadLen = out.offset(); // 4
      numericReply  = false;
      suppressFrame = false;
    } break;

    // :rc# (ignored/no-op)
    case ROT_OP_CONTINUOUS_c:
      numericReply = false;
    break;

    // :r>#
    case ROT_OP_MOVE_CW_GT:
      commandError = move(DIR_FORWARD);
      numericReply = false;
    break;

    // :r<#
    case ROT_OP_MOVE_CCW_LT:
      commandError = move(DIR_REVERSE);
      numericReply = false;
    break;

    // :rG#
    case ROT_OP_GET_ANGLE_G: {
      const float deg = (float)axis3.getInstrumentCoordinate();
      CanPayload out(payload, sizeof(payload));
      out.writeF32LE(deg);
      payloadLen = out.offset(); // 4
      numericReply  = false;
      suppressFrame = false;
    } break;

    // :rr[deg]#
    case ROT_OP_GOTO_REL_rr: {
      float rel = 0.0F;
      if (!args.readF32LE(rel)) { commandError = CE_PARAM_FORM; break; }
      const double t = axis3.getTargetCoordinate();
      commandError = gotoTarget((double)t + (double)rel);
      numericReply = false;
    } break;

    // :rS[deg]#
    case ROT_OP_GOTO_S: {
      float deg = 0.0F;
      if (!args.readF32LE(deg)) { commandError = CE_PARAM_FORM; break; }
      commandError = gotoTarget(deg);
      // numericReply=true, suppressFrame=true (defaults) for CE_1 style response if you set it elsewhere
    } break;

    // :rZ#
    case ROT_OP_ZERO_Z:
      settings.parkState = PS_UNPARKED;
      commandError = axis3.resetPosition(0.0);
      axis3.setBacklashSteps(getBacklash());
      numericReply = false;
    break;

    // :rF#
    case ROT_OP_HALFTRAVEL_F:
      settings.parkState = PS_UNPARKED;
      commandError = axis3.resetPosition((axis3.getLimitMax() + axis3.getLimitMin())/2.0F);
      axis3.setBacklashSteps(getBacklash());
      numericReply = false;
    break;

    // :rC#
    case ROT_OP_HALFTARGET_C: {
      if (AXIS3_SENSE_HOME != OFF) {
        if (settings.parkState == PS_UNPARKED) {
          axis3.setFrequencySlew(settings.gotoRate);
          commandError = axis3.autoSlewHome();
          if (commandError == CE_NONE) homing = true;
        } else commandError = CE_PARKED;
      } else {
        commandError = gotoTarget((axis3.getLimitMax() + axis3.getLimitMin())/2.0F);
      }
      numericReply = false;
    } break;

    // :r+#
    case ROT_OP_DEROT_EN_PLUS:
      #ifdef MOUNT_PRESENT
        if (transform.mountType == ALTAZM) {
          if (settings.parkState == PS_UNPARKED) derotatorEnabled = true;
          else commandError = CE_PARKED;
        }
      #endif
      numericReply = false;
    break;

    // :r-#
    case ROT_OP_DEROT_DIS_MINUS:
      derotatorEnabled = false;
      axis3.setSynchronizedFrequency(0.0F);
      numericReply = false;
    break;

    // :rP#
    case ROT_OP_GOTO_PAR_P:
      #ifdef MOUNT_PRESENT
        {
          Coordinate current = mount.getPosition();
          commandError = gotoTarget(parallacticAngle(&current));
        }
      #endif
      numericReply = false;
    break;

    // :rR#
    case ROT_OP_DEROT_REV_R:
      derotatorReverse = !derotatorReverse;
      numericReply = false;
    break;

    // :GX98#
    case ROT_OP_AVAIL_GX98: {
      uint8_t c = (uint8_t)'N';
      #if defined(MOUNT_PRESENT)
        if (transform.mountType == ALTAZM) c = (uint8_t)'D';
        else c = (uint8_t)'R';
      #else
        c = (uint8_t)'R';
      #endif

      payload[payloadLen++] = c;
      numericReply  = false;
      suppressFrame = false;
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
