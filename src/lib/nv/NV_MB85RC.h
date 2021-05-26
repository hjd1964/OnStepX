// -----------------------------------------------------------------------------------
// non-volatile storage (caching, for 85RC series I2C FRAMS)

#pragma once

#include <Arduino.h>
#include "NV.h"

class NonVolatileStorageMB85RC : public NonVolatileStorage {
  public:
    // prepare      prepare FRAM for operation
    // size:        NV size in bytes
    // cacheEnable: enable or disable the cache (note NV size must be divisible by 8 if enabled)
    // wait:        minimum time in milliseconds to wait (after last write) before writing cache or doing the commit
    // checkEnable: enable or disable checksum error detection
    // wire:        I2C interface pointer (set to NULL if not used)
    // address:     I2C address
    // result:      true if the device was found, or false if not
    bool init(uint16_t size, bool cacheEnable, uint16_t wait, bool checkEnable, TwoWire* wire = NULL, uint8_t address = 0);

  private:
    // returns false if ready to read or write immediately
    bool busy();
    
    // read byte at position i from storage
    uint8_t readFromStorage(uint16_t i);

    // write value j to position i in storage 
    void writeToStorage(uint16_t i, uint8_t j);

    TwoWire* wire;
    uint8_t framAddress = 0;
    uint32_t nextOpMs = 0;
};

#define NVS NonVolatileStorageMB85RC
