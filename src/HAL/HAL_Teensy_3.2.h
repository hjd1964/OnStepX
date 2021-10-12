// Platform setup ------------------------------------------------------------------------------------
// Teensy3.0, 3.1, 3.2
#pragma once

// We define a more generic symbol, in case more Teensy boards based on different lines are supported
#define __TEENSYDUINO__

// This platform has digitalReadFast, digitalWriteFast, etc.
#define HAL_HAS_DIGITAL_FAST

// 1/500 second sidereal timer
#define HAL_FRACTIONAL_SEC 500.0

// This platform has 16 bit PWM
#ifndef HAL_ANALOG_WRITE_BITS
  #define HAL_ANALOG_WRITE_BITS 8
#endif
#ifndef HAL_ANALOG_WRITE_RANGE
  #define HAL_ANALOG_WRITE_RANGE 255
#endif

// Lower limit (fastest) step rate in uS for this platform (in SQW mode) and width of step pulse
#if F_CPU >= 120000000
  #define HAL_MAXRATE_LOWER_LIMIT 10
  #define HAL_PULSE_WIDTH 800
#elif F_CPU >= 96000000
  #define HAL_MAXRATE_LOWER_LIMIT 12
  #define HAL_PULSE_WIDTH 900
#elif F_CPU >= 72000000
  #define HAL_MAXRATE_LOWER_LIMIT 14
  #define HAL_PULSE_WIDTH 1000
#else
  #define HAL_MAXRATE_LOWER_LIMIT 28
  #define HAL_PULSE_WIDTH 1500
#endif

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ------------------------------------------------------------------------------
#ifdef NV_DEFAULT
  #include "EEPROM.h"
  #include "../lib/nv/NV_EEPROM.h"
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT() { \
  analogReadResolution(10); \
  analogWriteResolution(HAL_ANALOG_WRITE_BITS); \
  nv.init(E2END + 1, true, 0, false); \
}

#define HAL_RESET() { \
  SCB_AIRCR = 0x05FA0004; \
  asm volatile ("dsb"); \
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define _Tpin 38
#define HAL_TEMP() ( (-((analogRead(_Tpin)/1024.0)*3.3-0.719)/0.001715)+25.0 )
