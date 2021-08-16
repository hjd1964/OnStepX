//--------------------------------------------------------------------------------------------------
// telescope mount control, limits

#include "Limits.h"

#ifdef MOUNT_PRESENT

#include "../../../tasks/OnTask.h"
extern Tasks tasks;
#include "../../../telescope/Telescope.h"

#include "../Mount.h"
#include "../goto/Goto.h"
#include "../guide/Guide.h"

inline void limitsWrapper() { limits.poll(); }

void Limits::init() {
  // confirm the data structure size
  if (LimitSettingsSize < sizeof(LimitSettings)) { initError.nv = true; DL("ERR: Limits::init(); LimitSettingsSize error NV subsystem writes disabled"); nv.readOnly(true); }

  // write the default settings to NV
  if (!validKey) {
    VLF("MSG: Mount, limits writing defaults to NV");
    nv.writeBytes(NV_MOUNT_LIMITS_BASE, &settings, sizeof(LimitSettings));
  }

  // get settings from NV
  nv.readBytes(NV_MOUNT_LIMITS_BASE, &settings, sizeof(LimitSettings));

  // start limit monitor task
  VF("MSG: Mount, limits start monitor task (rate 100ms priority 2)... ");
  if (tasks.add(100, 0, true, 2, limitsWrapper, "MntLmt")) { VL("success"); } else { VL("FAILED!"); }
}

// target coordinate check ahead of sync, goto, etc.
CommandError Limits::validateCoords(Coordinate *coords) {
  if (flt(coords->a, settings.altitude.min)) return CE_SLEW_ERR_BELOW_HORIZON;
  if (fgt(coords->a, settings.altitude.max)) return CE_SLEW_ERR_ABOVE_OVERHEAD;
  if (transform.mountType == ALTAZM) {
    if (flt(coords->z, axis1.settings.limits.min)) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (fgt(coords->z, axis1.settings.limits.max)) return CE_SLEW_ERR_OUTSIDE_LIMITS;
  } else {
    if (flt(coords->h, axis1.settings.limits.min)) {
        VF("MSG: Mount, validate failed HA past min limit by ");
        V(radToDeg(coords->h - axis1.settings.limits.min)*3600.0); VL(" arc-secs");
      return CE_SLEW_ERR_OUTSIDE_LIMITS;
    }
    if (fgt(coords->h, axis1.settings.limits.max)) {
        VF("MSG: Mount, validate failed Dec past min limit by ");
        V(radToDeg(coords->h - axis1.settings.limits.max)*3600.0); VL(" arc-secs");
      return CE_SLEW_ERR_OUTSIDE_LIMITS;
    }
    if (AXIS2_TANGENT_ARM == OFF) {
      if (flt(coords->d, axis2.settings.limits.min)) {
        VF("MSG: Mount, validate failed Dec past min limit by ");
        V(radToDeg(coords->d - axis2.settings.limits.min)*3600.0); VL(" arc-secs");
        return CE_SLEW_ERR_OUTSIDE_LIMITS;
      }
      if (fgt(coords->d, axis2.settings.limits.max)) {
        VF("MSG: Mount, validate failed Dec past max limit by ");
        V(radToDeg(coords->d - axis2.settings.limits.max)*3600.0); VL(" arc-secs");
        return CE_SLEW_ERR_OUTSIDE_LIMITS;}
    }
  }
  return CE_NONE;
}

// true if an limit related error is exists
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

// return limit error code
uint8_t Limits::errorCode() {
  enum GeneralErrors: uint8_t {
  ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT_MIN, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM,
  ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC, ERR_PARK, ERR_GOTO_SYNC, ERR_UNSPECIFIED,
  ERR_ALT_MAX, ERR_WEATHER_INIT, ERR_SITE_INIT, ERR_NV_INIT};

  // priority highest to lowest
  if (error.motorFault) return (uint8_t)ERR_MOTOR_FAULT;
  if (error.limitSense.axis1.min || error.limitSense.axis1.max ||
      error.limitSense.axis2.min || error.limitSense.axis2.max) return (uint8_t)ERR_LIMIT_SENSE;
  if (error.altitude.min) return (uint8_t)ERR_ALT_MIN;
  if (error.altitude.max) return (uint8_t)ERR_ALT_MAX;
  if (transform.mountType == ALTAZM) {
    if (error.limit.axis1.min || error.limit.axis1.max) return (uint8_t)ERR_AZM;
    if (error.limit.axis2.min) return (uint8_t)ERR_ALT_MIN;
    if (error.limit.axis2.max) return (uint8_t)ERR_ALT_MAX;
  } else {
    if (error.limit.axis1.min || error.limit.axis1.max) return (uint8_t)ERR_UNDER_POLE;
    if (error.limit.axis2.min || error.limit.axis2.max) return (uint8_t)ERR_DEC;
  }
  if (error.meridian.east || error.meridian.west) return (uint8_t)ERR_MERIDIAN;
  if (initError.nv || initError.value) return (uint8_t)ERR_NV_INIT;
  if (initError.tls) return (uint8_t)ERR_SITE_INIT;
  if (initError.weather) return (uint8_t)ERR_WEATHER_INIT;
  return ERR_NONE;
}

