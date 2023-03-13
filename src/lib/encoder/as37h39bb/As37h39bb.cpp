// Broadcom AS37-H39B-B BISS-C encoders

#include "As37h39bb.h"

#if AXIS1_ENCODER == AS37_H39B_B || AXIS2_ENCODER == AS37_H39B_B || AXIS3_ENCODER == AS37_H39B_B || \
    AXIS4_ENCODER == AS37_H39B_B || AXIS5_ENCODER == AS37_H39B_B || AXIS6_ENCODER == AS37_H39B_B || \
    AXIS7_ENCODER == AS37_H39B_B || AXIS8_ENCODER == AS37_H39B_B || AXIS9_ENCODER == AS37_H39B_B

// Designed according protocol description found in as38-H39e-b-an100.pdf and
// Renishaw application note E201D02_02

// BiSS-C 6-bit CRC table (x^6 + x^1 + 1)
uint8_t _CRC6[64] = {
  0x00, 0x03, 0x06, 0x05, 0x0C, 0x0F, 0x0A, 0x09,
  0x18, 0x1B, 0x1E, 0x1D, 0x14, 0x17, 0x12, 0x11,
  0x30, 0x33, 0x36, 0x35, 0x3C, 0x3F, 0x3A, 0x39,
  0x28, 0x2B, 0x2E, 0x2D, 0x24, 0x27, 0x22, 0x21,
  0x23, 0x20, 0x25, 0x26, 0x2F, 0x2C, 0x29, 0x2A,
  0x3B, 0x38, 0x3D, 0x3E, 0x37, 0x34, 0x31, 0x32,
  0x13, 0x10, 0x15, 0x16, 0x1F, 0x1C, 0x19, 0x1A,
  0x0B, 0x08, 0x0D, 0x0E, 0x07, 0x04, 0x01, 0x02};

// BiSS-C 6-bit CRC of 41 bit data (16 multi-turn + 23 position + 2 err/wrn)
uint8_t _crcBiSS(uint64_t data) {
  uint8_t crc;
  uint64_t idx;
  idx = ((data >> 36) & 0b011111);
  crc = ((data >> 30) & 0b111111);
  idx = crc ^ _CRC6[idx];
  crc = ((data >> 24) & 0b111111);
  idx = crc ^ _CRC6[idx];
  crc = ((data >> 18) & 0b111111);
  idx = crc ^ _CRC6[idx];
  crc = ((data >> 12) & 0b111111);
  idx = crc ^ _CRC6[idx];
  crc = ((data >> 6) & 0b111111);
  idx = crc ^ _CRC6[idx];
  crc = (data & 0b111111);
  idx = crc ^ _CRC6[idx];
  crc = _CRC6[idx];
  return (~crc & 0b111111);
}

// initialize AS37H39BB encoder
// nvAddress holds settings for the 9 supported axes, 9*4 = 72 bytes; set nvAddress 0 to disable
As37h39bb::As37h39bb(int16_t maPin, int16_t sloPin, int16_t axis) {
  if (axis < 1 || axis > 9) return;

  clkPin = maPin;
  this->sloPin = sloPin;
  this->axis = axis;
}

// get device ready for use
void As37h39bb::init() {
  if (initialized) { VF("WRN: Encoder As37h39bb"); V(axis); VLF(" init(), already initialized!"); return; }

  pinMode(clkPin, OUTPUT);
  digitalWriteF(clkPin, LOW);
  pinMode(sloPin, INPUT_PULLUP);

  initialized = true;
}

// set encoder origin
void As37h39bb::setOrigin(uint32_t count) {
  if (!initialized) { VF("WRN: Encoder As37h39bb"); V(axis); VLF(" setOrigin(), not initialized!"); return; }

  long temp = offset;
  offset = 0;
  origin = 0;

  VLF("----------------------------------------------------------------------------------------");
  VF("MSG: Encoder As37h39bb"); V(axis); VLF(", >>> with the mount in the home position <<<");
  VF("MSG: Encoder As37h39bb"); V(axis); VF(", if used AXIS"); V(axis); VF("_ENCODER_OFFSET in counts should be set to "); VL(uint32_t(-read()));
  origin = count;
  VF("MSG: Encoder As37h39bb"); V(axis); VF(", counts at home should be 0 and currently are "); VL(read());
  VLF("----------------------------------------------------------------------------------------");

  offset = temp;
}

// read encoder count
int32_t As37h39bb::read() {
  if (!initialized) { VF("WRN: Encoder As37h39bb"); V(axis); VLF(" read(), not initialized!"); return 0; }

  uint32_t temp;
  if (readEncLatest(temp)) {
    return (int32_t)temp + offset;
  } else return INT32_MAX;
}

// write encoder count
void As37h39bb::write(int32_t count) {
  if (!initialized) { VF("WRN: Encoder As37h39bb"); V(axis); VLF(" write(), not initialized!"); return; }

  if (count != INT32_MAX) {
    uint32_t temp;
    if (readEncLatest(temp)) {
      offset = count - (int32_t)temp;
    }
  }
}

// read encoder count with (1 second) error recovery
bool As37h39bb::readEncLatest(uint32_t &position) {
  uint32_t temp = position;
  bool success = readEnc(temp);
  if (success) {
    lastValidTime = millis();
    lastValidPosition = temp;
    position = temp;
    return true;
  } else {
    if ((long)(millis() - lastValidTime) > 1000) return false;
    position = lastValidPosition;
    return true;
  }
}

