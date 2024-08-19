//--------------------------------------------------------------------------------------------------
// telescope mount control, sync and goto

#include "Goto.h"

#if defined(MOUNT_PRESENT)

#include "../../../lib/tasks/OnTask.h"

#include "../../Telescope.h"
#include "../Mount.h"
#include "../coordinates/Transform.h"
#include "../guide/Guide.h"
#include "../home/Home.h"
#include "../park/Park.h"
#include "../limits/Limits.h"
#include "../status/Status.h"

#if GOTO_FEATURE == ON
inline void gotoWrapper() { goTo.poll(); }
#endif

void Goto::init() {
  // confirm the data structure size
  if (GotoSettingsSize < sizeof(GotoSettings)) { nv.initError = true; DLF("ERR: Goto::init(), GotoSettingsSize error"); }

  // write the default settings to NV
  if (!nv.hasValidKey() || nv.isNull(NV_MOUNT_GOTO_BASE, sizeof(GotoSettings))) {
    VLF("MSG: Mount, goto writing defaults to NV");
    nv.writeBytes(NV_MOUNT_GOTO_BASE, &settings, sizeof(GotoSettings));
  }

  // read the settings
  nv.readBytes(NV_MOUNT_GOTO_BASE, &settings, sizeof(GotoSettings));

  // force defaults if needed
  #if MFLIP_PAUSE_HOME_MEMORY != ON
    settings.meridianFlipPause = (MFLIP_PAUSE_HOME_DEFAULT == ON);
  #endif

  if (MFLIP_AUTOMATIC_MEMORY != ON || !transform.meridianFlips) settings.meridianFlipAuto = (MFLIP_AUTOMATIC_DEFAULT == ON);

  #if PIER_SIDE_PREFERRED_MEMORY != ON
    settings.preferredPierSide = (PierSideSelect)PIER_SIDE_PREFERRED_DEFAULT;
  #endif

  // calculate base and current maximum step rates
  #if GOTO_FEATURE == ON
    usPerStepBase = 1000000.0F/((axis1.getStepsPerMeasure()/RAD_DEG_RATIO)*SLEW_RATE_BASE_DESIRED);
    #if SLEW_RATE_MEMORY != ON
      settings.usPerStepCurrent = usPerStepBase;
    #endif
  #else
    usPerStepBase = 1000000.0F/((axis1.getStepsPerMeasure()/RAD_DEG_RATIO)*1.0F);
    settings.usPerStepCurrent = usPerStepBase;
  #endif
  if (usPerStepBase < usPerStepLowerLimit()) usPerStepBase = usPerStepLowerLimit()*2.0F;
  if (settings.usPerStepCurrent > 1000000.0F) settings.usPerStepCurrent = usPerStepBase;
  if (settings.usPerStepCurrent < usPerStepBase/2.0F) settings.usPerStepCurrent = usPerStepBase/2.0F;
  if (settings.usPerStepCurrent > usPerStepBase*2.0F) settings.usPerStepCurrent = usPerStepBase*2.0F;

  if (AXIS1_SYNC_THRESHOLD != OFF || AXIS2_SYNC_THRESHOLD != OFF) absoluteEncodersPresent = true;
  if (AXIS1_HOME_TOLERANCE != 0.0F || AXIS2_HOME_TOLERANCE != 0.0F ||
      AXIS1_TARGET_TOLERANCE != 0.0F || AXIS2_TARGET_TOLERANCE != 0.0F || absoluteEncodersPresent) encodersPresent = true;

  updateAccelerationRates();
}

// goto to equatorial target position (Native coordinate system) using the defaut preferredPierSide
CommandError Goto::request() {
  return request(target, settings.preferredPierSide);
}

#if GOTO_FEATURE == ON

