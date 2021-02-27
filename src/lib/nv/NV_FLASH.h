// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in, flash emulation)

#pragma once

#include "Arduino.h"
#include "NV.h"

#ifdef ESP32
class NonVolatileStorageFLASH : public NonVolatileStorage {
  public:
    // prepare FLASH based EEPROM emulation for operation
    // cacheSize: cache size in bytes (should be 0 or match the EEPROM E2END size)
    // result: always true
    bool init(uint16_t cacheSize);

    // call frequently to perform any operations that need to happen in the background
    void poll();

    // returns true if all data in any cache has been written
    bool committed();

  private:
    // read byte at position i from storage
    uint8_t readFromStorage(uint16_t i);

    // write value j to position i in storage 
    void writeToStorage(uint16_t i, uint8_t j);  

    bool dirty = false;
    uint32_t lastWrite = 0;
};
#endif