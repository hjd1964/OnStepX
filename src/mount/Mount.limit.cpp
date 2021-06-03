//--------------------------------------------------------------------------------------------------
// telescope mount control, limits

#include "../Common.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../tasks/OnTask.h"
extern Tasks tasks;

#include "../telescope/Telescope.h"
#include "Mount.h"

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
  if (!limitsEnabled) return;
  const char* errPre = "MSG: Mount::limitPoll() exceeded ";

  updatePosition(CR_MOUNT_ALT);
  if (current.a < limits.altitude.min) { limitStop(GA_BREAK); error.altitude.min = true; V(errPre); VF("min altitude "); VL(radToDeg(limits.altitude.min)); } else error.altitude.min = false;
  if (current.a > limits.altitude.max) { limitStop(GA_BREAK); error.altitude.max = true; V(errPre); VF("max altitude "); VL(radToDeg(limits.altitude.max)); } else error.altitude.max = false;

  if (meridianFlip != MF_NEVER && current.pierSide == PIER_SIDE_EAST) {
    if (current.h < -limits.pastMeridianE) { limitStopAxis1(GA_REVERSE); error.meridian.east = true; V(errPre); VLF("meridian East"); } else error.meridian.east = false;
  } else error.meridian.east = false;
  if (meridianFlip != MF_NEVER && current.pierSide == PIER_SIDE_WEST) {
    if (current.h > limits.pastMeridianW) { limitStopAxis1(GA_FORWARD); error.meridian.west = true; V(errPre); VLF("meridian West"); } else error.meridian.west = false;
  } else error.meridian.west = false;

  if (current.a1 < axis1.settings.limits.min) { limitStopAxis1(GA_REVERSE); error.limit.axis1.min = true; V(errPre); VLF(" min axis1"); } else error.limit.axis1.min = false;
  if (current.a1 > axis1.settings.limits.max) { limitStopAxis1(GA_FORWARD); error.limit.axis1.max = true; V(errPre); VLF("max axis1"); } else error.limit.axis1.max = false;
  if (current.a2 < axis2.settings.limits.min) { limitStopAxis2((current.pierSide == PIER_SIDE_EAST)?GA_REVERSE:GA_FORWARD); error.limit.axis2.min = true; V(errPre); VLF("min axis2"); } else error.limit.axis2.min = false;
  if (current.a2 > axis2.settings.limits.max) { limitStopAxis2((current.pierSide == PIER_SIDE_EAST)?GA_FORWARD:GA_REVERSE); error.limit.axis2.max = true; V(errPre); VLF("max axis2"); } else error.limit.axis2.max = false;
}

void Mount::limitStop(GuideAction stopDirection) {
  gotoStop();
  guideStopAxis1(stopDirection);
  guideStopAxis2(stopDirection);
  trackingState = TS_NONE;
}

void Mount::limitStopAxis1(GuideAction stopDirection) {
  gotoStop();
  guideStopAxis1(stopDirection);
  if (stopDirection == GA_FORWARD) trackingState = TS_NONE;
}

void Mount::limitStopAxis2(GuideAction stopDirection) {
  gotoStop();
  guideStopAxis2(stopDirection);
}

bool Mount::errorAny() {
  return error.altitude.min ||
         error.altitude.max ||
         error.initNV ||
         error.initSite ||
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
  if (error.initNV) return (uint8_t)ERR_NV_INIT;
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
  if (error.initSite) return (uint8_t)ERR_SITE_INIT;
  if (error.initWeather) return (uint8_t)ERR_WEATHER_INIT;
  return ERR_NONE;
}

#endif
