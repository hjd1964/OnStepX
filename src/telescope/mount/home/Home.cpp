//--------------------------------------------------------------------------------------------------
// telescope mount control, homing

#include "Home.h"

#ifdef MOUNT_PRESENT

#include "../Mount.h"
#include "../goto/Goto.h"
#include "../guide/Guide.h"

// init the home position (according to settings and mount type)
void Home::init() {
  #ifndef AXIS1_HOME_DEFAULT
    if (transform.mountType == GEM) position.h = Deg90; else { position.h = 0; position.z = 0; }
  #else
    if (transform.mountType == ALTAZM) position.z = AXIS1_HOME_DEFAULT; else position.h = AXIS1_HOME_DEFAULT;
  #endif
  #ifndef AXIS2_HOME_DEFAULT
    if (transform.mountType == ALTAZM) position.a = 0.0; else position.d = site.locationEx.latitude.sign*Deg90;
  #else
    if (transform.mountType == ALTAZM) position.a = AXIS2_HOME_DEFAULT; else position.d = AXIS2_HOME_DEFAULT;
  #endif
  position.pierSide = PIER_SIDE_NONE;
}

// move mount to the home position
CommandError Home::request() {
  #if SLEW_GOTO == ON
    if (goTo.state != GS_NONE) return CE_SLEW_IN_MOTION;
    if (guide.state != GU_NONE) {
      if (guide.state == GU_HOME_GUIDE) guide.abortHome();
      return CE_SLEW_IN_MOTION;
    }

    if ((AXIS1_SENSE_HOME) != OFF && (AXIS2_SENSE_HOME) != OFF) {
      CommandError e = reset();
      if (e != CE_NONE) return e;
    }

    #if AXIS2_TANGENT_ARM == OFF
      // stop tracking
      wasTracking = mount.isTracking();
      mount.tracking(false);
    #endif

    // make sure the motors are powered on
    mount.enable(true);

    VLF("MSG: Mount, moving to home");

    if ((AXIS1_SENSE_HOME) != OFF && (AXIS2_SENSE_HOME) != OFF) {
      isRequestWithReset = false;
      guide.startHome(GUIDE_HOME_TIME_LIMIT*1000UL);
    } else {
      #if AXIS2_TANGENT_ARM == OFF
        state = HS_HOMING;
        if (transform.mountType == ALTAZM) transform.horToEqu(&position);
        return goTo.request(&position, PSS_EAST_ONLY, false);
      #else
        axis2.setFrequencySlew(goTo.rate);
        if (transform.mountType == ALTAZM) axis2.setTargetCoordinate(position.a); else {
        axis2.setTargetCoordinate(axis2.getIndexPosition());
        VLF("Mount, home target coordinates set");
        axis2.autoSlewRateByDistance(degToRadF((float)(SLEW_ACCELERATION_DIST)));
      #endif
    }
  #endif
  return CE_NONE;
}

// reset mount, moves to the home position first if home switches are present
CommandError Home::requestWithReset() {
  if ((AXIS1_SENSE_HOME) != OFF && (AXIS2_SENSE_HOME) != OFF) {
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

// once homed mark as done
void Home::requestDone() {
  state = HS_NONE;
  reset(false);
}

// reset mount at home
CommandError Home::reset(bool fullReset) {
  #if SLEW_GOTO == ON
    if (goTo.state != GS_NONE) return CE_SLEW_IN_MOTION;
  #endif
  if (guide.state != GU_NONE) {
    if (guide.state == GU_HOME_GUIDE) guide.abortHome();
    return CE_SLEW_IN_MOTION;
  }

  // stop tracking
  mount.tracking(false);

  // make sure the motors are powered off
  if (fullReset) mount.enable(false);
  
  // setup axis1 and axis2
  axis1.resetPosition(0.0L);
  axis2.resetPosition(0.0L);

  if (transform.mountType == ALTAZM) {
    axis1.setInstrumentCoordinate(position.z);
    axis2.setInstrumentCoordinate(position.a);
  } else {
    axis1.setInstrumentCoordinate(position.h);
    axis2.setInstrumentCoordinate(position.d);
  }

  axis1.setBacklash(mount.settings.backlash.axis1);
  axis2.setBacklash(mount.settings.backlash.axis2);

  axis1.setFrequencySlew(degToRadF(0.1F));
  axis2.setFrequencySlew(degToRadF(0.1F));

  #if SLEW_GOTO == ON
    if (fullReset) goTo.alignReset();
  #endif

  mount.setHome(true);

  if (fullReset) { VLF("MSG: Mount, reset at home and in standby"); } else { VLF("MSG: Mount, reset at home"); }

  return CE_NONE;
}

Home home;

#endif
