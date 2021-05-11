//--------------------------------------------------------------------------------------------------
// telescope mount control, homing
#include "../OnStepX.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../coordinates/Transform.h"
#include "../commands/ProcessCmds.h"
#include "../motion/Axis.h"
#include "Mount.h"

CommandError Mount::resetHome() {
  if (guideState != GU_NONE) return CE_SLEW_IN_MOTION;
  if (gotoState  != GS_NONE) return CE_SLEW_IN_SLEW;

  // setup where the home position is
  updateHomePosition();
  
  // setup axis1 and axis2
  axis1.enable(false);
  axis1.setMotorCoordinateSteps(0);
  axis1.setBacklash(0);
  axis1.setInstrumentCoordinate(home.h);
  axis1.setFrequencyMax(degToRad(4.0));
  axis2.enable(false);
  axis2.setMotorCoordinateSteps(0);
  axis2.setBacklash(0);
  axis2.setInstrumentCoordinate(home.d);
  axis2.setFrequencyMax(degToRad(4.0));
  atHome = true;

  setTrackingState(TS_NONE);
  updateTrackingRates();

  VLF("MSG: Mount, reset at home and in standby");
  return CE_NONE;
 }

void Mount::updateHomePosition() {
  #ifndef AXIS1_HOME_DEFAULT
    if (transform.mountType == GEM) home.h = Deg90; else home.h = 0;
  #else
    if (transform.mountType == ALTAZM) home.z = AXIS1_HOME_DEFAULT; else home.h = AXIS1_HOME_DEFAULT;
  #endif
  #ifndef AXIS2_HOME_DEFAULT
    if (transform.mountType == ALTAZM) home.a = 0.0; else home.d = transform.site.locationEx.latitude.sign*Deg90;
  #else
    if (transform.mountType == ALTAZM) home.a = AXIS2_HOME_DEFAULT; else home.d = AXIS2_HOME_DEFAULT;
  #endif
  home.pierSide = PIER_SIDE_NONE;
}

#endif
