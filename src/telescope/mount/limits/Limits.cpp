//--------------------------------------------------------------------------------------------------
// telescope mount control, limits

#include "Limits.h"

#ifdef MOUNT_PRESENT

#include "../../../lib/tasks/OnTask.h"
#include "../../../lib/nv/Nv.h"

#include "../../../telescope/Telescope.h"
#include "../Mount.h"
#include "../goto/Goto.h"
#include "../guide/Guide.h"
#include "../site/Site.h"

inline void limitsWrapper() { limits.poll(); }

void Limits::init() {
  // confirm the data structure size
  if (LimitSettingsSize < sizeof(LimitSettings)) { nv.initError = true; DL("ERR: Limits::init(), LimitSettingsSize error"); }

  // write the default settings to NV
  if (!nv.hasValidKey() || nv.isNull(NV_MOUNT_LIMITS_BASE, sizeof(LimitSettings))) {
    VLF("MSG: Mount, limits writing defaults to NV");
    nv.writeBytes(NV_MOUNT_LIMITS_BASE, &settings, sizeof(LimitSettings));
  }

  // get settings from NV
  nv.readBytes(NV_MOUNT_LIMITS_BASE, &settings, sizeof(LimitSettings));

  constrainMeridianLimits();

  // start limit monitor task
  VF("MSG: Mount, start limits monitor task (rate 100ms priority 2)... ");
  if (tasks.add(100, 0, true, 2, limitsWrapper, "MtLimit")) { VLF("success"); } else { VLF("FAILED!"); }
}

// constrain meridian limits to the allowed range
void Limits::constrainMeridianLimits() {
  if (settings.pastMeridianE > Deg360) {
    settings.pastMeridianE = Deg360;
    DLF("WRN: Limits::init(), pastMeridianE > 360 deg setting to 360 deg");
  }
  if (settings.pastMeridianE < -Deg360) {
    settings.pastMeridianE = -Deg360;
    DLF("WRN: Limits::init(), pastMeridianE < -360 deg setting to -360 deg");
  }
  if (settings.pastMeridianW > Deg360) {
    settings.pastMeridianW = Deg360;
    DLF("WRN: Limits::init(), pastMeridianW > 360 deg setting to 360 deg");
  }
  if (settings.pastMeridianW < -Deg360) {
    settings.pastMeridianW = -Deg360;
    DLF("WRN: Limits::init(), pastMeridianW < -360 deg setting to -360 deg");
  }
}

// target coordinate check ahead of sync, goto, etc.
CommandError Limits::validateTarget(Coordinate *coords, bool isGoto) {
  bool eastReachable, westReachable;
  double eastCorrection, westCorrection;
  return validateTarget(coords, &eastReachable, &westReachable, &eastCorrection, &westCorrection, isGoto);
}

