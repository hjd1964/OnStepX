//--------------------------------------------------------------------------------------------------
// telescope mount control

#include "Mount.h"

#ifdef MOUNT_PRESENT

#include "../../lib/tasks/OnTask.h"

#include "../Telescope.h"
#include "coordinates/Transform.h"
#include "goto/Goto.h"
#include "guide/Guide.h"
#include "home/Home.h"
#include "library/Library.h"
#include "limits/Limits.h"
#include "park/Park.h"
#include "pec/Pec.h"
#include "site/Site.h"
#include "st4/St4.h"
#include "status/Status.h"

inline void mountWrapper() { mount.poll(); }

void Mount::init() {
  // confirm the data structure size
  if (MountSettingsSize < sizeof(MountSettings)) { nv.initError = true; DL("ERR: Mount::init(), MountSettingsSize error"); }

  // write the default settings to NV
  if (!nv.hasValidKey()) {
    VLF("MSG: Mount, writing defaults to NV");
    nv.writeBytes(NV_MOUNT_SETTINGS_BASE, &settings, sizeof(MountSettings));
  }

  // read the settings
  nv.readBytes(NV_MOUNT_SETTINGS_BASE, &settings, sizeof(MountSettings));

  // get the main axes ready
  delay(100);
  if (!axis1.init(&motor1)) { initError.driver = true; DLF("ERR: Axis1, no motion controller!"); }
  axis1.setBacklash(settings.backlash.axis1);
  axis1.setMotionLimitsCheck(false);
  if (AXIS1_POWER_DOWN == ON) axis1.setPowerDownTime(AXIS1_POWER_DOWN_TIME);

  delay(100);
  if (!axis2.init(&motor2)) { initError.driver = true; DLF("ERR: Axis2, no motion controller!"); }
  axis2.setBacklash(settings.backlash.axis2);
  axis2.setMotionLimitsCheck(false);
  if (AXIS2_POWER_DOWN == ON) axis2.setPowerDownTime(AXIS2_POWER_DOWN_TIME);
}

void Mount::begin() {
  axis1.calibrate();
  axis1.enable(MOUNT_ENABLE_IN_STANDBY == ON);
  axis2.calibrate();
  axis2.enable(MOUNT_ENABLE_IN_STANDBY == ON);

  // initialize the critical subsystems
  site.init();
  transform.init();

  // setup compensated tracking as configured
  if (TRACK_COMPENSATION_MEMORY == OFF) settings.rc = RC_DEFAULT;
  if (transform.mountType == ALTAZM) {
    if (settings.rc == RC_MODEL) settings.rc = RC_MODEL_DUAL;
    if (settings.rc == RC_REFRACTION) settings.rc = RC_REFRACTION_DUAL;
  }

  // initialize the other subsystems
  home.init();
  home.reset();
  limits.init();
  guide.init();

  if (transform.mountType == FORK) {
    limits.settings.pastMeridianE = Deg360;
    limits.settings.pastMeridianW = Deg360;
  }

  if (AXIS1_WRAP == ON) {
    axis1.coordinateWrap(Deg360);
    axis1.settings.limits.min = -Deg360;
    axis1.settings.limits.max = Deg360;
    limits.settings.pastMeridianE = Deg360;
    limits.settings.pastMeridianW = Deg360;
  }

  goTo.init();
  library.init();
  park.init();

  #if AXIS1_PEC == ON
    pec.init();
  #endif

  #if ST4_INTERFACE == ON
    st4.init();
  #endif

  #if TRACK_AUTOSTART == ON
    if (park.state == PS_PARKED) {
      #if GOTO_FEATURE == ON
        if (site.isDateTimeReady()) {
          VLF("MSG: Mount, autostart tracking from park");
          park.restore(true);
        } else {
          VLF("MSG: Mount, autostart tracking from park requires date/time");
        }
      #endif
    } else {
      if (transform.mountType != ALTAZM || site.isDateTimeReady()) {
        VLF("MSG: Mount, autostart tracking sidereal");
        tracking(true);
        trackingRate = hzToSidereal(SIDEREAL_RATE_HZ);
      } else {
        VLF("MSG: Mount, can't autostart ALTAZM tracking without date/time");
      }
    }
  #else
    tracking(false);
    #if GOTO_FEATURE == ON
      if (park.state == PS_PARKED) park.restore(false);
    #endif
  #endif

  #if ALIGN_MAX_NUM_STARS > 1 && ALIGN_MODEL_MEMORY == ON
    transform.align.modelRead();
  #endif

  VF("MSG: Mount, start tracking monitor task (rate 1000ms priority 6)... ");
  if (tasks.add(1000, 0, true, 6, mountWrapper, "MntTrk")) { VLF("success"); } else { VLF("FAILED!"); }

  update();
}

