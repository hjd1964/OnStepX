//--------------------------------------------------------------------------------------------------
// telescope mount control
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../lib/nv/NV.h"
extern NVS nv;
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../coordinates/Transform.h"
#include "../coordinates/Site.h"
#include "../motion/StepDrivers.h"
#include "../motion/Axis.h"
#include "../telescope/Telescope.h"
extern Telescope telescope;
#include "Mount.h"

inline void mountGuideWrapper() { telescope.mount.guidePoll(); }

void Mount::init() {

  transform.init();
  if (transform.mountType == GEM) meridianFlip = MF_ALWAYS;

  // get PEC ready
  #if AXIS1_PEC == ON
    initPec();
  #endif

  // get the main axes ready
  axis1.init(1);
  axis2.init(2);
  stepsPerSiderealSecondAxis1 = radToDeg(axis1.getStepsPerMeasure())/240.0;
  stepsPerCentisecondAxis1    = (stepsPerSiderealSecondAxis1*SIDEREAL_RATIO)/100.0;

  // get misc settings from NV
  if (MiscSize < sizeof(Misc)) { DL("ERR: Mount::init(); MiscSize error NV subsystem writes disabled"); nv.readOnly(true); }
  nv.readBytes(NV_MOUNT_MISC_BASE, &misc, MiscSize);

  // calculate base and current maximum step rates
  usPerStepBase = 1000000.0/((axis1.getStepsPerMeasure()/RAD_DEG_RATIO)*SLEW_RATE_BASE_DESIRED);
  if (usPerStepBase < usPerStepLowerLimit()) usPerStepBase = usPerStepLowerLimit()*2.0;
  if (misc.usPerStepCurrent < usPerStepBase) misc.usPerStepCurrent = usPerStepBase;
  if (misc.usPerStepCurrent > 2048) misc.usPerStepCurrent = 2048;

  // get limit settings from NV
  if (LimitsSize < sizeof(Limits)) { DL("ERR: Mount::init(); LimitsSize error NV subsystem writes disabled"); nv.readOnly(true); }
  nv.readBytes(NV_LIMITS_BASE, &limits, LimitsSize);

  // start guide monitor task
  VF("MSG: Mount, start guide monitor task... ");
  if (tasks.add(10, 0, true, 1, mountGuideWrapper, "MntGuid")) VL("success"); else VL("FAILED!");

  // startup state is reset and at home
  resetHome();

  // get tracking ready
  #if TRACK_AUTOSTART == ON
    VLF("MSG: Mount, starting tracking");
    setTrackingState(TS_SIDEREAL);
    trackingRate = hzToSidereal(SIDEREAL_RATE_HZ);
  #else
    setTrackingState(TS_NONE);
  #endif
  updateTrackingRates();

  // get slewing ready
  updateAccelerationRates();
}

void Mount::setTrackingState(TrackingState state) {
  trackingState = state;
  axis1.setTracking(true);
  axis2.setTracking(true);
  if (trackingState == TS_SIDEREAL) { axis1.enable(true); axis2.enable(true); atHome = false; }
}

void Mount::updatePosition() {
  current = transform.instrumentToMount(axis1.getInstrumentCoordinate(), axis2.getInstrumentCoordinate());
}

void Mount::updateTrackingRates() {
  if (transform.mountType != ALTAZM) {
    trackingRateAxis1 = trackingRate;
    if (rateCompensation != RC_REFR_BOTH && rateCompensation != RC_FULL_BOTH) trackingRateAxis2 = 0;
  }
  if (trackingState != TS_SIDEREAL || gotoState != GS_NONE) {
    trackingRateAxis1 = 0;
    trackingRateAxis2 = 0;
  } else {
    #ifdef HAL_FAST_PROCESSOR
      stepsPerCentisecondAxis1 = ((radToDeg(axis1.getStepsPerMeasure())/(trackingRateAxis1*240.0))*SIDEREAL_RATIO)/100.0;
    #endif
  }
  axis1.setFrequency(siderealToRad(trackingRateAxis1 + guideRateAxis1 + pecRateAxis1 + deltaRateAxis1)*SIDEREAL_RATIO);
  axis2.setFrequency(siderealToRad(trackingRateAxis2 + guideRateAxis2 + deltaRateAxis2)*SIDEREAL_RATIO);
}

void Mount::updateAccelerationRates() {
  double radsPerSecondMax = (1000000.0/misc.usPerStepCurrent)/axis1.getStepsPerMeasure();
  double radsPerSecondPerSecond = (degToRad(SLEW_ACCELERATION_DIST)/radsPerSecondMax) * 2.0;
  axis1.setSlewAccelerationRate(radsPerSecondPerSecond);
  radsPerSecondPerSecond = (degToRad(SLEW_RAPID_STOP_DIST)/radsPerSecondMax) * 2.0;
  axis1.setSlewAccelerationRateAbort(radsPerSecondPerSecond);

  radsPerSecondMax = (1000000.0/misc.usPerStepCurrent)/axis2.getStepsPerMeasure();
  radsPerSecondPerSecond = (degToRad(SLEW_ACCELERATION_DIST)/radsPerSecondMax) * 2.0;
  axis2.setSlewAccelerationRate(radsPerSecondPerSecond);
  radsPerSecondPerSecond = (degToRad(SLEW_RAPID_STOP_DIST)/radsPerSecondMax) * 2.0;
  axis2.setSlewAccelerationRateAbort(radsPerSecondPerSecond);
}

double Mount::usPerStepLowerLimit() {
  // basis is platform/clock-rate specific (for square wave)
  double r_us = HAL_MAXRATE_LOWER_LIMIT;
  
  // higher speed ISR code path?
  #if STEP_WAVE_FORM == PULSE || STEP_WAVE_FORM == DEDGE
    r_us /= 1.6;
  #endif
  
  // on-the-fly mode switching used?
  #if MODE_SWITCH_BEFORE_SLEW == OFF
    if (axis1StepsGoto != 1 || axis2StepsGoto != 1) r_us *= 1.7;
  #endif

  // average required goto us rates for each axis with any micro-step mode switching applied
  double r_us_axis1 = r_us/axis1.getStepsPerStepGoto();
  double r_us_axis2 = r_us/axis2.getStepsPerStepGoto();
  
  // average in axis2 step rate scaling for drives where the reduction ratio isn't equal
  r_us = (r_us_axis1 + r_us_axis2/timerRateRatio)/2.0;
 
  // the timer granulaity can start to make for some very abrupt rate changes below 0.25us
  if (r_us < 0.25) { r_us = 0.25; DLF("WRN, Mount::usPerStepLowerLimit(): r_us exceeds design limit"); }

  // return rate in us units
  return r_us;
}

bool Mount::anyError() {
  return error.altitude.minExceeded ||
         error.altitude.maxExceeded ||
         error.meridian.eastExceeded ||
         error.meridian.westExceeded ||
         error.parkFailed;
}

void Mount::resetErrors() {
  error.altitude.minExceeded = false;
  error.altitude.minExceeded = false;
  error.meridian.eastExceeded = false;
  error.meridian.westExceeded = false;
  error.parkFailed = false;
}

#endif
