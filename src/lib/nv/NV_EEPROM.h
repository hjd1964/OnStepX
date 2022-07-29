// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in)

#pragma once

#include <Arduino.h>

#if !defined(ARDUINO_ARDUINO_NANO33BLE) && !defined(ARDUINO_ARCH_SAMD) && !defined(ARDUINO_ARCH_MBED_RP2040) && !defined(ARDUINO_ARCH_RP2040)

  #include "NV.h"
  
  // don't bring in the EEPROM library here if this is an Mega2560, etc.
  #ifndef __AVR__
    #include "EEPROM.h"
  #endif

  #if defined(E2END)

    class NonVolatileStorageEEPROM : public NonVolatileStorage {
      public:
        // prepare      FLASH based EEPROM emulation for operation
        // size:        NV size in bytes
        // cacheEnable: enable or disable the cache (note NV size must be divisible by 8 if enabled)
        // wait:        minimum time in milliseconds to wait (after last write) before writing cache or doing the commit
        // checkEnable: enable or disable checksum error detection
        // wire:        I2C interface pointer (set to NULL if not used)
        // address:     I2C address
        // result:      true if the device was found, or false if not
        bool init(uint16_t size, bool cacheEnable, uint16_t wait, bool checkEnable, TwoWire* wire = NULL, uint8_t address = 0);

      private:
        // read byte at position i from storage
        uint8_t readFromStorage(uint16_t i);

        // write value j to position i in storage 
        void writeToStorage(uint16_t i, uint8_t j);  
    };

    #define NVS NonVolatileStorageEEPROM

  #endif

#endif