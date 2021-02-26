//--------------------------------------------------------------------------------------------------
// telescope mount control, guide
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;

#include "Axis.h"
#include "Mount.h"



#endif