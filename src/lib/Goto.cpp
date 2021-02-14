//--------------------------------------------------------------------------------------------------
// telescope mount goto control
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../coordinates/Convert.h"
extern Convert convert;
#include "../coordinates/Transform.h"
extern Transform transform;
#include "../commands/ProcessCmds.h"
extern GeneralErrors generalErrors;
#include "../StepDrivers/StepDrivers.h"
#include "Axis.h"
#include "Clock.h"
extern Clock clock;
#include "Goto.h"

void Goto::init() {
}

#endif
