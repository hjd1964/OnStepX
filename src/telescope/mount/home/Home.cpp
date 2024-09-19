//--------------------------------------------------------------------------------------------------
// telescope mount control, homing

#include "Home.h"

#ifdef MOUNT_PRESENT

#include "../Mount.h"
#include "../goto/Goto.h"
#include "../guide/Guide.h"
#include "../../../lib/tasks/OnTask.h"

// init the home position (according to settings and mount type)
void Home::init() {
  // confirm the data structure size
  if (SettingsSize < sizeof(Settings)) { nv.initError = true; DL("ERR: Home::init(), SettingsSize error"); }

  // write the default settings to NV
  if (!nv.hasValidKey()) {
    VLF("MSG: Mount, home writing defaults to NV");
    nv.writeBytes(NV_MOUNT_HOME_BASE, &settings, sizeof(Settings));
  }

  // get settings from NV
  nv.readBytes(NV_MOUNT_HOME_BASE, &settings, sizeof(Settings));

  #ifndef AXIS1_HOME_DEFAULT
    if (transform.mountType == GEM) position.h = Deg90; else { position.h = 0.0L; position.z = 0.0L; position.aa1 = 0.0L; }
  #else
    if (transform.mountType == ALTAZM) position.z = degToRad(AXIS1_HOME_DEFAULT); else
    if (transform.mountType == ALTALT) position.aa1 = degToRad(AXIS1_HOME_DEFAULT); else position.h = degToRad(AXIS1_HOME_DEFAULT);
  #endif
  #ifndef AXIS2_HOME_DEFAULT
    if (transform.mountType == ALTAZM) position.a = 0.0; else
    if (transform.mountType == ALTALT) position.aa2 = 0.0; else position.d = site.locationEx.latitude.sign*Deg90;
  #else
    if (transform.mountType == ALTAZM) position.a = degToRad(AXIS2_HOME_DEFAULT);
    if (transform.mountType == ALTALT) position.aa2 = degToRad(AXIS2_HOME_DEFAULT); else position.d = degToRad(AXIS2_HOME_DEFAULT);
  #endif

  setReversal();

  position.pierSide = PIER_SIDE_NONE;
}

// move mount to the home position
CommandError Home::request() {
    if (!site.dateIsReady || !site.timeIsReady) return CE_SLEW_ERR_IN_STANDBY;
    if (goTo.state != GS_NONE) return CE_SLEW_IN_MOTION;
    if (guide.state != GU_NONE) {
      if (guide.state == GU_HOME_GUIDE) guide.stop();
      return CE_SLEW_IN_MOTION;
    }

    wasTracking = mount.isTracking();
    #if AXIS1_SECTOR_GEAR == ON || AXIS2_TANGENT_ARM == OFF
      mount.tracking(false);
    #endif

    // make sure the motors are powered on
    mount.enable(true);
    goTo.firstGoto = false;

    if (hasSense) {
      #if AXIS1_SECTOR_GEAR == OFF && AXIS2_TANGENT_ARM == OFF
        double a1 = axis1.getInstrumentCoordinate();
        double a2 = axis2.getInstrumentCoordinate();
        if (transform.mountType == ALTAZM) {
          a1 -= position.z;
          a2 -= position.a;
        } else
        if (transform.mountType == ALTALT) {
          a1 -= position.aa1;
          a2 -= position.aa2;
        } else {
          a1 -= position.h;
          a2 -= position.d;
        }

        // both -180 to 180
        VF("MSG: Mount, homing from a1="); V(radToDeg(a1)); VF(" degrees a2="); V(radToDeg(a2)); VLF(" degrees");
        if (abs(a1) > degToRad(AXIS1_SENSE_HOME_DIST_LIMIT) - abs(arcsecToRad(settings.axis1.senseOffset))) return CE_SLEW_ERR_OUTSIDE_LIMITS;
        if (abs(a2) > degToRad(AXIS2_SENSE_HOME_DIST_LIMIT) - abs(arcsecToRad(settings.axis2.senseOffset))) return CE_SLEW_ERR_OUTSIDE_LIMITS;

        CommandError e = reset(false);
        if (e != CE_NONE) return e;
      #endif

      VLF("MSG: Mount, guiding to home");
      state = HS_HOMING;
      isRequestWithReset = false;
      guide.startHome();
    } else {
      #if AXIS1_SECTOR_GEAR == OFF && AXIS2_TANGENT_ARM == OFF
        VLF("MSG: Mount, moving to home");
        state = HS_HOMING;

        if (transform.mountType == ALTAZM) transform.horToEqu(&position); else
        if (transform.mountType == ALTALT) transform.aaToEqu(&position);

        CommandError result = goTo.request(position, PSS_EAST_ONLY, false);
        if (result != CE_NONE) {
          VF("WRN: Mount, moving to home goto failed (code "); V(result); VLF(")");
          state = HS_NONE;
        }
        return result;
      #else
        #if AXIS1_SECTOR_GEAR == ON
          VLF("MSG: Mount, moving SG to home");
          axis1.setFrequencySlew(goTo.rate);
          axis1.setTargetCoordinate(axis1.getIndexPosition());
          VLF("Mount, axis1 home target coordinates set");
          axis1.autoGoto(degToRadF((float)(SLEW_ACCELERATION_DIST)));
        #endif
        #if AXIS2_TANGENT_ARM == ON
          VLF("MSG: Mount, moving TA to home");
          axis2.setFrequencySlew(goTo.rate*((float)(AXIS2_SLEW_RATE_PERCENT)/100.0F));
          axis2.setTargetCoordinate(axis2.getIndexPosition());
          VLF("Mount, axis2 home target coordinates set");
          axis2.autoGoto(degToRadF((float)(SLEW_ACCELERATION_DIST)));
        #endif
      #endif
    }
  return CE_NONE;
}