// target coordinate check ahead of sync, goto, etc.
CommandError Limits::validateTarget(Coordinate *coords, bool *eastReachable, bool *westReachable, double *eastCorrection, double *westCorrection, bool isGoto) {
  if (flt(coords->a, settings.altitude.min)) return CE_SLEW_ERR_BELOW_HORIZON;
  if (fgt(coords->a, settings.altitude.max)) return CE_SLEW_ERR_ABOVE_OVERHEAD;

  double a1e, a2e, a1w, a2w;

  PierSide lastPierSide = coords->pierSide; 
  coords->pierSide = PIER_SIDE_EAST;
  transform.mountToInstrument(coords, &a1e, &a2e);
  coords->pierSide = PIER_SIDE_WEST;
  transform.mountToInstrument(coords, &a1w, &a2w);
  coords->pierSide = lastPierSide;

  float eastLimitMin = axis1.getLimitMin();
  float eastLimitMax = axis1.getLimitMax();
  float westLimitMin = axis1.getLimitMin();
  float westLimitMax = axis1.getLimitMax();

  if (AXIS1_SECTOR_GEAR == ON) {
    if (isGoto) {
      *eastReachable = true;
      *westReachable = true;
      return CE_NONE;
    } else {
      eastLimitMin = -Deg180;
      eastLimitMax = Deg180;
      westLimitMin = -Deg180;
      westLimitMax = Deg180;
    }
  }

  if (transform.mountType == GEM) {
    if (-limits.settings.pastMeridianE > eastLimitMin) eastLimitMin = -limits.settings.pastMeridianE;
    if (limits.settings.pastMeridianW < westLimitMax) westLimitMax = limits.settings.pastMeridianW;
    westLimitMin += Deg180;
    westLimitMax += Deg180;
  }

  double a1 = axis1.getInstrumentCoordinate();

  bool inRange;
  if ((a1e >= eastLimitMin) && (a1e <= eastLimitMax)) inRange = true; else inRange = false;

  if ((a1e + Deg360 > eastLimitMin) && (a1e + Deg360 < eastLimitMax) && inRange && (dist(a1, a1e) > dist(a1, a1e + Deg360))) {
    a1e += Deg360;
    *eastCorrection = Deg360;
    VF("MSG: Mount, validate destination east axis1 best normalized to "); VL(radToDeg(a1e));
  } else
  if ((a1e - Deg360 > eastLimitMin) && (a1e - Deg360 < eastLimitMax) && inRange && (dist(a1, a1e) > dist(a1, a1e - Deg360))) {
    a1e -= Deg360;
    *eastCorrection = -Deg360;
    VF("MSG: Mount, validate destination east axis1 best normalized to "); VL(radToDeg(a1e));
  }

  if ((a1e + Deg360 > eastLimitMin) && (a1e + Deg360 < eastLimitMax) && !inRange) {
    a1e += Deg360;
    *eastCorrection = Deg360;
    VF("MSG: Mount, validate destination east axis1 normalized to "); VL(radToDeg(a1e));
  } else
  if ((a1e - Deg360 > eastLimitMin) && (a1e - Deg360 < eastLimitMax) && !inRange) {
    a1e -= Deg360;
    *eastCorrection = -Deg360;
    VF("MSG: Mount, validate destination east axis1 normalized to "); VL(radToDeg(a1e));
  }

  if ((a1w >= westLimitMin) && (a1w <= westLimitMax)) inRange = true; else inRange = false;

  if ((a1w + Deg360 > westLimitMin) && (a1w + Deg360 < westLimitMax) && inRange && (dist(a1, a1w) > dist(a1, a1w + Deg360))) {
    a1w += Deg360;
    *westCorrection = Deg360;
    VF("MSG: Mount, validate destination best west axis1 normalized to "); VL(radToDeg(a1w));
  } else
  if ((a1w - Deg360 > westLimitMin) && (a1w - Deg360 < westLimitMax) && inRange && (dist(a1, a1w) > dist(a1, a1w - Deg360))) {
    a1w -= Deg360;
    *westCorrection = -Deg360;
    VF("MSG: Mount, validate destination best west axis1 normalized to "); VL(radToDeg(a1w));
  }

  if ((a1w + Deg360 > westLimitMin) && (a1w + Deg360 < westLimitMax) && !inRange) {
    a1w += Deg360;
    *westCorrection = Deg360;
    VF("MSG: Mount, validate destination west axis1 normalized to "); VL(radToDeg(a1w));
  } else
  if ((a1w - Deg360 > westLimitMin) && (a1w - Deg360 < westLimitMax) && !inRange) {
    a1w -= Deg360;
    *westCorrection = -Deg360;
    VF("MSG: Mount, validate destination west axis1 normalized to "); VL(radToDeg(a1w));
  }

  *eastReachable = a1e >= eastLimitMin && a1e <= eastLimitMax;
  *westReachable = a1w >= westLimitMin && a1w <= westLimitMax;

  VF("MSG: Mount, validate east target axis1 "); V(radToDeg(eastLimitMin)); VF(" < "); V(radToDeg(a1e)); VF(" < "); V(radToDeg(eastLimitMax));
  if (*eastReachable) { VLF(" TRUE"); } else { VLF(" FALSE"); }
  VF("MSG: Mount, validate west target axis1 "); V(radToDeg(westLimitMin)); VF(" < "); V(radToDeg(a1w)); VF(" < "); V(radToDeg(westLimitMax));
  if (*westReachable) { VLF(" TRUE"); } else { VLF(" FALSE"); }

  if (!*eastReachable && !*westReachable) {
    VLF("MSG: Mount, validate target outside limits");
    *eastCorrection = 0.0;
    *westCorrection = 0.0;
    return CE_SLEW_ERR_OUTSIDE_LIMITS;
  }

  #if AXIS2_TANGENT_ARM == OFF
    if (transform.isEquatorial()) {
      if (flt(coords->d, axis2.getLimitMin())) {
        VF("MSG: Mount, validate failed Dec past min limit by ");
        V(radToDeg(coords->d - axis2.getLimitMin())*3600.0); VLF(" arc-secs");
        return CE_SLEW_ERR_OUTSIDE_LIMITS;
      }
      if (fgt(coords->d, axis2.getLimitMax())) {
        VF("MSG: Mount, validate failed Dec past max limit by ");
        V(radToDeg(coords->d - axis2.getLimitMax())*3600.0); VLF(" arc-secs");
        return CE_SLEW_ERR_OUTSIDE_LIMITS;
      }
    }
  #endif

  return CE_NONE;
}

