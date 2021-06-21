//--------------------------------------------------------------------------------------------------
// telescope mount control, guiding

#include "../Common.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../commands/ProcessCmds.h"
#include "Mount.h"

float Mount::limitGuideRate(float rate) {
  if (rate > radToDeg(radsPerSecondCurrent)*120.0F) rate = radToDeg(radsPerSecondCurrent)*120.0F;
  return rate;
}

float Mount::guideRateSelectToRate(GuideRateSelect guideRateSelect, uint8_t axis) {
  switch (guideRateSelect) {
    case GR_QUARTER: return 0.25F;
    case GR_HALF: return 0.5F;
    case GR_1X: return 1.0F;
    case GR_2X: return 2.0F;
    case GR_4X: return limitGuideRate(4.0F);
    case GR_8X: return limitGuideRate(8.0F);
    case GR_20X: return limitGuideRate(20.0F);
    case GR_48X: return limitGuideRate(48.0F);
    case GR_HALF_MAX: return radToDeg(radsPerSecondCurrent)*120.0F;
    case GR_MAX: return radToDeg(radsPerSecondCurrent)*240.0F;
    case GR_CUSTOM: if (axis == 1) return customGuideRateAxis1; else if (axis == 2) return customGuideRateAxis2; else return 0.0F;
    default: return 0.0F;
  }
}

bool Mount::guideValidAxis1(GuideAction guideAction) {
  if (!limitsEnabled) return true;

  double a1;
  if (transform.mountType == ALTAZM) a1 = current.z; else a1 = current.h;

  if (guideAction == GA_REVERSE || guideAction == GA_SPIRAL) {
    if (meridianFlip != MF_NEVER && current.pierSide == PIER_SIDE_EAST) { if (current.h < -limits.pastMeridianE) return false; }
    if (a1 < axis1.settings.limits.min) return false;
  }
  if (guideAction == GA_FORWARD || guideAction == GA_SPIRAL) {
    if (meridianFlip != MF_NEVER && current.pierSide == PIER_SIDE_WEST) { if (current.h > limits.pastMeridianW) return false; }
    if (a1 > axis1.settings.limits.max) return false;
  }
  return true;
}

bool Mount::guideValidAxis2(GuideAction guideAction) {
  if (!limitsEnabled) return true;

  if (guideAction == GA_REVERSE || guideAction == GA_SPIRAL) {
    if (current.pierSide == PIER_SIDE_WEST) {
      if (current.a2 > axis2.settings.limits.max) return false;
    } else {
      if (current.a2 < axis2.settings.limits.min) return false;
    }
  }
  if (guideAction == GA_FORWARD || guideAction == GA_SPIRAL) {
    if (current.pierSide == PIER_SIDE_WEST) {
      if (current.a2 < axis2.settings.limits.min) return false;
    } else {
      if (current.a2 > axis2.settings.limits.max) return false;
    }
  }
  if (guideAction == GA_SPIRAL) {
    if (abs(current.a2) > degToRad(75.0)) return false;
  }
  return true;
}

CommandError Mount::guideValidate(int axis, GuideAction guideAction) {
  if (!axis1.isEnabled() || !axis2.isEnabled())                         return CE_SLEW_ERR_IN_STANDBY;
  if (axis1.driver.getStatus().fault || axis2.driver.getStatus().fault) return CE_SLEW_ERR_HARDWARE_FAULT;
  if (park.state == PS_PARKED)                                          return CE_SLEW_ERR_IN_PARK;
  if (gotoState != GS_NONE)                                             return CE_SLEW_IN_MOTION;
  if (guideAction == GA_SPIRAL && axis1.autoSlewActive())               return CE_SLEW_IN_MOTION;
  if (guideAction == GA_SPIRAL && axis2.autoSlewActive())               return CE_SLEW_IN_MOTION;
  updatePosition(CR_MOUNT_ALT);
  if (axis == 1 || guideAction == GA_SPIRAL) {
    if (!guideValidAxis1(guideAction)) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (guideRateSelect < 3) {
      if (errorAny() || axis1.motionError()) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    }
  }
  if (axis == 2 || guideAction == GA_SPIRAL) {
    if (!guideValidAxis2(guideAction)) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (guideRateSelect < 3) {
      if (errorAny() || axis2.motionError()) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    }
  }
  return CE_NONE;
}

