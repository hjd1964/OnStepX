// Platform setup ------------------------------------------------------------------------------------
// Teensy3.0, 3.1, 3.2
#pragma once

// We define a more generic symbol, in case more Teensy boards based on different lines are supported
#define __TEENSYDUINO__

// This platform has digitalReadFast, digitalWriteFast, etc.
#define HAL_HAS_DIGITAL_FAST

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

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------

// SerialA is manidatory
#define SERIAL_A Serial
// SerialB is optional
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B Serial1
#endif
// SerialC is optional, not used
// SerialD/E are optional
#if defined(USB_DUAL_SERIAL) || defined(USB_TRIPLE_SERIAL)
  #define SERIAL_D SerialUSB1
  #define SERIAL_D_BAUD_DEFAULT 9600
#endif
#if defined(USB_TRIPLE_SERIAL)
  #define SERIAL_E SerialUSB2
  #define SERIAL_E_BAUD_DEFAULT 9600
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
#define HAL_INIT() { analogReadResolution(10); nv.init(E2END + 1, true, 0, false); }

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define _Tpin 38
#define HAL_TEMP() ( (-((analogRead(_Tpin)/1024.0)*3.3-0.719)/0.001715)+25.0 )