void Limits::stop(GuideAction stopDirection) {
  #if SLEW_GOTO == ON
    goTo.stop();
  #endif
  guide.stopAxis1(stopDirection, true);
  guide.stopAxis2(stopDirection, true);
  mount.tracking(false);
}

void Limits::stopAxis1(GuideAction stopDirection) {
  #if SLEW_GOTO == ON
    goTo.stop();
  #endif
  guide.stopAxis1(stopDirection, true);
  if (stopDirection == GA_FORWARD) mount.tracking(false);
}

void Limits::stopAxis2(GuideAction stopDirection) {
  #if SLEW_GOTO == ON
    goTo.stop();
  #endif
  guide.stopAxis2(stopDirection, true);
}

void Limits::poll() {
  if (!limitsEnabled) return;

  static int autoFlipCount = 0;

  LimitsError lastError = error;

  Coordinate current = mount.getMountPosition(CR_MOUNT_ALT);

  if (current.a < settings.altitude.min) error.altitude.min = true; else error.altitude.min = false;
  if (current.a > settings.altitude.max) error.altitude.max = true; else error.altitude.max = false;

  if (transform.meridianFlips && current.pierSide == PIER_SIDE_EAST) {
    if (current.h < -settings.pastMeridianE) { stopAxis1(GA_REVERSE); error.meridian.east = true; } else error.meridian.east = false;
  } else error.meridian.east = false;
  if (transform.meridianFlips && current.pierSide == PIER_SIDE_WEST) {
    if (autoFlipCount == 0) {
      if (current.h > settings.pastMeridianW) {
        #if SLEW_GOTO == ON
          if (goTo.isAutoFlipEnabled() && mount.isTracking()) {
            // disable meridian limit west for a second to allow goto to exit the out of limits region
            autoFlipCount = 10;
            VLF("MSG: Mount, start automatic meridian flip");
            Coordinate target = mount.getMountPosition();
            CommandError e = goTo.request(&target, PSS_EAST_ONLY, false);
            if (e != CE_NONE) {
              stopAxis1(GA_FORWARD);
              error.meridian.west = true; VF("MSG: Limits::limitPoll() goto for automatic meridian flip failed ("); V(e); VL(")");
            }
          } else
        #endif
        { stopAxis1(GA_FORWARD); error.meridian.west = true; }
      } else error.meridian.west = false;
    } else { autoFlipCount--; error.meridian.west = false; }
  } else error.meridian.west = false;

  if (flt(current.a1, axis1.settings.limits.min)) { stopAxis1(GA_REVERSE); error.limit.axis1.min = true; } else error.limit.axis1.min = false;
  if (fgt(current.a1, axis1.settings.limits.max)) { stopAxis1(GA_FORWARD); error.limit.axis1.max = true; } else error.limit.axis1.max = false;
  if (flt(current.a2, axis2.settings.limits.min)) { stopAxis2((current.pierSide == PIER_SIDE_EAST)?GA_REVERSE:GA_FORWARD); error.limit.axis2.min = true; } else error.limit.axis2.min = false;
  if (fgt(current.a2, axis2.settings.limits.max)) { stopAxis2((current.pierSide == PIER_SIDE_EAST)?GA_FORWARD:GA_REVERSE); error.limit.axis2.max = true; } else error.limit.axis2.max = false;

  #if DEBUG == VERBOSE
    const char* errPre = "MSG: Mount, Error state changed: ";
    if (lastError.altitude.min != error.altitude.min ||
        lastError.altitude.max != error.altitude.max ||
        lastError.meridian.east != error.meridian.east ||
        lastError.meridian.west != error.meridian.west ||
        lastError.limit.axis1.min != error.limit.axis1.min ||
        lastError.limit.axis1.max != error.limit.axis1.max ||
        lastError.limit.axis2.min != error.limit.axis2.min ||
        lastError.limit.axis2.max != error.limit.axis2.max) {
      V(errPre);
      V(error.altitude.min?    "Alt-! " :"Alt-. ");
      V(error.altitude.max?    "Alt+! " :"Alt+. ");
      V(error.meridian.east?   "ME! "  :"ME. ");
      V(error.meridian.west?   "MW! "  :"MW. ");
      V(error.limit.axis1.min? "Ax1-! ":"Ax1-. ");
      V(error.limit.axis1.max? "Ax1+! ":"Ax1+. ");
      V(error.limit.axis2.min? "Ax2-! ":"Ax2-. ");
      VL(error.limit.axis2.max?"Ax2+!"  :"Ax2+.");
    }
  #endif

  if (transform.mountType == ALTAZM) {
    if (error.altitude.min) stopAxis2(GA_REVERSE);
    if (error.altitude.max) stopAxis2(GA_FORWARD);
  } else {
    if (!lastError.altitude.min && error.altitude.min) stop(GA_BREAK);
    if (!lastError.altitude.max && error.altitude.max) stop(GA_BREAK);
  }
}

Limits limits;

#endif