CommandError Mount::guideStartAxis1(GuideAction guideAction, GuideRateSelect guideRateSelect, unsigned long guideTimeLimit) {
  if (guideAction == GA_NONE || guideActionAxis1 == guideAction) return CE_NONE;
  CommandError e = guideValidate(1, guideAction); if (e != CE_NONE) return e;

  guideActionAxis1 = guideAction;
  double rate = guideRateSelectToRate(guideRateSelect);

  VF("MSG: guideStartAxis1(); guide ");
  if (guideAction == GA_REVERSE) { VF("reverse"); } else { VF("forward"); }
  VF(" started at "); V(rate); VL("X");

  // unlimited 0 means the maximum period, about 49 days
  if (guideTimeLimit == 0) guideTimeLimit = 0x1FFFFFFF;
  guideFinishTimeAxis1 = millis() + guideTimeLimit;

  if (rate <= 2) {
    guideState = GU_PULSE_GUIDE;
    if (guideAction == GA_REVERSE) guideRateAxis1 = -rate; else guideRateAxis1 = rate;
    updateTrackingRates();
  } else {
    guideState = GU_GUIDE;
    axis1.setFrequencyMax(degToRad(rate/240.0));
    guideAxis1AutoSlew(guideAction);
  }

  return CE_NONE;
}

void Mount::guideStopAxis1(GuideAction stopDirection) {
  if (guideActionAxis1 > GA_BREAK) {
    if (stopDirection != GA_BREAK && stopDirection != guideActionAxis1) return;
    if (guideRateAxis1 == 0.0F) {
      VLF("MSG: guideStopAxis1(); requesting guide stop");
      guideActionAxis1 = GA_BREAK;
      axis1.autoSlewStop();
    } else {
      VLF("MSG: guideStopAxis1(); guide stopped");
      guideActionAxis1 = GA_NONE;
      guideRateAxis1 = 0.0F;
      updateTrackingRates();
    }
  }
}

CommandError Mount::guideStartAxis2(GuideAction guideAction, GuideRateSelect guideRateSelect, unsigned long guideTimeLimit) {
  if (guideAction == GA_NONE || guideActionAxis2 == guideAction) return CE_NONE;
  CommandError e = guideValidate(1, guideAction); if (e != CE_NONE) return e;

  guideActionAxis2 = guideAction;
  double rate = guideRateSelectToRate(guideRateSelect);

  VF("MSG: guideStartAxis2(); guide ");
  if (guideAction == GA_REVERSE) { VF("reverse"); } else { VF("forward"); }
  VF(" started at "); V(rate); VL("X");

  // unlimited 0 means the maximum period, about 49 days
  if (guideTimeLimit == 0) guideTimeLimit = 0x1FFFFFFF;
  guideFinishTimeAxis2 = millis() + guideTimeLimit;

  if (rate <= 2) {
    guideState = GU_PULSE_GUIDE;
    if (guideAction == GA_REVERSE) guideRateAxis2 = -rate; else guideRateAxis2 = rate;
    updateTrackingRates();
  } else {
    guideState = GU_GUIDE;
    axis2.setFrequencyMax(degToRad(rate/240.0F));
    updatePosition(CR_MOUNT);
    guideAxis2AutoSlew(guideAction);
  }

  return CE_NONE;
}

void Mount::guideStopAxis2(GuideAction stopDirection) {
  if (guideActionAxis2 > GA_BREAK) {
    if (stopDirection != GA_BREAK && stopDirection != guideActionAxis2) return;
    if (guideRateAxis2 == 0.0F) {
      VLF("MSG: guideStopAxis2(); requesting guide stop");
      guideActionAxis2 = GA_BREAK;
      axis2.autoSlewStop();
    } else {
      VLF("MSG: guideStopAxis2(); guide stopped");
      guideActionAxis2 = GA_NONE;
      guideRateAxis2 = 0.0F;
      updateTrackingRates();
    }
  }
}

CommandError Mount::guideSpiralStart(GuideRateSelect guideRateSelect, unsigned long guideTimeLimit) {
  if (guideState == GU_SPIRAL_GUIDE) { guideSpiralStop(); return CE_NONE; }
  if (guideActionAxis1 != GA_NONE || guideActionAxis2 != GA_NONE) return CE_SLEW_IN_MOTION;
  CommandError e = guideValidate(0, GA_SPIRAL); if (e != CE_NONE) return e;

  if (guideRateSelect < GR_2X)       guideRateSelect = GR_2X;
  if (guideRateSelect > GR_HALF_MAX) guideRateSelect = GR_HALF_MAX;
  this->guideRateSelect = guideRateSelect;

  VF("MSG: guideSpiralStart(); using guide rates to "); V(guideRateSelectToRate(guideRateSelect)); VL("X");

  // unlimited 0 means the maximum period, about 49 days
  if (guideTimeLimit == 0) guideTimeLimit = 0x1FFFFFFF;
  spiralStartTime = millis();
  guideFinishTimeAxis1 = spiralStartTime + guideTimeLimit;
  guideFinishTimeAxis2 = guideFinishTimeAxis1;

  // setup and call the polling routine once to start the guides
  spiralScaleAxis1 = cos(current.a2);
  guideSpiralPoll();

  guideState = GU_SPIRAL_GUIDE;
  return CE_NONE;
}

