// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in)

#if !defined(ARDUINO_ARCH_SAMD)

  #include "Arduino.h"
  #include "EEPROM.h"
  #include "NV_EEPROM.h"

  #if defined(E2END)

    bool NonVolatileStorageEEPROM::init(uint16_t size, bool cacheEnable, uint16_t wait, bool checkEnable, TwoWire* wire, uint8_t address) {
      // setup size, cache, etc.
      NonVolatileStorage::init(size, cacheEnable, wait, checkEnable);
      return true;
    }

    uint8_t NonVolatileStorageEEPROM::readFromStorage(uint16_t i) {
      return EEPROM.read(i);
    }

    void NonVolatileStorageEEPROM::writeToStorage(uint16_t i,  uint8_t j) {
      EEPROM.write(i, j);
    }
  #endif

#endif