// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in)

#include "Arduino.h"
#include "EEPROM.h"
#include "NV_EEPROM.h"

#ifdef E2END

bool NonVolatileStorageEEPROM::init(uint16_t size, bool cache, uint16_t wait, bool check, TwoWire* wire, uint8_t address) {
  // setup size, cache, etc.
  NonVolatileStorage::init(size, 0, wait, check);
  return true;
}

uint8_t NonVolatileStorageEEPROM::readFromStorage(uint16_t i) {
  return EEPROM.read(i);
}

void NonVolatileStorageEEPROM::writeToStorage(uint16_t i,  uint8_t j) {
  EEPROM.write(i, j);
}
#endif
