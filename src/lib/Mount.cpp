//--------------------------------------------------------------------------------------------------
// telescope mount control
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#if STEPS_PER_WORM_ROTATION == 0
  #define AXIS1_PEC ON
#else
  #define AXIS1_PEC OFF
#endif

#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;

#include "../coordinates/Transform.h"
#include "../coordinates/Site.h"
#include "../StepDrivers/StepDrivers.h"
#include "Axis.h"
#include "Mount.h"

void Mount::init() {
  transform.init();

  VF("MSG: Mount::init, type "); VL(transform.mountType);

  if (transform.mountType == GEM) meridianFlip = MF_ALWAYS;

  // get the main axes ready
  axis1.init(1);
  axis2.init(2);

  // set slew rate
  usPerStepBase = 1000000.0/((axis1.getStepsPerMeasure()/RAD_DEG_RATIO)*SLEW_RATE_BASE_DESIRED);
  if (usPerStepBase < usPerStepLowerLimit()) usPerStepBase = usPerStepLowerLimit()*2.0;
  usPerStepCurrent = usPerStepBase;

  radsPerCentisecond = (degToRad(15.0/3600.0)/100.0)*SIDEREAL_RATIO;

  // startup state
  resetHome();

  #if TRACK_AUTOSTART == ON
    VLF("MSG: Mount::init, starting tracking");
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

extern GeneralErrors generalErrors;

void Mount::resetGeneralErrors() {
  generalErrors.altitudeMin = false;
  generalErrors.limitSense = false;
  generalErrors.decMinMax = false;
  generalErrors.azmMinMax = false;
  generalErrors.raMinMax  = false;
  generalErrors.raMeridian = false;
  generalErrors.sync = false;
  generalErrors.altitudeMax = false;
  generalErrors.park = false;
}

#endif
