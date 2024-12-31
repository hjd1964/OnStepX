// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in)

#pragma once

#include "../../../Common.h"

#if defined(NV_DRIVER) && NV_DRIVER == NV_EEPROM

// don't bring in the EEPROM library here if this is an Mega2560, etc.
#ifndef __AVR__
  #include <EEPROM.h>
#endif

#include "../NvBase.h"

class NonVolatileStorageEEPROM : public NonVolatileStorage {
  public:
    // prepare         FLASH based EEPROM emulation for operation
    // E2END:          NV size in bytes - 1
    // NV_CACHED:      true to enable the cache (note NV size must be divisible by 8 if enabled)
    // NV_WAIT:        minimum time in milliseconds to wait (after last write) before writing cache or doing the commit
    // result:         true if the device was found, or false if not
    bool init();

  private:
    // read byte at position i from storage
    uint8_t readFromStorage(uint16_t i);

    // write value j to position i in storage 
    void writeToStorage(uint16_t i, uint8_t j);  
};

extern NonVolatileStorageEEPROM nv;

#endif