void Mount::guideSpiralStop() {
  guideStopAxis1(GA_FORWARD);
  guideStopAxis1(GA_REVERSE);
  guideStopAxis2(GA_FORWARD);
  guideStopAxis2(GA_REVERSE);
}

// set guide spiral rates in RA/Azm and Dec/Alt, rate is in x-sidereal, guide elapsed time is in ms 
void Mount::guideSpiralPoll() {
  // current elapsed time in seconds
  float T = ((long)(millis() - spiralStartTime))/1000.0;

  // actual rate we'll be using (in sidereal X)
  float rate = guideRateSelectToRate(guideRateSelect);
  float maxRate = guideRateSelectToRate(GR_MAX);
  if (rate > maxRate/2.0) rate = maxRate/2.0;

  // apparaent FOV (in arc-seconds) = rate*15.0*2.0;
  // current radius assuming movement at 2 seconds per fov
  double radius = pow(T/6.28318, 1.0/1.74);

  // current angle in radians
  float angle = (radius - trunc(radius))*6.28318;

  // calculate the Axis rates for this moment (in sidereal X)
  customGuideRateAxis1 = rate*cos(angle);
  customGuideRateAxis2 = rate*sin(angle);

  // set any new directions
  guideActionAxis1 = GA_FORWARD;
  guideActionAxis2 = GA_FORWARD;
  if (customGuideRateAxis1 < 0) { customGuideRateAxis1 = fabs(customGuideRateAxis1); guideActionAxis1 = GA_REVERSE; }
  if (customGuideRateAxis2 < 0) { customGuideRateAxis2 = fabs(customGuideRateAxis2); guideActionAxis2 = GA_REVERSE; }
  guideAxis1AutoSlew(guideActionAxis1);
  guideAxis2AutoSlew(guideActionAxis2);

  // adjust Axis1 rate due to spherical coordinates and limit it to rates we can reach.  worst case the
  // shape of the spiral will degrade to an 2:1 aspect oval at half max rate (fastest allowed) and |Axis2| = 75°
  customGuideRateAxis1 /= spiralScaleAxis1;
  if (customGuideRateAxis1 > maxRate) customGuideRateAxis1 = maxRate;

  // set the new guide rates
  axis1.setFrequencyMax(siderealToRadF(customGuideRateAxis1));
  axis2.setFrequencyMax(siderealToRadF(customGuideRateAxis2));
}

void Mount::guidePoll() {
  // check fast guide completion axis1
  if (guideActionAxis1 == GA_BREAK && guideRateAxis1 == 0.0F && !axis1.autoSlewActive()) {
    guideActionAxis1 = GA_NONE;
    updateTrackingRates();
  } else { // check for guide timeout axis1
    if (guideActionAxis1 > GA_BREAK && (long)(millis() - guideFinishTimeAxis1) >= 0) guideStopAxis1(GA_BREAK);
  }

  // check fast guide completion axis2
  if (guideActionAxis2 == GA_BREAK && guideRateAxis2 == 0.0F && !axis2.autoSlewActive()) {
    guideActionAxis2 = GA_NONE;
    updateTrackingRates();
  } else { // check for guide timeout axis2
    if (guideActionAxis2 > GA_BREAK && (long)(millis() - guideFinishTimeAxis2) >= 0) guideStopAxis2(GA_BREAK);
  }

  // do spiral guiding
  if (guideState == GU_SPIRAL_GUIDE) {
    if (guideActionAxis1 > GA_BREAK && guideActionAxis2 > GA_BREAK) guideSpiralPoll(); else
    if (guideActionAxis1 != GA_BREAK || guideActionAxis2 != GA_BREAK) guideSpiralStop();
  }

  // watch for guides finished
  if (guideActionAxis1 == GA_NONE && guideActionAxis2 == GA_NONE) guideState = GU_NONE;
}

void Mount::guideAxis1AutoSlew(GuideAction guideAction) {
  if (guideAction == GA_REVERSE) axis1.autoSlew(DIR_REVERSE); else axis1.autoSlew(DIR_FORWARD);
}

void Mount::guideAxis2AutoSlew(GuideAction guideAction) {
  if (current.pierSide == PIER_SIDE_WEST) {
    if (guideAction == GA_REVERSE) axis2.autoSlew(DIR_FORWARD); else axis2.autoSlew(DIR_REVERSE);
  } else {
    if (guideAction == GA_REVERSE) axis2.autoSlew(DIR_REVERSE); else axis2.autoSlew(DIR_FORWARD);
  }
}

#endif