// true if an error exists
bool Limits::isError() {
  return initError.nv ||
         initError.value ||
         initError.tls ||
         error.altitude.min ||
         error.altitude.max ||
         error.limit.axis1.min ||
         error.limit.axis1.max ||
         error.limit.axis2.min ||
         error.limit.axis2.max ||
         error.limitSense.axis1.min ||
         error.limitSense.axis1.max ||
         error.limitSense.axis2.min ||
         error.limitSense.axis2.max ||
         error.meridian.east ||
         error.meridian.west;
}

// true if an error exists that impacts goto safety
bool Limits::isGotoError() {
  return initError.nv ||
         initError.value ||
         !site.dateIsReady ||
         !site.timeIsReady ||
         error.limitSense.axis1.min ||
         error.limitSense.axis1.max ||
         error.limitSense.axis2.min ||
         error.limitSense.axis2.max;
}

// return general error code
uint8_t Limits::errorCode() {
  enum GeneralErrors: uint8_t {
  ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT_MIN, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM,
  ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC, ERR_PARK, ERR_GOTO_SYNC, ERR_UNSPECIFIED,
  ERR_ALT_MAX, ERR_WEATHER_INIT, ERR_SITE_INIT, ERR_NV_INIT};

  // priority highest to lowest
  if (mount.motorFault()) return (uint8_t)ERR_MOTOR_FAULT;
  if (error.limitSense.axis1.min || error.limitSense.axis1.max ||
      error.limitSense.axis2.min || error.limitSense.axis2.max) return (uint8_t)ERR_LIMIT_SENSE;
  if (error.altitude.min) return (uint8_t)ERR_ALT_MIN;
  if (error.altitude.max) return (uint8_t)ERR_ALT_MAX;
  if (transform.isEquatorial()) {
    if (error.limit.axis1.min || error.limit.axis1.max) return (uint8_t)ERR_UNDER_POLE;
    if (error.limit.axis2.min || error.limit.axis2.max) return (uint8_t)ERR_DEC;
  } else {
    if (error.limit.axis1.min || error.limit.axis1.max) return (uint8_t)ERR_AZM;
    if (error.limit.axis2.min) return (uint8_t)ERR_ALT_MIN;
    if (error.limit.axis2.max) return (uint8_t)ERR_ALT_MAX;
  }
  if (error.meridian.east || error.meridian.west) return (uint8_t)ERR_MERIDIAN;
  if (initError.nv || initError.value) return (uint8_t)ERR_NV_INIT;
  if (initError.tls) return (uint8_t)ERR_SITE_INIT;
  if (initError.weather) return (uint8_t)ERR_WEATHER_INIT;
  return ERR_NONE;
}

void Limits::stop() {
  #if GOTO_FEATURE == ON
    goTo.abort();
  #endif
  guide.stopAxis1(GA_BREAK, true);
  guide.stopAxis2(GA_BREAK, true);
  mount.tracking(false);
}

void Limits::stopAxis1(GuideAction stopDirection) {
  #if GOTO_FEATURE == ON
    goTo.abort();
  #endif
  guide.stopAxis1(stopDirection, true);
  if (stopDirection == GA_FORWARD || !transform.isEquatorial()) mount.tracking(false);
}

void Limits::stopAxis2(GuideAction stopDirection) {
  #if GOTO_FEATURE == ON
    goTo.abort();
  #endif
  guide.stopAxis2(stopDirection, true);
  if (!transform.isEquatorial()) mount.tracking(false);
}

