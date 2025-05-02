// Platform setup ------------------------------------------------------------------------------------
#pragma once

// This is for fast processors with hardware FP
#define HAL_FAST_PROCESSOR

// Base rate for critical task timing (0.0095s = 0.14", 0.2 sec/day)
#define HAL_FRACTIONAL_SEC 105.2631579F

// Analog read and write
#ifndef HAL_VCC
  #define HAL_VCC 3.3F
#endif
#ifndef ANALOG_READ_RANGE
  #define ANALOG_READ_RANGE 1023 // always use 2^n - 1, within the platform's limits
#else
  #error "Configuration (Config.h): ANALOG_READ_RANGE can't be changed on this platform"
#endif
#ifndef ANALOG_WRITE_RANGE
  #define ANALOG_WRITE_RANGE 255 // always use 2^n - 1, within the platform's limits
#endif

// Lower limit (fastest) step rate for this platform (in SQW mode) and width of step pulse
#define HAL_MAXRATE_LOWER_LIMIT 16
#define HAL_PULSE_WIDTH 200 // in ns, measured 1/18/22 (ESP32 v2.0.0)

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#ifndef HAL_WIRE
  #define HAL_WIRE Wire
#endif
#ifndef HAL_WIRE_CLOCK
  #define HAL_WIRE_CLOCK 100000
#endif

// Non-volatile storage ----------------------------------------------------------------------------
#if NV_DRIVER == NV_DEFAULT
  #undef NV_DRIVER
  #define NV_DRIVER NV_ESP
#endif

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
// Correction for ESP32's internal temperture sensor
#ifndef INTERNAL_TEMP_CORRECTION
  #define INTERNAL_TEMP_CORRECTION 0
#endif
#define HAL_TEMP() ( temperatureRead() + INTERNAL_TEMP_CORRECTION )

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL, optionally also early init of SERIAL_SIP/PIP or SERIAL_BT
#ifndef SERIAL_BT_MODE
  #define SERIAL_BT_MODE OFF
#endif
#if SERIAL_BT_MODE != OFF
  #include <BluetoothSerial.h>
  extern BluetoothSerial bluetoothSerial;
  #define SERIAL_BT bluetoothSerial
#endif
#if SERIAL_BT_MODE == SLAVE
  #ifdef SERIAL_BT_PASSKEY
    #define SERIAL_BT_BEGIN() if (strlen(SERIAL_BT_PASSKEY) != 0) SERIAL_BT.setPin(SERIAL_BT_PASSKEY); SERIAL_BT.begin(SERIAL_BT_NAME)
  #else
    #define SERIAL_BT_BEGIN() SERIAL_BT.begin(SERIAL_BT_NAME)
  #endif
#else
  #define SERIAL_BT_BEGIN()
#endif

#ifndef ANALOG_READ_RANGE
  #define ANALOG_READ_RANGE 4095
#endif
#define HAL_INIT() { \
  analogWriteResolution((int)log2(ANALOG_WRITE_RANGE + 1)); \
  SERIAL_BT_BEGIN(); \
}

//---------------------------------------------------------------------------------------------------
// Misc. includes to support this processor's operation
#include "../../lib/analog/AN_ESP32.h"

// MCU reset
#define HAL_RESET() ESP.restart()

// a really short fixed delay (none needed)
#define HAL_DELAY_25NS()

// stand-in for delayNanoseconds(), assumes 240MHz clock
#include "xtensa/core-macros.h"
#define delayNanoseconds(ns) { unsigned int c = xthal_get_ccount() + ns/4.166F; do {} while ((int)(xthal_get_ccount() - c) < 0); }