// goto equatorial position (Native or Mount coordinate system)
CommandError Goto::request(Coordinate coords, PierSideSelect pierSideSelect, bool native) {

  if (native) {
    coords.pierSide = PIER_SIDE_NONE;
    transform.nativeToMount(&coords);
  }

  CommandError e = setTarget(&coords, pierSideSelect);
  if (e == CE_SLEW_IN_SLEW) { abort(); return e; }
  if (e != CE_NONE) return e;

  lastAlignTarget = target;

  #if AXIS1_SECTOR_GEAR == ON || AXIS2_TANGENT_ARM == ON
    double a1, a2;

    // handle special case of a sector gear RA
    #if AXIS1_SECTOR_GEAR == ON
      transform.mountToInstrument(&target, &a1, &a2);
      a1 = a1 - axis1.getIndexPosition();
      if (a1 < axis1.settings.limits.min) return CE_SLEW_ERR_OUTSIDE_LIMITS;
      if (a1 > axis1.settings.limits.max) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    #endif

    // handle special case of a tangent arm Dec
    #if AXIS2_TANGENT_ARM == ON
      transform.mountToInstrument(&target, &a1, &a2);
      a2 = a2 - axis2.getIndexPosition();
      if (a2 < axis2.settings.limits.min) return CE_SLEW_ERR_OUTSIDE_LIMITS;
      if (a2 > axis2.settings.limits.max) return CE_SLEW_ERR_OUTSIDE_LIMITS;
    #endif
  #endif

  limits.enabled(true);
  mount.syncFromOnStepToEncoders = false;
  if (firstGoto) {
    mount.tracking(true);
    firstGoto = false;
  }
  guide.backlashEnableControl(true);

  // allow slewing near target for Eq modes but disable for alt/az, parking, homing if encoders are not present
  nearDestinationRefineStages = 0;
  slewDestinationDistHA = 0.0;
  slewDestinationDistDec = 0.0;
  if ((encodersPresent || (park.state != PS_PARKING && home.state != HS_HOMING))) {
    nearDestinationRefineStages = GOTO_REFINE_STAGES;
    if (transform.isEquatorial()) { 
      slewDestinationDistHA = degToRad(GOTO_OFFSET);
      slewDestinationDistDec = degToRad(GOTO_OFFSET);
      if (target.pierSide == PIER_SIDE_WEST) slewDestinationDistDec = -slewDestinationDistDec;
    }
  }

  // prepare for goto
  Coordinate current = mount.getMountPosition(CR_MOUNT_HOR);
  state = GS_GOTO;
  stage = GG_NEAR_DESTINATION_START;
  start = current;
  destination = target;

  // add waypoint if needed
  if (transform.isEquatorial() && MFLIP_SKIP_HOME == OFF && start.pierSide != destination.pierSide) {
    VLF("MSG: Mount, goto changes pier side, setting waypoint at home");
    waypoint(&current);
  }

  // start the goto monitor
  if (taskHandle != 0) tasks.remove(taskHandle);
  taskHandle = tasks.add(0, 0, true, 3, gotoWrapper, "MntGoto");
  if (taskHandle) {
    VLF("MSG: Mount, create goto monitor task (idle, priority 3)... success");
    VLF("MSG: Mount, attempting goto");

    e = startAutoSlew();
    if (e != CE_NONE) {
      VF("MSG: Mount, goto failed with error "); VL(e);
      return e;
    }

    tasks.setPeriodMicros(taskHandle, FRACTIONAL_SEC_US);
    VF("MSG: Mount, goto monitor task set rate "); V(FRACTIONAL_SEC_US); VL("us");

    mountStatus.soundAlert();

  } else { DLF("WRN: Mount, start goto monitor task... FAILED!"); }

  return CE_NONE;
}
#else

// sync replaces goto to equatorial position (Native or Mount coordinate system) when GOTO_FEATURE is OFF
CommandError Goto::request(Coordinate coords, PierSideSelect pierSideSelect, bool native) {
  mountStatus.soundAlert();

  CommandError result = requestSync(coords, pierSideSelect, native);

  // check if parking and mark as finished or unparked as needed
  if (park.state == PS_PARKING) park.requestDone();

  // check if homing
  if (home.state == HS_HOMING) home.requestDone();

  return result;
}
#endif

