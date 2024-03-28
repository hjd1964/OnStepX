// -----------------------------------------------------------------------------------
// non-volatile storage (caching, for 85RC series I2C FRAMS)

#include "NV_MB85RC.h"

#define MSB(i) (i >> 8)
#define LSB(i) (i & 0xFF)

// no wait needed for 85RC series
#ifndef FRAM_WRITE_WAIT
#define FRAM_WRITE_WAIT 0
#endif

bool NonVolatileStorageMB85RC::init(uint16_t size, bool cacheEnable, uint16_t wait, bool checkEnable, TwoWire* wire, uint8_t address) {
  // setup size, cache, etc.
  NonVolatileStorage::init(size, cacheEnable, wait, checkEnable, wire, address);

  this->wire = wire;
  framAddress = address;
  wire->begin();

  wire->beginTransmission(framAddress);
  bool error = wire->endTransmission();
  return !error;
}

bool NonVolatileStorageMB85RC::busy() {
  return (int32_t)(millis() - nextOpMs) < 0;
  // posssibly a better way?
  // wire->beginTransmission(framAddress);
  // return wire->endTransmission() == 0;
}

uint8_t NonVolatileStorageMB85RC::readFromStorage(uint16_t i) {
  while (busy()) {}

  wire->beginTransmission(framAddress);
  wire->write(MSB(i));
  wire->write(LSB(i));
  wire->endTransmission();

  size_t result = wire->requestFrom(framAddress, (uint8_t)1);
  if (result != 1) return 0;

  return wire->read();
}

void NonVolatileStorageMB85RC::writeToStorage(uint16_t i,  uint8_t j) {
  while (busy()) {}
    
  wire->beginTransmission(framAddress);
  wire->write(MSB(i));
  wire->write(LSB(i));
  wire->write(j);
  wire->endTransmission();
  nextOpMs = millis() + FRAM_WRITE_WAIT;
}

