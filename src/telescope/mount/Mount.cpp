//--------------------------------------------------------------------------------------------------
// telescope mount control

#include "Mount.h"

#ifdef MOUNT_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;
#include "../Telescope.h"
#include "site/Site.h"

inline void mountGuideWrapper() { telescope.mount.guidePoll(); }
inline void mountTrackingWrapper() { telescope.mount.trackPoll(); }

void Mount::init(bool validKey) {

  transform.init(validKey);
  if (transform.mountType != ALTAZM) meridianFlip = MF_ALWAYS; else meridianFlip = MF_NEVER;

  // get library ready
  library.init(validKey);

  // get the main axes ready
  delay(1000);
  axis1.init(1, transform.mountType == ALTAZM, validKey);
  axis1.setMotionLimitsCheck(false);
  if (AXIS1_DRIVER_POWER_DOWN == ON) axis1.setPowerDownTime(DEFAULT_POWER_DOWN_TIME);

  stepsPerSiderealSecondAxis1 = (axis1.getStepsPerMeasure()/RAD_DEG_RATIO_F)/240.0F;
  stepsPerCentisecondAxis1 = (stepsPerSiderealSecondAxis1*SIDEREAL_RATIO_F)/100.0F;

  delay(1000);
  axis2.init(2, transform.mountType == ALTAZM, validKey);
  axis2.setMotionLimitsCheck(false);
  if (AXIS2_DRIVER_POWER_DOWN == ON) axis1.setPowerDownTime(DEFAULT_POWER_DOWN_TIME);

  // get limits ready
  limitInit(validKey);

  // write the default misc settings to NV
  if (!validKey) {
    VLF("MSG: Mount, writing default misc settings to NV");
    nv.writeBytes(NV_MOUNT_MISC_BASE, &misc, MiscSize);
  }

  // get misc settings from NV
  if (MiscSize < sizeof(Misc)) { initError.nv = true; DL("ERR: Mount::init(); MiscSize error NV subsystem writes disabled"); nv.readOnly(true); }
  nv.readBytes(NV_MOUNT_MISC_BASE, &misc, MiscSize);
  axis1.setBacklash(misc.backlash.axis1);
  axis2.setBacklash(misc.backlash.axis2);
  #if BUZZER_MEMORY == ON
    sound.enabled = misc.buzzer;
  #endif
  #if MFLIP_PAUSE_HOME_MEMORY != ON
    misc.meridianFlipPause = false;
  #endif
  #if MFLIP_AUTOMATIC_MEMORY != ON
    misc.meridianFlipAuto = false;
  #endif

  // calculate base and current maximum step rates
  usPerStepBase = 1000000.0/((axis1.getStepsPerMeasure()/RAD_DEG_RATIO)*SLEW_RATE_BASE_DESIRED);
  #if SLEW_RATE_MEMORY != ON
    misc.usPerStepCurrent = usPerStepBase;
  #endif
  if (usPerStepBase < usPerStepLowerLimit()) usPerStepBase = usPerStepLowerLimit()*2.0F;
  if (misc.usPerStepCurrent < usPerStepBase/2.0F) misc.usPerStepCurrent = usPerStepBase/2.0F;
  if (misc.usPerStepCurrent > 1000000.0F) misc.usPerStepCurrent = 1000000.0F;

  // start guide monitor task
  VF("MSG: Mount, start guide monitor task (rate 10ms priority 1)... ");
  if (tasks.add(10, 0, true, 1, mountGuideWrapper, "MntGuid")) { VL("success"); } else { VL("FAILED!"); }

  parkInit(validKey);

  #if ST4_INTERFACE == ON
    st4Init();
  #endif

  #if AXIS1_PEC == ON
    pecInit(validKey);
  #endif

  // startup state is reset and at home
  resetHome(false);

  // set tracking state
  #if TRACK_AUTOSTART == ON
    if (park.state == PS_PARKED) {
      VLF("MSG: Mount, parked autostart tracking ignored");
    } else {
      VLF("MSG: Mount, set tracking sidereal");
      setTrackingState(TS_SIDEREAL);
      trackingRate = hzToSidereal(SIDEREAL_RATE_HZ);
      if (!site.dateTimeReady()) {
        VLF("MSG: Mount, set date/time is unknown so limits are disabled");
        limitsEnabled = false;
      }
    }
  #else
    setTrackingState(TS_NONE);
    if (park.state == PS_PARKED) parkRestore(false);
  #endif

  // start tracking monitor task
  VF("MSG: Mount, start tracking monitor task (rate 1000ms priority 7)... ");
  if (tasks.add(1000, 0, true, 7, mountTrackingWrapper, "MntTrk")) { VL("success"); } else { VL("FAILED!"); }

  updateTrackingRates();

  // get slewing ready
  updateAccelerationRates();
}

