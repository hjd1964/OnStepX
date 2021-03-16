//--------------------------------------------------------------------------------------------------
// telescope mount control, limits
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
#include "../commands/ProcessCmds.h"
#include "../motion/Axis.h"
#include "../telescope/Telescope.h"
extern Telescope telescope;
#include "Mount.h"

inline void mountLimitWrapper() { telescope.mount.limitPoll(); }

void Mount::limitInit() {
  // get limit settings from NV
  if (LimitsSize < sizeof(Limits)) { DL("ERR: Mount::limitInit(); LimitsSize error NV subsystem writes disabled"); nv.readOnly(true); }
  nv.readBytes(NV_LIMITS_BASE, &limits, LimitsSize);

  // start limit monitor task
  VF("MSG: Mount, start limit monitor task (rate 10ms priority 3)... ");
  if (tasks.add(10, 0, true, 3, mountLimitWrapper, "MntLmt")) VL("success"); else VL("FAILED!");
}

void Mount::limitPoll() {
  if (!limitsEnabled) return;

  updatePosition(CR_MOUNT_ALT); Y;
  double a1, a2;
  if (transform.mountType == ALTAZM) a1 = current.z; else a1 = current.h;

  #if AXIS2_TANGENT_ARM == ON
    a2 = axis2.getInstrumentCoordinate();
  #else
    if (transform.mountType == ALTAZM) a2 = current.a; else a2 = current.d;
  #endif

  if (current.a < limits.altitude.min) { limitStop(GA_BREAK); error.altitude.min = true; } else error.altitude.min = false;
  if (current.a > limits.altitude.max) { limitStop(GA_BREAK); error.altitude.max = true; } else error.altitude.max = false;

  if (meridianFlip != MF_NEVER && current.pierSide == PIER_SIDE_EAST) {
  //VF("Axis1: current.h "); V(radToDeg(current.h)); VF(" < "); V(radToDeg(-limits.pastMeridianE)); VLF(" -pastMeridianE");
    if (current.h < -limits.pastMeridianE) { limitStop(GA_REVERSE); error.meridian.east = true; } else error.meridian.east = false;
  } else error.meridian.east = false;
  if (meridianFlip != MF_NEVER && current.pierSide == PIER_SIDE_WEST) {
  //VF("Axis1: current.h "); V(radToDeg(current.h)); VF(" > "); V(radToDeg(limits.pastMeridianW)); VLF(" pastMeridianW");
    if (current.h > limits.pastMeridianW) { limitStop(GA_FORWARD); error.meridian.west = true; } else error.meridian.west = false;
  } else error.meridian.west = false;

//VF("Axis1: Min "); V(radToDeg(axis1.settings.limits.min)); VF(" < "); V(radToDeg(a1)); VF(" < "); VL(radToDeg(axis1.settings.limits.max));
//VF("Axis2: Min "); V(radToDeg(axis2.settings.limits.min)); VF(" < "); V(radToDeg(a2)); VF(" < "); VL(radToDeg(axis2.settings.limits.max));
  if (a1 < axis1.settings.limits.min) { limitStopAxis1(GA_REVERSE); error.limit.axis1.min = true; } else error.limit.axis1.min = false;
  if (a1 > axis1.settings.limits.max) { limitStopAxis1(GA_FORWARD); error.limit.axis1.max = true; } else error.limit.axis1.max = false;
  if (a2 < axis2.settings.limits.min) { limitStopAxis2((current.pierSide == PIER_SIDE_EAST)?GA_REVERSE:GA_FORWARD); error.limit.axis2.min = true; } else error.limit.axis2.min = false;
  if (a2 > axis2.settings.limits.max) { limitStopAxis2((current.pierSide == PIER_SIDE_EAST)?GA_FORWARD:GA_REVERSE); error.limit.axis2.max = true; } else error.limit.axis2.max = false;
}

void Mount::limitStop(GuideAction stopDirection) {
  gotoStop();
  guideStopAxis1(stopDirection);
  guideStopAxis2(stopDirection);
}

void Mount::limitStopAxis1(GuideAction stopDirection) {
  gotoStop();
  guideStopAxis1(stopDirection);
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
}

#endif
