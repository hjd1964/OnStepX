//--------------------------------------------------------------------------------------------------
// telescope mount control, sync and goto

#include "Goto.h"

#if defined(MOUNT_PRESENT) && SLEW_GOTO == ON

#include "../../../tasks/OnTask.h"
extern Tasks tasks;

#include "../../Telescope.h"
#include "../Mount.h"
#include "../coordinates/Transform.h"
#include "../guide/Guide.h"
#include "../home/Home.h"
#include "../park/Park.h"
#include "../limits/Limits.h"
#include "../status/Status.h"

inline void gotoWrapper() { goTo.poll(); }

void Goto::init() {
  // confirm the data structure size
  if (GotoSettingsSize < sizeof(GotoSettings)) { initError.nv = true; DL("ERR: Goto::init(); GotoSettingsSize error NV subsystem writes disabled"); nv.readOnly(true); }

  // write the default settings to NV
  if (!validKey) {
    VLF("MSG: Mount, goto writing default settings to NV");
    nv.writeBytes(NV_MOUNT_GOTO_BASE, &settings, sizeof(GotoSettings));
  }

  // read the settings
  nv.readBytes(NV_MOUNT_GOTO_BASE, &settings, sizeof(GotoSettings));

  // force defaults if needed
  #if MFLIP_PAUSE_HOME_MEMORY != ON
    settings.meridianFlipPause = false;
  #endif

  #if MFLIP_AUTOMATIC_MEMORY != ON
    misc.meridianFlipAuto = false;
  #endif

  // calculate base and current maximum step rates
  usPerStepBase = 1000000.0/((mount.axis1.getStepsPerMeasure()/RAD_DEG_RATIO)*SLEW_RATE_BASE_DESIRED);
  #if SLEW_RATE_MEMORY != ON
    settings.usPerStepCurrent = usPerStepBase;
  #endif
  if (usPerStepBase < usPerStepLowerLimit()) usPerStepBase = usPerStepLowerLimit()*2.0F;
  if (settings.usPerStepCurrent < usPerStepBase/2.0F) settings.usPerStepCurrent = usPerStepBase/2.0F;
  if (settings.usPerStepCurrent > 1000000.0F) settings.usPerStepCurrent = 1000000.0F;
  updateAccelerationRates();
}

// goto to equatorial target position (Native coordinate system) using the defaut preferredPierSide
CommandError Goto::request() {
  return request(&target, preferredPierSide);
}

// goto equatorial position (Native or Mount coordinate system)
CommandError Goto::request(Coordinate *coords, PierSideSelect pierSideSelect, bool native) {

  CommandError e = setTarget(coords, pierSideSelect, native);
  if (e == CE_SLEW_IN_SLEW) { stop(); return e; }
  if (e != CE_NONE) return e;

  limits.enabled(true);
  mount.syncToEncoders(false);
  if (mount.isHome() && park.state != PS_PARKING) mount.tracking(true);

  Coordinate current = mount.getMountPosition(CR_MOUNT_HOR);
  start = current;
  destination = target;

  // prepare for goto, check for any waypoints and set them if found
  state = GS_GOTO;
  stage = GG_DESTINATION;
  if (MFLIP_SKIP_HOME == OFF && transform.mountType != ALTAZM && start.pierSide != destination.pierSide) {
    VLF("MSG: Mount, goto changes pier side, setting waypoint at home");
    waypoint(&current);
  }

  // start the goto monitor
  if (taskHandle != 0) tasks.remove(taskHandle);
  taskHandle = tasks.add(10, 0, true, 1, gotoWrapper, "MntGoto");
  if (taskHandle) {
    VLF("MSG: Mount, start goto monitor task (rate 10ms priority 1)... success");

    double a1, a2;
    transform.mountToInstrument(&destination, &a1, &a2);
    if (park.state == PS_PARKING && stage == GG_DESTINATION) {
      mount.axis1.setTargetCoordinatePark(a1);
      mount.axis2.setTargetCoordinatePark(a2);
    } else {
      mount.axis1.setTargetCoordinate(a1);
      mount.axis2.setTargetCoordinate(a2);
    }
    VF("MSG: Mount, goto target coordinates set (a1="); V(radToDeg(a1)); V("°, a2="); V(radToDeg(a2)); VL("°)");

    // slew rate in rads per second
    mount.axis1.setFrequencySlew(radsPerSecondCurrent);
    mount.axis2.setFrequencySlew(radsPerSecondCurrent);
    VF("MSG: Mount, goto slew rate set ("); V(radToDeg(radsPerSecondCurrent)); VL("°/s)");

    mount.axis1.autoSlewRateByDistance(degToRadF((float)(SLEW_ACCELERATION_DIST)));
    mount.axis2.autoSlewRateByDistance(degToRadF((float)(SLEW_ACCELERATION_DIST)));

    status.sound.alert();

  } else { DLF("MSG: Mount, start goto monitor task... FAILED!"); }

  return CE_NONE;
}

