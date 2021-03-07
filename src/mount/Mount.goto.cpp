//--------------------------------------------------------------------------------------------------
// telescope mount control, sync and goto
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../coordinates/Transform.h"
#include "../motion/Axis.h"
#include "../telescope/Telescope.h"
extern Telescope telescope;
#include "Mount.h"

inline void mountGotoWrapper() { telescope.mount.gotoPoll(); }

CommandError Mount::validateGoto() {
  if ( axis1.fault()     ||  axis2.fault())        return CE_SLEW_ERR_HARDWARE_FAULT;
  if (!axis1.isEnabled() || !axis2.isEnabled())    return CE_SLEW_ERR_IN_STANDBY;
  if (parkState  != PS_UNPARKED)                   return CE_SLEW_ERR_IN_PARK;
  if (guideState != GU_NONE)                       return CE_SLEW_IN_MOTION;
  if (gotoState  == GS_GOTO_SYNC)                  return CE_SLEW_IN_MOTION;
  if (gotoState  != GS_NONE)                       return CE_SLEW_IN_SLEW;
  return CE_NONE;
}

CommandError Mount::validateGotoCoords(Coordinate *coords) {
  if (flt(coords->a, limits.altitude.min))         return CE_SLEW_ERR_BELOW_HORIZON;
  if (fgt(coords->a, limits.altitude.max))         return CE_SLEW_ERR_ABOVE_OVERHEAD;
  if (AXIS2_TANGENT_ARM == OFF && transform.mountType != ALTAZM) {
    if (flt(coords->d, axis2.settings.limits.min)) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    if (fgt(coords->d, axis2.settings.limits.max)) return CE_SLEW_ERR_OUTSIDE_LIMITS;
  }
  if (flt(coords->h, axis1.settings.limits.min))   return CE_SLEW_ERR_OUTSIDE_LIMITS;
  if (fgt(coords->h, axis1.settings.limits.max))   return CE_SLEW_ERR_OUTSIDE_LIMITS;
  return CE_NONE;
}

CommandError Mount::setMountTarget(Coordinate *coords, PierSideSelect pierSideSelect) {
  CommandError e = validateGoto();
  if (e == CE_SLEW_ERR_IN_STANDBY && atHome) { axis1.enable(true); axis2.enable(true); e = validateGoto(); }
  if (e != CE_NONE) return e;

  target = *coords;
  transform.rightAscensionToHourAngle(&target);
  transform.equToHor(&target);
  e = validateGotoCoords(&target);
  if (e != CE_NONE) return e;

  // east side of pier is always the default polar-home position
  // east side of pier - we're in the western sky and the HA's are positive
  // west side of pier - we're in the eastern sky and the HA's are negative

  updatePosition();
  target.pierSide = current.pierSide;

  double a1, a2;
  transform.nativeToMount(&target, &a1, &a2);

  if (meridianFlip != MF_ALWAYS) { target.pierSide = PIER_SIDE_EAST; return CE_NONE; }

  if (atHome) {
    if (a1 < 0) target.pierSide = PIER_SIDE_WEST; else target.pierSide = PIER_SIDE_EAST;
  } else
  if (pierSideSelect == PSS_EAST || pierSideSelect == PSS_EAST_ONLY) {
    if (a1 < -limits.pastMeridianE) target.pierSide = PIER_SIDE_WEST; else target.pierSide = PIER_SIDE_EAST;
  } else
  if (pierSideSelect == PSS_WEST || pierSideSelect == PSS_WEST_ONLY) {
    if (a1 >  limits.pastMeridianW) target.pierSide = PIER_SIDE_EAST; else target.pierSide = PIER_SIDE_WEST;
  } else
  if (pierSideSelect == PSS_BEST || pierSideSelect == PSS_SAME_ONLY) {
    if (current.pierSide == PIER_SIDE_WEST && a1 >  limits.pastMeridianW) target.pierSide = PIER_SIDE_EAST;
    if (current.pierSide == PIER_SIDE_EAST && a1 < -limits.pastMeridianE) target.pierSide = PIER_SIDE_WEST;
  }

  if (pierSideSelect == PSS_EAST_ONLY && target.pierSide != PIER_SIDE_EAST) return CE_SLEW_ERR_OUTSIDE_LIMITS; else
  if (pierSideSelect == PSS_WEST_ONLY && target.pierSide != PIER_SIDE_WEST) return CE_SLEW_ERR_OUTSIDE_LIMITS; else
  if (pierSideSelect == PSS_SAME_ONLY && target.pierSide != current.pierSide) return CE_SLEW_ERR_OUTSIDE_LIMITS;

  return CE_NONE;
}

CommandError Mount::syncEqu(Coordinate *coords, PierSideSelect pierSideSelect) {
  CommandError e = setMountTarget(coords, pierSideSelect);
  if (e != CE_NONE) return e;
  
  double a1, a2;
  transform.mountToInstrument(&target, &a1, &a2);
  axis1.setInstrumentCoordinate(a1);
  axis2.setInstrumentCoordinate(a2);

  limitsEnabled = true;
  misc.syncToEncodersOnly = false;

  VLF("MSG: Mount::syncEqu, instrument coordinates updated");

  return CE_NONE;
}

