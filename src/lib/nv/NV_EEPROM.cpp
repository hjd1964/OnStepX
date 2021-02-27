// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in)

#include "Arduino.h"
#include "EEPROM.h"
#include "NV_EEPROM.h"

#ifndef NV_ENDURANCE
  #define NV_ENDURANCE 1 // 0 = LOW (< 100K), 1 = MID (~ 100K), 2 = HIGH (~ 1M), 3 = VERY HIGH (> 1M)
#endif

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