// sync to equatorial target position (Native coordinate system) using the defaut preferredPierSide
CommandError Goto::requestSync() {
  return requestSync(&target, preferredPierSide);
}

// sync to equatorial position (Native or Mount coordinate system)
CommandError Goto::requestSync(Coordinate *coords, PierSideSelect pierSideSelect, bool native) {
  
  CommandError e = setTarget(coords, pierSideSelect, native);
  if (e != CE_NONE) return e;
  
  double a1, a2;
  transform.mountToInstrument(&target, &a1, &a2);

  mount.axis1.setInstrumentCoordinate(a1);
  mount.axis2.setInstrumentCoordinate(a2);
  mount.syncToEncoders(true);

  limits.enabled(true);
  if (mount.isHome()) mount.tracking(true);

  VLF("MSG: Mount, sync instrument coordinates updated");

  return CE_NONE;
}

// get target equatorial position (Native coordinate system)
Coordinate Goto::getTarget() {
  return target;
}

// set target equatorial position (Native coordinate system)
void Goto::setTarget(Coordinate *coords) {
  target = *coords;
}

// converts from native to mount coordinates and checks for valid target
CommandError Goto::setTarget(Coordinate *coords, PierSideSelect pierSideSelect, bool native) {

  CommandError e = validate();
  if (e == CE_SLEW_ERR_IN_STANDBY && mount.isHome()) { mount.enable(true); e = validate(); }
  if (e != CE_NONE) return e;

  target = *coords;
  if (native) transform.nativeToMount(&target);
  transform.equToHor(&target);

  e = limits.validateCoords(&target);
  if (e != CE_NONE) return e;

  // east side of pier is always the default polar-home position
  // east side of pier - we're in the western sky and the HA's are positive
  // west side of pier - we're in the eastern sky and the HA's are negative

  Coordinate current = mount.getMountPosition(CR_MOUNT);
  target.pierSide = current.pierSide;

  double a1;
  if (transform.mountType == ALTAZM) a1 = target.z; else a1 = target.h;

  if (!transform.meridianFlips) { target.pierSide = PIER_SIDE_EAST; return CE_NONE; }

  if (mount.isHome()) {
    if (a1 < 0) target.pierSide = PIER_SIDE_WEST; else target.pierSide = PIER_SIDE_EAST;
  } else
  if (pierSideSelect == PSS_EAST || pierSideSelect == PSS_EAST_ONLY) {
    if (a1 < -limits.settings.pastMeridianE) target.pierSide = PIER_SIDE_WEST; else target.pierSide = PIER_SIDE_EAST;
  } else
  if (pierSideSelect == PSS_WEST || pierSideSelect == PSS_WEST_ONLY) {
    if (a1 >  limits.settings.pastMeridianW) target.pierSide = PIER_SIDE_EAST; else target.pierSide = PIER_SIDE_WEST;
  } else
  if (pierSideSelect == PSS_BEST || pierSideSelect == PSS_SAME_ONLY) {
    if (current.pierSide == PIER_SIDE_WEST && a1 >  limits.settings.pastMeridianW) target.pierSide = PIER_SIDE_EAST;
    if (current.pierSide == PIER_SIDE_EAST && a1 < -limits.settings.pastMeridianE) target.pierSide = PIER_SIDE_WEST;
  }

  if (pierSideSelect == PSS_EAST_ONLY && target.pierSide != PIER_SIDE_EAST) return CE_SLEW_ERR_OUTSIDE_LIMITS; else
  if (pierSideSelect == PSS_WEST_ONLY && target.pierSide != PIER_SIDE_WEST) return CE_SLEW_ERR_OUTSIDE_LIMITS; else
  if (pierSideSelect == PSS_SAME_ONLY && target.pierSide != current.pierSide) return CE_SLEW_ERR_OUTSIDE_LIMITS;

  return CE_NONE;
}

