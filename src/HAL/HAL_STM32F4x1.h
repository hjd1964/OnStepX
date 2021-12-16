// Platform setup ------------------------------------------------------------------------------------
#pragma once

// We define a more generic symbol, in case more STM32 boards based on different lines are supported
#define __ARM_STM32__

#define HAL_FAST_PROCESSOR

// 1/500 second sidereal timer
#define HAL_FRACTIONAL_SEC 500.0F

// This platform has 16 bit PWM
#ifndef ANALOG_WRITE_PWM_BITS
  #define ANALOG_WRITE_PWM_BITS 8
#endif
#ifndef ANALOG_WRITE_PWM_RANGE
  #define ANALOG_WRITE_PWM_RANGE 255
#endif

// Lower limit (fastest) step rate in uS for this platform (in SQW mode) and width of step pulse
#ifdef STM32F401xC
  #define HAL_MAXRATE_LOWER_LIMIT    16  // assumes optimization set to Fastest (-O3)
  #define HAL_PULSE_WIDTH           500  // in ns
#elif STM32F411xE
  #define HAL_MAXRATE_LOWER_LIMIT    20  // Lower limit (fastest) step rate in us (in SQW mode) assumes optimization set to Fastest (-O3)
  #define HAL_PULSE_WIDTH           600  // in ns
#else
  #define HAL_MAXRATE_LOWER_LIMIT    60
  #define HAL_PULSE_WIDTH         10000
#endif

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
  #if PINMAP == MaxSTM3I
    // The MaxPCB3I has an 8192 byte EEPROM built-in (rated for 5M write cycles)
    #define E2END 8191
    #undef NV_ENDURANCE
    #define NV_ENDURANCE HIGH
    #define NV_EEPROM_ADDRESS 0x50
    #undef  NV_ENDURANCE
    #define NV_ENDURANCE NVE_HIGH
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
  analogWriteResolution(ANALOG_WRITE_PWM_BITS); \
  nv.init(E2END + 1, true, 0, false, &HAL_Wire, NV_EEPROM_ADDRESS); \
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP() ( NAN )

// Allow MCU reset -----------------------------------------------------------------------------------
#define HAL_RESET() NVIC_SystemReset()