// sync to equatorial target position (Native coordinate system) using the default preferredPierSide
CommandError Goto::requestSync() {
  return requestSync(target, settings.preferredPierSide);
}

// sync to equatorial position (Native or Mount coordinate system)
CommandError Goto::requestSync(Coordinate coords, PierSideSelect pierSideSelect, bool native) {
  
  if (native) {
    coords.pierSide = PIER_SIDE_NONE;
    transform.nativeToMount(&coords);
  }

  CommandError e = setTarget(&coords, pierSideSelect, false);
  if (e != CE_NONE) return e;
  
  if (mount.isHome()) mount.tracking(true);

  double a1, a2;
  transform.mountToInstrument(&target, &a1, &a2);

  axis1.setInstrumentCoordinate(a1 + target.a1Correction);
  axis2.setInstrumentCoordinate(a2);

  limits.enabled(true);
  mount.syncFromOnStepToEncoders = true;

  VLF("MSG: Mount, sync instrument coordinates updated");

  return CE_NONE;
}

// checks for valid target and determines pier side (Mount coordinate system)
CommandError Goto::setTarget(Coordinate *coords, PierSideSelect pierSideSelect, bool isGoto) {
  CommandError e = validate();
  if (e == CE_SLEW_ERR_IN_STANDBY && (encodersPresent || mount.isHome())) {
    mount.enable(true);
    e = validate();
  }
  if (e == CE_NONE && isGoto && limits.isAboveOverhead()) e = CE_SLEW_ERR_OUTSIDE_LIMITS;
  if (e != CE_NONE) return e;

  target = *coords;

  if (transform.mountType == ALTAZM) transform.horToEqu(&target); else
  if (transform.mountType == ALTALT) transform.aaToEqu(&target); else transform.equToHor(&target);

  // east side of pier is always the default polar-home position
  // east side of pier - we're in the western sky and the HA's are positive
  // west side of pier - we're in the eastern sky and the HA's are negative

  Coordinate current = mount.getMountPosition(CR_MOUNT);

  if (!transform.meridianFlips) pierSideSelect = PSS_EAST_ONLY;

  bool pierSideBest = false;
  if (pierSideSelect == PSS_BEST) {
    if (current.pierSide == PIER_SIDE_WEST) pierSideSelect = PSS_WEST; else pierSideSelect = PSS_EAST;
    pierSideBest = true;
  }
  if (pierSideSelect == PSS_SAME_ONLY) {
    if (current.pierSide == PIER_SIDE_WEST) pierSideSelect = PSS_WEST_ONLY; else pierSideSelect = PSS_EAST_ONLY;
  }

  bool eastReachable, westReachable;

  target.pierSide = current.pierSide;
  target.a1Correction = 0.0;

  double axis1TargetCorrectionE = 0.0;
  double axis1TargetCorrectionW = 0.0;
  e = limits.validateTarget(&target, &eastReachable, &westReachable, &axis1TargetCorrectionE, &axis1TargetCorrectionW, isGoto);
  if (e != CE_NONE) return e;

  double a1, a2, a1e, a1w, a2e, a2w;
  a1 = axis1.getInstrumentCoordinate();
  a2 = axis2.getInstrumentCoordinate();

  target.pierSide = PIER_SIDE_EAST;
  transform.mountToInstrument(&target, &a1e, &a2e);
  a1e += axis1TargetCorrectionE;

  target.pierSide = PIER_SIDE_WEST;
  transform.mountToInstrument(&target, &a1w, &a2w);
  a1w += axis1TargetCorrectionW;

  target.pierSide = current.pierSide;

  VF("MSG: Mount, set-target current axis1 "); V(radToDeg(a1)); VF(" and axis2 "); VL(radToDeg(a2));
  VF("MSG: Mount, set-target targetE axis1 "); V(radToDeg(a1e)); VF(" and axis2 "); VL(radToDeg(a2e));
  VF("MSG: Mount, set-target targetW axis1 "); V(radToDeg(a1w)); VF(" and axis2 "); VL(radToDeg(a2w));

  double eastDistance, westDistance;
  if (dist(a1, a1e) > dist(a2, a2e)) eastDistance = dist(a1, a1e); else eastDistance = dist(a2, a2e);
  if (dist(a1, a1w) > dist(a2, a2w)) westDistance = dist(a1, a1w); else westDistance = dist(a2, a2w);

  if (mount.isHome() && transform.mountType == GEM) {
    VLF("MSG: Mount, set-target destination from home based on HA");
    if (target.h < 0) pierSideSelect = PSS_WEST; else pierSideSelect = PSS_EAST;
    pierSideBest = true;
  }

  target.pierSide = PIER_SIDE_NONE;

  if (pierSideSelect == PSS_EAST_ONLY) {
    VLF("MSG: Mount, set-target using PPS_EAST_ONLY");
    if (eastReachable) target.pierSide = PIER_SIDE_EAST;
  } else
  if (pierSideSelect == PSS_WEST_ONLY) {
    VLF("MSG: Mount, set-target using PPS_WEST_ONLY");
    if (westReachable) target.pierSide = PIER_SIDE_WEST;
  } else
  if (pierSideSelect == PSS_EAST) {
    VLF("MSG: Mount, set-target using PPS_EAST");
    if (westReachable && !eastReachable) target.pierSide = PIER_SIDE_WEST; else
    if (isGoto && westReachable && pierSideBest && westDistance < eastDistance) {
      VLF("MSG: Mount, set-target destination in alternate (W) orientation is closer");
      target.pierSide = PIER_SIDE_WEST; 
    } else
    if (eastReachable) target.pierSide = PIER_SIDE_EAST;
  } else
  if (pierSideSelect == PSS_WEST) {
    VLF("MSG: Mount, set-target using PPS_WEST");
    if (eastReachable && !westReachable) target.pierSide = PIER_SIDE_EAST; else
    if (isGoto && eastReachable && pierSideBest && eastDistance < westDistance) {
      VLF("MSG: Mount, set-target destination in normal (E) orientation is closer");
      target.pierSide = PIER_SIDE_EAST;
    } else
    if (westReachable) target.pierSide = PIER_SIDE_WEST;
  }

  if (target.pierSide == PIER_SIDE_NONE) {
    VLF("MSG: Mount, set-target destination outside limits");
    return CE_SLEW_ERR_OUTSIDE_LIMITS;
  }

  VF("MSG: Mount, set-target destination ");
  if (current.pierSide == PIER_SIDE_NONE) { VF("NONE"); } else
  if (current.pierSide == PIER_SIDE_EAST) { VF("EAST"); } else
  if (current.pierSide == PIER_SIDE_WEST) { VF("WEST"); } else { VF("?"); }
  if (current.pierSide == target.pierSide) { VF(" stays "); } else { VF(" to "); }
  if (target.pierSide == PIER_SIDE_NONE) { VLF("NONE"); } else
  if (target.pierSide == PIER_SIDE_EAST) { VLF("EAST"); } else
  if (target.pierSide == PIER_SIDE_WEST) { VLF("WEST"); } else { VLF("?"); }

  if (target.pierSide != PIER_SIDE_EAST && target.pierSide != PIER_SIDE_WEST) {
    VLF("MSG: Mount, set-target destination pier side defaults to EAST");
    target.pierSide = PIER_SIDE_EAST;
  }

  // adjust Axis1 coordinate range as needed to allow going past +/-180 degrees
  if (target.pierSide == PIER_SIDE_EAST) target.a1Correction = axis1TargetCorrectionE;
  if (target.pierSide == PIER_SIDE_WEST) target.a1Correction = axis1TargetCorrectionW;

  transform.observedPlaceToMount(&target);
  if (transform.mountType == ALTAZM) transform.horToEqu(&target); else
  if (transform.mountType == ALTALT) transform.aaToEqu(&target); else transform.equToHor(&target);

  transform.hourAngleToRightAscension(&target, false);

  return CE_NONE;
}

