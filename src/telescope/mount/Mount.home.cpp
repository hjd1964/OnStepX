//--------------------------------------------------------------------------------------------------
// telescope mount control, homing

#include "Mount.h"

#ifdef MOUNT_PRESENT

CommandError Mount::returnHome() {
  if (gotoState != GS_NONE || guideState != GU_NONE || axis1.autoSlewActive() || axis2.autoSlewActive()) return CE_SLEW_IN_MOTION;

  if (AXIS1_SENSE_HOME != OFF && AXIS2_SENSE_HOME != OFF) {
    CommandError e = resetHome(false);
    if (e != CE_NONE) return e;
  }

  // stop tracking
  setTrackingState(TS_NONE);
  updateTrackingRates();

  // make sure the motors are powered on
  axis1.enable(true);
  axis2.enable(true);

  // set slew rate limit
  axis1.setFrequencySlew(radsPerSecondCurrent);
  axis2.setFrequencySlew(radsPerSecondCurrent);

  VLF("MSG: Mount, moving to home");

  if (AXIS1_SENSE_HOME != OFF && AXIS2_SENSE_HOME != OFF) {
    // use guiding and switches to find home
    guideState = GU_HOME_GUIDE;
    guideActionAxis1 = guideActionAxis2 = GA_HOME;
    guideFinishTimeAxis1 = guideFinishTimeAxis2 = millis() + 5UL*60UL*1000UL; // 5 minutes
    axis1.autoSlewHome();
    axis2.autoSlewHome();
  } else {
    // use a goto to find home
    updatePosition(CR_MOUNT);
    if (transform.mountType == ALTAZM) axis1.setTargetCoordinate(home.z); else axis1.setTargetCoordinate(home.h);
    if (transform.mountType == ALTAZM) axis2.setTargetCoordinate(home.a); else axis2.setTargetCoordinate(home.d);
    VLF("Mount::returnHome(); target coordinates set");
    axis1.autoSlewRateByDistance(degToRadF((float)(SLEW_ACCELERATION_DIST)));
    axis2.autoSlewRateByDistance(degToRadF((float)(SLEW_ACCELERATION_DIST)));
  }

  return CE_NONE;
}

CommandError Mount::resetHome(bool resetPark) {
  if (gotoState != GS_NONE || guideState != GU_NONE || axis1.autoSlewActive() || axis2.autoSlewActive()) return CE_SLEW_IN_MOTION;

  // clear park state
  if (resetPark) {
    park.state = PS_NONE;
    nv.updateBytes(NV_MOUNT_PARK_BASE, &park, ParkSize);
  }

  // stop tracking
  setTrackingState(TS_NONE);
  updateTrackingRates();

  // make sure the motors are powered down
  axis1.enable(false);
  axis2.enable(false);

  // setup where the home position is
  updateHomePosition();
  
  // setup axis1 and axis2
  axis1.setBacklash(0.0F);
  axis2.setBacklash(0.0F);
  axis1.resetPosition(0.0L);
  axis2.resetPosition(0.0L);
  if (transform.mountType == ALTAZM) {
    axis1.setInstrumentCoordinate(home.z);
    axis2.setInstrumentCoordinate(home.a);
  } else {
    axis1.setInstrumentCoordinate(home.h);
    axis2.setInstrumentCoordinate(home.d);
  }
  axis1.setBacklash(misc.backlash.axis1);
  axis2.setBacklash(misc.backlash.axis2);
  axis1.setFrequencySlew(degToRadF(0.1F));
  axis2.setFrequencySlew(degToRadF(0.1F));
  atHome = true;

  // clear align state
  alignState.currentStar = 0;
  alignState.lastStar = 0;

  VLF("MSG: Mount, reset at home and in standby");
  return CE_NONE;
 }

void Mount::updateHomePosition() {
  #ifndef AXIS1_HOME_DEFAULT
    if (transform.mountType == GEM) home.h = Deg90; else { home.h = 0; home.z = 0; }
  #else
    if (transform.mountType == ALTAZM) home.z = AXIS1_HOME_DEFAULT; else home.h = AXIS1_HOME_DEFAULT;
  #endif
  #ifndef AXIS2_HOME_DEFAULT
    if (transform.mountType == ALTAZM) home.a = 0.0; else home.d = site.locationEx.latitude.sign*Deg90;
  #else
    if (transform.mountType == ALTAZM) home.a = AXIS2_HOME_DEFAULT; else home.d = AXIS2_HOME_DEFAULT;
  #endif
  home.pierSide = PIER_SIDE_NONE;
}

#endif
