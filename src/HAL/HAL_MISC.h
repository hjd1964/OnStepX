// Platform setup ------------------------------------------------------------------------------------
#pragma once

// We define a more generic symbol, in case more Platform_Name boards based on different lines are supported

// Lower limit (fastest) step rate in uS for this platform (in SQW mode)
#ifndef HAL_MAXRATE_LOWER_LIMIT
  #define HAL_MAXRATE_LOWER_LIMIT 60
#endif

// width of step pulse
#ifndef HAL_PULSE_WIDTH
  #define HAL_PULSE_WIDTH 10000
#endif

// SerialA is manidatory
#ifndef SerialA
  #define SerialA Serial
#endif

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ----------------------------------------------------------------------------
#ifdef NV_DEFAULT
  #include "../lib/nv/NV_EEPROM.h"
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#ifndef HAL_INIT
  #define HAL_INIT { nv.init(2048, true, 0, false); }
#endif

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#ifndef HAL_TEMP
  #define HAL_TEMP ( NAN )
#endif