// stop any presently active goto
void Goto::abort() {
  if (state == GS_GOTO && stage > GG_READY_ABORT) stage = GG_READY_ABORT;
}

// general status checks ahead of sync or goto
CommandError Goto::validate() {
  if (!axis1.isEnabled())      return CE_SLEW_ERR_IN_STANDBY;
  if (!axis2.isEnabled())      return CE_SLEW_ERR_IN_STANDBY;
  if (park.state == PS_PARKED) return CE_SLEW_ERR_IN_PARK;
  if (state != GS_NONE)        return CE_SLEW_IN_SLEW;
  if (guide.state != GU_NONE)  return CE_SLEW_IN_MOTION;
  if (mount.isSlewing())       return CE_SLEW_IN_MOTION;
  if (limits.isGotoError())    return CE_SLEW_ERR_OUTSIDE_LIMITS;
  if (mount.motorFault())      return CE_SLEW_ERR_HARDWARE_FAULT;
  return CE_NONE;
}

// add an align star (at the current position relative to target)
CommandError Goto::alignAddStar(bool sync) {
  if (alignState.currentStar > alignState.lastStar) return CE_PARAM_RANGE;

  CommandError e = CE_NONE;

  // first star, get ready for a new pointing model, init/sync then call gta.addStar 
  if (alignState.currentStar == 1) {
    #if ALIGN_MAX_NUM_STARS > 1  
      transform.align.init(transform.mountType, site.location.latitude);
    #endif
    e = requestSync(gotoTarget, PSS_SAME_ONLY);
    lastAlignTarget = mount.getMountPosition(CR_MOUNT_ALL);
    transform.hourAngleToRightAscension(&lastAlignTarget, true);
  }

  // add an align star
  if (e == CE_NONE) {
    Coordinate mountPosition = mount.getMountPosition(CR_MOUNT_ALL);
    if (sync) { lastAlignTarget = gotoTarget; }

    // update the targets HA and Horizon coords as necessary
    transform.rightAscensionToHourAngle(&lastAlignTarget, true);
    if (transform.mountType == ALTAZM) transform.equToHor(&lastAlignTarget); else
    if (transform.mountType == ALTALT) transform.equToAa(&lastAlignTarget);

    #if ALIGN_MAX_NUM_STARS > 1
      e = transform.align.addStar(alignState.currentStar, alignState.lastStar, &lastAlignTarget, &mountPosition);
    #else
      UNUSED(mountPosition);
    #endif

    if (e == CE_NONE) alignState.currentStar++;
  }

  return e;
}

