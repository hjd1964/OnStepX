// Platform setup ------------------------------------------------------------------------------------
// Teensy3.6
#pragma once

// We define a more generic symbol, in case more Teensy boards based on different lines are supported
#define __TEENSYDUINO__

// This platform has digitalReadFast, digitalWriteFast, etc.
#define HAL_HAS_DIGITAL_FAST

// This platform has 16 bit PWM
#ifndef HAL_ANALOG_WRITE_BITS
  #define HAL_ANALOG_WRITE_BITS 8
#endif

// Lower limit (fastest) step rate in uS for this platform (in SQW mode) and width of step pulse
#if F_CPU >= 240000000
  #define HAL_MAXRATE_LOWER_LIMIT 2
  #define HAL_PULSE_WIDTH 260
#elif F_CPU >= 180000000
  #define HAL_MAXRATE_LOWER_LIMIT 2.6
  #define HAL_PULSE_WIDTH 400
#else
  #define HAL_MAXRATE_LOWER_LIMIT 4.8
  #define HAL_PULSE_WIDTH 500
#endif
#define HAL_FAST_PROCESSOR

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------

// SerialA is manidatory
#define SERIAL_A Serial
// SerialB is optional
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B Serial1
#endif
// SerialC is optional
#if SERIAL_C_BAUD_DEFAULT != OFF
  #define SERIAL_C Serial4
#endif
// SerialD/E are optional
#if defined(USB_DUAL_SERIAL) || defined(USB_TRIPLE_SERIAL)
  #if defined(SERIAL_D_BAUD_DEFAULT) && SERIAL_D_BAUD_DEFAULT != OFF
    #define SERIAL_D SerialUSB1
  #endif
#endif
#if defined(USB_TRIPLE_SERIAL)
  #if defined(SERIAL_E_BAUD_DEFAULT) && SERIAL_E_BAUD_DEFAULT != OFF
    #define SERIAL_E SerialUSB2
  #endif
#endif

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire1
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ------------------------------------------------------------------------------
#ifdef NV_DEFAULT
  #include "EEPROM.h"
  #include "../lib/nv/NV_EEPROM.h"
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT() { \
  analogReadResolution(10); \
  analogWriteResolution(HAL_ANALOG_WRITE_BITS); \
  nv.init(E2END + 1, true, 0, false); \
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define _Tpin 70
#define HAL_TEMP() ( (-((analogRead(_Tpin)/1024.0)*3.3-0.719)/0.001715)+25.0 )

//--------------------------------------------------------------------------------------------------
// for using the DAC as a digital output on Teensy3.6 A21=66 A22=67
//#define digitalWrite(x,y) { if (x==66 || x==67) { if ((y)==LOW) analogWrite(x,0); else analogWrite(x,255); } else digitalWrite(x,y); }
