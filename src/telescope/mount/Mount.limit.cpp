//--------------------------------------------------------------------------------------------------
// telescope mount control, limits

#include "Mount.h"

#ifdef MOUNT_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;
#include "../../telescope/Telescope.h"

inline void mountLimitWrapper() { telescope.mount.limitPoll(); }

void Mount::limitInit(bool validKey) {
  if (LimitsSize < sizeof(Limits)) { initError.nv = true; DL("ERR: Mount::limitInit(); LimitsSize error NV subsystem writes disabled"); nv.readOnly(true); }

  // write the default limits to NV
  if (!validKey) {
    VLF("MSG: Mount, writing default limits to NV");
    nv.writeBytes(NV_MOUNT_LIMITS_BASE, &limits, LimitsSize);
  }

  // get limit settings from NV
  nv.readBytes(NV_MOUNT_LIMITS_BASE, &limits, LimitsSize);

  // start limit monitor task
  VF("MSG: Mount, start limit monitor task (rate 100ms priority 3)... ");
  if (tasks.add(100, 0, true, 3, mountLimitWrapper, "MntLmt")) { VL("success"); } else { VL("FAILED!"); }
}

void Mount::limitPoll() {
  static int autoFlipCount = 0;

  if (!limitsEnabled) return;
  MountError lastError = error;

  updatePosition(CR_MOUNT_ALT);
  if (current.a < limits.altitude.min) error.altitude.min = true; else error.altitude.min = false;
  if (current.a > limits.altitude.max) error.altitude.max = true; else error.altitude.max = false;

  if (meridianFlip != MF_NEVER && current.pierSide == PIER_SIDE_EAST) {
    if (current.h < -limits.pastMeridianE) { limitStopAxis1(GA_REVERSE); error.meridian.east = true; } else error.meridian.east = false;
  } else error.meridian.east = false;
  if (meridianFlip != MF_NEVER && current.pierSide == PIER_SIDE_WEST) {
    if (autoFlipCount == 0) {
      if (current.h > limits.pastMeridianW) {
        if (misc.meridianFlipAuto && trackingState == TS_SIDEREAL) {
          // disable meridian limit west for a second to allow goto to exit the out of limits region
          autoFlipCount = 10;
          VLF("MSG: Mount::limitPoll() start automatic meridian flip");
          updatePosition(CR_MOUNT_EQU);
          Coordinate newTarget = current;
          CommandError e = gotoEqu(&newTarget, PSS_EAST_ONLY, false);
          if (e != CE_NONE) { limitStopAxis1(GA_FORWARD); error.meridian.west = true; VF("MSG: Mount::limitPoll() goto for automatic meridian flip failed ("); V(e); VL(")"); }
        } else { limitStopAxis1(GA_FORWARD); error.meridian.west = true; }
      } else error.meridian.west = false;
    } else { autoFlipCount--; error.meridian.west = false; }
  } else error.meridian.west = false;

  if (flt(current.a1, axis1.settings.limits.min)) { limitStopAxis1(GA_REVERSE); error.limit.axis1.min = true; } else error.limit.axis1.min = false;
  if (fgt(current.a1, axis1.settings.limits.max)) { limitStopAxis1(GA_FORWARD); error.limit.axis1.max = true; } else error.limit.axis1.max = false;
  if (flt(current.a2, axis2.settings.limits.min)) { limitStopAxis2((current.pierSide == PIER_SIDE_EAST)?GA_REVERSE:GA_FORWARD); error.limit.axis2.min = true; } else error.limit.axis2.min = false;
  if (fgt(current.a2, axis2.settings.limits.max)) { limitStopAxis2((current.pierSide == PIER_SIDE_EAST)?GA_FORWARD:GA_REVERSE); error.limit.axis2.max = true; } else error.limit.axis2.max = false;

  #if DEBUG == VERBOSE
    const char* errPre = "MSG: Mount::limitPoll() Error state changed: ";
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
    if (error.altitude.min) limitStopAxis2(GA_REVERSE);
    if (error.altitude.max) limitStopAxis2(GA_FORWARD);
  } else {
    if (!lastError.altitude.min && error.altitude.min) limitStop(GA_BREAK);
    if (!lastError.altitude.max && error.altitude.max) limitStop(GA_BREAK);
  }
}

void Mount::limitStop(GuideAction stopDirection) {
  gotoStop();
  guideStopAxis1(stopDirection, true);
  guideStopAxis2(stopDirection, true);
  trackingState = TS_NONE;
}

void Mount::limitStopAxis1(GuideAction stopDirection) {
  gotoStop();
  guideStopAxis1(stopDirection, true);
  if (stopDirection == GA_FORWARD) trackingState = TS_NONE;
}

void Mount::limitStopAxis2(GuideAction stopDirection) {
  gotoStop();
  guideStopAxis2(stopDirection, true);
}

bool Mount::errorAny() {
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
         error.meridian.west ||
         error.motorFault ||
         error.parkFailed;
}

void Mount::errorReset() {
  // most errors are self healing, once the condition has be rectified they automatically reset
  // park failed is the exception
  error.parkFailed = false;
}

uint8_t Mount::errorNumber() {
  enum GeneralErrors: uint8_t {
  ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT_MIN, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM,
  ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC, ERR_PARK, ERR_GOTO_SYNC, ERR_UNSPECIFIED,
  ERR_ALT_MAX, ERR_WEATHER_INIT, ERR_SITE_INIT, ERR_NV_INIT};

  // priority highest to lowest
  if (error.limitSense.axis1.min || error.limitSense.axis1.max ||
      error.limitSense.axis2.min || error.limitSense.axis2.max) return (uint8_t)ERR_LIMIT_SENSE;
  if (error.motorFault) return (uint8_t)ERR_MOTOR_FAULT;
  if (initError.nv || initError.value) return (uint8_t)ERR_NV_INIT;
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
  if (initError.tls) return (uint8_t)ERR_SITE_INIT;
  if (initError.weather) return (uint8_t)ERR_WEATHER_INIT;
  return ERR_NONE;
}

#endif
