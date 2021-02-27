// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in, flash emulation)

#include "Arduino.h"
#include "EEPROM.h"
#include "NV_FLASH.h"

#ifdef ESP32

#ifndef NV_ENDURANCE
  #define NV_ENDURANCE 0 // 0 = LOW (< 100K), 1 = MID (~ 100K), 2 = HIGH (~ 1M), 3 = VERY HIGH (> 1M)
#endif

bool NonVolatileStorageFLASH::init(uint16_t size) {
  EEPROM.begin(size);

  // cache isn't needed
  NonVolatileStorage::init(0);

  // set NV size
  this->size = size;
}

void NonVolatileStorageFLASH::poll() {
  if (dirty && ((long)(millis() - lastWrite) > 5000)) {
    #ifdef ESP32
      timerAlarmsDisable();
    #endif
    EEPROM.commit();
    #ifdef ESP32
      timerAlarmsEnable();
    #endif
    dirty = false;
  }
}

bool NonVolatileStorageFLASH::committed() {
  return !dirty;
}

uint8_t NonVolatileStorageFLASH::readFromStorage(uint16_t i) {
  return EEPROM.read(i);
}

void NonVolatileStorageFLASH::writeToStorage(uint16_t i,  uint8_t j) {
  EEPROM.write(i, j);
  lastWrite = millis();
  dirty = true;
}
#endif