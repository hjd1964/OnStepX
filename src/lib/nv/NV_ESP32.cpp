// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in, flash emulation ESP32)

#ifdef ESP32

#include "Arduino.h"
#include "EEPROM.h"
#include "../../tasks/OnTask.h"
extern Tasks tasks;

#include "NV_ESP32.h"

bool NonVolatileStorageEEPROM::init(uint16_t size, bool cache, uint16_t wait, bool check, TwoWire* wire, uint8_t address) {
  // setup size, cache, etc.
  NonVolatileStorage::init(size, cache, wait, check);

  EEPROM.begin(size);
  return true;
}

void NonVolatileStorageESP32::poll() {
  if (dirty && ((long)(millis() - commitReadyTimeMs) >= 0)) {
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
  dirty = true;
}
#endif