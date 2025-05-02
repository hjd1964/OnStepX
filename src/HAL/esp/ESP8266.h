// Platform setup ------------------------------------------------------------------------------------
#pragma once

// We define a more generic symbol, in case more Platform_Name boards based on different lines are supported

// Base rate for critical task timing
#define HAL_FRACTIONAL_SEC 100.0F

// Analog read and write
#ifndef HAL_VCC
  #define HAL_VCC 3.3F
#endif
#ifndef ANALOG_READ_RANGE
  #define ANALOG_READ_RANGE 1023
#endif
#ifndef ANALOG_WRITE_RANGE
  #define ANALOG_WRITE_RANGE 255
#endif

// Lower limit (fastest) step rate in us for this platform (in SQW mode) and width of step pulse
#define HAL_MAXRATE_LOWER_LIMIT 60
#define HAL_PULSE_WIDTH 10000 // in ns, estimated

// New symbol for the default I2C port ---------------------------------------------------------------
#include <Wire.h>
#ifndef HAL_WIRE
  #define HAL_WIRE Wire
#endif
#ifndef HAL_WIRE_CLOCK
  #define HAL_WIRE_CLOCK 100000
#endif

// Non-volatile storage ------------------------------------------------------------------------------
#if NV_DRIVER == NV_DEFAULT
  #define E2END 1023
  #undef NV_DRIVER
  #define NV_DRIVER NV_ESP
#endif

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP() ( NAN )

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT() { analogWriteRange((int)log2(ANALOG_WRITE_RANGE + 1)); }

//-----------------------------------------------------------------------------------------------------
// Misc. includes and defines to support this processor's operation

// MCU reset
#define HAL_RESET() ESP.restart()

// stand-in for delayNanoseconds(), assumes 80MHz clock
#define delayNanoseconds(ns) { unsigned int c = ESP.getCycleCount() + ns/12.5F; do {} while ((int)(ESP.getCycleCount() - c) < 0); }
