// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in)

#include "Arduino.h"
#include "EEPROM.h"
#include "NV_EEPROM.h"

bool NonVolatileStorageEEPROM::init(uint16_t cacheSize) {
  // setup cache size
  NonVolatileStorage::init(cacheSize);
  // set NV size
  this->size = E2END + 1;
  return true;
}

uint8_t NonVolatileStorageEEPROM::readFromStorage(uint16_t i) {
  return EEPROM.read(i);
}

void NonVolatileStorageEEPROM::writeToStorage(uint16_t i,  uint8_t j) {
  EEPROM.write(i, j);
}
