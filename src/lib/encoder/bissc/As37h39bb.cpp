// Broadcom AS37-H39B-B BISS-C encoders

#include "As37h39bb.h"

#ifdef HAS_AS37_H39B_B

#include <stdio.h>

// initialize BiSS-C encoder
As37h39bb::As37h39bb(int16_t axis, int16_t maPin, int16_t sloPin) {
  if (axis < 1 || axis > 9) return;

  this->axis = axis;

  this->maPin = maPin;
  this->sloPin = sloPin;

  snprintf(encoderName, sizeof(encoderName), "AS37_H39B_B Ax%d", axis);
  encoderBits = 23;
  encoderMultiTurnBits = 16;
  encoderErrInvert = false;
}

// Designed according protocol description found in as38-H39e-b-an100.pdf and
// Renishaw application note E201D02_02

// BiSS-C 6-bit CRC of 41 bit data (16 multi-turn + 23 position + 2 err/wrn)
uint8_t As37h39bb::crc6(uint64_t data) {
  uint8_t crc;
  uint64_t idx;
  idx = ((data >> 36) & 0b011111);
  crc = ((data >> 30) & 0b111111);
  idx = crc ^ CRC6[idx];
  crc = ((data >> 24) & 0b111111);
  idx = crc ^ CRC6[idx];
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