// get current equatorial position (Native coordinate system)
Coordinate Mount::getPosition(CoordReturn coordReturn) {
  updatePosition(coordReturn);
  return transform.mountToNative(&current, false);
}

// get current equatorial position (Mount coordinate system)
Coordinate Mount::getMountPosition(CoordReturn coordReturn) {
  updatePosition(coordReturn);
  return current;
}

// enables or disables tracking, enabling tracking powers on the motors if necessary
void Mount::tracking(bool state) {
  if (state == true) {
    enable(state);
    if (isEnabled()) trackingState = TS_SIDEREAL;
  } else

  if (state == false) {
    trackingState = TS_NONE;
  }

  update();
}

// enables or disables power to the mount motors
// first enable starts the mount status indications
void Mount::enable(bool state) {
  static bool firstEnable = true;

  if (state == true) {
    #if LIMIT_STRICT == ON
      if (!site.dateIsReady || !site.timeIsReady) return;
    #endif

    if (firstEnable) { mountStatus.ready(); }

    firstEnable = false;
  } else {
    trackingState = TS_NONE;
    update();
  }

  axis1.enable(state);
  axis2.enable(state);
}

// allow syncing to the encoders instead of from them
void Mount::syncToEncoders(bool state) {
  syncToEncodersEnabled = state;
}

