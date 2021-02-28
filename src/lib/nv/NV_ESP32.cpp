// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in, flash emulation ESP32)

#ifdef ESP32

#include "Arduino.h"
#include "EEPROM.h"
#include "NV_ESP32.h"

#ifndef NV_ENDURANCE
  #define NV_ENDURANCE 0 // 0 = LOW (< 100K), 1 = MID (~ 100K), 2 = HIGH (~ 1M), 3 = VERY HIGH (> 1M)
#endif

bool NonVolatileStorageESP32::init(uint16_t size) {
  EEPROM.begin(size);

  // cache isn't needed
  NonVolatileStorage::init(0);

  // set NV size
  this->size = size;
}

void NonVolatileStorageESP32::poll() {
  if (dirty && ((long)(millis() - lastWrite) > 5000)) {
    timerAlarmsDisable();
    EEPROM.commit();
    timerAlarmsEnable();
    dirty = false;
  }
}

bool NonVolatileStorageESP32::committed() {
  return !dirty;
}

uint8_t NonVolatileStorageESP32::readFromStorage(uint16_t i) {
  return EEPROM.read(i);
}

void NonVolatileStorageESP32::writeToStorage(uint16_t i,  uint8_t j) {
  EEPROM.write(i, j);
  lastWrite = millis();
  dirty = true;
}
#endif