// Platform setup ------------------------------------------------------------------------------------

// This is for fast processors with hardware FP
#define HAL_FAST_PROCESSOR

// Lower limit (fastest) step rate in uS for this platform (in SQW mode)
#define HAL_MAXRATE_LOWER_LIMIT 16

// Width of step pulse
#define HAL_PULSE_WIDTH 2500

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------

// SerialA is manidatory
#define HAL_SERIAL_A Serial
// SerialB is optional
#if SERIAL_B_BAUD_DEFAULT != OFF
  #define SERIAL_B Serial2
#endif
// SerialC is optional
#if SERIAL_C_BAUD_DEFAULT != OFF
  #if SERIAL_C_BAUD_DEFAULT > 0
    #error "Bluetooth on ESP32 SERIAL_C_BAUD_DEFAULT gets set to ON or OFF only."
  #endif
  #include <BluetoothSerial.h>
  BluetoothSerial bluetoothSerial;
  #define SERIAL_C bluetoothSerial
  #undef  SERIAL_C_BAUD_DEFAULT
  #define SERIAL_C_BAUD_DEFAULT SERIAL_C_BLUETOOTH_NAME
#endif
// SerialD is optional
#if PINMAP == InsteinESP1
  #ifndef SERIAL_D_BAUD_DEFAULT
    #define SERIAL_D_BAUD_DEFAULT 9600
  #endif
  #define SERIAL_D Serial1
  #define SERIAL_D_RX 21
  #define SERIAL_D_TX 22
  #define HAL_SERIAL_D_ENABLED
#elif defined(SERIAL_D_BAUD_DEFAULT)
  #if SERIAL_D_BAUD_DEFAULT != OFF
    #define SERIAL_D Serial1
    #define HAL_SERIAL_D_ENABLED
  #endif
#endif

// New symbol for the default I2C port ---------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

#include "HAL_ESP32_Analog.h"

// Non-volatile storage ----------------------------------------------------------------------------
#ifdef NV_DEFAULT
  #define E2END 4095
  #include "../lib/nv/NV_ESP32.h"
  #define NVS NonVolatileStorageESP32
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT { nv.init(E2END + 1); }

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
// Correction for ESP32's internal temperture sensor
#define INTERNAL_TEMP_CORRECTION 0
#define HAL_TEMP ( temperatureRead() + INTERNAL_TEMP_CORRECTION )