// stop any presently active goto
void Goto::stop() {
  if (state == GS_GOTO && (stage == GG_DESTINATION || stage == GG_WAYPOINT)) stage = GG_READY_ABORT;
}

// general status checks ahead of sync or goto
CommandError Goto::validate() {
  if (mount.axis1.fault())      return CE_SLEW_ERR_HARDWARE_FAULT;
  if (mount.axis2.fault())      return CE_SLEW_ERR_HARDWARE_FAULT;
  if (!mount.axis1.isEnabled()) return CE_SLEW_ERR_IN_STANDBY;
  if (!mount.axis2.isEnabled()) return CE_SLEW_ERR_IN_STANDBY;
  if (park.state == PS_PARKED)  return CE_SLEW_ERR_IN_PARK;
  if (state != GS_NONE)         return CE_SLEW_IN_SLEW;
  if (guide.state != GU_NONE)   return CE_SLEW_IN_MOTION;
  if (mount.isSlewing())        return CE_SLEW_IN_MOTION;
  return CE_NONE;
}

// add an align star (at the current position relative to target)
CommandError Goto::alignAddStar() {
  if (alignState.currentStar > alignState.lastStar) return CE_PARAM_RANGE;

  CommandError e = CE_NONE;

  // first star, get ready for a new pointing model, init/sync then call gta.addStar 
  if (alignState.currentStar == 1) {
    #if ALIGN_MAX_NUM_STARS > 1  
      transform.align.init(site.location.latitude, transform.mountType);
    #endif
    e = requestSync(&target, preferredPierSide);
  }

  // add an align star
  if (e == CE_NONE) {
    Coordinate actual = mount.getMountPosition(CR_MOUNT);
    #if ALIGN_MAX_NUM_STARS > 1  
      e = transform.align.addStar(alignState.currentStar, alignState.lastStar, &target, &actual);
    #endif
    if (e == CE_NONE) alignState.currentStar++;
  }

  return e;
}

// set any additional destinations required for a goto
void Goto::waypoint(Coordinate *current) {
  // HA goes from +90...0..-90
  //                W   .   E
  // meridian flip, only happens for equatorial mounts

  stage = GG_WAYPOINT;

  // default goes straight to the home position
  destination = home.position;

  // if the home position is at 0 hours, we're done
  if (home.position.h == 0.0) return;

  // decide if we should first move to 60 deg. HA (4 hours) to get away from the horizon limits
  if (current->a < Deg10 && fabs(start.h) > Deg90) destination.h = Deg60;

  // decide if we should first move to 45 deg. HA (3 hours) to get away from the horizon limits
  // if at a low latitude and in the opposite sky, |HA| = 6 is very low on the horizon and we need
  // to delay arriving there during a meridian flip.  In the extreme case, where the user is very
  // near the Earths equator an Horizon limit of -10 or -15 may be necessary for proper operation
  if (current->a < Deg20 && site.locationEx.latitude.absval < Deg45) {
    if (site.location.latitude >= 0) {
      if (current->d <= Deg90 - site.location.latitude) destination.h = Deg45;
    } else {
      if (current->d >= -Deg90 - site.location.latitude) destination.h = Deg45;
    }
  }
}

// update acceleration rates for goto and guiding
void Goto::updateAccelerationRates() {
  radsPerSecondCurrent = (1000000.0F/settings.usPerStepCurrent)/(float)mount.axis1.getStepsPerMeasure();
  rate = radsPerSecondCurrent;
  float secondsToAccelerate = degToRadF((float)(SLEW_ACCELERATION_DIST))/radsPerSecondCurrent;
  float radsPerSecondPerSecond = radsPerSecondCurrent/secondsToAccelerate;
  mount.axis1.setSlewAccelerationRate(radsPerSecondPerSecond);
  mount.axis1.setSlewAccelerationRateAbort(radsPerSecondPerSecond*2.0F);
  mount.axis2.setSlewAccelerationRate(radsPerSecondPerSecond);
  mount.axis2.setSlewAccelerationRateAbort(radsPerSecondPerSecond*2.0F);
}

