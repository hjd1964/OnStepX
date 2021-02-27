// Platform setup ------------------------------------------------------------------------------------

// We define a more generic symbol, in case more STM32 boards based on different lines are supported
#define __ARM_STM32__

#define HAL_FAST_PROCESSOR

#define HAL_MAXRATE_LOWER_LIMIT 14   // Lower limit (fastest) step rate in uS (in SQW mode) assumes optimization set to Fastest (-O3)
#define HAL_PULSE_WIDTH         500  // Width of step pulse

#include <HardwareTimer.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------

// SerialA is manidatory
#define SERIAL_A Serial
#if SERIAL_B_BAUD_DEFAULT != OFF
  // SerialB is optional
  #define SERIAL_B Serial1
#endif
// SerialC is optional
#if SERIAL_C_BAUD_DEFAULT != OFF
  #define SERIAL_C Serial3
#endif

// Handle special case of using software serial for a GPS
#if SerialGPS == SoftwareSerial2
  #include <SoftwareSerial.h>
  SoftwareSerial SWSerialGPS(PA3, PA2); // RX2, TX2
  #undef SerialGPS
  #define SerialGPS SWSerialGPS
#endif

// New symbol for the default I2C port ---------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ------------------------------------------------------------------------------
#undef E2END
#ifdef NV_DEFAULT
  #if PINMAP == FYSETC_S6_2
    // The FYSETC S6 v2 has a 4096 byte EEPROM built-in
    #define E2END 4095
    #define I2C_EEPROM_ADDRESS 0x50
  #elif PINMAP == FYSETC_S6
    // The FYSETC S6 has a 2048 byte EEPROM built-in
    #define E2END 2047
    #define I2C_EEPROM_ADDRESS 0x50
  #else
    // fall back to the DS3231/AT24C32
    #define E2END 4095
    #define I2C_EEPROM_ADDRESS 0x57
  #endif
  #include "../lib/nv/NV_24XX.h"
  #define NVS NonVolatileStorage24XX
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT { analogWriteResolution(8); nv.init(E2END + 1, &Wire, I2C_EEPROM_ADDRESS); }

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP ( -999 )

// Allow MCU reset -----------------------------------------------------------------------------------
#define HAL_RESET NVIC_SystemReset()
