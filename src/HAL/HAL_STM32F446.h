// Platform setup ------------------------------------------------------------------------------------
#pragma once

// We define a more generic symbol, in case more STM32 boards based on different lines are supported
#define __ARM_STM32__

#define HAL_FAST_PROCESSOR

// 1/500 second sidereal timer
#define HAL_FRACTIONAL_SEC 500.0F

// This platform has 16 bit PWM
#ifndef HAL_ANALOG_WRITE_BITS
  #define HAL_ANALOG_WRITE_BITS 8
#endif
#ifndef HAL_ANALOG_WRITE_RANGE
  #define HAL_ANALOG_WRITE_RANGE 255
#endif

// Lower limit (fastest) step rate in uS for this platform (in SQW mode) and width of step pulse
#define HAL_MAXRATE_LOWER_LIMIT 14   // assumes optimization set to Fastest (-O3)
#define HAL_PULSE_WIDTH         500  // in ns

#include <HardwareTimer.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

// New symbol for the default I2C port ---------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ------------------------------------------------------------------------------
#undef E2END
#ifdef NV_DEFAULT
  #if PINMAP == FYSETC_S6_2
    // The FYSETC S6 v2 has a 4096 byte EEPROM built-in
    #define E2END 4095
    #define NV_EEPROM_ADDRESS 0x50
  #elif PINMAP == FYSETC_S6
    // The FYSETC S6 has a 2048 byte EEPROM built-in
    #define E2END 2047
    #define NV_EEPROM_ADDRESS 0x50
  #else
    // fall back to the DS3231/AT24C32
    #define E2END 4095
    #define NV_EEPROM_ADDRESS 0x57
  #endif
  #include "../lib/nv/NV_24XX.h"
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT() { \
  analogWriteResolution(HAL_ANALOG_WRITE_BITS); \
  nv.init(E2END + 1, true, 0, false, &HAL_Wire, NV_EEPROM_ADDRESS); \
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP() ( NAN )

// Allow MCU reset -----------------------------------------------------------------------------------
#define HAL_RESET() NVIC_SystemReset()
