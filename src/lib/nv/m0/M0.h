// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in, flash emulation M0)

#pragma once

#include "../NvBase.h"

#if defined(NV_DRIVER) && NV_DRIVER == NV_M0

#include "FlashAsEEPROM.h" // https://github.com/cmaglie/FlashStorage

class NonVolatileStorageM0 : public NonVolatileStorage {
  public:
    // prepare         FLASH based EEPROM emulation for operation
    // E2END:          NV size in bytes - 1
    // NV_CACHED:      true to enable the cache (note NV size must be divisible by 8 if enabled)
    // NV_WAIT:        minimum time in milliseconds to wait (after last write) before writing cache or doing the commit
    // result:         true if the device was found, or false if not
    bool init(uint16_t size, bool cacheEnable, uint16_t wait, bool checkEnable, TwoWire* wire = NULL, uint8_t address = 0);

    // call frequently to perform any operations that need to happen in the background
    void poll(bool disableInterrupts = true);

    // returns true if all data in any cache has been written
    bool committed();

  private:
    // read byte at position i from storage
    uint8_t readFromStorage(uint16_t i);

    // write value j to position i in storage 
    void writeToStorage(uint16_t i, uint8_t j);  

    bool dirty = false;
};

extern NonVolatileStorageM0 nv;

#endif