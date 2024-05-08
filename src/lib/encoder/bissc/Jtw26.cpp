// JTW 26 BIT BISS-C encoders

#include "Jtw26.h"

#ifdef HAS_JTW_26BIT

// initialize BiSS-C encoder
// nvAddress holds settings for the 9 supported axes, 9*4 = 72 bytes; set nvAddress 0 to disable
Jtw26::Jtw26(int16_t maPin, int16_t sloPin, int16_t axis) {
  if (axis < 1 || axis > 9) return;

  this->maPin = maPin;
  this->sloPin = sloPin;
  this->axis = axis;
}

// read encoder count
IRAM_ATTR bool Jtw26::readEnc(uint32_t &position) {

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
  #elif defined(__TEENSYDUINO__)
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

        // the first 26 bits are the encoder absolute count
        for (int i = 0; i < 26; i++) {
          digitalWriteF(maPin, LOW_MA);
          if (digitalReadF(sloPin) == HIGH_SLO) bitSet(position, 25 - i);
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
  #elif defined(__TEENSYDUINO__)
    interrupts();
  #endif

  // trap errors
  int16_t errors = 0;
  UNUSED(encWrn);

  uint64_t encData = (uint64_t)position;
  encData = (encData << 1) | encErr;
  encData = (encData << 1) | encWrn;

  if (!foundAck)   { VF("WRN: Encoder JTW_26BIT"); V(axis); VLF(", Ack bit invalid"); errors++; } else
  if (!foundStart) { VF("WRN: Encoder JTW_26BIT"); V(axis); VLF(", Start bit invalid"); errors++; } else
  if (!foundCds)   { VF("WRN: Encoder JTW_26BIT"); V(axis); VLF(", Cds bit invalid"); errors++; } else
  if (!encErr)     { VF("WRN: Encoder JTW_26BIT"); V(axis); VLF(", Error bit set"); errors++; } else
  if (!encWrn)     { VF("WRN: Encoder JTW_26BIT"); V(axis); VLF(", Warn bit set"); } else errors = 0;
  if (errors > 0) return false;

  if (crc6(encData) != encCrc) {
    bad++;
    VF("WRN: Encoder JTW_26BIT"); V(axis); VF(", Crc failed ("); V(((float)bad/good)*100.0F); VLF("%)"); 
    return false;
  } else good++;

  // extend negative to 32 bits
  if (bitRead(position, 26)) { position |= 0b11111100000000000000000000000000; }

  #ifdef JTW_26BIT_AS_24BIT
    position += origin*4L;
  #else
    position += origin;
  #endif

  if ((int32_t)position > 67108864) position -= 67108864;
  if ((int32_t)position < 0) position += 67108864;

  position -= 33554432;

  #ifdef JTW_26BIT_AS_24BIT
    position = (int32_t)position/4L;
  #endif

  return true;
}

// Designed according protocol description found in as38-H39e-b-an100.pdf and
// Renishaw application note E201D02_02

// BiSS-C 6-bit CRC of 28 bit data (26 position + 2 err/wrn)
uint8_t Jtw26::crc6(uint64_t data) {
  uint8_t crc;
  uint64_t idx;
  idx = ((data >> 24) & 0b001111);
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
