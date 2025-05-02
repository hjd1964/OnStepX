// Platform setup ------------------------------------------------------------------------------------
#pragma once

// We define a more generic symbol, in case more Teensy boards based on different lines are supported
#define __TEENSYDUINO__

// This platform has digitalReadFast, digitalWriteFast, etc.
#define HAL_HAS_DIGITAL_FAST

// Base rate for critical task timing (0.0019s = 0.03", 0.2 sec/day)
#define HAL_FRACTIONAL_SEC 526.3157895F

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

// Lower limit (fastest) step rate in uS for this platform (in SQW mode) and width of step pulse
#define HAL_MAXRATE_LOWER_LIMIT 1.5
#define HAL_PULSE_WIDTH 0  // effectively disable pulse mode
#define HAL_FAST_PROCESSOR
#define HAL_VFAST_PROCESSOR

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#ifndef HAL_WIRE
  #define HAL_WIRE Wire
#endif
#ifndef HAL_WIRE_CLOCK
  #define HAL_WIRE_CLOCK 100000
#endif

// Non-volatile storage ------------------------------------------------------------------------------
#if NV_DRIVER == NV_DEFAULT
  #undef NV_DRIVER
  #define NV_DRIVER NV_EEPROM
#endif

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP() ( tempmonGetTemp() )

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#include "imxrt.h"

#define HAL_INIT() { \
  analogReadResolution((int)log2(ANALOG_READ_RANGE + 1)); \
  analogWriteResolution((int)log2(ANALOG_WRITE_RANGE + 1)); \
}

#define HAL_RESET() { \
  SCB_AIRCR = 0x05FA0004; \
  asm volatile ("dsb"); \
}

//---------------------------------------------------------------------------------------------------
// Misc. includes to support this processor's operation

#ifdef EmptyStr
  #undef EmptyStr
#endif
#define EmptyStr "\1"

#define CAT_ATTR PROGMEM

// a really short fixed delay
#define HAL_DELAY_25NS() delayNanoseconds(20)
