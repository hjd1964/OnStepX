//--------------------------------------------------------------------------------------------------
// telescope mount control

#include "Mount.h"

#ifdef MOUNT_PRESENT

#include "../../lib/tasks/OnTask.h"
#include "../../lib/nv/Nv.h"

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

#if MOUNT_COORDS_MEMORY == ON && (NV_DRIVER != NV_MB85RC32 && NV_DRIVER != NV_MB85RC64 && NV_DRIVER != NV_MB85RC256)
  #error "Configuration (Config.h): Setting MOUNT_COORDS_MEMORY requires a NV storage device with very high write endurance (FRAM)"
#endif

inline void mountWrapper() { mount.poll(); }
inline void autostartWrapper() { mount.autostartPostponed(); }

void Mount::init() {
  absoluteCoordinateOriginsEstablished = true;

  nvKey = nv().kv().computeKey("MOUNT_SETTINGS");
  if (!nv().kv().getOrInit(nvKey, settings)) { DLF("WRN: Nv, init failed for MOUNT_SETTINGS"); }

  // get the main axes ready
  delay(100);
  if (!axis1.init(&motor1)) { initError.driver = true; DLF("ERR: Mount::init(), no motion controller for Axis1!"); } else {
    axis1.setBacklash(settings.backlash.axis1);
    axis1.setMotionLimitsCheck(false);
    if (AXIS1_POWER_DOWN == ON) axis1.setPowerDownTime(AXIS1_POWER_DOWN_TIME);
    #ifdef AXIS1_ENCODER_ORIGIN
      uint32_t origin = UINT32_MAX;
      if (AXIS1_ENCODER_ORIGIN == 0) {
        nv().kv().getOrInit("AXIS1_ENCODER_ORIGIN", origin);
        if (origin != UINT32_MAX) axis1.motor->encoderSetOrigin(origin); else absoluteCoordinateOriginsEstablished = false;
      }
    #endif
  }

  delay(100);
  if (!axis2.init(&motor2)) { initError.driver = true; DLF("ERR: Mount::init(), no motion controller for Axis2!"); } else {
    axis2.setBacklash(settings.backlash.axis2);
    axis2.setMotionLimitsCheck(false);
    if (AXIS2_POWER_DOWN == ON) axis2.setPowerDownTime(AXIS2_POWER_DOWN_TIME);
    #ifdef AXIS2_ENCODER_ORIGIN
      uint32_t origin = UINT32_MAX;
      if (AXIS2_ENCODER_ORIGIN == 0) {
        nv().kv().getOrInit("AXIS2_ENCODER_ORIGIN", origin);
        if (origin != UINT32_MAX) axis2.motor->encoderSetOrigin(origin); else absoluteCoordinateOriginsEstablished = false;
      }
    #endif
  }
}

