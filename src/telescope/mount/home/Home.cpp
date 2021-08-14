//--------------------------------------------------------------------------------------------------
// telescope mount control, homing

#include "Home.h"

#ifdef MOUNT_PRESENT

#include "../Mount.h"
#include "../goto/Goto.h"
#include "../guide/Guide.h"
#include "../park/Park.h"

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
    if (goTo.state != GS_NONE || guide.state != GU_NONE || mount.isSlewing()) return CE_SLEW_IN_MOTION;

    if (AXIS1_SENSE_HOME != OFF && AXIS2_SENSE_HOME != OFF) {
      CommandError e = reset(false);
      if (e != CE_NONE) return e;
    }

    // stop tracking
    mount.tracking(false);

    // make sure the motors are powered on
    mount.enable(true);

    VLF("MSG: Mount, moving to home");

    if (AXIS1_SENSE_HOME != OFF && AXIS2_SENSE_HOME != OFF) {
      guide.startHome(GUIDE_HOME_TIME_LIMIT*1000UL);
    } else {
      // set slew rate limit
      axis1.setFrequencySlew(goTo.rate);
      axis2.setFrequencySlew(goTo.rate);
      // use a goto to find home
      if (transform.mountType == ALTAZM) axis1.setTargetCoordinate(position.z); else axis1.setTargetCoordinate(position.h);
      if (transform.mountType == ALTAZM) axis2.setTargetCoordinate(position.a); else axis2.setTargetCoordinate(position.d);
      VLF("Mount::returnHome(); target coordinates set");
      axis1.autoSlewRateByDistance(degToRadF((float)(SLEW_ACCELERATION_DIST)));
      axis2.autoSlewRateByDistance(degToRadF((float)(SLEW_ACCELERATION_DIST)));
    }
  #endif
  return CE_NONE;
}

// reset mount at home
CommandError Home::reset(bool resetPark) {
  #if SLEW_GOTO == ON
    if (goTo.state != GS_NONE) return CE_SLEW_IN_MOTION;
  #endif

  if (guide.state != GU_NONE || axis1.isSlewing() || axis2.isSlewing()) return CE_SLEW_IN_MOTION;

  #if SLEW_GOTO == ON
    // clear park state
    if (resetPark) park.reset();
  #endif

  // stop tracking
  mount.tracking(false);

  // make sure the motors are powered off
  mount.enable(false);
  
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
  atHome = true;

  axis1.setBacklash(mount.settings.backlash.axis1);
  axis2.setBacklash(mount.settings.backlash.axis2);

  axis1.setFrequencySlew(degToRadF(0.1F));
  axis2.setFrequencySlew(degToRadF(0.1F));

  #if SLEW_GOTO == ON
    goTo.alignReset();
  #endif

  VLF("MSG: Mount, reset at home and in standby");
  return CE_NONE;
}

Home home;

#endif
