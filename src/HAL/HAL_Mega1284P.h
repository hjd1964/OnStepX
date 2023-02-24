// Platform setup ------------------------------------------------------------------------------------
#pragma once

// This is for fast processors with hardware FP
#define HAL_SLOW_PROCESSOR

// 1/100 second sidereal timer
#define HAL_FRACTIONAL_SEC 100.0F

// Analog read and write
#ifndef ANALOG_READ_RANGE
  #define ANALOG_READ_RANGE 1023
#endif
#ifndef ANALOG_WRITE_RANGE
  #define ANALOG_WRITE_RANGE 255
#endif
#ifndef ANALOG_WRITE_PWM_BITS
  #define ANALOG_WRITE_PWM_BITS 8
#endif

// Lower limit (fastest) step rate in uS for this platform (in SQW mode) and width of step pulse
#define HAL_MAXRATE_LOWER_LIMIT 76.8

#define HAL_PULSE_WIDTH 0  // in ns, measured 1/18/22 (ESP32 v2.0.0)

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ----------------------------------------------------------------------------
#if NV_DRIVER == NV_DEFAULT
  #include "../lib/nv/NV_EEPROM.h"
  #define HAL_NV_INIT() nv.init(2048, true, 0, false);
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT() { ; }

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP() ( -999 )

// MCU reset
//#define HAL_RESET() ESP.restart()

// a really short fixed delay (none needed)
//#define HAL_DELAY_25NS()

// stand-in for delayNanoseconds(), assumes 240MHz clock
//#include "xtensa/core-macros.h"
//#define delayNanoseconds(ns) { unsigned int c = xthal_get_ccount() + ns/4.166F; do {} while ((int)(xthal_get_ccount() - c) < 0); }
