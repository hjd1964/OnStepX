//--------------------------------------------------------------------------------------------------
// telescope control
#pragma once
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../Extended.Config.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"

#include "Axis.h"
#include "Convert.h"
#include "../commands/ProcessCmds.h"
#include "../StepDrivers/StepDrivers.h"
#include "Transform.h"

#if (defined(AXIS1_DRIVER_MODEL) && AXIS1_DRIVER_MODEL != OFF) || (defined(AXIS2_DRIVER_MODEL) && AXIS2_DRIVER_MODEL != OFF)

typedef struct Limits {
  double horizon;
  double overhead;
  double pastMeridianE;
  double pastMeridianW;
} Limits;

enum MeridianFlip { MeridianFlipNever,  MeridianFlipAlign, MeridianFlipAlways };

class Mount {
  public:
    void init(int8_t mountType);

    // handle telescope commands
    bool command(char reply[], char command[], char parameter[], bool *supressFrame, bool *numericReply, CommandError *commandError);

    CommandError validateGoto();
    CommandError validateGotoCoords(Coordinate coords);

    CommandError syncEqu(Coordinate target);

  private:
    void updatePosition();
    Convert convert;
    Transform transform;
    Coordinate position, target;

    Limits limits = { degToRad(-10), degToRad(85), degToRad(15), degToRad(15) };
    uint8_t mountType = 0;
    bool tracking = false;
    bool atHome = true;
    bool safetyLimitsOn = false;
    bool syncToEncodersOnly = false;
    MeridianFlip meridianFlip = MeridianFlipAlways;

};

#endif
/*
#if AXIS3_DRIVER_MODEL != OFF
  const AxisPins Axis3Pins = {AXIS3_STEP_PIN, AXIS3_DIR_PIN, AXIS3_ENABLE_PIN, false, false, true};
  Axis axis3{Axis3Pins, Axis3DriverModePins, Axis3DriverModeSettings};
  void moveAxis3() { axis3.move(AXIS3_STEP_PIN, AXIS3_DIR_PIN); }
#endif
#if AXIS4_DRIVER_MODEL != OFF
  const AxisPins Axis4Pins = {AXIS4_STEP_PIN, AXIS4_DIR_PIN, AXIS4_ENABLE_PIN, false, false, true};
  Axis axis4{Axis4Pins, Axis4DriverModePins, Axis4DriverModeSettings};
  void moveAxis4() { axis4.move(AXIS4_STEP_PIN, AXIS4_DIR_PIN); }
#endif
#if AXIS5_DRIVER_MODEL != OFF
  const AxisPins Axis5Pins = {AXIS5_STEP_PIN, AXIS5_DIR_PIN, AXIS5_ENABLE_PIN, false, false, true};
  Axis axis5{Axis5Pins, Axis5DriverModePins, Axis5DriverModeSettings};
  void moveAxis5() { axis5.move(AXIS5_STEP_PIN, AXIS5_DIR_PIN); }
#endif
#if AXIS6_DRIVER_MODEL != OFF
  const AxisPins Axis6Pins = {AXIS6_STEP_PIN, AXIS6_DIR_PIN, AXIS6_ENABLE_PIN, false, false, true};
  Axis axis6{Axis6Pins, Axis6DriverModePins, Axis6DriverModeSettings};
  void moveAxis6() { axis6.move(AXIS6_STEP_PIN, AXIS6_DIR_PIN); }
#endif
*/
