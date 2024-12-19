// -----------------------------------------------------------------------------------
// CAN library
#pragma once

#include "../../Common.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF
  #include "san/San.h"
  #include "esp32/Esp32.h"
  #include "mcp2515/Mcp2515.h"
  #include "teensy4/Can0.h"
  #include "teensy4/Can1.h"
  #include "teensy4/Can2.h"
  #include "teensy4/Can3.h"
#endif