CommandError Mount::gotoEqu(Coordinate *coords, PierSideSelect pierSideSelect) {
  CommandError e = setMountTarget(coords, pierSideSelect);
  if (e != CE_NONE) return e;

  limitsEnabled = true;
  misc.syncToEncodersOnly = false;

  updatePosition();
  transform.equToHor(&current);
  start = current;
  destination = target;

  // prepare for goto, check for any waypoints and set them if found
  gotoState = GS_GOTO;
  gotoStage = GG_DESTINATION;
  if (MFLIP_SKIP_HOME == OFF && transform.mountType != ALTAZM && start.pierSide != destination.pierSide) {
    VLF("MSG: Mount::gotoEqu, goto changes pier side, attempting to set waypoint");
    setWaypoint();
  }

  // start the goto monitor
  if (gotoTaskHandle != 0) tasks.remove(gotoTaskHandle);
  gotoTaskHandle = tasks.add(10, 0, true, 1, mountGotoWrapper, "MntGoto");
  if (gotoTaskHandle) {
    VLF("MSG: Mount::gotoEqu(); start goto monitor task... success"); 

    axis1.setTracking(false);
    axis2.setTracking(false);
    VLF("MSG: Mount::gotoEqu(); automatic tracking stopped");

    axis1.markOriginCoordinate();
    axis2.markOriginCoordinate();
    VLF("MSG: Mount::gotoEqu(); origin coordinates set");

    double a1, a2;
    transform.mountToInstrument(&destination, &a1, &a2);
    axis1.setTargetCoordinate(a1);
    axis2.setTargetCoordinate(a2);
    VLF("MSG: Mount::gotoEqu(); target coordinates set");

    // slew rate in rads per second
    double slewRateAxis1 = (1000000.0/misc.usPerStepCurrent)/axis1.getStepsPerMeasure();
    double slewRateAxis2 = (1000000.0/misc.usPerStepCurrent)/axis2.getStepsPerMeasure();
    axis1.setFrequencyMax(slewRateAxis1);
    axis2.setFrequencyMax(slewRateAxis2);
    VLF("MSG: Mount::gotoEqu(); slew rate set");

    axis1.autoSlewRateByDistance(degToRad(SLEW_ACCELERATION_DIST));
    axis2.autoSlewRateByDistance(degToRad(SLEW_ACCELERATION_DIST));
    VLF("MSG: Mount::gotoEqu(); slew started");

    //updateTrackingRates();
  } else DLF("MSG: Mount::gotoEqu(); start goto monitor task... FAILED!");

  return CE_NONE;
}

void Mount::gotoPoll() {
  if (gotoStage == GG_WAYPOINT) {
    if (axis1.nearTarget() && axis2.nearTarget()) {
      if (destination.h == home.h && destination.d == home.d && destination.pierSide == home.pierSide) {
        VLF("MSG: Mount::gotoPoll(); home reached");
        gotoStage = GG_DESTINATION;
        destination = target;
      } else {
        VLF("MSG: Mount::gotoPoll(); waypoint reached");
        destination = home;
      }

      axis1.autoSlewStop();
      axis2.autoSlewStop();
      VLF("MSG: Mount::gotoPoll(); slew stopped");

      axis1.markOriginCoordinate();
      axis2.markOriginCoordinate();
      VLF("MSG: Mount::gotoPoll(); origin coordinates set");

      double a1, a2;
      transform.mountToInstrument(&destination, &a1, &a2);
      axis1.setTargetCoordinate(a1);
      axis2.setTargetCoordinate(a2);
      VLF("MSG: Mount::gotoPoll(); target coordinates set");

      axis1.autoSlewRateByDistance(degToRad(SLEW_ACCELERATION_DIST));
      axis2.autoSlewRateByDistance(degToRad(SLEW_ACCELERATION_DIST));
      VLF("MSG: Mount::gotoPoll(); slew started");
    }
  } else
  if (gotoStage == GG_DESTINATION) {
    if (axis1.nearTarget() && axis2.nearTarget()) {
      VLF("MSG: Mount::gotoPoll(); destination reached");

      axis1.autoSlewStop();
      axis2.autoSlewStop();
      VLF("MSG: Mount::gotoPoll(); slew stopped");

      // flag the goto as finished
      gotoState = GS_NONE;
      gotoStage = GG_NONE;

      // back to normal tracking
      updateTrackingRates();

      // lock tracking with movement
      axis1.setTracking(true);
      axis2.setTracking(true);
      VLF("MSG: Mount::gotoPoll(); automatic tracking resumed");

      // kill this monitor
      tasks.setDurationComplete(gotoTaskHandle);
      gotoTaskHandle = 0;
      VLF("MSG: Mount::gotoPoll(); goto monitor task terminated");

      return;
    }

    // keep updating the axis targets to match the mount target
    if (transform.mountType == ALTAZM) transform.equToHor(&target);
    double a1, a2;
    transform.mountToInstrument(&target, &a1, &a2);
    axis1.setTargetCoordinate(a1);
    axis2.setTargetCoordinate(a2);
  }

  // keep moving the target (sidereal tracking)
 // target.h += radsPerCentisecond;
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
  if (current.a < Deg20 && transform.site.locationEx.latitude.absval < Deg45) {
    if (transform.site.location.latitude >= 0) {
      if (current.d <= Deg90 - transform.site.location.latitude) destination.h = Deg45;
    } else {
      if (current.d >= -Deg90 - transform.site.location.latitude) destination.h = Deg45;
    }
  }
}

#endif