// reset the alignment model
void Goto::alignReset() {
  alignState.currentStar = 0;
  alignState.lastStar = 0;
  #if ALIGN_MAX_NUM_STARS > 1
    transform.align.modelClear();
  #endif
}

#if GOTO_FEATURE == ON
// set any additional destinations required for a goto
void Goto::waypoint(Coordinate *current) {
  // HA goes from +90...0..-90
  //                W   .   E
  // meridian flip, only happens for equatorial mounts

  stage = GG_WAYPOINT_HOME;

  // default goes straight to the home position
  destination = home.getPosition(CR_MOUNT);

  // if the home position is at 0 hours, we're done
  if (destination.h == 0.0) return;

  double d60 = degToRad(120);
  double d45 = degToRad(135);
  if (current->pierSide == PIER_SIDE_EAST) { d60 = Deg180 - d60; d45 = Deg180 - d45; }

  // decide if we should first move to 60 deg. HA (4 hours) to get away from the horizon limits
  if (current->a < Deg10 && fabs(start.h) > Deg90) { destination.h = d60; stage = GG_WAYPOINT_AVOID; return; }

  // decide if we should first move to 45 deg. HA (3 hours) to get away from the horizon limits
  // if at a low latitude and in the opposite sky, |HA| = 6 is very low on the horizon and we need
  // to delay arriving there during a meridian flip.  In the extreme case, where the user is very
  // near the Earths equator an Horizon limit of -10 or -15 may be necessary for proper operation
  if (current->a < Deg20 && site.locationEx.latitude.absval < Deg45) {
    if (site.location.latitude >= 0) {
      if (current->d <= Deg90 - site.location.latitude) { destination.h = d45; stage = GG_WAYPOINT_AVOID; }
    } else {
      if (current->d >= -Deg90 - site.location.latitude) { destination.h = d45; stage = GG_WAYPOINT_AVOID; }
    }
  }
}

