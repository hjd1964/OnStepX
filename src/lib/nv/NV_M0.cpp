// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in, flash emulation for Arduino M0)

#include "NV_M0.h"

#if defined(ARDUINO_ARCH_SAMD)

  bool NonVolatileStorageM0::init(uint16_t size, bool cacheEnable, uint16_t wait, bool checkEnable, TwoWire* wire, uint8_t address) {
    if (size != 1024 || wait == false) return false;

    // setup size, cache, etc.
    NonVolatileStorage::init(size, cacheEnable, wait, checkEnable, wire, address);

    return true;
  }

  void NonVolatileStorageM0::poll(bool disableInterrupts) {
    if (dirty && ((long)(millis() - commitReadyTimeMs) >= 0)) {
      EEPROM.commit();
      dirty = false;
    }
    // stop compiler warnings
    (void)(disableInterrupts);
  }

  bool NonVolatileStorageM0::committed() {
    return !dirty;
  }

  uint8_t NonVolatileStorageM0::readFromStorage(uint16_t i) {
    return EEPROM.read(i);
  }

  void NonVolatileStorageM0::writeToStorage(uint16_t i,  uint8_t j) {
    EEPROM.write(i, j);
    dirty = true;
  }
#endif