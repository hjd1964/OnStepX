// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in)

#pragma once

#include "Arduino.h"
#include "EEPROM.h"
#include "NV.h"

#ifdef E2END

class NonVolatileStorageEEPROM : public NonVolatileStorage {
  public:
    // prepare EEPROM for operation
    // cacheSize: cache size in bytes (should be 0 to disable or match the EEPROM size)
    // result: always true
    bool init(uint16_t cacheSize);

  private:
    // read byte at position i from storage
    uint8_t readFromStorage(uint16_t i);

    // write value j to position i in storage 
    void writeToStorage(uint16_t i, uint8_t j);  
};

#endif