// monitor goto
void Goto::poll() {
  if (stage == GG_READY_ABORT) {
    VLF("MSG: Mount, goto abort requested");
    stage = GG_ABORT;
    meridianFlipHome.paused = false;
    meridianFlipHome.resume = false;
    axis1.autoSlewAbort();
    axis2.autoSlewAbort();
  }

  // abort any goto that might hang!
  if (axis1.isSlewing()) {
    if (!axis1.nearTarget()) nearTargetTimeoutAxis1 = millis();
    if ((long)(millis() - nearTargetTimeoutAxis1) > 15000) {
      DLF("WRN: Mount, goto axis1 timed out aborting slew!");
      axis1.autoSlewAbort();
    }
  }
  if (axis2.isSlewing()) {
    if (!axis2.nearTarget()) nearTargetTimeoutAxis2 = millis();
    if ((long)(millis() - nearTargetTimeoutAxis2) > 15000) {
      DLF("WRN: Mount, goto axis2 timed out aborting slew!");
      axis2.autoSlewAbort();
    }
  }

  if (!mount.isSlewing()) {
    if (stage == GG_WAYPOINT_AVOID) {
      VLF("MSG: Mount, goto waypoint reached");
      stage = GG_WAYPOINT_HOME;
      destination = home.getPosition(CR_MOUNT);
      startAutoSlew();
    } else

    if (stage == GG_WAYPOINT_HOME) {
      if (settings.meridianFlipPause && !meridianFlipHome.resume) { meridianFlipHome.paused = true; goto skip; }
      meridianFlipHome.paused = false;
      meridianFlipHome.resume = false;

      VLF("MSG: Mount, goto home reached");
      stage = GG_NEAR_DESTINATION_START;
      destination = target;
      startAutoSlew();
    } else

    if (stage == GG_NEAR_DESTINATION_START) {
      if (nearDestinationRefineStages >= 1) {
        VLF("MSG: Mount, goto near destination wait started");
        nearDestinationTimeout = millis() + GOTO_SETTLE_TIME;
        stage = GG_NEAR_DESTINATION_WAIT;
      } else stage = GG_NEAR_DESTINATION;
    } else

    if (stage == GG_NEAR_DESTINATION_WAIT) {
      if ((long)(millis() - nearDestinationTimeout) > 0) {
        VLF("MSG: Mount, goto near destination wait done");
        stage = GG_NEAR_DESTINATION;
      }
    } else

    if (stage == GG_NEAR_DESTINATION) {
      if (nearDestinationRefineStages >= 1) {

        if (--nearDestinationRefineStages) stage = GG_NEAR_DESTINATION_START; else stage = GG_DESTINATION;

        VLF("MSG: Mount, goto near destination reached");
        destination = target;
        if (!alignActive() || GOTO_OFFSET_ALIGN == OFF) {
          slewDestinationDistHA = 0.0;
          slewDestinationDistDec = 0.0;
        }
        startAutoSlew();
      } else {
        stage = GG_DESTINATION;
        VLF("MSG: Mount, goto near destination skipped");
      }
    } else

    if (stage == GG_DESTINATION || stage == GG_ABORT) {
      VLF("MSG: Mount, goto destination reached");
      state = GS_NONE;
      mount.update();

      // back to normal motor frequencies
      axis1.setFrequencyScale(1.0F);
      axis2.setFrequencyScale(1.0F);

      // kill this monitor
      tasks.setDurationComplete(taskHandle);
      taskHandle = 0;
      VLF("MSG: Mount, goto monitor task terminated");

      // check if parking and mark as finished or unparked as needed
      if (park.state == PS_PARKING) {
        if (stage == GG_ABORT) park.requestAborted(); else park.requestDone();
      }

      // check if homing
      if (home.state == HS_HOMING) {
        if (stage == GG_ABORT) home.requestAborted(); else home.requestDone();
      }

      // reset goto stage
      stage = GG_NONE;

      mountStatus.soundAlert();

      return;
    }
  }

  // adjust rates near the horizon to help avoid exceeding the minimum altitude limit
  if (transform.isEquatorial() && MOUNT_HORIZON_AVOIDANCE == ON) {
    if (site.locationEx.latitude.absval > degToRad(10.0)) {
      static float last_a2 = 0;
      Coordinate coords = mount.getMountPosition(CR_MOUNT_ALT);
      float a2 = site.locationEx.latitude.sign*coords.d;

      // range 0.2 to 1.0, where a larger distance has less slowdown effect
      float slowdownFactor = radToDeg(coords.a - limits.settings.altitude.min)/(SLEW_ACCELERATION_DIST*2.0);

      // constrain
      if (slowdownFactor > 1.0F) slowdownFactor = 1.0F;
      if (slowdownFactor < 0.2F) slowdownFactor = 0.2F;

      // if Dec is decreasing slow down the Dec axis, if Dec is increasing slow down the RA axis
      float sfr = 0.5F/FRACTIONAL_SEC;
      float slowdownFactor1 = 1.0F;
      float slowdownFactor2 = 1.0F;
      static float slowdownFactor1a = 1.0F;
      static float slowdownFactor2a = 1.0F;
      if (a2 < last_a2) slowdownFactor2 = slowdownFactor; else slowdownFactor1 = slowdownFactor;

      if (slowdownFactor1a < slowdownFactor1) { slowdownFactor1a += sfr; }
      if (slowdownFactor1a > slowdownFactor1) { slowdownFactor1a -= sfr; }
      if (slowdownFactor1a > 1.0F) slowdownFactor1a = 1.0F;
      if (slowdownFactor1a < 0.2F) slowdownFactor1a = 0.2F;
      if (slowdownFactor2a < slowdownFactor2) { slowdownFactor2a += sfr; }
      if (slowdownFactor2a > slowdownFactor2) { slowdownFactor2a -= sfr; }
      if (slowdownFactor2a > 1.0F) slowdownFactor2a = 1.0F;
      if (slowdownFactor2a < 0.2F) slowdownFactor2a = 0.2F;

      axis1.setFrequencyScale(slowdownFactor1a);
      axis2.setFrequencyScale(slowdownFactor2a);

      last_a2 = a2;
    } else {
      axis1.setFrequencyScale(1.0F);
      axis2.setFrequencyScale(1.0F);
    }
  }

  skip:

  // keep updating the axis targets to match the mount target
  // but allow timeout to stop tracking to guarantee synchronization
  if (AXIS1_TARGET_TOLERANCE != 0.0F || AXIS2_TARGET_TOLERANCE != 0.0F || !axis1.nearTarget() || !axis2.nearTarget()) nearTargetTimeout = millis();

  if (mount.isTracking()) {
    target.r += siderealToRad(mount.trackingRateOffsetRA)/FRACTIONAL_SEC;
    target.d += siderealToRad(mount.trackingRateOffsetDec)/FRACTIONAL_SEC;
    transform.rightAscensionToHourAngle(&target, false);
    if (stage >= GG_NEAR_DESTINATION_START) {
      if (millis() - nearTargetTimeout < 5000) {
        Coordinate nearTarget = target;
        nearTarget.h -= slewDestinationDistHA;
        nearTarget.d -= slewDestinationDistDec;

        if (transform.mountType == ALTAZM) transform.equToHor(&nearTarget); else
        if (transform.mountType == ALTALT) transform.equToAa(&nearTarget);

        double a1, a2;
        transform.mountToInstrument(&nearTarget, &a1, &a2);

        axis1.setTargetCoordinate(a1 + nearTarget.a1Correction);
        axis2.setTargetCoordinate(a2);
      }
    }
  }
}

