// Platform setup ------------------------------------------------------------------------------------
#pragma once

// We define a more generic symbol, in case more Platform_Name boards based on different lines are supported
#define __Platform_Name__

// Lower limit (fastest) step rate in uS for this platform (in SQW mode) and width of step pulse
#define HAL_MAXRATE_LOWER_LIMIT  60
#define HAL_PULSE_WIDTH          10000  // in ns

// SerialA is manidatory
#define SerialA Serial
// SerialB, SerialC, SerialD are optional

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ----------------------------------------------------------------------------
// Non-volatile storage ----------------------------------------------------------------------------
#ifdef NV_DEFAULT
  #include "../lib/nv/NV_EEPROM.h"
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT { nv.init(0); }

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP ( NAN )
