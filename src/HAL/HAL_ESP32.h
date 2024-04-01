// Platform setup ------------------------------------------------------------------------------------
#pragma once

// This is for fast processors with hardware FP
#define HAL_FAST_PROCESSOR

// Base rate for critical task timing (0.0095s = 0.14", 0.2 sec/day)
#define HAL_FRACTIONAL_SEC 105.2631579F

// Analog read and write
#ifndef ANALOG_WRITE_RANGE
  #define ANALOG_WRITE_RANGE 1023
#endif
#ifndef ANALOG_WRITE_PWM_BITS
  #define ANALOG_WRITE_PWM_BITS 10
#endif

// Lower limit (fastest) step rate in uS for this platform (in SQW mode) and width of step pulse
#if !defined(ESP_ARDUINO_VERSION) || ESP_ARDUINO_VERSION <= 131072 + 0 // version 2.0.0
  #define HAL_MAXRATE_LOWER_LIMIT 16
#else
  #define HAL_MAXRATE_LOWER_LIMIT 40
#endif
#define HAL_PULSE_WIDTH 200  // in ns, measured 1/18/22 (ESP32 v2.0.0)

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#ifndef HAL_WIRE_CLOCK
  #define HAL_WIRE_CLOCK 100000
#endif

// Non-volatile storage ----------------------------------------------------------------------------
#if NV_DRIVER == NV_DEFAULT
  #define E2END 4095
  #define NV_ENDURANCE NVE_LOW
  #include "../lib/nv/NV_ESP.h"
  #define HAL_NV_INIT() nv.init(E2END + 1, false, 5000, false)
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL, optionally also early init of SERIAL_SIP/PIP or SERIAL_BT

#if SERIAL_BT_MODE == SLAVE
  #include <BluetoothSerial.h>
  extern BluetoothSerial bluetoothSerial;
  #define SERIAL_BT bluetoothSerial
#endif
#if defined(SERIAL_BT)
  #define SERIAL_BT_BEGIN() SERIAL_BT.begin(SERIAL_BT_NAME);
#else
  #define SERIAL_BT_BEGIN()
#endif

#if SERIAL_BT_MODE != OFF && SERIAL_IP_MODE != OFF
  #error "Configuration (Config.h): SERIAL_BT_MODE and SERIAL_IP_MODE can't be enabled at the same time, disable one or both options."
#endif

#if !defined(ESP_ARDUINO_VERSION) || ESP_ARDUINO_VERSION < 131072 + 3 // version 2.0.3
  #ifndef ANALOG_READ_RANGE
    #define ANALOG_READ_RANGE 4095
  #endif
  #define HAL_INIT() { \
    analogWriteResolution(ANALOG_WRITE_PWM_BITS); \
    SERIAL_BT_BEGIN(); \
    if (I2C_SDA_PIN != OFF && I2C_SCL_PIN != OFF) { \
      HAL_Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); \
      HAL_Wire.setClock(HAL_WIRE_CLOCK); \
    } \
  }
#else
  #ifdef ANALOG_WRITE_PWM_FREQUENCY
    #ifndef ANALOG_READ_RANGE
      #define ANALOG_READ_RANGE 1023
    #endif
    #define HAL_INIT() { \
      analogReadResolution(10); \
      analogWriteResolution(ANALOG_WRITE_PWM_BITS); \
      analogWriteFrequency(ANALOG_WRITE_PWM_FREQUENCY); \
      SERIAL_BT_BEGIN(); \
      if (I2C_SDA_PIN != OFF && I2C_SCL_PIN != OFF) { \
        HAL_Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); \
        HAL_Wire.setClock(HAL_WIRE_CLOCK); \
      } \
    }
  #else
    #define HAL_INIT() { \
      analogReadResolution(10); \
      analogWriteResolution(ANALOG_WRITE_PWM_BITS); \
      SERIAL_BT_BEGIN(); \
      if (I2C_SDA_PIN != OFF && I2C_SCL_PIN != OFF) { \
        HAL_Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); \
        HAL_Wire.setClock(HAL_WIRE_CLOCK); \
      } \
    }
  #endif
#endif

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
// Correction for ESP32's internal temperture sensor
#ifndef INTERNAL_TEMP_CORRECTION
#define INTERNAL_TEMP_CORRECTION 0
#endif
#define HAL_TEMP() ( temperatureRead() + INTERNAL_TEMP_CORRECTION )

//---------------------------------------------------------------------------------------------------
// Misc. includes to support this processor's operation
#include "../lib/analog/AN_ESP32.h"

// MCU reset
#define HAL_RESET() ESP.restart()

// a really short fixed delay (none needed)
#define HAL_DELAY_25NS()

#ifdef ARDUINO_ESP32C3_DEV
  // stand-in for delayNanoseconds(), assumes 80MHz clock
  #define delayNanoseconds(ns) { unsigned int c = ESP.getCycleCount() + ns/12.5F; do {} while ((int)(ESP.getCycleCount() - c) < 0); }
#else
  // stand-in for delayNanoseconds(), assumes 240MHz clock
  #include "xtensa/core-macros.h"
  #define delayNanoseconds(ns) { unsigned int c = xthal_get_ccount() + ns/4.166F; do {} while ((int)(xthal_get_ccount() - c) < 0); }
#endif