// read encoder count
IRAM_ATTR bool As37h39bb::readEnc(uint32_t &position) {
  bool foundAck = false;
  bool foundStart = false;
  bool foundCds = false;

  uint8_t  encErr = 0;
  uint8_t  encWrn = 0;
  uint8_t  as37Crc = 0;

  uint32_t encTurns = 0;

  // prepare for a reading
  position = 0;
  encErr = 0;
  encWrn = 0;
  as37Crc = 0;

  // bit delay in nanoseconds
  int rate = lround(500000.0/AS37_CLOCK_RATE_KHZ);

  #ifdef ESP32
    portMUX_TYPE as37Mutex = portMUX_INITIALIZER_UNLOCKED;
    taskENTER_CRITICAL(&as37Mutex);
  #elif defined(__TEENSYDUINO__)
    noInterrupts();
  #endif

  // sync phase
  for (int i = 0; i < 20; i++) {
    digitalWriteF(clkPin, LOW);
    if (digitalReadF(sloPin) == LOW) foundAck = true;
    delayNanoseconds(rate);
    digitalWriteF(clkPin, HIGH);
    delayNanoseconds(rate);
    if (foundAck) break;
  }

  // if we have an Ack
  if (foundAck) {
    for (int i = 0; i < 20; i++) {
      digitalWriteF(clkPin, LOW);
      if (digitalReadF(sloPin) == HIGH) foundStart = true;
      delayNanoseconds(rate);
      digitalWriteF(clkPin, HIGH);
      delayNanoseconds(rate);
      if (foundStart) break;
    }

    // if we have an Start
    if (foundStart) {
      digitalWriteF(clkPin, LOW);
      if (digitalReadF(sloPin) == LOW) foundCds = true;
      delayNanoseconds(rate);
      digitalWriteF(clkPin, HIGH);
      delayNanoseconds(rate);

      // if we have an Cds, read the data
      if (foundCds) {

        // the first 16 bits are the multi-turn count
        for (int i = 0; i < 16; i++) {
          digitalWriteF(clkPin, LOW);
          if (digitalReadF(sloPin) == HIGH) bitSet(encTurns, 15 - i);
          delayNanoseconds(rate);
          digitalWriteF(clkPin, HIGH);
          delayNanoseconds(rate);
        }
        
        // the next 23 bits are the encoder absolute count
        for (int i = 0; i < 23; i++) {
          digitalWriteF(clkPin, LOW);
          if (digitalReadF(sloPin) == HIGH) bitSet(position, 22 - i);
          delayNanoseconds(rate);
          digitalWriteF(clkPin, HIGH);
          delayNanoseconds(rate);
        }

        // the Err bit
        digitalWriteF(clkPin, LOW);
        if (digitalReadF(sloPin) == HIGH) encErr = 1;
        delayNanoseconds(rate);
        digitalWriteF(clkPin, HIGH);
        delayNanoseconds(rate);

        // the Wrn bit
        digitalWriteF(clkPin, LOW);
        if (digitalReadF(sloPin) == HIGH) encWrn = 1;
        delayNanoseconds(rate);
        digitalWriteF(clkPin, HIGH);
        delayNanoseconds(rate);

        // the last 6 bits are the CRC
        for (int i = 0; i < 6; i++) {
          digitalWriteF(clkPin, LOW);
          if (digitalReadF(sloPin) == HIGH) bitSet(as37Crc, 5 - i);
          delayNanoseconds(rate);
          digitalWriteF(clkPin, HIGH);
          delayNanoseconds(rate);
        }
      }
    }
  }

  // send a CDM (invert)
  digitalWriteF(clkPin, LOW);
  delayNanoseconds(rate*4);
  digitalWriteF(clkPin, HIGH);

  #ifdef ESP32
    taskEXIT_CRITICAL(&as37Mutex);
  #elif defined(__TEENSYDUINO__)
    interrupts();
  #endif

  // trap errors
  int16_t errors = 0;
  UNUSED(encWrn);

  uint64_t encData = (uint64_t)position | ((uint64_t)encTurns << 23);
  encData = (encData << 1) | encErr;
  encData = (encData << 1) | encWrn;

  if (_crcBiSS(encData) != as37Crc) {
    bad++;
    VF("WRN: Encoder As37h39bb"); V(axis); VF(", Crc invalid (overall "); V(((float)bad/good)*100.0F); V('%'); VLF(")"); errors++;
  } else {
    good++;
    if (!foundAck) { VF("WRN: Encoder As37h39bb"); V(axis); VLF(", Ack bit invalid"); errors++; } else
    if (!foundStart) { VF("WRN: Encoder As37h39bb"); V(axis); VLF(", Start bit invalid"); errors++; } else
    if (!foundCds) { VF("WRN: Encoder As37h39bb"); V(axis); VLF(", Cds bit invalid"); errors++; } else
    if (encErr) { VF("WRN: Encoder As37h39bb"); V(axis); VLF(", Error bit set"); errors++; } else errors = 0;
  }

  if (errors > 0) {
    if (errors <= 2) warn = true; else error = true;
    return false;
  }

  #ifndef AS37_SINGLE_TURN
    // combine absolute and 9 low order bits of multi-turn count for a 32 bit count
    position = position | ((encTurns & 0b0111111111) << 23);
  #else
    // extend negative to 32 bits
    if (bitRead(position, 23)) { position |= 0b11111111100000000000000000000000; }
  #endif

  position += origin;

  #ifdef AS37_SINGLE_TURN
    if ((int32_t)position > 8388608) position -= 8388608;
    if ((int32_t)position < 0) position += 8388608;
  #endif

  position -= 4194304;

  return true;
}

#endif