// start slews with approach correction and parking/homing support
CommandError Goto::startAutoSlew() {
  CommandError e;

  nearTargetTimeoutAxis1 = millis();
  nearTargetTimeoutAxis2 = millis();

  if (stage == GG_NEAR_DESTINATION || stage == GG_DESTINATION) {
    destination.h -= slewDestinationDistHA;
    destination.d -= slewDestinationDistDec;
  }

  double a1, a2;
  transform.mountToInstrument(&destination, &a1, &a2);

  if (stage == GG_DESTINATION && park.state == PS_PARKING) {
    axis1.setTargetCoordinatePark(a1 + destination.a1Correction);
    axis2.setTargetCoordinatePark(a2);
  } else {
    axis1.setTargetCoordinate(a1 + destination.a1Correction);
    axis2.setTargetCoordinate(a2);
  }

  VF("MSG: Mount, goto target coordinates set (a1="); V(radToDeg(a1)); VF(" deg, a2="); V(radToDeg(a2)); VLF(" deg)");

  e = axis1.autoGoto(radsPerSecondCurrent);
  if (e == CE_NONE) e = axis2.autoGoto(radsPerSecondCurrent*((float)(AXIS2_SLEW_RATE_PERCENT)/100.0F));

  nearTargetTimeout = millis();

  return e;
}
#endif

