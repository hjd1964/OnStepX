//--------------------------------------------------------------------------------------------------
// telescope mount control, homing
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../coordinates/Transform.h"
extern Transform transform;
#include "../commands/ProcessCmds.h"
#include "Axis.h"
extern Axis axis1;
extern Axis axis2;
#include "Mount.h"

CommandError Mount::resetHome() {
  if (guideState != GU_NONE)      return CE_SLEW_IN_MOTION;
  if (gotoState  == GS_GOTO_SYNC) return CE_SLEW_IN_MOTION;
  if (gotoState  != GS_NONE)      return CE_SLEW_IN_SLEW;

  // setup where the home position is
  updateHomePosition();
  
  // setup axis1 and axis2
  axis1.enable(false);
  axis1.setMotorCoordinateSteps(0);
  axis1.clearBacklashCount();
  axis1.setInstrumentCoordinate(home.h);
  axis1.setFrequencyMax(degToRad(4.0));
  axis2.enable(false);
  axis2.setMotorCoordinateSteps(0);
  axis2.clearBacklashCount();
  axis2.setInstrumentCoordinate(home.d);
  axis2.setFrequencyMax(degToRad(4.0));
  atHome = true;

  setTrackingState(TS_NONE);
  updateTrackingRates();

  VLF("MSG: Mount::resetHome, in standby");
  return CE_NONE;
 }

#endif
