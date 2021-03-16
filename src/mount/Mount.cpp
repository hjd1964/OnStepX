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
inline void mountTrackingWrapper() { telescope.mount.trackPoll(); }

void Mount::init() {

  transform.init();
  if (transform.mountType == GEM) meridianFlip = MF_ALWAYS;

  // get PEC ready
  #if AXIS1_PEC == ON
    initPec();
  #endif

  // get parking ready
  parkInit();

  // get the main axes ready
  axis1.init(1);
  axis2.init(2);
  stepsPerSiderealSecondAxis1 = (axis1.getStepsPerMeasure()/RAD_DEG_RATIO_F)/240.0F;
  stepsPerCentisecondAxis1    = (stepsPerSiderealSecondAxis1*SIDEREAL_RATIO_F)/100.0F;

  // get limits ready
  limitInit();

  // get misc settings from NV
  if (MiscSize < sizeof(Misc)) { DL("ERR: Mount::init(); MiscSize error NV subsystem writes disabled"); nv.readOnly(true); }
  nv.readBytes(NV_MOUNT_MISC_BASE, &misc, MiscSize);
  axis1.setBacklash(misc.backlash.axis1);
  axis2.setBacklash(misc.backlash.axis2);

  // calculate base and current maximum step rates
  usPerStepBase = 1000000.0/((axis1.getStepsPerMeasure()/RAD_DEG_RATIO)*SLEW_RATE_BASE_DESIRED);
  if (usPerStepBase < usPerStepLowerLimit()) usPerStepBase = usPerStepLowerLimit()*2.0F;
  if (misc.usPerStepCurrent < usPerStepBase) misc.usPerStepCurrent = usPerStepBase;
  if (misc.usPerStepCurrent > 10000.0F) misc.usPerStepCurrent = 10000.0F;

  // start guide monitor task
  VF("MSG: Mount, start guide monitor task (rate 10ms priority 1)... ");
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

  // start tracking monitor task
  VF("MSG: Mount, start tracking monitor task (rate 1000ms priority 7)... ");
  if (tasks.add(1000, 0, true, 7, mountTrackingWrapper, "MntTrk")) VL("success"); else VL("FAILED!");

  updateTrackingRates();

  // get slewing ready
  updateAccelerationRates();
}

void Mount::setTrackingState(TrackingState state) {
  trackingState = state;
  axis1.setTracking(true);
  axis2.setTracking(true);
  if (trackingState == TS_SIDEREAL) {
    axis1.enable(true);
    axis2.enable(true);
    atHome = false;
    #if AXIS1_PEC == ON
      pecInit();
    #endif
  }
}

void Mount::updatePosition(CoordReturn coordReturn) {
  current = transform.instrumentToMount(axis1.getInstrumentCoordinate(), axis2.getInstrumentCoordinate());
  if (transform.mountType == ALTAZM) {
    if (coordReturn == CR_MOUNT_EQU) transform.horToEqu(&current);
  } else {
    if (coordReturn == CR_MOUNT_ALT) transform.equToAlt(&current); else
    if (coordReturn == CR_MOUNT_HOR) transform.equToHor(&current);
  }
}

void Mount::updateTrackingRates() {
  if (trackingState != TS_SIDEREAL || gotoState != GS_NONE) {
    trackingRateAxis1 = 0.0F;
    trackingRateAxis2 = 0.0F;
  }
  axis1.setFrequency(siderealToRadF(trackingRateAxis1 + guideRateAxis1 + pecRateAxis1)*SIDEREAL_RATIO_F);
  axis2.setFrequency(siderealToRadF(trackingRateAxis2 + guideRateAxis2)*SIDEREAL_RATIO_F);
}

#ifdef HAL_NO_DOUBLE_PRECISION
  #define DiffRange  0.0087266463F         // 30 arc-minutes in radians
  #define DiffRange2 0.017453292F          // 60 arc-minutes in radians
#else
  #define DiffRange  2.908882086657216e-4L // 1 arc-minute in radians
  #define DiffRange2 5.817764173314432e-4L // 2 arc-minutes in radians
#endif

