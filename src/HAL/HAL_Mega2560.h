// Platform setup ------------------------------------------------------------------------------------
// Mega2560
#pragma once

// This platform has digitalReadFast, digitalWriteFast, etc.
#define HAL_HAS_DIGITAL_FAST

// This platform doesn't support true double precision math
#define HAL_NO_DOUBLE_PRECISION

// This is for ~16MHz AVR processors or similar.
#define HAL_SLOW_PROCESSOR

// Lower limit (fastest) step rate in uS for this platform (in SQW mode)
#define HAL_MAXRATE_LOWER_LIMIT   76.8
#define HAL_PULSE_WIDTH          10000

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------

// SerialA is manidatory
#define SERIAL_A Serial
// SerialB is optional
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B Serial1
#endif
// SerialC is optional
#if SERIAL_C_BAUD_DEFAULT != OFF
  #if PINMAP != Classic
    #if PINMAP != Rumba
      #define SERIAL_C Serial2
    #else
      #define SERIAL_C Serial3
    #endif
  #endif
#endif

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ----------------------------------------------------------------------------
#ifdef NV_DEFAULT
  #include "../lib/nv/NV_EEPROM.h"
  #define NVS NonVolatileStorageEEPROM
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT { nv.init(0); }

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP ( -999 )