void Mount::setTrackingState(TrackingState state) {
  trackingState = state;
  // bring up mount status LED
  statusInit();
  if (trackingState == TS_SIDEREAL) {
    axis1.enable(true);
    axis2.enable(true);
    atHome = false;
  }
}

void Mount::updatePosition(CoordReturn coordReturn) {
  current = transform.instrumentToMount(axis1.getInstrumentCoordinate(), axis2.getInstrumentCoordinate());
  if (transform.mountType == ALTAZM) {
    if (coordReturn == CR_MOUNT_EQU || coordReturn == CR_MOUNT_ALL) transform.horToEqu(&current);
  } else {
    if (coordReturn == CR_MOUNT_ALT) transform.equToAlt(&current); else
    if (coordReturn == CR_MOUNT_HOR || coordReturn == CR_MOUNT_ALL) transform.equToHor(&current);
  }
  if (atHome) current.pierSide = PIER_SIDE_NONE;
}

// get current equatorial position (Native coordinate system)
Coordinate Mount::getPosition() {
  updatePosition(CR_MOUNT_EQU);
  return transform.mountToNative(&current, false);
}

// get target equatorial position (Native coordinate system)
Coordinate Mount::getTarget() {
  return gotoTarget;
}

// set target equatorial position (Native coordinate system)
void Mount::setTarget(Coordinate *coords) {
  gotoTarget = *coords;
}

// goto target equatorial position
void Mount::requestGoto() {
  gotoEqu(&gotoTarget, preferredPierSide);
}

#define SS_STOPPED 0
#define SS_SLEWING 1