void Limits::poll() {
  static int autoFlipDelayCycles = 0;
  if (autoFlipDelayCycles > 0) autoFlipDelayCycles--;

  LimitsError lastError = error;

  Coordinate current = mount.getMountPosition(CR_MOUNT_ALT);

  #if TRACK_AUTOSTART == OFF && TRACK_WITHOUT_LIMITS == OFF
    if (!limitsEnabled && mount.isTracking()) {
      VLF("MSG: Mount, tracking without limits disallowed");
      mount.tracking(false);
    }
  #endif

  if (limitsEnabled && guide.state != GU_HOME_GUIDE && guide.state != GU_HOME_GUIDE_ABORT) {
    // overhead and horizon limits
    if (current.a < settings.altitude.min) error.altitude.min = true; else error.altitude.min = false;
    if (fabs(settings.altitude.max - Deg90) > OneArcSec) {
      if (current.a > settings.altitude.max) error.altitude.max = true; else error.altitude.max = false;
    } else error.altitude.max = false;

    // meridian limits
    if (transform.mountType == GEM && current.pierSide == PIER_SIDE_EAST) {
      if (current.h < -settings.pastMeridianE) {
        stopAxis1(GA_REVERSE);
        error.meridian.east = true;
      } else error.meridian.east = false;
    } else error.meridian.east = false;

    if (transform.mountType == GEM && current.pierSide == PIER_SIDE_WEST) {
      if (current.h > settings.pastMeridianW && autoFlipDelayCycles == 0) {
        #if GOTO_FEATURE == ON && AXIS1_SECTOR_GEAR == OFF && AXIS2_TANGENT_ARM == OFF
          if (goTo.isAutoFlipEnabled() && mount.isTracking()) {
            // disable this limit for a second to allow goto to exit the out of limits region
            autoFlipDelayCycles = 10;
            VLF("MSG: Mount, start automatic meridian flip");
            Coordinate target = mount.getMountPosition();
            CommandError e = goTo.request(target, PSS_EAST_ONLY, false);
            if (e != CE_NONE) {
              stopAxis1(GA_FORWARD);
              error.meridian.west = true;
              DF("WRN: Limits::limitPoll(), goto for automatic meridian flip failed ("); D(e); DL(")");
            }
          } else
        #endif
        {
          stopAxis1(GA_FORWARD);
          error.meridian.west = true;
        }
      } else error.meridian.west = false;
    } else error.meridian.west = false;

    // for Fork and Alt/Azm mounts limits are based on shaft angles
    // so convert axis1 into normal PIER_SIDE_EAST coordinates
    if (transform.mountType != GEM && current.pierSide == PIER_SIDE_WEST) current.a1 += Deg180;

    if (mount.isHome()) current.a1 = home.getPosition(CR_MOUNT).a1;

    #if AXIS1_SECTOR_GEAR == ON
      current.a1 = axis1.getMotorPosition();
    #endif

    // min and max limits
    if (flt(current.a1, axis1.getLimitMin())) {
      stopAxis1(GA_REVERSE);
      error.limit.axis1.min = true;
      // ---------------------------------------------------------
      if (lastError.limit.axis1.min != error.limit.axis1.min) {
        D("WRN: Mount, limits min axis1 "); D(radToDeg(current.a1)); D(" > "); D(radToDeg(axis1.getLimitMin()));
        D(" ("); D(current.pierSide == PIER_SIDE_WEST ? "W" : "E"); D(")");
        DLF(" FAILED");
        V("MSG: Mount, axis1 = "); VL(degToRad(axis1.getInstrumentCoordinate()));
      }
      // ---------------------------------------------------------
    } else error.limit.axis1.min = false;

    if (fgt(current.a1, axis1.getLimitMax()) && autoFlipDelayCycles == 0) {
      #if GOTO_FEATURE == ON && AXIS1_SECTOR_GEAR == OFF && AXIS2_TANGENT_ARM == OFF
        if (current.pierSide == PIER_SIDE_EAST && goTo.isAutoFlipEnabled() && mount.isTracking()) {
          // disable this limit for a second to allow goto to exit the out of limits region
          autoFlipDelayCycles = 10;
          VLF("MSG: Mount, start automatic meridian flip");
          Coordinate target = mount.getMountPosition();
          CommandError e = goTo.request(target, PSS_WEST_ONLY, false);
          if (e != CE_NONE) {
            stopAxis1(GA_FORWARD);
            error.limit.axis1.max = true;
            DF("WRN: Limits::limitPoll(), goto for automatic meridian flip failed ("); D(e); DL(")");
          }
        } else
      #endif
      {
        stopAxis1(GA_FORWARD);
        error.limit.axis1.max = true;
        // -------------------------------------------------------------
        if (lastError.limit.axis1.max != error.limit.axis1.max) {
          D("WRN: Mount, limits max axis1 "); D(radToDeg(current.a1)); D(" < "); D(radToDeg(axis1.getLimitMax()));
          DLF(" FAILED");
        }
        // -------------------------------------------------------------
      }
    } else error.limit.axis1.max = false;

    if (mount.isHome()) current.a2 = home.getPosition(CR_MOUNT).a2;

    #if AXIS2_TANGENT_ARM == ON
      current.a2 = axis2.getMotorPosition();
    #endif

    if (flt(current.a2, axis2.getLimitMin())) {
      stopAxis2((current.pierSide == PIER_SIDE_EAST) ? GA_REVERSE : GA_FORWARD);
      error.limit.axis2.min = true;
    } else error.limit.axis2.min = false;

    if (fgt(current.a2, axis2.getLimitMax())) {
      stopAxis2((current.pierSide == PIER_SIDE_EAST) ? GA_FORWARD : GA_REVERSE);
      error.limit.axis2.max = true;
    } else error.limit.axis2.max = false;

  } else {
    error.altitude.min = false;
    error.altitude.max = false;
    error.limit.axis1.min = false;
    error.limit.axis1.max = false;
    error.limit.axis2.min = false;
    error.limit.axis2.max = false;
    error.meridian.east = false;
    error.meridian.west = false;
  }

  // min and max limit switches
  error.limitSense.axis1.min = axis1.motionErrorSensed(DIR_REVERSE);
  if (error.limitSense.axis1.min) {
    if (!axis1.commonMinMaxSense || lastError.limitSense.axis1.min != error.limitSense.axis1.min) stopAxis1(GA_REVERSE);
  }

  error.limitSense.axis1.max = axis1.motionErrorSensed(DIR_FORWARD);
  if (error.limitSense.axis1.max) {
    if (!axis1.commonMinMaxSense || lastError.limitSense.axis1.max != error.limitSense.axis1.max) stopAxis1(GA_FORWARD);
  }

  error.limitSense.axis2.min = axis2.motionErrorSensed(DIR_REVERSE);
  if (error.limitSense.axis2.min) {
    if (!axis2.commonMinMaxSense || lastError.limitSense.axis2.min != error.limitSense.axis2.min)
      stopAxis2((current.pierSide == PIER_SIDE_EAST) ? GA_REVERSE : GA_FORWARD);
  }

  error.limitSense.axis2.max = axis2.motionErrorSensed(DIR_FORWARD);
  if (error.limitSense.axis2.max) {
    if (!axis2.commonMinMaxSense || lastError.limitSense.axis2.max != error.limitSense.axis2.max)
      stopAxis2((current.pierSide == PIER_SIDE_EAST) ? GA_FORWARD : GA_REVERSE);
  }

  #if DEBUG == VERBOSE
    const char* errPre = "MSG: Mount, limit state: ";
    if (lastError.altitude.min != error.altitude.min ||
        lastError.altitude.max != error.altitude.max ||
        lastError.meridian.east != error.meridian.east ||
        lastError.meridian.west != error.meridian.west ||
        lastError.limit.axis1.min != error.limit.axis1.min ||
        lastError.limit.axis1.max != error.limit.axis1.max ||
        lastError.limit.axis2.min != error.limit.axis2.min ||
        lastError.limit.axis2.max != error.limit.axis2.max) {
      V(errPre);
      V(error.altitude.min?         "Alt-< " :"Alt-  ");
      V(error.altitude.max?         "Alt+< " :"Alt+  ");
      V(error.meridian.east?        "ME< "   :"ME  "  );
      V(error.meridian.west?        "MW< "   :"MW  "  );
      V(error.limit.axis1.min?      "A1L-< " :"A1L-  ");
      V(error.limit.axis1.max?      "A1L+< " :"A1L+  ");
      V(error.limit.axis2.min?      "A2L-< " :"A2L-  ");
      V(error.limit.axis2.max?      "A2L+< " :"A2L+  ");
    }
  #endif

  // respond to overhead and horizon limits
  if (transform.mountType == ALTAZM) {
    if (error.altitude.min) stopAxis2(GA_REVERSE);
    if (error.altitude.max) stopAxis2(GA_FORWARD);
  } else {
    if (!lastError.altitude.min && error.altitude.min) stop();
    if (!lastError.altitude.max && error.altitude.max) stop();
  }

  // if first time breaking a meridian or min/max limit stop all guides
  if ((!lastError.meridian.east && error.meridian.east) ||
      (!lastError.meridian.west && error.meridian.west) ||
      (!lastError.limit.axis1.min && error.limit.axis1.min) ||
      (!lastError.limit.axis1.max && error.limit.axis1.max) ||
      (!lastError.limit.axis2.min && error.limit.axis2.min) ||
      (!lastError.limit.axis2.max && error.limit.axis2.max)) stop();
}

Limits limits;

#endif
