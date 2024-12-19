// -----------------------------------------------------------------------------------
// non-volatile storage (for 24XX series I2C EEPROMS)

#pragma once

#include "../NvBase.h"

#if defined(NV_DRIVER) && (NV_DRIVER == NV_2416 || NV_DRIVER == NV_2432 || NV_DRIVER == NV_AT24C32 || NV_DRIVER == NV_2464 || NV_DRIVER == NV_24128 || NV_DRIVER == NV_24256)

class NonVolatileStorage24XX : public NonVolatileStorage {
  public:
    // prepare         FLASH based EEPROM emulation for operation
    // E2END:          NV size in bytes - 1
    // NV_CACHED:      true to enable the cache (note NV size must be divisible by 8 if enabled)
    // NV_WAIT:        minimum time in milliseconds to wait (after last write) before writing cache or doing the commit
    // HAL_WIRE:       I2C interface pointer
    // NV_I2C_ADDRESS: I2C address
    // result:         true if the device was found, or false if not
    bool init();

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

extern NonVolatileStorage24XX nv;

#endif
