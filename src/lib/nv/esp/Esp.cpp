// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in, flash emulation ESP8266/ESP32)

#include "Esp.h"

#if defined(NV_DRIVER) && NV_DRIVER == NV_ESP

// from the NV library
#if defined(ESP32)
  extern void timerAlarmsDisable();
  extern void timerAlarmsEnable();
#endif

bool NonVolatileStorageESP::init() {
  if (E2END + 1 > 4096 || NV_WAIT == 0) return false;

  // setup size, cache, etc.
  if (!NonVolatileStorage::init(E2END + 1, NV_CACHED, NV_WAIT, false)) return false;

  EEPROM.begin(E2END + 1);

  return true;
}

void NonVolatileStorageESP::poll(bool disableInterrupts) {
  if (dirty && ((long)(millis() - commitReadyTimeMs) >= 0)) {
    #if defined(ESP32)
      if (disableInterrupts) timerAlarmsDisable();
    #endif
    EEPROM.commit();
    #if defined(ESP32)
      if (disableInterrupts) timerAlarmsEnable();
    #endif
    dirty = false;
  }
  #if !defined(ESP32)
    // stop compiler warnings
    (void)(disableInterrupts);
  #endif
}

bool NonVolatileStorageESP::committed() {
  return !dirty;
}

uint8_t NonVolatileStorageESP::readFromStorage(uint16_t i) {
  return EEPROM.read(i);
}

void NonVolatileStorageESP::writeToStorage(uint16_t i,  uint8_t j) {
  EEPROM.write(i, j);
  dirty = true;
}

NonVolatileStorageESP nv;

#endif
