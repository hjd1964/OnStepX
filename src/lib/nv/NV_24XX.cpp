// -----------------------------------------------------------------------------------
// non-volatile storage (caching, for 24XX series I2C EEPROMS)

#include "Arduino.h"
#include "Wire.h"
#include "NV_24XX.h"

#ifndef NV_ENDURANCE
  #define NV_ENDURANCE 1 // 0 = LOW (< 100K), 1 = MID (~ 100K), 2 = HIGH (~ 1M), 3 = VERY HIGH (> 1M)
#endif

// universal value works for all known 24XX series, 10ms
#define EEPROM_WRITE_WAIT 10

#define MSB(i) (i >> 8)
#define LSB(i) (i & 0xFF)

bool NonVolatileStorage24XX::init(uint16_t size, TwoWire* wire, uint8_t eepromAddress) {
  // setup cache
  NonVolatileStorage::init(size);

  this->size = size;
  this->wire = wire;
  this->eepromAddress = eepromAddress;
  wire->begin();

  wire->beginTransmission(eepromAddress);
  bool error = wire->endTransmission();
  return !error;
}

bool NonVolatileStorage24XX::busy() {
  return (int32_t)(millis() - nextOpMs) < 0;
}

uint8_t NonVolatileStorage24XX::readFromStorage(uint16_t i) {
  while (busy()) {}

  wire->beginTransmission(eepromAddress);
  wire->write(MSB(i));
  wire->write(LSB(i));
  wire->endTransmission();

  wire->requestFrom(eepromAddress, (uint8_t)1);

  uint8_t result = 0;
  if (wire->available()) result = wire->read();
  return result;
}

void NonVolatileStorage24XX::writeToStorage(uint16_t i,  uint8_t j) {
  while (busy()) {}
    
  wire->beginTransmission(eepromAddress);
  wire->write(MSB(i));
  wire->write(LSB(i));
  wire->write(j);
  wire->endTransmission();
  nextOpMs = millis() + EEPROM_WRITE_WAIT;
}