// update acceleration rates for goto and guiding
void Goto::updateAccelerationRates() {
  radsPerSecondCurrent = (1000000.0F/settings.usPerStepCurrent)/(float)axis1.getStepsPerMeasure();
  rate = radsPerSecondCurrent;
  #if GOTO_FEATURE == ON
    float secondsToAccelerate = (degToRadF((float)(SLEW_ACCELERATION_DIST))/radsPerSecondCurrent)*2.0F;
    float secondsToAccelerateAbort = (degToRadF((float)(SLEW_RAPID_STOP_DIST))/radsPerSecondCurrent)*2.0F;
  #else
    float secondsToAccelerate = (degToRadF((float)(5.0F))/radsPerSecondCurrent)*2.0F;
    float secondsToAccelerateAbort = (degToRadF((float)(2.0F))/radsPerSecondCurrent)*2.0F;
  #endif
  axis1.setSlewAccelerationRate(radsPerSecondCurrent/secondsToAccelerate);
  axis1.setSlewAccelerationRateAbort(radsPerSecondCurrent/secondsToAccelerateAbort);
  axis2.setSlewAccelerationRate(radsPerSecondCurrent/secondsToAccelerate);
  axis2.setSlewAccelerationRateAbort(radsPerSecondCurrent/secondsToAccelerateAbort);
}

// estimate average microseconds per step lower limit
float Goto::usPerStepLowerLimit() {
  // basis is platform/clock-rate specific (for square wave)
  float r_us = HAL_MAXRATE_LOWER_LIMIT;
  
  // higher speed ISR code path?
  #if STEP_WAVE_FORM == PULSE || STEP_WAVE_FORM == DEDGE
    r_us /= 1.6F;
  #endif

  // average required goto us rates for each axis with any micro-step mode switching applied
  float r_us_axis1 = r_us/axis1.getStepsPerStepSlewing();
  float r_us_axis2 = r_us/axis2.getStepsPerStepSlewing();

  // average in axis2 step rate scaling for drives where the reduction ratio isn't equal
  r_us = (1.0F/(1.0F/r_us_axis1 + 1.0F/r_us_axis2))*2.0F;

  // return rate in us units
  return r_us;
}

Goto goTo;

#endif
