// Platform setup ----------------------------------------------------------------------------------
// Teensy3.5
#pragma once

// We define a more generic symbol, in case more Teensy boards based on different lines are supported
#define __TEENSYDUINO__

// This platform has digitalReadFast, digitalWriteFast, etc.
#define HAL_HAS_DIGITAL_FAST

// Turn on support for using DACs to output digital signals using Arduino standard commands
#define HAL_DAC_AS_DIGITAL

// Base rate for critical task timing (0.0057s = 0.09", 0.2 sec/day)
#define HAL_FRACTIONAL_SEC 175.4385965F

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
#define HAL_MAXRATE_LOWER_LIMIT 12
#define HAL_PULSE_WIDTH 400  // in ns, measured 1/18/22
#define HAL_FAST_PROCESSOR

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#ifndef HAL_WIRE
  #define HAL_WIRE Wire1
#endif
#ifndef HAL_WIRE_CLOCK
  #define HAL_WIRE_CLOCK 100000
#endif

// Non-volatile storage ----------------------------------------------------------------------------
#if NV_DRIVER == NV_DEFAULT
  #undef NV_DRIVER
  #define NV_DRIVER NV_EEPROM
#endif

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define _Tpin 70
#define HAL_TEMP() ( (-((analogRead(_Tpin)/1024.0)*3.3-0.719)/0.001715)+25.0 )

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
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

// a really short fixed delay (none needed)
#define HAL_DELAY_25NS()
