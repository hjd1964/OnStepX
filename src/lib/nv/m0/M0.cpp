// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in, flash emulation for Arduino M0)

#include "M0.h"

#if defined(NV_DRIVER) && NV_DRIVER == NV_M0

bool NonVolatileStorageM0::init(uint16_t size, bool cacheEnable, uint16_t wait, bool checkEnable, TwoWire* wire, uint8_t address) {
  if (E2END + 1 != 1024 || wait == false) return false;

  // setup size, cache, etc.
  NonVolatileStorage::init(E2END + 1, NV_CACHED, NV_WAIT, false);

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

NonVolatileStorageM0 nv;

#endif