void Mount::trackPoll() {
  if (trackingState != TS_SIDEREAL || (transform.mountType != ALTAZM && rateCompensation == RC_NONE)) {
    if (trackingState == TS_SIDEREAL) trackingRateAxis1 = trackingRate; else trackingRateAxis1 = 0.0F;
    trackingRateAxis2 = 0.0F;
    updateTrackingRates();
    return;
  }

  // get positions 1 (or 10) arc-min ahead and behind the current
  updatePosition(CR_MOUNT); Y;
  if (transform.mountType == ALTAZM) transform.horToEqu(&current); else transform.equToHor(&current); Y;
  Coordinate ahead = current;
  Coordinate behind = current;
  ahead.h += DiffRange;
  behind.h -= DiffRange;

  // apply (optional) pointing model and refraction
  if (rateCompensation == RC_FULL_RA || rateCompensation == RC_FULL_BOTH) {
    transform.mountToObservedPlace(&ahead); Y;
    transform.mountToObservedPlace(&behind); Y;
  }
  transform.topocentricToObservedPlace(&ahead); Y;
  transform.topocentricToObservedPlace(&behind); Y;

  // convert back into horizon coordinates
  float aheadAxis1, aheadAxis2, behindAxis1, behindAxis2;
  if (transform.mountType == ALTAZM) {
    transform.equToHor(&ahead); aheadAxis1 = ahead.z; aheadAxis2 = ahead.a; Y;
    transform.equToHor(&behind); behindAxis1 = behind.z; behindAxis2 = behind.a; Y;
  } else {
    aheadAxis1 = ahead.h; aheadAxis2 = ahead.d;
    behindAxis1 = behind.h; behindAxis2 = behind.d;
  }

  // calculate the Axis1 tracking rate
  if (aheadAxis1 < -Deg90 && behindAxis1 > Deg90) aheadAxis1 += Deg360;
  if (behindAxis1 < -Deg90 && aheadAxis1 > Deg90) behindAxis1 += Deg360;
  float rate1 = (aheadAxis1 - behindAxis1)/DiffRange2;
  if (fabs(trackingRateAxis1 - rate1) <= 0.005F)
    trackingRateAxis1 = (trackingRateAxis1*9.0F + rate1)/10.0F; else trackingRateAxis1 = rate1;

  // calculate the Axis2 Dec/Alt tracking rate (if dual axis or ALTAZM mode)
  if (rateCompensation == RC_REFR_BOTH || rateCompensation == RC_FULL_BOTH || transform.mountType == ALTAZM) {
    float rate2;
    rate2 = (aheadAxis2 - behindAxis2)/DiffRange2;
    if (current.pierSide == PIER_SIDE_WEST) rate2 = -rate2;
    if (fabs(trackingRateAxis2 - rate2) <= 0.005F) trackingRateAxis2 = (trackingRateAxis2*9.0F + rate2)/10.0F; else trackingRateAxis2 = rate2;
  } else trackingRateAxis2 = 0.0F;

  // override for special case of near a celestial pole
  if (fabs(current.d) > Deg85) { if (transform.mountType == ALTAZM) trackingRateAxis1 = 0.0F; else trackingRateAxis1 = trackingRate; trackingRateAxis2 = 0.0F; }

  // override for both rates for special case near the zenith
  if (current.a > Deg85) { if (transform.mountType == ALTAZM) trackingRateAxis1 = 0.0F; else trackingRateAxis1 = ztr(current.a); trackingRateAxis2 = 0.0F; }

  updateTrackingRates();
}

float Mount::ztr(float a) {
  if (a > degToRad(89.8F)) return 0.99998667F; else if (a > degToRad(89.5F)) return 0.99996667F;

  float altH = a + degToRad(0.25F); if (altH < 0.0F) altH = 0.0F;
  float altL = a - degToRad(0.25F); if (altL < 0.0F) altL = 0.0F;

  float altHr = altH - transform.trueRefrac(altH);
  float altLr = altL - transform.trueRefrac(altL);

  float r = (altH - altL)/(altHr - altLr); if (r > 1.0F) r = 1.0F;
  return r;
}

void Mount::updateAccelerationRates() {
  radsPerSecondCurrent = (1000000.0/misc.usPerStepCurrent)/axis1.getStepsPerMeasure();
  float secondsToAccelerate = (degToRad(SLEW_ACCELERATION_DIST)/radsPerSecondCurrent)*2.0;
  float radsPerSecondPerSecond = radsPerSecondCurrent/secondsToAccelerate;
  axis1.setSlewAccelerationRate(radsPerSecondPerSecond);
  axis1.setSlewAccelerationRateAbort(radsPerSecondPerSecond*2.0F);
  axis2.setSlewAccelerationRate(radsPerSecondPerSecond);
  axis2.setSlewAccelerationRateAbort(radsPerSecondPerSecond*2.0F);
}

float Mount::usPerStepLowerLimit() {
  // basis is platform/clock-rate specific (for square wave)
  float r_us = HAL_MAXRATE_LOWER_LIMIT;
  
  // higher speed ISR code path?
  #if STEP_WAVE_FORM == PULSE || STEP_WAVE_FORM == DEDGE
    r_us /= 1.6F;
  #endif
  
  // on-the-fly mode switching used?
  #if MODE_SWITCH_BEFORE_SLEW == OFF
    if (axis1StepsGoto != 1 || axis2StepsGoto != 1) r_us *= 1.7F;
  #endif

  // average required goto us rates for each axis with any micro-step mode switching applied
  float r_us_axis1 = r_us/axis1.getStepsPerStepGoto();
  float r_us_axis2 = r_us/axis2.getStepsPerStepGoto();
  
  // average in axis2 step rate scaling for drives where the reduction ratio isn't equal
  r_us = (r_us_axis1 + r_us_axis2/timerRateRatio)/2.0F;
 
  // the timer granulaity can start to make for some very abrupt rate changes below 0.25us
  if (r_us < 0.25F) { r_us = 0.25F; DLF("WRN, Mount::usPerStepLowerLimit(): r_us exceeds design limit"); }

  // return rate in us units
  return r_us;
}

CommandError Mount::alignAddStar() {
  if (alignState.currentStar > alignState.lastStar) return CE_PARAM_RANGE;

  CommandError e = CE_NONE;

  // first star, get ready for a new pointing model, init/sync then call gta.addStar 
  if (alignState.currentStar == 1) {
    #if ALIGN_MAX_STARS > 1  
      transform.align.init(transform.site.location.latitude, transform.mountType);
    #endif
    e = syncEqu(&gotoTarget, preferredPierSide);
  }

  // add an align star
  if (e == CE_NONE) {
    updatePosition(CR_MOUNT);
    #if ALIGN_MAX_STARS > 1  
      e = transform.align.addStar(alignState.currentStar, alignState.lastStar, &gotoTarget, &current);
    #endif
    if (e == CE_NONE) alignState.currentStar++;
  }

  return e;
}
#endif