void Mount::updateTrackingRates() {
  static int lastSlewingStatus = 0;
  int slewingStatus = 0;

  if (gotoState == GS_NONE || guideState < GU_GUIDE) {
    if (trackingState != TS_SIDEREAL) {
      trackingRateAxis1 = 0.0F;
      trackingRateAxis2 = 0.0F;
    }

    float f1 = 0, f2 = 0;
    if (guideActionAxis1 == GA_NONE || guideState == GU_PULSE_GUIDE) {
      f1 = trackingRateAxis1 + guideRateAxis1 + pecRateAxis1;
      axis1.setFrequencyBase(siderealToRadF(f1)*SIDEREAL_RATIO_F);
    }
    if (guideActionAxis2 == GA_NONE || guideState == GU_PULSE_GUIDE) {
      f2 = trackingRateAxis2 + guideRateAxis2;
      axis2.setFrequencyBase(siderealToRadF(f2)*SIDEREAL_RATIO_F);
    }

    f1 = abs(f1); f2 = abs(f2); if (f2 > f1) f1 = f2;
    if (f1 < 0.20F) slewingStatus = SS_STOPPED; else
    if (f1 > 3.0F) slewingStatus = SS_SLEWING; else slewingStatus = 500.0F/f1;
  } else slewingStatus = SS_SLEWING;

  if (slewingStatus != lastSlewingStatus) {
    lastSlewingStatus = slewingStatus;
    // flash mount status LED porportional to tracking rate
    statusSetPeriodMillis(slewingStatus);
    // disable polling of the weather sensor during slews
    weather.disable = (slewingStatus == SS_SLEWING);
  }
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

  // get positions 1 (or 30) arc-min ahead and behind the current
  updatePosition(CR_MOUNT_ALL);
  double altitude = current.a;
  double declination = current.d;
//  if (transform.mountType == ALTAZM) transform.horToEqu(&current); else transform.equToHor(&current); Y;

  // on fast processors calculate true coordinate for a little more accuracy
  #ifndef HAL_SLOW_PROCESSOR
    transform.mountToTopocentric(&current);
    if (transform.mountType == ALTAZM) transform.horToEqu(&current);
  #endif

  Coordinate ahead = current;
  Coordinate behind = current;
  Y;
  ahead.h += DiffRange;
  behind.h -= DiffRange;

  // create horizon coordinates that would exist ahead and behind the current position
  if (transform.mountType == ALTAZM) {
    transform.equToHor(&ahead); Y;
    transform.equToHor(&behind); Y;
  }

  // apply (optional) pointing model and refraction
  if (rateCompensation == RC_FULL_RA || rateCompensation == RC_FULL_BOTH) {
    transform.topocentricToObservedPlace(&ahead); Y;
    transform.topocentricToObservedPlace(&behind); Y;
    transform.observedPlaceToMount(&ahead); Y;
    transform.observedPlaceToMount(&behind); Y;
  } else if (rateCompensation == RC_REFR_RA || rateCompensation == RC_REFR_BOTH) {
    transform.topocentricToObservedPlace(&ahead); Y;
    transform.topocentricToObservedPlace(&behind); Y;
  }

  // transfer to variables named appropriately for mount coordinates
  float aheadAxis1, aheadAxis2, behindAxis1, behindAxis2;
  if (transform.mountType == ALTAZM) {
    aheadAxis1 = ahead.z; aheadAxis2 = ahead.a;
    behindAxis1 = behind.z; behindAxis2 = behind.a;
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

  //char s[20];
  //sprintF(s,"%1.8f",trackingRateAxis1);
  //D("TR Axis1 = "); DL(s);
  //sprintF(s,"%1.8f",trackingRateAxis2);
  //D("TR Axis2 = "); DL(s);
  
  // override for special case of near a celestial pole
  if (fabs(declination) > Deg85) { if (transform.mountType == ALTAZM) trackingRateAxis1 = 0.0F; else trackingRateAxis1 = trackingRate; trackingRateAxis2 = 0.0F; }

  // override for both rates for special case near the zenith
  if (altitude > Deg85) { if (transform.mountType == ALTAZM) trackingRateAxis1 = 0.0F; else trackingRateAxis1 = ztr(current.a); trackingRateAxis2 = 0.0F; }

  updateTrackingRates();
}

float Mount::ztr(float a) {
  if (a > degToRadF(89.8F)) return 0.99998667F; else if (a > degToRadF(89.5F)) return 0.99996667F;

  float altH = a + degToRadF(0.25F); if (altH < 0.0F) altH = 0.0F;
  float altL = a - degToRadF(0.25F); if (altL < 0.0F) altL = 0.0F;

  float altHr = altH - transform.trueRefrac(altH);
  float altLr = altL - transform.trueRefrac(altL);

  float r = (altH - altL)/(altHr - altLr); if (r > 1.0F) r = 1.0F;
  return r;
}

void Mount::updateAccelerationRates() {
  radsPerSecondCurrent = (1000000.0F/misc.usPerStepCurrent)/(float)axis1.getStepsPerMeasure();
  float secondsToAccelerate = (degToRadF((float)(SLEW_ACCELERATION_DIST))/radsPerSecondCurrent)*2.0F;
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
  float r_us_axis1 = r_us/axis1.getStepsPerStepSlewing();
  float r_us_axis2 = r_us/axis2.getStepsPerStepSlewing();
  
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
    #if ALIGN_MAX_NUM_STARS > 1  
      transform.align.init(site.location.latitude, transform.mountType);
    #endif
    e = syncEqu(&gotoTarget, preferredPierSide);
  }

  // add an align star
  if (e == CE_NONE) {
    updatePosition(CR_MOUNT);
    #if ALIGN_MAX_NUM_STARS > 1  
      e = transform.align.addStar(alignState.currentStar, alignState.lastStar, &gotoTarget, &current);
    #endif
    if (e == CE_NONE) alignState.currentStar++;
  }

  return e;
}
#endif
