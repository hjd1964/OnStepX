// Platform setup ------------------------------------------------------------------------------------
#pragma once

// We define a more generic symbol, in case more Teensy boards based on different lines are supported
#define __M0__

#include <avr/dtostrf.h>

// Base rate for critical task timing
#define HAL_FRACTIONAL_SEC 100.0F

// Analog read and write
#ifndef HAL_VCC
  #define HAL_VCC 3.3F
#endif
#ifndef ANALOG_READ_RANGE
  #define ANALOG_READ_RANGE 1023 // always use 2^n - 1, within the platform's limits
#else
  #error "Configuration (Config.h): ANALOG_READ_RANGE can't be changed on this platform"
#endif
#ifndef ANALOG_WRITE_RANGE
  #define ANALOG_WRITE_RANGE 255 // always use 2^n - 1, within the platform's limits
#else
  #error "Configuration (Config.h): ANALOG_WRITE_RANGE can't be changed on this platform"
#endif

// Lower limit (fastest) step rate in us for this platform (in SQW mode) and width of step pulse
#define HAL_MAXRATE_LOWER_LIMIT 40
#define HAL_PULSE_WIDTH 900 // in ns, estimated

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
  // Library FlashStorage defaults to 1024 bytes https://github.com/cmaglie/FlashStorage
  #define E2END 1023
  #undef NV_DRIVER
  #define NV_DRIVER NV_M0
#endif

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP() ( NAN )

//----------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT() { ; }

//-----------------------------------------------------------------------------------------------------
// Misc. includes and defines to support this processor's operation

// stand-in for delayNanoseconds()
#define delayNanoseconds(ns) delayMicroseconds(ceilf(ns/1000.0F))
