//--------------------------------------------------------------------------------------------------
// telescope mount control, guiding
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../coordinates/Transform.h"
#include "../commands/ProcessCmds.h"
#include "../motion/Axis.h"
#include "Mount.h"

double Mount::guideRateSelectToRate(GuideRateSelect guideRateSelect, uint8_t axis) {
  switch (guideRateSelect) {
    case GR_QUARTER: return 0.25;
    case GR_HALF: return 0.5;
    case GR_1X: return 1.0;
    case GR_2X: return 2.0;
    case GR_4X: return 4.0;
    case GR_8X: return 8.0;
    case GR_20X: return 20.0;
    case GR_48X: return 48.0;
    case GR_HALF_MAX: return (2000000.0/misc.usPerStepCurrent)/degToRad(axis1.getStepsPerMeasure());
    case GR_MAX: return (1000000.0/misc.usPerStepCurrent)/degToRad(axis1.getStepsPerMeasure());
    case GR_CUSTOM: if (axis == 1) return 48.0; else if (axis == 2) return 48.0; else return 0;
    default: return 0;
  }
}

bool Mount::validGuideAxis1(GuideAction guideAction) {
  if (!limitsEnabled) return true;
  updatePosition();
  if (guideAction == GA_FORWARD) {
    if (meridianFlip != MF_NEVER && current.pierSide == PIER_SIDE_EAST) { if (current.h < -limits.pastMeridianE) return false; }
    if (current.h < axis1.settings.limits.min) return false;
  }
  if (guideAction == GA_REVERSE) {
    if (meridianFlip != MF_NEVER && current.pierSide == PIER_SIDE_WEST) { if (current.h > limits.pastMeridianW) return false; }
    if (current.h > axis1.settings.limits.max) return false;
  }
  return true;
}

bool Mount::validGuideAxis2(GuideAction guideAction) {
  double a2;
  if (!limitsEnabled) return true;
  updatePosition();
  transform.equToHor(&current);
  if (guideAction == GA_FORWARD) {
    // if (AXIS2_TANGENT_ARM == ON) { cli(); a2=posAxis2/axis2Settings.stepsPerMeasure; sei(); } else 
    // if (AXIS2_TANGENT_ARM == ON) a2 = axis2.getMotorCoordinate(); else
    a2 = axis2.getInstrumentCoordinate();
    if (current.h < axis2.settings.limits.min && current.pierSide == PIER_SIDE_WEST) return false;
    if (a2 > axis2.settings.limits.max && current.pierSide == PIER_SIDE_EAST) return false;
    if (transform.mountType == ALTAZM && current.a > limits.altitude.max) return false;
  }
  if (guideAction == GA_REVERSE) {
    // if (AXIS2_TANGENT_ARM == ON) { cli(); a2=posAxis2/axis2Settings.stepsPerMeasure; sei(); } else 
    // if (AXIS2_TANGENT_ARM == ON) a2 = axis2.getMotorCoordinate(); else
    a2 = axis2.getInstrumentCoordinate();
    if (a2 < axis2.settings.limits.min && current.pierSide == PIER_SIDE_EAST) return false;
    if (a2 > axis2.settings.limits.max && current.pierSide == PIER_SIDE_WEST) return false;
    if (transform.mountType == ALTAZM && current.a < limits.altitude.min) return false;
  }
  return true;
}

CommandError Mount::startGuideAxis1(GuideAction guideAction, GuideRateSelect guideRateSelect, unsigned long guideTimeLimit) {
  if (guideAction == GA_NONE || guideActionAxis1 == guideAction) return CE_NONE;
  if (axis1.error.driverFault || axis1.error.motorFault) return CE_SLEW_ERR_HARDWARE_FAULT;
  if (parkState == PS_PARKED)          return CE_SLEW_ERR_IN_PARK;
  if (gotoState == GS_NONE)            return CE_SLEW_IN_MOTION;
  if (isSpiralGuiding())               return CE_SLEW_IN_MOTION;
  if (!validGuideAxis1(guideAction))   return CE_SLEW_ERR_OUTSIDE_LIMITS;
  if (guideRateSelect < 3) {
    if (anyError())                    return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (axis1.motionError())           return CE_SLEW_ERR_OUTSIDE_LIMITS;
  }

  guideActionAxis1 = guideAction;
  if (guideAction == GA_FORWARD) guideRateAxis1 =  guideRateSelectToRate(guideRateSelect); else
  if (guideAction == GA_REVERSE) guideRateAxis1 = -guideRateSelectToRate(guideRateSelect);
  guideFinishTimeAxis1 = millis() + guideTimeLimit;

  return CE_NONE;
}

CommandError Mount::startGuideAxis2(GuideAction guideAction, GuideRateSelect guideRateSelect, unsigned long guideTimeLimit) {
  if (guideAction == GA_NONE || guideActionAxis2 == guideAction) return CE_NONE;
  if (axis2.error.driverFault || axis2.error.motorFault) return CE_SLEW_ERR_HARDWARE_FAULT;
  if (parkState == PS_PARKED)          return CE_SLEW_ERR_IN_PARK;
  if (gotoState == GS_NONE)            return CE_SLEW_IN_MOTION;
  if (isSpiralGuiding())               return CE_SLEW_IN_MOTION;
  if (!validGuideAxis2(guideAction))   return CE_SLEW_ERR_OUTSIDE_LIMITS;
  if (guideRateSelect < 3) {
    if (anyError())                    return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (axis2.motionError())           return CE_SLEW_ERR_OUTSIDE_LIMITS;
  }

  guideActionAxis2 = guideAction;
  if (guideAction == GA_FORWARD) guideRateAxis2 =  guideRateSelectToRate(guideRateSelect); else
  if (guideAction == GA_REVERSE) guideRateAxis2 = -guideRateSelectToRate(guideRateSelect);
  guideFinishTimeAxis2 = millis() + guideTimeLimit;

  return CE_NONE;
}

void Mount::stopGuideAxis1() {
  if (guideActionAxis1 > GA_BREAK) guideActionAxis1 = GA_BREAK;
}

void Mount::stopGuideAxis2() {
  if (guideActionAxis2 > GA_BREAK) guideActionAxis2 = GA_BREAK;
}

bool Mount::isSpiralGuiding() {
  return false;  
}

#endif
