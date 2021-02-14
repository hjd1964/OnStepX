//--------------------------------------------------------------------------------------------------
// telescope mount goto control
#pragma once
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../commands/ProcessCmds.h"
#include "../StepDrivers/StepDrivers.h"
#include "Axis.h"

class Goto {
  public:
    void init();
  private:
};

#endif
