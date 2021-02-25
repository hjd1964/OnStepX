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
  if (parkState  != PS_UNPARKED)                return CE_SLEW_ERR_IN_PARK;
  if (guideState != GU_NONE)                    return CE_SLEW_IN_MOTION;
  if (gotoState  == GS_GOTO_SYNC)               return CE_SLEW_IN_MOTION;
  if (gotoState  != GS_NONE)                    return CE_SLEW_IN_SLEW;
  return CE_NONE;
}

CommandError Mount::validateGotoCoords(Coordinate *coords) {
  if (coords->a < limits.minAltitude)           return CE_SLEW_ERR_BELOW_HORIZON;
  if (coords->a > limits.maxAltitude)           return CE_SLEW_ERR_ABOVE_OVERHEAD;
  if (AXIS2_TANGENT_ARM == OFF && mountType != ALTAZM) {
    if (coords->d < axis2.getMinCoordinate())   return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (coords->d > axis2.getMaxCoordinate())   return CE_SLEW_ERR_OUTSIDE_LIMITS;
  }
  if (coords->h < axis1.getMinCoordinate())     return CE_SLEW_ERR_OUTSIDE_LIMITS;
  if (coords->h > axis1.getMaxCoordinate())     return CE_SLEW_ERR_OUTSIDE_LIMITS;
  return CE_NONE;
}

CommandError Mount::setMountTarget(Coordinate *coords, bool allowPierSideChange) {
  CommandError e = validateGoto();
  if (e == CE_SLEW_ERR_IN_STANDBY && atHome) { tracking = true; axis1.enable(true); axis2.enable(true); e = validateGoto(); }
  if (e != CE_NONE) return e;

  target = *coords;
  transform.rightAscensionToHourAngle(&target);
  transform.equToHor(&target);
  e = validateGotoCoords(&target);
  if (e != CE_NONE) return e;

  // east side of pier - we're in the western sky and the HA's are positive
  // west side of pier - we're in the eastern sky and the HA's are negative
  updatePosition();
  target.pierSide = current.pierSide;

  double a1, a2;
  transform.nativeToMount(&target, &a1, &a2);

  D("limit.pastMeridianE="); DL(radToDeg(limits.pastMeridianE));
  D("preferredPierSide="); DL(preferredPierSide);
  DL("");
  DL("Start:");
  D(" h="); DL(radToDeg(a1));
  D(" d="); DL(radToDeg(a2));
  D(" p="); DL(target.pierSide);

  if (meridianFlip == MF_ALWAYS) {
    if (atHome) {
      if (a1 < 0) target.pierSide = PIER_SIDE_WEST; else target.pierSide = PIER_SIDE_EAST;
    } else
    if (preferredPierSide == EAST && allowPierSideChange) {
      if (a1 < -limits.pastMeridianE) target.pierSide = PIER_SIDE_WEST; else target.pierSide = PIER_SIDE_EAST;
    } else
    if (preferredPierSide == WEST && allowPierSideChange) {
      if (a1 >  limits.pastMeridianW) target.pierSide = PIER_SIDE_EAST; else target.pierSide = PIER_SIDE_WEST;
    } else
    if (preferredPierSide == BEST) {
      if (current.pierSide == PIER_SIDE_WEST && a1 >  limits.pastMeridianW) target.pierSide = PIER_SIDE_EAST;
      if (current.pierSide == PIER_SIDE_EAST && a1 < -limits.pastMeridianE) target.pierSide = PIER_SIDE_WEST;
    }
    if (!atHome && !allowPierSideChange && target.pierSide != current.pierSide) return CE_SLEW_ERR_OUTSIDE_LIMITS;
  } else {
    // east side of pier is always the default polar-home position
    target.pierSide = PIER_SIDE_EAST;
  }
  return CE_NONE;
}

