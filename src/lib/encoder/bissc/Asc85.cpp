// Lika ASC85 25 BIT BISS-C encoder

#include "Asc85.h"

#ifdef HAS_LIKA_ASC85

// initialize BiSS-C encoder
LikaAsc85::LikaAsc85(int16_t maPin, int16_t sloPin, int16_t axis) {
  if (axis < 1 || axis > 9) return;

  this->maPin = maPin;
  this->sloPin = sloPin;
  this->axis = axis;
}

// read encoder count
IRAM_ATTR bool LikaAsc85::readEnc(uint32_t &position) {

  bool foundAck = false;
  bool foundStart = false;
  bool foundCds = false;

  uint8_t  encErr = 0;
  uint8_t  encWrn = 0;
  uint8_t  encCrc = 0;

  // prepare for a reading
  position = 0;

  // bit delay in nanoseconds
  int rate = lround(500000.0/BISSC_CLOCK_RATE_KHZ);

  #ifdef ESP32
    portMUX_TYPE bisscMutex = portMUX_INITIALIZER_UNLOCKED;
    taskENTER_CRITICAL(&bisscMutex);
  #else
    noInterrupts();
  #endif

  // sync phase
  for (int i = 0; i < 20; i++) {
    digitalWriteF(maPin, LOW_MA);
    if (digitalReadF(sloPin) == LOW_SLO) foundAck = true;
    delayNanoseconds(rate);
    digitalWriteF(maPin, HIGH_MA);
    delayNanoseconds(rate);
    if (foundAck) break;
  }

  // if we have an Ack
  if (foundAck) {
    for (int i = 0; i < 10; i++) {
      digitalWriteF(maPin, LOW_MA);
      if (digitalReadF(sloPin) == HIGH_SLO) foundStart = true;
      delayNanoseconds(rate);
      digitalWriteF(maPin, HIGH_MA);
      delayNanoseconds(rate);
      if (foundStart) break;
    }

    // if we have an Start
    if (foundStart) {
      digitalWriteF(maPin, LOW_MA);
      if (digitalReadF(sloPin) == LOW_SLO) foundCds = true;
      delayNanoseconds(rate);
      digitalWriteF(maPin, HIGH_MA);
      delayNanoseconds(rate);

      // if we have an Cds, read the data
      if (foundCds) {

        // the first 25 bits are the encoder absolute count
        for (int i = 0; i < 25; i++) {
          digitalWriteF(maPin, LOW_MA);
          if (digitalReadF(sloPin) == HIGH_SLO) bitSet(position, 24 - i);
          delayNanoseconds(rate);
          digitalWriteF(maPin, HIGH_MA);
          delayNanoseconds(rate);
        }

        // the Err bit
        digitalWriteF(maPin, LOW_MA);
        if (digitalReadF(sloPin) == HIGH_SLO) encErr = 1;
        delayNanoseconds(rate);
        digitalWriteF(maPin, HIGH_MA);
        delayNanoseconds(rate);

        // the Wrn bit
        digitalWriteF(maPin, LOW_MA);
        if (digitalReadF(sloPin) == HIGH_SLO) encWrn = 1;
        delayNanoseconds(rate);
        digitalWriteF(maPin, HIGH_MA);
        delayNanoseconds(rate);

        // the last 6 bits are the CRC
        for (int i = 0; i < 6; i++) {
          digitalWriteF(maPin, LOW_MA);
          if (digitalReadF(sloPin) == HIGH_SLO) bitSet(encCrc, 5 - i);
          delayNanoseconds(rate);
          digitalWriteF(maPin, HIGH_MA);
          delayNanoseconds(rate);
        }
      }
    }
  }

  // send a CDM (invert)
  digitalWriteF(maPin, LOW_MA);
  delayNanoseconds(rate*4);
  digitalWriteF(maPin, HIGH_MA);

  #ifdef ESP32
    taskEXIT_CRITICAL(&bisscMutex);
  #else
    interrupts();
  #endif

  // trap errors
  int16_t errors = 0;
  UNUSED(encWrn);

  uint64_t encData = (uint64_t)position;
  encData = (encData << 1) | encErr;
  encData = (encData << 1) | encWrn;

  if (!foundAck)   { DF("WRN: Encoder LIKA_ASC85"); D(axis); DLF(", Ack bit invalid"); errors++; } else
  if (!foundStart) { DF("WRN: Encoder LIKA_ASC85"); D(axis); DLF(", Start bit invalid"); errors++; } else
  if (!foundCds)   { DF("WRN: Encoder LIKA_ASC85"); D(axis); DLF(", Cds bit invalid"); errors++; } else
  if (!encErr)     { DF("WRN: Encoder LIKA_ASC85"); D(axis); DLF(", Error bit set"); errors++; } else
  if (!encWrn)     { DF("WRN: Encoder LIKA_ASC85"); D(axis); DLF(", Warn bit set"); warn++; } else errors = 0;
  if (errors > 0) { error++; return false; }

  if (crc6(encData) != encCrc) {
    bad++;
    DF("WRN: Encoder LIKA_ASC85"); D(axis); DF(", Crc failed ("); D(((float)bad/good)*100.0F); DLF("%)"); 
    return false;
  } else good++;

  // extend negative to 32 bits
  if (bitRead(position, 25)) { position |= 0b11111110000000000000000000000000; }

  position += origin;

  if ((int32_t)position >= 33554432) position -= 33554432;
  if ((int32_t)position < 0) position += 33554432;

  position -= 16777216;

  return true;
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
