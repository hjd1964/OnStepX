// Platform setup ------------------------------------------------------------------------------------
#pragma once

// We define a more generic symbol, in case more STM32 boards based on different lines are supported
#define __ARM_STM32__

#define HAL_FAST_PROCESSOR

// Base rate for critical task timing (0.0057s = 0.09", 0.2 sec/day)
#define HAL_FRACTIONAL_SEC 175.4385965F

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

// Lower limit (fastest) step rate in uS for this platform (in SQW mode) and width of step pulse
#ifdef STM32F401xC
  #define HAL_MAXRATE_LOWER_LIMIT    20  // assumes optimization set to Fastest (-O3)
  #define HAL_PULSE_WIDTH           300  // in ns, estimated
#elif STM32F411xE
  #define HAL_MAXRATE_LOWER_LIMIT    20  // Lower limit (fastest) step rate in us (in SQW mode) assumes optimization set to Fastest (-O3)
  #define HAL_PULSE_WIDTH           300  // in ns, estimated
#else
  #define HAL_MAXRATE_LOWER_LIMIT    60
  #define HAL_PULSE_WIDTH           900  // in ns, estimated
#endif

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
  #if PINMAP == MaxSTM3I
    // The MaxPCB3I has an 8192 byte EEPROM built-in (rated for 5M write cycles)
    #define NV_DRIVER NV_2464
  #else
    // fall back to the DS3231/AT24C32
    #define NV_DRIVER NV_AT24C32
  #endif
#endif

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP() ( NAN )

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT() { \
  analogWriteResolution((int)log2(ANALOG_WRITE_RANGE + 1)); \
}

//---------------------------------------------------------------------------------------------------
// Misc. includes to support this processor's operation

// always bring in the software serial library early as strange things happen otherwise
#include <SoftwareSerial.h>

// MCU reset
#define HAL_RESET() NVIC_SystemReset()

// a really short fixed delay (none needed)
#define HAL_DELAY_25NS()

// stand-in for delayNanoseconds()
#define delayNanoseconds(ns) delayMicroseconds(ceilf(ns/1000.0F))
