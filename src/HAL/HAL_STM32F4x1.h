// Platform setup ------------------------------------------------------------------------------------

// We define a more generic symbol, in case more STM32 boards based on different lines are supported
#define __ARM_STM32__

#define HAL_FAST_PROCESSOR

#ifdef STM32F401xC
  #define HAL_MAXRATE_LOWER_LIMIT    16  // Lower limit (fastest) step rate in us (in SQW mode) assumes optimization set to Fastest (-O3)
  #define HAL_PULSE_WIDTH           500  // Width of step pulse in ns
#elif STM32F411xE
  #define HAL_MAXRATE_LOWER_LIMIT    20  // Lower limit (fastest) step rate in us (in SQW mode) assumes optimization set to Fastest (-O3)
  #define HAL_PULSE_WIDTH           600  // Width of step pulse in ns
#else
  #define HAL_MAXRATE_LOWER_LIMIT    60  // Conservative defaults
  #define HAL_PULSE_WIDTH         10000
#endif

#include <HardwareTimer.h>

// Interrupts
#define cli() noInterrupts()
#define sei() interrupts()

// New symbols for the Serial ports so they can be remapped if necessary -----------------------------
// HardwareSerial HWSerial1(PA10, PA9);  // RX1, TX1
// HardwareSerial HWSerial2(PA3 , PA2);  // RX2, TX2
// HardwareSerial HWSerial6(PA12, PA11); // RX6, TX6

// SerialA is manidatory
#define SERIAL_A Serial
// SerialB is optional
#if SERIAL_B_BAUD_DEFAULT != OFF
  HardwareSerial HWSerial2(PA3, PA2);    // RX2, TX2
  #define SERIAL_B HWSerial2
#endif
// SerialC is optional
#if SERIAL_C_BAUD_DEFAULT != OFF
  HardwareSerial HWSerial1(PA10, PA9); // RX1, TX1
  #define SERIAL_C HWSerial1
#endif
  
// New symbol for the default I2C port ---------------------------------------------------------------
#include <Wire.h>
#define HAL_Wire Wire
#define HAL_WIRE_CLOCK 100000

// Non-volatile storage ------------------------------------------------------------------------------
#undef E2END
#if defined(NV_M24C32)
  // The MaxPCB3I has an 8192 byte EEPROM built-in (rated for 5M write cycles)
  #define NV_ENDURANCE HIGH
  #define E2END 8191
  #define I2C_EEPROM_ADDRESS 0x50
  #include "NV/NV_I2C_EEPROM_24XX_C.h"
#elif defined(NV_MB85RC256V)
  #include "NV/NV_I2C_FRAM_MB85RC256V.h"
#else
  // defaults to 0x57 and 4KB as used on DS3231 RTC modules
  #include "NV/NV_I2C_EEPROM_24XX_C.h"
#endif

//--------------------------------------------------------------------------------------------------
// General purpose initialize for HAL
#define HAL_INIT { analogWriteResolution(8); }

//--------------------------------------------------------------------------------------------------
// Internal MCU temperature (in degrees C)
#define HAL_TEMP ( -999 )

// Allow MCU reset -----------------------------------------------------------------------------------
#define HAL_RESET NVIC_SystemReset()