void Mount::begin() {
  startupAuthorityTrustedValue = false;

  axis1.calibrateDriver();
  axis1.enable(MOUNT_ENABLE_IN_STANDBY == ON);
  axis2.calibrateDriver();
  axis2.enable(MOUNT_ENABLE_IN_STANDBY == ON);

  // initialize the critical subsystems
  site.init();

  // invalid mount type, set default
  if (settings.mountType < MOUNT_SUBTYPE_FIRST || settings.mountType > MOUNT_SUBTYPE_LAST) {
    settings.mountType = MOUNT_SUBTYPE;
    nv().kv().put(nvKey, settings);
    VLF("MSG: Transform, revert mount type to default");
  }

  if (MOUNT_TYPE == ALTALT) settings.mountType = MOUNT_SUBTYPE;
  transform.init(settings.mountType);

  // setup compensated tracking as configured
  if (TRACK_COMPENSATION_MEMORY == OFF) settings.rc = RC_DEFAULT;
  if (!transform.isEquatorial()) {
    if (settings.rc == RC_MODEL) settings.rc = RC_MODEL_DUAL;
    if (settings.rc == RC_REFRACTION) settings.rc = RC_REFRACTION_DUAL;
  }

  // initialize the other subsystems
  // Paired mount-axis absolute position sources can establish the initial
  // coordinate basis at startup, so allow reset() to bypass sync-threshold
  // checks only in that case.
  const bool absoluteAxisAuthority = axis1.motor->hasAbsoluteEncoder() && axis2.motor->hasAbsoluteEncoder();
  home.reset(true, absoluteAxisAuthority && absoluteCoordinateOriginsEstablished);
  limits.init();
  guide.init();

  if (AXIS1_WRAP == ON) {
    axis1.coordinateWrap(Deg360);
    axis1.setLimitMin(-Deg360);
    axis1.setLimitMax(Deg360);
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

  tracking(false);

  // restore where we were pointing
  #if MOUNT_COORDS_MEMORY == ON
    lastPosition.a1 = (float)axis1.getInstrumentCoordinate();
    lastPosition.a2 = (float)axis2.getInstrumentCoordinate();
    lastPosition.mountType = (uint8_t)(transform.mountType & 0x0Fu);
    lastPosition.seq = 0;
    lastPosition.untrusted = true;

    nvKeyLastA = nv().kv().computeKey("MOUNT_LAST_POS_A");
    nvKeyLastB = nv().kv().computeKey("MOUNT_LAST_POS_B");

    if (!goTo.absoluteEncodersPresent && park.state != PS_PARKED) {

      MountPositionMemory a{}, b{};
      const bool foundA = (nv().kv().get(nvKeyLastA, a) == KvPartition::Status::Ok);
      const bool foundB = (nv().kv().get(nvKeyLastB, b) == KvPartition::Status::Ok);

      const bool va = foundA && (a.mountType == (uint8_t)(transform.mountType & 0x0Fu)) && !a.untrusted;
      const bool vb = foundB && (b.mountType == (uint8_t)(transform.mountType & 0x0Fu)) && !b.untrusted;

      const MountPositionMemory* best = nullptr;

      if (va && vb) {
        const uint8_t d = (uint8_t)((a.seq - b.seq) & 3u);
        best = (d == 1u) ? &a : (d == 3u) ? &b : &a;
      } else if (va) {
        best = &a;
      } else if (vb) {
        best = &b;
      }

      if (best) {
        lastPosition = *best;
        if (limits.validateInstrumentCoordinate(1, lastPosition.a1, true) == CE_NONE &&
            limits.validateInstrumentCoordinate(2, lastPosition.a2, true) == CE_NONE) {
          CommandError e = limits.setInstrumentCoordinate(1, lastPosition.a1, true);
          if (e == CE_NONE) e = limits.setInstrumentCoordinate(2, lastPosition.a2, true);
          if (e == CE_NONE) {
            captureNominalIndexPositions();
            mount.syncFromOnStepToEncoders = true;
            setStartupAuthorityTrusted(true);
          } else { DLF("WRN: Mount, coordinate memory restore failed"); }
        } else {
          DLF("WRN: Mount, coordinate memory restore failed");
        }
      }
    }
  #endif

  #if ALIGN_MAX_NUM_STARS > 1 && ALIGN_MODEL_MEMORY == ON
    transform.align.modelRead();
  #endif

  #if MOUNT_STARTUP_MODE == SA_PERMISSIVE
    setStartupAuthorityTrusted(true);
  #elif MOUNT_STARTUP_MODE == SA_AUTO
    #if MOUNT_COORDS_MEMORY == OFF
      if (!axis1.motor->hasAbsoluteEncoder() && !axis2.motor->hasAbsoluteEncoder()) {
        setStartupAuthorityTrusted(true);
      }
    #endif
  #endif

  VF("MSG: Mount, start tracking monitor task (rate 1000ms priority 6)... ");
  if (tasks.add(1000, 0, true, 6, mountWrapper, "MtTrack")) { VLF("success"); } else { VLF("FAILED!"); }

  update();
  autostart();
}

void Mount::setStartupAuthorityTrusted(bool state) {
  #if NV_INIT_ERROR_REVOKES_AUTHORITY == ON
    if (state && initError.nv) return;
  #endif

  startupAuthorityTrustedValue = state;
}

void Mount::captureNominalIndexPositions() {
  nominalIndexAxis1Steps = axis1.getIndexPositionSteps();
  nominalIndexAxis2Steps = axis2.getIndexPositionSteps();
}

long Mount::getNominalIndexPositionSteps(uint8_t axisNumber) const {
  switch (axisNumber) {
    case 1: return nominalIndexAxis1Steps;
    case 2: return nominalIndexAxis2Steps;
    default: return 0;
  }
}

#if MOUNT_COORDS_MEMORY == ON
void Mount::saveCoordinateMemory(bool trusted) {
  if (goTo.absoluteEncodersPresent) return;
  if (nv().device().endurance() != NvDevice::Endurance::High) return;

  lastPosition.a1 = (float)axis1.getInstrumentCoordinate();
  lastPosition.a2 = (float)axis2.getInstrumentCoordinate();
  lastPosition.mountType = (uint8_t)(transform.mountType & 0x0Fu);
  lastPosition.untrusted = !trusted;
  if (lastPosition.seq == 3) lastPosition.seq = 0; else lastPosition.seq++;
  nv().kv().put((lastPosition.seq & 1)? nvKeyLastA : nvKeyLastB, lastPosition);
}
#endif

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

// handle all autostart tasks
void Mount::autostart() {
  tasks.setDurationComplete(tasks.getHandleByName("MtAuto"));
  tasks.add(2000, 0, true, 7, autostartWrapper, "MtAuto");
}

void Mount::autostartPostponed() {
  // wait until OnStepX is fully up and running
  if (!telescope.ready) return;

  // stop this task if already completed
  static bool autoStartDone = false;
  if (autoStartDone) {
    tasks.setDurationComplete(tasks.getHandleByName("MtAuto"));
    return;
  }

  // handle the one case where this completes without the date/time available
  static bool autoTrackDone = false;
  if (!autoTrackDone && TRACK_AUTOSTART == ON && transform.isEquatorial() && park.state != PS_PARKED && !home.settings.automaticAtBoot) {
    VLF("MSG: Mount, autostart tracking");
    tracking(true);
    autoStartDone = true;
    return;
  }

  // wait for the date/time to be set
  if (!site.isDateTimeReady()) return;

  // auto unpark
  static bool autoUnparkDone = false;
  if (!autoUnparkDone && park.state == PS_PARKED) {
    #if GOTO_FEATURE == ON
      VLF("MSG: Mount, autostart park restore");
      CommandError e = park.restore(TRACK_AUTOSTART == ON);
      if (e != CE_NONE) {
        DF("WRN: Mount, autostart park restore failed with code "); DL(e);
        autoStartDone = true;
        return;
      }
    #endif
  }
  autoUnparkDone = true;

  // auto home
  static bool autoHomeDone = false;
  if (!autoHomeDone && home.settings.automaticAtBoot) {
    VLF("MSG: Mount, autostart home");
    CommandError e = home.request();
    if (e != CE_NONE) {
      DF("WRN: Mount, autostart home request failed with code "); DL(e);
      autoStartDone = true;
      return;
    }
  }
  autoHomeDone = true;

  // wait for any homing operation to complete
  if (home.state == HS_HOMING) return;
  if (home.failed) {
    DLF("WRN: Mount, autostart home failed");
    autoStartDone = true;
    return;
  }

  // auto tracking
  if (!autoTrackDone && TRACK_AUTOSTART == ON) {
    VLF("MSG: Mount, autostart tracking");
    tracking(true);
  }
  autoTrackDone = true;

  autoStartDone = true;
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
void Mount::enable(bool state) {
  if (state == true) {
    #if LIMIT_STRICT == ON
      if (!site.dateIsReady || !site.timeIsReady) return;
    #endif

    mountStatus.wake();
  } else {
    trackingState = TS_NONE;
    update();
  }

  axis1.enable(state);
  axis2.enable(state);
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
      f1 = trackingRateAxis1;
      if (transform.mountType != ALTAZM && transform.mountType != ALTALT) {
        f1 += guide.rateAxis1;
        #if AXIS1_PEC == ON
          f1 += pec.rate;
        #endif
      }
      axis1.setSynchronizedFrequency(siderealToRadF(f1)*SIDEREAL_RATIO_F*site.getSiderealRatio());
    }

    if (!guide.activeAxis2() || guide.state == GU_PULSE_GUIDE) {
      f2 = trackingRateAxis2;
      if (transform.mountType != ALTAZM && transform.mountType != ALTALT)  f2 += guide.rateAxis2;
      axis2.setSynchronizedFrequency(siderealToRadF(f2)*SIDEREAL_RATIO_F*site.getSiderealRatio());
    }

    f1 = fabs(f1);
    f2 = fabs(f2);
    if (f2 > f1) f1 = f2;
    if (f1 < 0.20F) statusFlashMs = SF_STOPPED; else
    if (f1 > 3.0F) statusFlashMs = SF_SLEWING; else statusFlashMs = 500.0F/f1;
  } else {
    statusFlashMs = SF_SLEWING;
    axis2.setSynchronizedFrequency(0.0F);
  }

  if (statusFlashMs != lastStatusFlashMs) {
    lastStatusFlashMs = statusFlashMs;
    mountStatus.flashRate(statusFlashMs);
    xBusy = statusFlashMs == SF_SLEWING;
  }
}