CommandError Mount::syncEqu(Coordinate *coords) {
  CommandError e = setMountTarget(coords, PIER_SIDE_SYNC_CHANGE_SIDES == ON);
  if (e != CE_NONE) return e;
  
  double a1, a2;
  transform.mountToInstrument(&target, &a1, &a2);
  axis1.setInstrumentCoordinate(a1);
  axis2.setInstrumentCoordinate(a2);

  DL("Final:");
  D(" h="); DL(radToDeg(a1));
  D(" d="); DL(radToDeg(a2));
  D(" p="); DL(target.pierSide);

  safetyLimitsOn = true;
  syncToEncodersOnly = true;

  VLF("MSG: Mount::syncEqu, instrument coordinates updated");

  return CE_NONE;
}

CommandError Mount::gotoEqu(Coordinate *coords) {
  CommandError e = setMountTarget(coords, true);
  if (e != CE_NONE) return e;

  safetyLimitsOn = true;
  syncToEncodersOnly = true;

  updatePosition();
  transform.equToHor(&current);
  start = current;
  destination = target;

  // prepare goto, check for any waypoints and set them if found
  gotoState = GS_GOTO;
  gotoStage = GG_DESTINATION;
  if (MFLIP_SKIP_HOME == OFF && mountType != ALTAZM && start.pierSide != destination.pierSide) {
    VLF("MSG: Mount::gotoEqu, goto changes pier side, attempting to set waypoint");
    setWaypoint();
  }

  // set the mount target
  double a1, a2;
  transform.mountToInstrument(&destination, &a1, &a2);

  // locked tracking with movement within axisn ISR, off
  axis1.setTracking(false); axis2.setTracking(false);
  VLF("MSG: Mount::gotoEqu, axis tracking stopped");

  axis1.setTargetCoordinate(a1);
  axis2.setTargetCoordinate(a2);
  axis1.setOriginCoordinate();
  axis2.setOriginCoordinate();
  VLF("MSG: Mount::gotoEqu, target coordinates set");

  DL("Final:");
  D(" h="); DL(radToDeg(a1));
  D(" d="); DL(radToDeg(a2));
  D(" p="); DL(destination.pierSide);

  // set rate limit, in sidereal
  gotoRateLimitAxis1 = ((1000000.0/usPerStepCurrent)/(axis1.getStepsPerMeasure()/RAD))*240.0;
  gotoRateLimitAxis2 = ((1000000.0/usPerStepCurrent)/(axis2.getStepsPerMeasure()/RAD))*240.0;

  gotoRateAxis1 = TRACK_BACKLASH_RATE;
  gotoRateAxis2 = TRACK_BACKLASH_RATE;
  updateTrackingRates();

  // start the goto monitor
  if (monitorTaskHandle != 0) tasks.remove(monitorTaskHandle);
  monitorTaskHandle = tasks.add(0, 0, true, 0, mountMonitorWrapper);
  tasks.setPeriod(monitorTaskHandle,10);
  if (monitorTaskHandle) VLF("MSG: Mount::gotoEqu, mount monitor started"); else DLF("MSG: Mount::gotoEqu, mount monitor failed to start");

  return CE_NONE;
}

