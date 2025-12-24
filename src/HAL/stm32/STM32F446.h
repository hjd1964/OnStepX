// Platform setup ------------------------------------------------------------------------------------
#pragma once

// We define a more generic symbol, in case more STM32 boards based on different lines are supported
#define __ARM_STM32__

// Base rate for critical task timing (0.0038s = 0.06", 0.2 sec/day)
#define HAL_FRACTIONAL_SEC 263.1578947F

// Analog read and write
#ifndef HAL_VCC
  #define HAL_VCC 3.3F
#endif
#ifndef ANALOG_READ_RANGE
  #define ANALOG_READ_RANGE 1023
#else
  #error "Configuration (Config.h): ANALOG_READ_RANGE can't be changed on this platform"
#endif
#ifndef ANALOG_WRITE_RANGE
  #define ANALOG_WRITE_RANGE 255
#endif

// STM32 Arduino cores (F4 + H7):
// assume global resolution functions exist; per-pin freq/res is not generic.
#define HAL_HAS_GLOBAL_PWM_RESOLUTION 1
#define HAL_HAS_GLOBAL_ADC_RESOLUTION 1

#define HAL_PWM_BITS_MAX 16

// F4 ADC is 12-bit; H7 can be higher, but core support varies.
// Default to 12 and let advanced HAL override to 16 for known-good H7 setup.
#define HAL_ADC_BITS_MAX 12

// Lower limit (fastest) step rate in uS for this platform (in SQW mode) and width of step pulse
#define HAL_FAST_PROCESSOR
#define HAL_MAXRATE_LOWER_LIMIT 14   // assumes optimization set to Fastest (-O3)
#define HAL_PULSE_WIDTH 250          // in ns, estimated

#include <HardwareTimer.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

// New symbol for the default I2C port ---------------------------------------------------------------
#include <Wire.h>
#ifndef HAL_WIRE
  #define HAL_WIRE Wire
#endif
#ifndef HAL_WIRE_CLOCK
  #define HAL_WIRE_CLOCK 100000
#endif

// Non-volatile storage ------------------------------------------------------------------------------
#undef E2END
#if NV_DRIVER == NV_DEFAULT
  #undef NV_DRIVER
  #if PINMAP == FYSETC_S6_2
    // The FYSETC S6 v2 has a 4096 byte EEPROM built-in
    #define NV_DRIVER NV_2432
  #elif PINMAP == FYSETC_S6
    // The FYSETC S6 has a 2048 byte EEPROM built-in
    #define NV_DRIVER NV_2416
  #else
    // Fallback to the EEPROM present on an ZS-042 module
    #define NV_DRIVER NV_AT24C32
  #endif
#endif

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP() ( NAN )

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT() { \
}

//---------------------------------------------------------------------------------------------------
// Misc. includes to support this processor's operation

// always bring in the software serial library early as strange things happen otherwise
#include <SoftwareSerial.h>

// MCU reset
#define HAL_RESET() NVIC_SystemReset()
