// -----------------------------------------------------------------------------------
// non-volatile storage (for 24XX series I2C EEPROMS)

#include "NV_24XX.h"

// universal value works for all known 24XX series, 10ms
#define EEPROM_WRITE_WAIT 10

#define MSB(i) (i >> 8)
#define LSB(i) (i & 0xFF)

bool NonVolatileStorage24XX::init(uint16_t size, bool cacheEnable, uint16_t wait, bool checkEnable, TwoWire* wire, uint8_t address) {
  // setup size, cache, etc.
  NonVolatileStorage::init(size, cacheEnable, wait, checkEnable, wire, address);

  // device page size must be >= 8 and a multipule of 8
  if (cacheEnable) pageWriteSize = 8;

  this->wire = wire;
  eepromAddress = address;
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
