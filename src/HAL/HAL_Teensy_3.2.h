// Platform setup ------------------------------------------------------------------------------------
// Teensy3.0, 3.1, 3.2
#pragma once

// We define a more generic symbol, in case more Teensy boards based on different lines are supported
#define __TEENSYDUINO__

// This platform has digitalReadFast, digitalWriteFast, etc.
#define HAL_HAS_DIGITAL_FAST

// 1/500 second sidereal timer
#define HAL_FRACTIONAL_SEC 500.0F

// This platform has up to 16 bit PWM
#ifndef ANALOG_WRITE_PWM_BITS
  #define ANALOG_WRITE_PWM_BITS 12
#endif
#ifndef ANALOG_WRITE_PWM_RANGE
  #define ANALOG_WRITE_PWM_RANGE 4095
#endif

// Lower limit (fastest) step rate in uS for this platform (in SQW mode) and width of step pulse
#if F_CPU >= 120000000
  #define HAL_MAXRATE_LOWER_LIMIT 10
  #define HAL_PULSE_WIDTH 400  // in ns, estimated
  #ifndef ANALOG_WRITE_PWM_FREQUENCY
    #define ANALOG_WRITE_PWM_FREQUENCY 14648.437
  #endif
#elif F_CPU >= 96000000
  #define HAL_MAXRATE_LOWER_LIMIT 12
  #define HAL_PULSE_WIDTH 450  // in ns, estimated
  #ifndef ANALOG_WRITE_PWM_FREQUENCY
    #define ANALOG_WRITE_PWM_FREQUENCY 11718.75
  #endif
#elif F_CPU >= 72000000
  #define HAL_MAXRATE_LOWER_LIMIT 14
  #define HAL_PULSE_WIDTH 500  // in ns, estimated
  #ifndef ANALOG_WRITE_PWM_FREQUENCY
    #define ANALOG_WRITE_PWM_FREQUENCY 8789.062
  #endif
#else
  #define HAL_MAXRATE_LOWER_LIMIT 28
  #define HAL_PULSE_WIDTH 750  // in ns, estimated
#endif

// TMC2209 setup
#define TMC2209_SOFTWARE_SERIAL
#define TMC_UART_SERIAL_NO_RX
#define TMC_UART_SERIAL_BAUD 230400

// New symbol for the default I2C port -------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ------------------------------------------------------------------------------
#if NV_DRIVER == NV_DEFAULT
  #include "EEPROM.h"
  #include "../lib/nv/NV_EEPROM.h"
  #define HAL_NV_INIT() nv.init(E2END + 1, true, 0, false);
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT() { \
  analogReadResolution(10); \
  analogWriteResolution(ANALOG_WRITE_PWM_BITS); \
}

#define HAL_RESET() { \
  SCB_AIRCR = 0x05FA0004; \
  asm volatile ("dsb"); \
}

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define _Tpin 38
#define HAL_TEMP() ( (-((analogRead(_Tpin)/1024.0)*3.3-0.719)/0.001715)+25.0 )
