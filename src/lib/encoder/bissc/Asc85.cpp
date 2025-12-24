// Lika ASC85 25 BIT BISS-C encoder

#include "Asc85.h"

#ifdef HAS_LIKA_ASC85

#include <stdio.h>

// initialize BiSS-C encoder
LikaAsc85::LikaAsc85(int16_t axis, int16_t maPin, int16_t sloPin) {
  if (axis < 1 || axis > 9) return;

  this->axis = axis;

  this->maPin = maPin;
  this->sloPin = sloPin;

  snprintf(encoderName, sizeof(encoderName), "LIKA_ASC85 Ax%d", axis);
  encoderBits = 25;
  encoderMultiTurnBits = 0;
}

// Designed according protocol description found in as38-H39e-b-an100.pdf and
// Renishaw application note E201D02_02

// BiSS-C 6-bit CRC of 27 bit data (25 position + 2 err/wrn)
uint8_t LikaAsc85::crc6(uint64_t data) {
  uint8_t crc;
  uint64_t idx;
  idx = ((data >> 24) & 0b000111);
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
