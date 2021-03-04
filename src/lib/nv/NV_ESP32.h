// -----------------------------------------------------------------------------------
// non-volatile storage (default/built-in, flash emulation ESP32)

#pragma once

#ifdef ESP32

#include "Arduino.h"
#include "NV.h"

class NonVolatileStorageESP32 : public NonVolatileStorage {
  public:
    // prepare FLASH based EEPROM emulation for operation
    // size:    NV size in bytes
    // cache:   enable or disable the cache (note NV size must be divisible by 8 if enabled)
    // wait:    minimum time in milliseconds to wait (after last write) before writing cache or doing the commit
    // check:   checksum error detection
    // wire:    I2C interface pointer (set to NULL if not used)
    // address: I2C address
    // result:  true if the device was found, or false if not
    bool init(uint16_t size, bool cache, uint16_t wait, bool check, TwoWire* wire = NULL, uint8_t address = 0);

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
};

#define NVS NonVolatileStorageESP32

#endif