void Mount::monitor() {
  double a1, a2;
  if (gotoStage == GG_WAYPOINT) {
    if (axis1.nearTarget() && axis2.nearTarget()) {
      if (destination.h == home.h && destination.d == home.d && destination.pierSide == home.pierSide) {
        VLF("MSG: Mount::monitor, home reached");
        gotoStage = GG_DESTINATION;
        destination = target;
      } else {
        VLF("MSG: Mount::monitor, waypoint reached");
        destination = home;
      }
      transform.mountToInstrument(&destination, &a1, &a2);
      axis1.setTargetCoordinate(a1);
      axis2.setTargetCoordinate(a2);
      axis1.setOriginCoordinate();
      axis2.setOriginCoordinate();
    }
  } else
  if (gotoStage == GG_DESTINATION) {
    if (axis1.nearTarget() && axis2.nearTarget()) {
      VLF("MSG: Mount::monitor, destination reached");

      // flag the goto as finished
      gotoState = GS_NONE;
      gotoStage = GG_NONE;

      // back to normal tracking
      gotoRateAxis1 = 0.0;
      gotoRateAxis2 = 0.0;
      updateTrackingRates();

      // one last check to be sure the full capability ISR's are active
      moveFastAxis1 = false; axis1.enableMoveFast(false);
      moveFastAxis2 = false; axis2.enableMoveFast(false);

      // locked tracking with movement within axisn ISR, on
      axis1.setTracking(true); axis2.setTracking(true);
      VLF("MSG: Mount::monitor, axis tracking resumed");

      // kill this monitor
      tasks.setDurationComplete(monitorTaskHandle);
      monitorTaskHandle = 0;
      VLF("MSG: Mount::monitor, monitor terminated");
      return;
    }

    // automatically switch to fast ISR's at high speed
    if ( moveFastAxis1 && gotoRateAxis1 < TRACK_BACKLASH_RATE*1.2) { moveFastAxis1 = false; axis1.enableMoveFast(false); }
    if (!moveFastAxis1 && gotoRateAxis1 > TRACK_BACKLASH_RATE*1.2) { moveFastAxis1 = true;  axis1.enableMoveFast(true);  }
    if ( moveFastAxis2 && gotoRateAxis2 < TRACK_BACKLASH_RATE*1.2) { moveFastAxis2 = false; axis2.enableMoveFast(false); }
    if (!moveFastAxis2 && gotoRateAxis2 > TRACK_BACKLASH_RATE*1.2) { moveFastAxis2 = true;  axis2.enableMoveFast(true);  }

    // keep updating the target
    if (mountType == ALTAZM) transform.equToHor(&target);
    transform.mountToInstrument(&target, &a1, &a2);
    axis1.setTargetCoordinate(a1);
    axis2.setTargetCoordinate(a2);

  }

  // keep moving the target
  target.h += radsPerCentisecond;

  // acceleration
  gotoRateAxis1 = (radToDeg(axis1.getOriginOrTargetDistance())/SLEW_ACCELERATION_DIST)*gotoRateLimitAxis1 + TRACK_BACKLASH_RATE;
  if (gotoRateAxis1 < TRACK_BACKLASH_RATE) gotoRateAxis1 = TRACK_BACKLASH_RATE; else
  if (gotoRateAxis1 > gotoRateLimitAxis1)  gotoRateAxis1 = gotoRateLimitAxis1;
  gotoRateAxis2 = (radToDeg(axis2.getOriginOrTargetDistance())/SLEW_ACCELERATION_DIST)*gotoRateLimitAxis2 + TRACK_BACKLASH_RATE;
  if (gotoRateAxis2 < TRACK_BACKLASH_RATE) gotoRateAxis2 = TRACK_BACKLASH_RATE; else
  if (gotoRateAxis2 > gotoRateLimitAxis2)  gotoRateAxis2 = gotoRateLimitAxis2;
  updateTrackingRates();
}

void Mount::setWaypoint() {
  // HA goes from +90...0..-90
  //                W   .   E
  // meridian flip, only happens for equatorial mounts

  gotoStage = GG_WAYPOINT;

  // default goes straight to the home position
  destination = home;

  // if the home position is at 0 hours, we're done
  if (home.h == 0.0) return;

  // decide if we should first move to 60 deg. HA (4 hours) to get away from the horizon limits
  if (current.a < Deg10 && fabs(start.h) > Deg90) destination.h = Deg60;

  // decide if we should first move to 45 deg. HA (3 hours) to get away from the horizon limits
  // if at a low latitude and in the opposite sky, |HA| = 6 is very low on the horizon and we need
  // to delay arriving there during a meridian flip.  In the extreme case, where the user is very
  // near the Earths equator an Horizon limit of -10 or -15 may be necessary for proper operation
  if (current.a < Deg20 && transform.site.latitude.absval < Deg45) {
    if (transform.site.latitude.value >= 0) {
      if (current.d <= Deg90 - transform.site.latitude.value) destination.h = Deg45;
    } else {
      if (current.d >= -Deg90 - transform.site.latitude.value) destination.h = Deg45;
    }
  }
}

#endif