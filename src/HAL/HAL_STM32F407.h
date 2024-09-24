// Platform setup ------------------------------------------------------------------------------------
#pragma once

// We define a more generic symbol, in case more STM32 boards based on different lines are supported
#define __ARM_STM32__

// Base rate for critical task timing (0.0038s = 0.06", 0.2 sec/day)
#define HAL_FRACTIONAL_SEC 263.1578947F

// Analog read and write
#ifndef ANALOG_READ_RANGE
  #define ANALOG_READ_RANGE 1023
#endif
#ifndef ANALOG_WRITE_RANGE
  #define ANALOG_WRITE_RANGE 255
#endif
#ifndef ANALOG_WRITE_PWM_BITS
  #define ANALOG_WRITE_PWM_BITS 8 // up to 16 bits
#endif

// Lower limit (fastest) step rate in uS for this platform (in SQW mode) and width of step pulse
#define HAL_FAST_PROCESSOR
#define HAL_MAXRATE_LOWER_LIMIT 14   // assumes optimization set to Fastest (-O3)
#define HAL_PULSE_WIDTH         250  // in ns, estimated

#include <HardwareTimer.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

// New symbol for the default I2C port ---------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
// no HAL_WIRE_CLOCK, it breaks the STM32 library

// Non-volatile storage ------------------------------------------------------------------------------
#if NV_DRIVER == NV_DEFAULT
  #define NV_ENDURANCE NVE_LOW
  #include "EEPROM.h"
  #include "../lib/nv/NV_EEPROM.h"
  #define HAL_NV_INIT() nv.init(E2END + 1, true, 0, false)
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT() { \
  HAL_Wire.setSDA(PB7); \
  HAL_Wire.setSCL(PB6); \
  analogWriteResolution(ANALOG_WRITE_PWM_BITS); \
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP() ( NAN )

//---------------------------------------------------------------------------------------------------
// Misc. includes to support this processor's operation

// MCU reset
#define HAL_RESET() NVIC_SystemReset()

// a really short fixed delay (none needed)
#define HAL_DELAY_25NS()

// stand-in for delayNanoseconds()
#define delayNanoseconds(ns) delayMicroseconds(ceilf(ns/1000.0F))
