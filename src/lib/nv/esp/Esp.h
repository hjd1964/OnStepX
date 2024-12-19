// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in, flash emulation ESP32)

#pragma once

#include "../NvBase.h"

#if defined(NV_DRIVER) && NV_DRIVER == NV_ESP

#include <EEPROM.h>

class NonVolatileStorageESP : public NonVolatileStorage {
  public:
    // prepare         FLASH based EEPROM emulation for operation
    // E2END:          NV size in bytes - 1
    // NV_CACHED:      true to enable the cache (note NV size must be divisible by 8 if enabled)
    // NV_WAIT:        minimum time in milliseconds to wait (after last write) before writing cache or doing the commit
    // result:         true if the device was found, or false if not
    bool init();

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

extern NonVolatileStorageESP nv;

#endif