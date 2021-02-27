// -----------------------------------------------------------------------------------
// non-volatile storage (caching, for 24XX series I2C EEPROMS)

#pragma once

#include "Arduino.h"
#include "Wire.h"
#include "NV.h"

class NonVolatileStorage24XX : public NonVolatileStorage {
  public:
    // prepare EEPROM of size on the I2C wire interface at eepromAddress for operation
    // size: size of EEPROM in bytes (cache will be of equal size)
    // wire: pointer to the TwoWire interface to use
    // eepromAddress: the EEPROM I2C address
    // result: true if the device was found, or false if not
    bool init(uint16_t size, TwoWire* wire, uint8_t eepromAddress);

  private:
    // returns false if ready to read or write immediately
    bool busy();
    
    // read byte at position i from storage
    uint8_t readFromStorage(uint16_t i);

    // write value j to position i in storage 
    void writeToStorage(uint16_t i, uint8_t j);

    TwoWire* wire;
    uint8_t eepromAddress = 0;
    uint32_t nextOpMs = 0;
};