// reset mount, moves to the home position first if home switches are present
CommandError Home::requestWithReset() {
  if (hasSense) {
    CommandError result = request();
    isRequestWithReset = true;
    return result;
  } else return reset();
}

// clear home state on abort
void Home::requestAborted() {
  state = HS_NONE;
  mount.tracking(wasTracking);
}

// after finding home switches displace the mount axes as specified
void Home::guideDone(bool success) {
  if (!success) { state = HS_NONE; reset(isRequestWithReset); return; }

  #if AXIS1_SECTOR_GEAR == OFF && AXIS2_TANGENT_ARM == OFF
    if (useOffset()) {
      reset(isRequestWithReset);

      if (transform.mountType == ALTAZM) transform.horToEqu(&position); else
      if (transform.mountType == ALTALT) transform.aaToEqu(&position);

      VF("MSG: Mount, finishing move to home with goto to (");
      double a1 = axis1.getInstrumentCoordinate() - arcsecToRad(site.locationEx.latitude.sign*settings.axis1.senseOffset);
      double a2 = axis2.getInstrumentCoordinate() - arcsecToRad(settings.axis2.senseOffset);
      V(radToDeg(a1)); VF(","); V(radToDeg(a2)); VLF(")");
      axis1.setTargetCoordinate(a1);
      axis1.autoGoto(goTo.getRadsPerSecond());
      axis2.setTargetCoordinate(a2);
      axis2.autoGoto(goTo.getRadsPerSecond());
      mount.syncFromOnStepToEncoders = true;

      state = HS_NONE;
    } else {
      state = HS_NONE;
      reset(isRequestWithReset);
    }
  #else
    state = HS_NONE;

    #if AXIS1_SECTOR_GEAR == ON 
      VLF("MSG: Mount, sector gear set origin");
      double h = axis1.getInstrumentCoordinate();
      if (axis1.resetPosition(0.0L) != 0) { DL("WRN: Home::guideDone(), failed to resetPosition Axis1"); exit; }
      axis1.setInstrumentCoordinate(h);
      mount.tracking(wasTracking);
    #endif

    #if AXIS2_TANGENT_ARM == ON 
      VLF("MSG: Mount, tangent arm set origin");
      double d = axis2.getInstrumentCoordinate();
      if (axis2.resetPosition(0.0L) != 0) { DL("WRN: Home::guideDone(), failed to resetPosition Axis2"); exit; }
      axis2.setInstrumentCoordinate(d);
    #endif
  #endif
}

// once homed mark as done
void Home::requestDone() {
  state = HS_NONE;
  reset(false);
}

