// -----------------------------------------------------------------------------------
// non-volatile storage (caching, for 85RC series I2C FRAMS)

#include "MB85RC.h"

#if defined(NV_DRIVER) && (NV_DRIVER == NV_MB85RC32 || NV_DRIVER == NV_MB85RC64 || NV_DRIVER == NV_MB85RC256)

#define MSB(i) (i >> 8)
#define LSB(i) (i & 0xFF)

// no wait needed for 85RC series
#ifndef FRAM_WRITE_WAIT
  #define FRAM_WRITE_WAIT 0
#endif

bool NonVolatileStorageMB85RC::init() {
  // setup size, cache, etc.
  if (!NonVolatileStorage::init(E2END + 1, NV_CACHED, NV_WAIT, false)) return false;

  this->wire = &HAL_WIRE;
  framAddress = NV_I2C_ADDRESS;
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

NonVolatileStorageMB85RC nv;

#endif
