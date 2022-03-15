// -----------------------------------------------------------------------------------
// non-volatile storage (for 24XX series I2C EEPROMS)

#pragma once

#include <Arduino.h>
#include "NV.h"

class NonVolatileStorage24XX : public NonVolatileStorage {
  public:
    // prepare      FLASH based EEPROM emulation for operation
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

    // write value j of count bytes to position starting at i in storage
    // these writes must be aligned with the page size!
    void writePageToStorage(uint16_t i, uint8_t *j, uint8_t count);
 
    TwoWire* wire;
    uint8_t eepromAddress = 0;
    uint32_t nextOpMs = 0;
};

#define NVS NonVolatileStorage24XX
