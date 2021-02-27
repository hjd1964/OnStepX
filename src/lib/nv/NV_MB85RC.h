// -----------------------------------------------------------------------------------
// non-volatile storage (caching, for 85RC series I2C FRAMS)

#pragma once

#include "Arduino.h"
#include "Wire.h"
#include "NV.h"

class NonVolatileStorageMB85RC : public NonVolatileStorage {
  public:
    // prepare FRAM for operation
    // size: size in bytes
    // wire: pointer to the TwoWire interface to use
    // framAddress: the I2C address
    // result: true if the device was found, or false if not
    bool init(uint16_t size, TwoWire* wire, uint8_t framAddress);

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
