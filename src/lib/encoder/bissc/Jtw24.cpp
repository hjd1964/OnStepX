// JTW 24 BIT BISS-C encoders

#include "Jtw24.h"

#ifdef HAS_JTW_24BIT

#define ENC_BISSC_NAME "JTW24"

#include <stdio.h>

// initialize BiSS-C encoder
Jtw24::Jtw24(int16_t axis, int16_t maPin, int16_t sloPin) {
  if (axis < 1 || axis > 9) return;

  this->axis = axis;

  this->maPin = maPin;
  this->sloPin = sloPin;

  snprintf(encoderName, sizeof(encoderName), "JTW_24BIT Ax%d", axis);
  encoderBits = 24;
  encoderMultiTurnBits = 0;
}

// Designed according protocol description found in as38-H39e-b-an100.pdf and
// Renishaw application note E201D02_02

// BiSS-C 6-bit CRC of 26 bit data (24 position + 2 err/wrn)
uint8_t Jtw24::crc6(uint64_t data) {
  uint8_t crc;
  uint64_t idx;
  idx = ((data >> 24) & 0b000011);
  crc = ((data >> 18) & 0b111111);
  idx = crc ^ CRC6[idx];
  crc = ((data >> 12) & 0b111111);
  idx = crc ^ CRC6[idx];
  crc = ((data >> 6) & 0b111111);
  idx = crc ^ CRC6[idx];
  crc = (data & 0b111111);
  idx = crc ^ CRC6[idx];
  crc = CRC6[idx];
  return (~crc & 0b111111);
}

#endif
