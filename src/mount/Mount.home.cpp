//--------------------------------------------------------------------------------------------------
// telescope mount control, homing

#include "../Common.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "Mount.h"

CommandError Mount::returnHome() {
  if (guideState != GU_NONE) return CE_SLEW_IN_MOTION;
  if (gotoState  != GS_NONE) return CE_SLEW_IN_SLEW;

  // stop tracking
  setTrackingState(TS_NONE);
  updateTrackingRates();

  // setup where the home position is
  updateHomePosition();

  // move to home
  axis1.setFrequencyMax(radsPerSecondCurrent);
  if (transform.mountType == ALTAZM) axis1.setTargetCoordinate(home.z); else axis1.setTargetCoordinate(home.h);
  axis1.autoSlewHome();
  axis2.setFrequencyMax(radsPerSecondCurrent);
  if (transform.mountType == ALTAZM) axis2.setTargetCoordinate(home.a); else axis2.setTargetCoordinate(home.d);
  axis2.autoSlewHome();

  atHome = true;

  VLF("MSG: Mount, moving to home");
  return CE_NONE;
}

CommandError Mount::resetHome(bool resetPark) {
  if (guideState != GU_NONE) return CE_SLEW_IN_MOTION;
  if (gotoState  != GS_NONE) return CE_SLEW_IN_MOTION;

  // clear park state
  if (resetPark) {
    park.state = PS_NONE;
    nv.updateBytes(NV_MOUNT_PARK_BASE, &park, ParkSize);
  }

  // stop tracking
  setTrackingState(TS_NONE);
  updateTrackingRates();

  // setup where the home position is
  updateHomePosition();
  
  // setup axis1 and axis2
  axis1.enable(false);
  axis1.setMotorCoordinateSteps(0);
  axis1.setBacklash(0);
  if (transform.mountType == ALTAZM) axis1.setInstrumentCoordinate(home.z); else axis1.setInstrumentCoordinate(home.h);
  axis1.setFrequencyMax(degToRad(4.0));
  axis2.enable(false);
  axis2.setMotorCoordinateSteps(0);
  axis2.setBacklash(0);
  if (transform.mountType == ALTAZM) axis2.setInstrumentCoordinate(home.a); else axis2.setInstrumentCoordinate(home.d);
  axis2.setFrequencyMax(degToRad(4.0));
  atHome = true;

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
