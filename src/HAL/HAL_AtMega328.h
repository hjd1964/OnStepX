// Platform setup ------------------------------------------------------------------------------------
// AtMega328
#pragma once

// We define a more generic symbol, in case more Teensy boards based on different lines are supported
// __AVR__

// This platform doesn't support true double precision math
#define HAL_NO_DOUBLE_PRECISION

// This platform has 8 bit PWM
#ifndef HAL_ANALOG_WRITE_BITS
  #define HAL_ANALOG_WRITE_BITS 8
#endif

// This is for ~16MHz AVR processors or similar.
#define HAL_SLOW_PROCESSOR

// Lower limit (fastest) step rate in uS for this platform (in SQW mode) and width of step pulse
#define HAL_MAXRATE_LOWER_LIMIT 76.8
#define HAL_PULSE_WIDTH         10000

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------

// SerialA is manidatory
#define SERIAL_A Serial

// New symbol for the default I2C port -------------------------------------------------------------
//#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ----------------------------------------------------------------------------
#ifdef NV_DEFAULT
  #include "../lib/nv/NV_EEPROM.h"
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT() { nv.init(E2END + 1, false, 0, false); }

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP() ( NAN )

//---------------------------------------------------------------------------------------------------
// Misc.
#define strtof strtod // there us no strtof defined for the AVR platform