void Mount::poll() {
  #if NV_INIT_ERROR_REVOKES_AUTHORITY == ON
    if (initError.nv && startupAuthorityTrustedValue) {
      startupAuthorityTrustedValue = false;
      DLF("WRN: Mount, startup authority trust cleared due to NV fault");
    }
  #endif

  // stop any movement then disable on motor hardware fault
  if (mount.motorFault()) {
    if (startupAuthorityTrustedValue) {
      startupAuthorityTrustedValue = false;
      DLF("WRN: Mount, startup authority trust cleared due to motor/encoder fault");
    }
    if (goTo.state > GS_NONE) goTo.abort(); else
    if (guide.state > GU_NONE) guide.abort(); else
    if (axis1.isEnabled() || axis2.isEnabled()) enable(false);
  }

  #ifdef HAL_NO_DOUBLE_PRECISION
    #define DiffRange  0.0087266463F         // 30 arc-minutes in radians
    #define DiffRange2 0.017453292F          // 60 arc-minutes in radians
  #else
    #define DiffRange  2.908882086657216e-4L // 1 arc-minute in radians
    #define DiffRange2 5.817764173314432e-4L // 2 arc-minutes in radians
  #endif
  #define DegenerateRange (DiffRange*1.5)    // we were using Deg85

  // keep track of where we are pointing
  #if MOUNT_COORDS_MEMORY == ON
    saveCoordinateMemory(startupAuthorityTrustedValue && home.state != HS_HOMING);
  #endif

  if (trackingState == TS_NONE) {
    trackingRateAxis1 = 0.0F;
    trackingRateAxis2 = 0.0F;
    update();
    return;
  }

  if (transform.isEquatorial() && settings.rc == RC_NONE && trackingRateOffsetRA == 0.0F && trackingRateOffsetDec == 0.0F) {
    trackingRateAxis1 = trackingRate;
    trackingRateAxis2 = 0.0F;
    update();
    return;
  }

  // get positions 1 (or 30) arc-min ahead and behind the current
  updatePosition(CR_MOUNT_ALL);
  double altitude = current.a;
  double declination = current.d;
  double altitude2 = current.aa2;

  // on fast processors calculate true coordinate for a little more accuracy
  #ifndef HAL_SLOW_PROCESSOR
    transform.mountToTopocentric(&current);
    if (transform.mountType == ALTAZM) transform.horToEqu(&current); else
    if (transform.mountType == ALTALT) transform.aaToEqu(&current);
  #endif

  Y;
  Coordinate ahead = current;
  Coordinate behind = current;
  double trackingRange = DiffRange*trackingRate;
  ahead.h += trackingRange;
  behind.h -= trackingRange;

  // create horizon coordinates that would exist ahead and behind the current position
  if (transform.mountType == ALTAZM) { transform.equToHor(&ahead); transform.equToHor(&behind); Y; } else
  if (transform.mountType == ALTALT) { transform.equToAa(&ahead); transform.equToAa(&behind); Y; }

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

  // apply non-equatorial guide rate offset to equatorial coordinates
  if (guide.state == GU_PULSE_GUIDE && (transform.mountType == ALTAZM || transform.mountType == ALTALT)) {
    float trackingRateGuideRadsRA = siderealToRad(guide.rateAxis1)*timeInSeconds*2.0;
    float trackingRateGuideRadsDec = siderealToRad(guide.rateAxis2)*timeInSeconds*2.0;
    ahead.h += trackingRateGuideRadsRA;
    behind.h -= trackingRateGuideRadsRA;
    ahead.d += trackingRateGuideRadsDec;
    behind.d -= trackingRateGuideRadsDec;
  }

  // transfer to variables named appropriately for mount coordinates
  float aheadAxis1, aheadAxis2, behindAxis1, behindAxis2;
  if (transform.mountType == ALTAZM) {
    transform.equToHor(&ahead);
    aheadAxis1 = ahead.z;
    aheadAxis2 = ahead.a;
    behindAxis1 = behind.z;
    behindAxis2 = behind.a;
  } else
  if (transform.mountType == ALTALT) {
    transform.equToAa(&ahead);
    aheadAxis1 = ahead.aa1;
    aheadAxis2 = ahead.aa2;
    behindAxis1 = behind.aa1;
    behindAxis2 = behind.aa2;
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
  if (fabs(declination) > Deg90 - DegenerateRange) {
    if (transform.isEquatorial()) trackingRateAxis1 = trackingRate; else
    if (transform.mountType == ALTAZM) trackingRateAxis1 = 0.0F; else
    if (transform.mountType == ALTALT) trackingRateAxis1 = 0.0F;
    trackingRateAxis2 = 0.0F;
  }

  // override for both rates for special case near the Azm axis of rotation (Zenith)
  if (altitude > Deg90 - DegenerateRange) {
//    if (transform.isEquatorial()) trackingRateAxis1 = ztr(current.a); else
//    if (transform.mountType == ALTALT) trackingRateAxis1 = ztr(current.a); else
    if (transform.mountType == ALTAZM) trackingRateAxis1 = 0.0F; 
    trackingRateAxis2 = 0.0F;
  }

  // override for both rates for special case near the aa1 axis of rotation
  if (transform.mountType == ALTALT && fabs(altitude2) > Deg90 - DegenerateRange) { trackingRateAxis1 = 0.0F; }

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
void Mount::updatePosition(CoordReturn coordReturn) {
  current = transform.instrumentToMount(axis1.getInstrumentCoordinate(), axis2.getInstrumentCoordinate());
  if (isHome()) {
    transform.mountToInstrument(&current, &current.a1, &current.a2);
    current.pierSide = PIER_SIDE_NONE;
  }

  if (transform.mountType == ALTAZM) {
    if (coordReturn == CR_MOUNT_EQU || coordReturn == CR_MOUNT_ALL) transform.horToEqu(&current);
  } else
  if (transform.mountType == ALTALT) {
    transform.aaToEqu(&current);
  } else {
    if (coordReturn == CR_MOUNT_ALT) transform.equToAlt(&current); else
    if (coordReturn == CR_MOUNT_HOR || coordReturn == CR_MOUNT_ALL) transform.equToHor(&current);
  }
}

Mount mount;

#endif