// reset mount at home
CommandError Home::reset(bool fullReset) {
  #if GOTO_FEATURE == ON
    if (goTo.state != GS_NONE) {
      axis1.autoSlewAbort();
      axis2.autoSlewAbort();
      return CE_SLEW_IN_MOTION;
    }
  #endif
  if (guide.state != GU_NONE) {
    if (guide.state == GU_HOME_GUIDE) guide.stop();
    return CE_SLEW_IN_MOTION;
  }

  // refresh the home position
  init();

  // stop tracking and set default rate
  mount.tracking(false);
  mount.trackingRate = hzToSidereal(SIDEREAL_RATE_HZ);
  mount.trackingRateOffsetRA = 0.0F;
  mount.trackingRateOffsetDec = 0.0F;
  goTo.firstGoto = true;

  tasks.yieldMicros(10000);

  if (transform.mountType == ALTAZM) {
    position.a1 = position.z;
    position.a2 = position.a;
  } else
  if (transform.mountType == ALTALT) {
    position.a1 = position.aa1;
    position.a2 = position.aa2;
  } else {
    position.a1 = position.h;
    position.a2 = position.d;
  }

  if (!goTo.absoluteEncodersPresent) {
    if (axis1.resetPosition(0.0L) != 0) { DL("WRN: Home::reset(), failed to resetPosition Axis1"); }
    if (axis2.resetPosition(0.0L) != 0) { DL("WRN: Home::reset(), failed to resetPosition Axis2"); }

    if (!fullReset && state == HS_HOMING && useOffset()) {
      axis1.setInstrumentCoordinate(position.a1 + arcsecToRad(site.locationEx.latitude.sign*settings.axis1.senseOffset));
      axis2.setInstrumentCoordinate(position.a2 + arcsecToRad(settings.axis2.senseOffset));
    } else {
      axis1.setInstrumentCoordinate(position.a1);
      axis2.setInstrumentCoordinate(position.a2);
    }
  }

  axis1.setBacklash(mount.settings.backlash.axis1);
  axis2.setBacklash(mount.settings.backlash.axis2);

  axis1.setFrequencySlew(degToRadF(0.1F));
  axis2.setFrequencySlew(degToRadF(0.1F));

  // make sure the motors are powered off
  if (fullReset) {
    mount.enable(MOUNT_ENABLE_IN_STANDBY == ON);

    #if GOTO_FEATURE == ON
      goTo.alignReset();
    #endif

    VF("MSG: Mount, reset at home (");
    V(radToDeg(position.a1)); VF(","); V(radToDeg(position.a2));
    VLF(") and in standby");
  } else {
    VF("MSG: Mount, reset at home (");
    V(radToDeg(position.a1)); VF(","); V(radToDeg(position.a2));
    VLF(")");
  }

  return CE_NONE;
}

// get the home position
Coordinate Home::getPosition(CoordReturn coordReturn) {
  switch (coordReturn) {
    case CR_MOUNT:
    break;
    case CR_MOUNT_EQU:
      if (transform.mountType == ALTAZM) transform.horToEqu(&position);
      if (transform.mountType == ALTALT) transform.aaToEqu(&position);
      transform.hourAngleToRightAscension(&position, true);
    break;
    case CR_MOUNT_ALT:
    case CR_MOUNT_HOR:
      if (transform.isEquatorial()) transform.equToHor(&position);
      else if (transform.mountType == ALTALT) transform.aaToHor(&position);
    break;
    case CR_MOUNT_ALL:
      if (transform.mountType == ALTAZM) transform.horToEqu(&position); else
      if (transform.mountType == ALTALT) transform.aaToEqu(&position); else transform.equToHor(&position);
      transform.hourAngleToRightAscension(&position, true);
    break;
  }
  return position;
}

bool Home::useOffset() {
  if (hasSense && (settings.axis1.senseOffset != 0 || settings.axis2.senseOffset != 0)) return true; else return false;
}

void Home::setReversal() {
  bool latitudeReversal = transform.isEquatorial() && site.locationEx.latitude.sign < 0.0;
  axis1.setReverse(latitudeReversal);
  if (!settings.axis1.senseReverse) axis1.setHomeReverse(latitudeReversal); else axis1.setHomeReverse(!latitudeReversal);
  axis2.setHomeReverse(settings.axis2.senseReverse);
}

Home home;

#endif
