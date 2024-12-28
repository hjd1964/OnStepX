// -----------------------------------------------------------------------------------
// non-volatile storage (for 24XX series I2C EEPROMS)

#include "24XX.h"

#if defined(NV_DRIVER) && (NV_DRIVER == NV_2416 || NV_DRIVER == NV_2432 || NV_DRIVER == NV_AT24C32 || NV_DRIVER == NV_2464 || NV_DRIVER == NV_24128 || NV_DRIVER == NV_24256)

// universal value works for all known 24XX series, 10ms
#define EEPROM_WRITE_WAIT 10

#define MSB(i) (i >> 8)
#define LSB(i) (i & 0xFF)

bool NonVolatileStorage24XX::init() {
  // setup size, cache, etc.
  if (!NonVolatileStorage::init(E2END + 1, NV_CACHED, NV_WAIT, false)) return false;

  // device page size must be >= 8 and a multipule of 8
  if (NV_CACHED) pageWriteSize = 8;

  this->wire = &HAL_WIRE;
  eepromAddress = NV_I2C_ADDRESS;
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

// write value j of count bytes to position starting at i in storage
// these writes must be aligned with the page size!
void NonVolatileStorage24XX::writePageToStorage(uint16_t i, uint8_t *j, uint8_t count) {
  while (busy()) {}

  wire->beginTransmission(eepromAddress);
  wire->write(MSB(i));
  wire->write(LSB(i));
  for (int k = 0; k < count; k++) { wire->write(*j); j++; }
  wire->endTransmission();
  nextOpMs = millis() + EEPROM_WRITE_WAIT;
}

NonVolatileStorage24XX nv;

#endif
