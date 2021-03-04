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
#include "Mount.h"

void Mount::init() {

  transform.init();
  if (transform.mountType == GEM) meridianFlip = MF_ALWAYS;

  // get PEC ready
  initPec();

  // get the main axes ready
  axis1.init(1);
  axis2.init(2);

  // calculate base slew rate
  usPerStepBase = 1000000.0/((axis1.getStepsPerMeasure()/RAD_DEG_RATIO)*SLEW_RATE_BASE_DESIRED);
  if (usPerStepBase < usPerStepLowerLimit()) usPerStepBase = usPerStepLowerLimit()*2.0;

  // get misc settings from NV
  if (MiscSize < sizeof(Misc)) { DL("ERR: Mount::init(); MiscSize error NV subsystem writes disabled"); nv.readOnly(true); }
  nv.readBytes(NV_MOUNT_MISC_BASE, &misc, MiscSize);

  // get limit settings from NV
  if (LimitsSize < sizeof(Limits)) { DL("ERR: Mount::init(); LimitsSize error NV subsystem writes disabled"); nv.readOnly(true); }
  nv.readBytes(NV_LIMITS_BASE, &limits, LimitsSize);

  if (misc.usPerStepCurrent < usPerStepBase) misc.usPerStepCurrent = usPerStepBase;
  if (misc.usPerStepCurrent > 2048) misc.usPerStepCurrent = 2048;

  // startup state
  resetHome();

  #if TRACK_AUTOSTART == ON
    VLF("MSG: Mount starting tracking");
    setTrackingState(TS_SIDEREAL);
    trackingRate = hzToSidereal(SIDEREAL_RATE_HZ);
  #endif
  updateTrackingRates();

  // automatic movement for axis1 and axis2
  axis1.setTracking(true);
  axis2.setTracking(true);
}

void Mount::setTrackingState(TrackingState state) {
  if (trackingState == TS_NONE) { axis1.enable(true); axis2.enable(true); }
  trackingState = state;
  if (trackingState == TS_SIDEREAL) atHome = false;
}

void Mount::updatePosition() {
  current = transform.instrumentToMount(axis1.getInstrumentCoordinate(), axis2.getInstrumentCoordinate());
}

void Mount::updateTrackingRates() {
  if (transform.mountType != ALTAZM) {
    trackingRateAxis1 = trackingRate;
    if (rateCompensation != RC_REFR_BOTH && rateCompensation != RC_FULL_BOTH) trackingRateAxis2 = 0;
  }
  if (trackingState != TS_SIDEREAL || gotoState != GS_NONE) { trackingRateAxis1 = 0; trackingRateAxis2 = 0; }
  axis1.setFrequency(siderealToRad(trackingRateAxis1 + guideRateAxis1 + deltaRateAxis1 + gotoRateAxis1)*SIDEREAL_RATIO);
  axis2.setFrequency(siderealToRad(trackingRateAxis2 + guideRateAxis2 + deltaRateAxis2 + gotoRateAxis2)*SIDEREAL_RATIO);
}

// check for platform rate limit (lowest maxRate) in us units
double Mount::usPerStepLowerLimit() {
  // for example 16us, this basis is platform/clock-rate specific (for square wave)
  double r_us = HAL_MAXRATE_LOWER_LIMIT;
  
  // higher speed ISR code path?
  #if STEP_WAVE_FORM == PULSE || STEP_WAVE_FORM == DEDGE
    r_us=r_us/1.6; // about 1.6x faster than SQW mode in my testing
  #endif
  
  // on-the-fly mode switching used?
  #if MODE_SWITCH_BEFORE_SLEW == OFF
    // if this code is enabled, r_us == 27us
    if (axis1StepsGoto != 1 || axis2StepsGoto != 1) r_us *= 1.7;
  #endif

  // average required goto us rates for each axis with any micro-step mode switching applied,
  // if tracking in 32X mode using 4X for gotos (32/4 = 8,) that's an 8x lower true rate so 27/8 = 3.4 is allowed
  double r_us_axis1 = r_us/axis1.getStepsPerStepGoto();
  double r_us_axis2 = r_us/axis2.getStepsPerStepGoto();
  
  // average in axis2 step rate scaling for drives where the reduction ratio isn't equal
  // if Axis1 is 10000 step/deg & Axis2 is 20000 steps/deg, Axis2 needs to run 2x speed so we must slow down.
  // 3.4 on one axis and 6.8 on the other for an average of 5.1
  r_us = (r_us_axis1 + r_us_axis2/timerRateRatio)/2.0;
 
  // the timer granulaity can start to make for some very abrupt rate changes below 0.25us
  if (r_us < 0.25) { r_us = 0.25; DLF("WRN, usPerStepLowerLimit(): r_us exceeds design limit"); }

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
