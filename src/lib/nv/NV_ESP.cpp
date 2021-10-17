// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in, flash emulation ESP8266/ESP32)

#include "NV_ESP.h"

#if defined(ESP8266) || defined(ESP32)

  // from the NV library
  #if defined(ESP32)
    extern void timerAlarmsDisable();
    extern void timerAlarmsEnable();
  #endif

  bool NonVolatileStorageESP::init(uint16_t size, bool cacheEnable, uint16_t wait, bool checkEnable, TwoWire* wire, uint8_t address) {
    if (size > 4096 || wait == false) return false;

    // setup size, cache, etc.
    NonVolatileStorage::init(size, cacheEnable, wait, checkEnable, wire, address);

    EEPROM.begin(size);
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

#endif