// updates the tracking rates, etc. as appropriate for the mount state
// called once a second by poll() but available here for immediate action
void Mount::update() {
  static int lastStatusFlashMs = 0;
  int statusFlashMs = 0;

  #if GOTO_FEATURE == ON
  if (goTo.state == GS_NONE && guide.state < GU_GUIDE) {
  #else
  if (guide.state < GU_GUIDE) {
  #endif
    if (trackingState != TS_SIDEREAL) {
      trackingRateAxis1 = 0.0F;
      trackingRateAxis2 = 0.0F;
    }

    float f1 = 0, f2 = 0;
    if (!guide.activeAxis1() || guide.state == GU_PULSE_GUIDE) {
      f1 = trackingRateAxis1 + guide.rateAxis1 + pec.rate;
      axis1.setFrequencyBase(siderealToRadF(f1)*SIDEREAL_RATIO_F*site.getSiderealRatio());
    }

    if (!guide.activeAxis2() || guide.state == GU_PULSE_GUIDE) {
      f2 = trackingRateAxis2 + guide.rateAxis2;
      axis2.setFrequencyBase(siderealToRadF(f2)*SIDEREAL_RATIO_F*site.getSiderealRatio());
    }

    f1 = fabs(f1);
    f2 = fabs(f2);
    if (f2 > f1) f1 = f2;
    if (f1 < 0.20F) statusFlashMs = SF_STOPPED; else
    if (f1 > 3.0F) statusFlashMs = SF_SLEWING; else statusFlashMs = 500.0F/f1;
  } else {
    statusFlashMs = SF_SLEWING;
    axis2.setFrequencyBase(0.0F);
  }

  if (statusFlashMs != lastStatusFlashMs) {
    lastStatusFlashMs = statusFlashMs;
    mountStatus.flashRate(statusFlashMs);
    xBusy = statusFlashMs == SF_SLEWING;
  }
}

void Mount::poll() {
  #ifdef HAL_NO_DOUBLE_PRECISION
    #define DiffRange  0.0087266463F         // 30 arc-minutes in radians
    #define DiffRange2 0.017453292F          // 60 arc-minutes in radians
  #else
    #define DiffRange  2.908882086657216e-4L // 1 arc-minute in radians
    #define DiffRange2 5.817764173314432e-4L // 2 arc-minutes in radians
  #endif

  if (trackingState == TS_NONE) {
    trackingRateAxis1 = 0.0F;
    trackingRateAxis2 = 0.0F;
    update();
    return;
  }

  if (transform.mountType != ALTAZM && settings.rc == RC_NONE && trackingRateOffsetRA == 0.0F && trackingRateOffsetDec == 0.0F) {
    trackingRateAxis1 = trackingRate;
    trackingRateAxis2 = 0.0F;
    update();
    return;
  }

  // get positions 1 (or 30) arc-min ahead and behind the current
  updatePosition(CR_MOUNT_ALL);
  double altitude = current.a;
  double declination = current.d;

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
  if (settings.rc == RC_MODEL || settings.rc == RC_MODEL_DUAL) {
    transform.topocentricToObservedPlace(&ahead); Y;
    transform.topocentricToObservedPlace(&behind); Y;
    transform.observedPlaceToMount(&ahead); Y;
    transform.observedPlaceToMount(&behind); Y;
  } else if (settings.rc == RC_REFRACTION || settings.rc == RC_REFRACTION_DUAL) {
    transform.topocentricToObservedPlace(&ahead); Y;
    transform.topocentricToObservedPlace(&behind); Y;
  }

  // drop the dual axis if not enabled
  if (settings.rc != RC_REFRACTION_DUAL && settings.rc != RC_MODEL_DUAL) { behind.d = ahead.d; }

  // apply tracking rate offset to equatorial coordinates
  float timeInSeconds = radToHrs(DiffRange)*3600.0F;
  float trackingRateOffsetRadsRA = siderealToRad(trackingRateOffsetRA)*timeInSeconds;
  float trackingRateOffsetRadsDec = siderealToRad(trackingRateOffsetDec)*timeInSeconds;
  ahead.h -= trackingRateOffsetRadsRA;
  behind.h += trackingRateOffsetRadsRA;
  ahead.d += trackingRateOffsetRadsDec;
  behind.d -= trackingRateOffsetRadsDec;

  // transfer to variables named appropriately for mount coordinates
  float aheadAxis1, aheadAxis2, behindAxis1, behindAxis2;
  if (transform.mountType == ALTAZM) {
    transform.equToHor(&ahead);
    aheadAxis1 = ahead.z;
    aheadAxis2 = ahead.a;
    behindAxis1 = behind.z;
    behindAxis2 = behind.a;
  } else {
    aheadAxis1 = ahead.h;
    aheadAxis2 = ahead.d;
    behindAxis1 = behind.h;
    behindAxis2 = behind.d;
  }

  // calculate the Axis1 tracking rate
  if (aheadAxis1 < -Deg90 && behindAxis1 > Deg90) aheadAxis1 += Deg360;
  if (behindAxis1 < -Deg90 && aheadAxis1 > Deg90) behindAxis1 += Deg360;
  float rate1 = (aheadAxis1 - behindAxis1)/DiffRange2;
  if (fabs(trackingRateAxis1 - rate1) <= 0.005F) trackingRateAxis1 = (trackingRateAxis1*9.0F + rate1)/10.0F; else trackingRateAxis1 = rate1;

  // calculate the Axis2 Dec/Alt tracking rate
  float rate2 = (aheadAxis2 - behindAxis2)/DiffRange2;
  if (current.pierSide == PIER_SIDE_WEST) rate2 = -rate2;
  if (fabs(trackingRateAxis2 - rate2) <= 0.005F) trackingRateAxis2 = (trackingRateAxis2*9.0F + rate2)/10.0F; else trackingRateAxis2 = rate2;

  // override for special case of near a celestial pole
  if (fabs(declination) > Deg85) {
    if (transform.mountType == ALTAZM) trackingRateAxis1 = 0.0F; else trackingRateAxis1 = trackingRate;
    trackingRateAxis2 = 0.0F;
  }

  // override for both rates for special case near the zenith
  if (altitude > Deg85) {
    if (transform.mountType == ALTAZM) trackingRateAxis1 = 0.0F; else trackingRateAxis1 = ztr(current.a);
    trackingRateAxis2 = 0.0F;
  }

  update();
}

// alternate tracking rate calculation method
float Mount::ztr(float a) {
  if (a > degToRadF(89.8F)) return 0.99998667F; else if (a > degToRadF(89.5F)) return 0.99996667F;

  float altH = a + degToRadF(0.25F); if (altH < 0.0F) altH = 0.0F;
  float altL = a - degToRadF(0.25F); if (altL < 0.0F) altL = 0.0F;

  float altHr = altH - transform.trueRefrac(altH);
  float altLr = altL - transform.trueRefrac(altL);

  float r = (altH - altL)/(altHr - altLr); if (r > 1.0F) r = 1.0F;
  return r;
}

// update where we are pointing *now*
// CR_MOUNT for Horizon or Equatorial mount coordinates, depending on mount
// CR_MOUNT_EQU for Equatorial mount coordinates, depending on mode
// CR_MOUNT_ALT for altitude (a) and Horizon or Equatorial mount coordinates, depending on mode
// CR_MOUNT_HOR for Horizon mount coordinates, depending on mode
void Mount::updatePosition(CoordReturn coordReturn) {
  current = transform.instrumentToMount(axis1.getInstrumentCoordinate(), axis2.getInstrumentCoordinate());
  if (transform.mountType == ALTAZM) {
    if (coordReturn == CR_MOUNT_EQU || coordReturn == CR_MOUNT_ALL) transform.horToEqu(&current);
  } else {
    if (coordReturn == CR_MOUNT_ALT) transform.equToAlt(&current); else
    if (coordReturn == CR_MOUNT_HOR || coordReturn == CR_MOUNT_ALL) transform.equToHor(&current);
  }
  if (isHome()) current.pierSide = PIER_SIDE_NONE;
}

Mount mount;

#endif
