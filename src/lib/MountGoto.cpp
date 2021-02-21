//--------------------------------------------------------------------------------------------------
// telescope mount control, sync and goto
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../coordinates/Transform.h"
extern Transform transform;
#include "../tasks/OnTask.h"
extern Tasks tasks;
#include "Axis.h"
extern Axis axis1;
extern Axis axis2;
#include "Mount.h"
extern void mountMonitorWrapper();

CommandError Mount::validateGoto() {
  if ( axis1.fault()     ||  axis2.fault())     return CE_SLEW_ERR_HARDWARE_FAULT;
  if (!axis1.isEnabled() || !axis2.isEnabled()) return CE_SLEW_ERR_IN_STANDBY;
  if (parkState != PS_UNPARKED)                 return CE_SLEW_ERR_IN_PARK;
  if (guideState != GU_NONE)                    return CE_MOUNT_IN_MOTION;
  if (gotoState == GS_GOTO_SYNC)                return CE_MOUNT_IN_MOTION;
  if (gotoState != GS_NONE)                     return CE_SLEW_IN_SLEW;
  return CE_NONE;
}

CommandError Mount::validateGotoCoords(Coordinate coords) {
  transform.equToHor(&coords);
  if (coords.a < limits.minAltitude)            return CE_GOTO_ERR_BELOW_HORIZON;
  if (coords.a > limits.maxAltitude)            return CE_GOTO_ERR_ABOVE_OVERHEAD;
  if (AXIS2_TANGENT_ARM == OFF && mountType != ALTAZM) {
    if (coords.d < axis2.getMinCoordinate())    return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (coords.d > axis2.getMaxCoordinate())    return CE_SLEW_ERR_OUTSIDE_LIMITS;
  }
  if (coords.h < axis1.getMinCoordinate())      return CE_SLEW_ERR_OUTSIDE_LIMITS;
  if (coords.h > axis1.getMaxCoordinate())      return CE_SLEW_ERR_OUTSIDE_LIMITS;
  return CE_NONE;
}

CommandError Mount::setMountTarget(Coordinate *coords) {
  target = *coords;

  CommandError e = validateGoto();
  if (e == CE_SLEW_ERR_IN_STANDBY && atHome) { tracking = true; axis1.enable(true); axis2.enable(true); e = validateGoto(); }
  if (e != CE_NONE) return e;

  e = validateGotoCoords(target);
  if (e != CE_NONE) return e;

  double a1, a2;
  transform.nativeToMount(&target, &a1, &a2);

  // east side of pier - we're in the western sky and the HA's are positive
  // west side of pier - we're in the eastern sky and the HA's are negative
  updatePosition();
  target.pierSide = current.pierSide;
  if (meridianFlip == MF_ALWAYS) {
    if (atHome) { if (a1 < 0) target.pierSide = PIER_SIDE_WEST; else target.pierSide = PIER_SIDE_EAST; } else
    #if PIER_SIDE_SYNC_CHANGE_SIDES == ON
      if (preferredPierSide == WEST) { newPierSide = PIER_SIDE_WEST; if (a1 >  limits.pastMeridianW) target.pierSide = PIER_SIDE_EAST; } else
      if (preferredPierSide == EAST) { newPierSide = PIER_SIDE_EAST; if (a1 < -limits.pastMeridianE) target.pierSide = PIER_SIDE_WEST; } else
    #endif
      {// preferredPierSideDefault == BEST
        if (current.pierSide == PIER_SIDE_WEST && a1 >  limits.pastMeridianW) target.pierSide = PIER_SIDE_EAST;
        if (current.pierSide == PIER_SIDE_EAST && a1 < -limits.pastMeridianE) target.pierSide = PIER_SIDE_WEST;
      }
    #if PIER_SIDE_SYNC_CHANGE_SIDES == OFF
      if (!atHome && target.pierSide != current.pierSide) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    #endif
  } else {
    // east side of pier is always the default polar-home position
    target.pierSide = PIER_SIDE_EAST;
  }
  return CE_NONE;
}

CommandError Mount::syncEqu(Coordinate *coords) {
  CommandError e = setMountTarget(coords);
  if (e != CE_NONE) return e;
  
  double a1, a2;
  transform.mountToInstrument(&target, &a1, &a2);
  axis1.setInstrumentCoordinate(a1);
  axis2.setInstrumentCoordinate(a2);

  safetyLimitsOn = true;
  syncToEncodersOnly = true;

  VLF("MSG: Mount::syncEqu, instrument coordinates updated");

  return CE_NONE;
}