// estimate average microseconds per step lower limit
float Goto::usPerStepLowerLimit() {
  // basis is platform/clock-rate specific (for square wave)
  float r_us = HAL_MAXRATE_LOWER_LIMIT;
  
  // higher speed ISR code path?
  #if STEP_WAVE_FORM == PULSE || STEP_WAVE_FORM == DEDGE
    r_us /= 1.6F;
  #endif
  
  // on-the-fly mode switching used?
  #if MODE_SWITCH_BEFORE_SLEW == OFF
    if (axis1StepsGoto != 1 || axis2StepsGoto != 1) r_us *= 1.7F;
  #endif

  // average required goto us rates for each axis with any micro-step mode switching applied
  float r_us_axis1 = r_us/mount.axis1.getStepsPerStepSlewing();
  float r_us_axis2 = r_us/mount.axis2.getStepsPerStepSlewing();
  
  // average in axis2 step rate scaling for drives where the reduction ratio isn't equal
  r_us = (r_us_axis1 + r_us_axis2)/2.0F;
 
  // the timer granulaity can start to make for some very abrupt rate changes below 0.25us
  if (r_us < 0.25F) { r_us = 0.25F; DLF("WRN, Mount::usPerStepLowerLimit(): r_us exceeds design limit"); }

  // return rate in us units
  return r_us;
}

// monitor goto
void Goto::poll() {
  if (stage == GG_READY_ABORT) {
    VLF("MSG: Mount, goto abort requested");

    meridianFlipHome.paused = false;
    meridianFlipHome.resume = false;
    mount.axis1.autoSlewAbort();
    mount.axis2.autoSlewAbort();

    stage = GG_ABORT;
  } else
  if (stage == GG_WAYPOINT) {
    if ((mount.axis1.atTarget() && mount.axis2.atTarget()) || !mount.isSlewing()) {
      if (destination.h == home.position.h && destination.d == home.position.d && destination.pierSide == home.position.pierSide) {

        if (settings.meridianFlipPause && !meridianFlipHome.resume) { meridianFlipHome.paused = true; goto skip; }

        meridianFlipHome.paused = false;
        meridianFlipHome.resume = false;
        VLF("MSG: Mount, goto home reached");
        stage = GG_DESTINATION;
        destination = target;
      } else {
        VLF("MSG: Mount, goto waypoint reached");
        destination = home.position;
      }

      mount.axis1.autoSlewStop();
      mount.axis2.autoSlewStop();
      VLF("MSG: Mount, goto stopped");

      double a1, a2;
      transform.mountToInstrument(&destination, &a1, &a2);
      if (park.state == PS_PARKING && stage == GG_DESTINATION) {
        mount.axis1.setTargetCoordinatePark(a1);
        mount.axis2.setTargetCoordinatePark(a2);
      } else {
        mount.axis1.setTargetCoordinate(a1);
        mount.axis2.setTargetCoordinate(a2);
      }
      VLF("MSG: Mount, goto next target coordinates set");

      mount.axis1.autoSlewRateByDistance(degToRadF((float)(SLEW_ACCELERATION_DIST)));
      mount.axis2.autoSlewRateByDistance(degToRadF((float)(SLEW_ACCELERATION_DIST)));
    }
  } else
  if (stage == GG_DESTINATION || stage == GG_ABORT) {
    if ((mount.axis1.atTarget() && mount.axis2.atTarget()) || !mount.isSlewing()) {
      VLF("MSG: Mount, goto destination reached (stopping)");
      mount.axis1.autoSlewRateByDistanceStop();
      mount.axis2.autoSlewRateByDistanceStop();

      // check if parking and mark as finished
      if (park.state == PS_PARKING && stage != GG_ABORT) park.requestDone();

      // flag the goto as finished
      state = GS_NONE;
      stage = GG_NONE;

      // back to normal tracking
      mount.update();

      // kill this monitor
      tasks.setDurationComplete(taskHandle);
      taskHandle = 0;
      VLF("MSG: Mount, goto monitor task terminated");

      status.sound.alert();

      return;
    }

  skip:
    // keep updating the axis targets to match the mount target
    if (mount.isTracking()) {
      double a1, a2;
      if (transform.mountType == ALTAZM) transform.equToHor(&target);
      transform.mountToInstrument(&target, &a1, &a2);
      mount.axis1.setTargetCoordinate(a1);
      mount.axis2.setTargetCoordinate(a2);
    }
  }

  // keep updating mount target
  if (mount.isTracking()) target.h += radsPerCentisecond;
}

Goto goTo;

#endif