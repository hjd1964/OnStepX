// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in)

#include "EEPROM.h"

#if defined(NV_DRIVER) && NV_DRIVER == NV_EEPROM

// bring in the EEPROM library here if this is an Mega2560, etc.
#ifdef __AVR__
  #include <EEPROM.h>
#endif

bool NonVolatileStorageEEPROM::init() {
  // setup size, cache, etc.
  return NonVolatileStorage::init(E2END + 1, NV_CACHED, NV_WAIT, false);
}

uint8_t NonVolatileStorageEEPROM::readFromStorage(uint16_t i) {
  return EEPROM.read(i);
}

void NonVolatileStorageEEPROM::writeToStorage(uint16_t i,  uint8_t j) {
  EEPROM.write(i, j);
}

NonVolatileStorageEEPROM nv;

#endif