CommandError Mount::gotoEqu(Coordinate *coords) {
  CommandError e = setMountTarget(coords);
  if (e != CE_NONE) return e;

  safetyLimitsOn = true;
  syncToEncodersOnly = true;

  destination = target;
  start = target;

  // prepare goto, check for any waypoints and set them if found
  gotoState = GS_GOTO;
  gotoStage = GG_DESTINATION;
  if (MFLIP_SKIP_HOME == OFF && mountType != ALTAZM && start.pierSide != destination.pierSide) setWaypoint();

  // set the mount target
  double a1, a2;
  transform.mountToInstrument(&destination, &a1, &a2);
  axis1.setTracking(false);
  axis2.setTracking(false);
  axis1.setTargetCoordinate(a1);
  axis2.setTargetCoordinate(a2);

  // start the goto monitor
  if (monitorTaskHandle != 0) tasks.remove(monitorTaskHandle);
  monitorTaskHandle = tasks.add(0, 0, true, 0, mountMonitorWrapper);
  tasks.setPeriod(monitorTaskHandle,10);

  VLF("MSG: Mount::gotoEqu, target coordinates set");

  return CE_NONE;
}

void Mount::monitor() {
  double a1, a2;
  if (gotoStage == GG_WAYPOINT) {
    if (axis1.nearTarget() && axis2.nearTarget()) {
      gotoStage = GG_DESTINATION;
      destination = target;
      transform.mountToInstrument(&destination, &a1, &a2);
      axis1.setTargetCoordinate(a1);
      axis2.setTargetCoordinate(a2);
    }
  } else
  if (gotoStage == GG_DESTINATION) {
    if (axis1.nearTarget() && axis2.nearTarget()) {
      gotoState = GS_NONE;
      gotoStage = GG_NONE;
      if (tracking == TS_SIDEREAL) { axis1.setTracking(true); axis2.setTracking(true); }
      tasks.setDurationComplete(monitorTaskHandle);
      monitorTaskHandle = 0;
      return;
    }

    if (mountType == ALTAZM) transform.equToHor(&target);
    transform.mountToInstrument(&target, &a1, &a2);
    axis1.setTargetCoordinate(a1);
    axis2.setTargetCoordinate(a2);
  }

  if (tracking == TS_SIDEREAL) target.h += radsPerCentisecond;
}

void Mount::setWaypoint() {
  // HA goes from +90...0..-90
  //                W   .   E
  // meridian flip, first phase, only happens for GEM mounts

  // first phase, decide if we should move to 60 deg. HA (4 hours) to get away from the horizon limits or just go straight to the home position
  if (home.h == 0.0) destination.h = 0.0; else {
    if (current.pierSide == PIER_SIDE_WEST) {
      if (current.a < degToRad(10.0) && start.h > degToRad(-90.0)) destination.h = degToRad(-60.0); else destination.h = -home.h;
    } else {
      if (current.a < degToRad(10.0) && start.h < degToRad(90.0)) destination.h = degToRad(60.0); else destination.h = home.h;
    }
  }
  destination.d = home.d;

  // first phase, override for additional waypoints
  // if Dec is in the general area of the pole slew both axis back at once, or...
  // if we're at a low latitude and in the opposite sky, |HA|=6 is very low on the horizon in this orientation and we need to delay arriving there during a meridian flip
  // in the extreme case, where the user is very near the Earths equator an Horizon limit of -10 or -15 may be necessary for proper operation.
  if (home.h > 0.0) {
    if (current.d > degToRad(90.0) - transform.site.latitude.value) {
      if (current.pierSide == PIER_SIDE_WEST) destination.h = -home.h; else destination.h = home.h;
    } else {
      if (current.a < degToRad(20.0) && transform.site.latitude.absval < degToRad(45.0) && current.d < 0.0) {
        if (current.pierSide == PIER_SIDE_WEST) destination.h = degToRad(-45.0); else destination.h = degToRad(45.0);
      }
    }
  } else {
    if (current.d < degToRad(-90.0) - transform.site.latitude.value) {
      if (current.pierSide == PIER_SIDE_WEST) destination.h = -home.h; else destination.h = home.h;
    } else { 
      if (current.a < degToRad(20.0) && transform.site.latitude.absval < degToRad(45.0) && current.d > 0.0) {
        if (current.pierSide == PIER_SIDE_WEST) destination.h = degToRad(-45.0); else destination.h = degToRad(45.0);
      }
    }
  }

  gotoStage = GG_WAYPOINT;
}

#endif