// Broadcom AS37-H39B-B BISS-C encoders

#include "As37h39bb.h"

#if AXIS1_ENCODER == AS37_H39B_B || AXIS2_ENCODER == AS37_H39B_B || AXIS3_ENCODER == AS37_H39B_B || \
    AXIS4_ENCODER == AS37_H39B_B || AXIS5_ENCODER == AS37_H39B_B || AXIS6_ENCODER == AS37_H39B_B || \
    AXIS7_ENCODER == AS37_H39B_B || AXIS8_ENCODER == AS37_H39B_B || AXIS9_ENCODER == AS37_H39B_B

// designed according protocol description found in as38-H39e-b-an100.pdf

As37h39bb::As37h39bb(int16_t maPin, int16_t sloPin, int16_t axis) {
  if (axis < 1 || axis > 9) return;
  initialized = true;

  clkPin = maPin;
  this->sloPin = sloPin;
  this->axis = axis;
  pinMode(clkPin, OUTPUT);
  digitalWriteF(clkPin, LOW);
  pinMode(sloPin, INPUT_PULLUP);
}

// read encoder count
int32_t As37h39bb::read() {
  if (!initialized) return 0;
  uint32_t temp;
  if (readEnc5(temp)) {
    return (int32_t)temp + offset;
  } else return INT32_MAX;
}

// write encoder count
void As37h39bb::write(int32_t count) {
  if (!initialized) return;

  if (count != INT32_MAX) {
    uint32_t temp;
    if (readEnc5(temp)) {
      offset = count - (int32_t)temp;
    }
  }
}

// read encoder count with (5 seconds) error recovery
bool As37h39bb::readEnc5(uint32_t &position) {
  uint32_t temp = position;
  bool success = readEnc(temp);
  if (success) {
    lastValidTime = millis();
    lastValidPosition = temp;
    position = temp;
    return true;
  } else {
    if ((long)(millis() - lastValidTime) > 5000) return false;
    position = lastValidPosition;
    return true;
  }
}

// read encoder count
bool As37h39bb::readEnc(uint32_t &position) {
  bool foundAck = false;
  bool foundStart = false;
  bool foundCds = false;

  uint8_t  encErr = 0;
  uint8_t  encWrn = 0;
  uint8_t  encCrc = 0;

  uint32_t encTurns = 0;

  // prepare for a reading
  position = 0;
  encErr = 0;
  encWrn = 0;
  encCrc = 0;

  // rate in microseconds, ie 2+2 = 4 = 250KHz
  int rate = lround(500.0/AS37_CLOCK_RATE_KHZ);

  // sync phase
  for (int i = 0; i < 20; i++) {
    digitalWriteF(clkPin, LOW);
    if (digitalReadF(sloPin) == LOW) foundAck = true;
    delayMicroseconds(rate);
    digitalWriteF(clkPin, HIGH);
    delayMicroseconds(rate);
    if (foundAck) break;
  }

  // if we have an Ack
  if (foundAck) {
    for (int i = 0; i < 20; i++) {
      digitalWriteF(clkPin, LOW);
      if (digitalReadF(sloPin) == HIGH) foundStart = true;
      delayMicroseconds(rate);
      digitalWriteF(clkPin, HIGH);
      delayMicroseconds(rate);
      if (foundStart) break;
    }

    // if we have an Start
    if (foundStart) {
      digitalWriteF(clkPin, LOW);
      if (digitalReadF(sloPin) == LOW) foundCds = true;
      delayMicroseconds(rate);
      digitalWriteF(clkPin, HIGH);
      delayMicroseconds(rate);
    }

    // if we have an Cds, read the data
    if (foundCds) {

      // the first 16 bits are the multi-turn count
      for (int i = 0; i < 16; i++) {
        digitalWriteF(clkPin, LOW);
        if (digitalReadF(sloPin) == HIGH) bitSet(encTurns, 15 - i);
        delayMicroseconds(rate);
        digitalWriteF(clkPin, HIGH);
        delayMicroseconds(rate);
      }
      
      // the next 23 bits are the encoder absolute count
      for (int i = 0; i < 23; i++) {
        digitalWriteF(clkPin, LOW);
        if (digitalReadF(sloPin) == HIGH) bitSet(position, 22 - i);
        delayMicroseconds(rate);
        digitalWriteF(clkPin, HIGH);
        delayMicroseconds(rate);
      }

      // the Err bit
      digitalWriteF(clkPin, LOW);
      if (digitalReadF(sloPin) == HIGH) encErr = 1;
      delayMicroseconds(rate);
      digitalWriteF(clkPin, HIGH);
      delayMicroseconds(rate);

      // the Wrn bit
      digitalWriteF(clkPin, LOW);
      if (digitalReadF(sloPin) == HIGH) encWrn = 1;
      delayMicroseconds(rate);
      digitalWriteF(clkPin, HIGH);
      delayMicroseconds(rate);

      // the last 6 bits are the CRC
      for (int i = 0; i < 6; i++) {
        digitalWriteF(clkPin, LOW);
        if (digitalReadF(sloPin) == HIGH) bitSet(encCrc, 5 - i);
        delayMicroseconds(rate);
        digitalWriteF(clkPin, HIGH);
        delayMicroseconds(rate);
      }
    }
  }

  // send a CDM (invert)
  digitalWriteF(clkPin, LOW);
  delayMicroseconds(rate * 10);
  digitalWriteF(clkPin, HIGH);

  // trap errors
  int16_t errors = 0;
  if (!foundAck) { VLF("WRN: BISSC Encoder, Ack bit invalid"); errors++; } else
  if (!foundStart) { VLF("WRN: BISSC Encoder, Start bit invalid"); errors++; } else
  if (!foundCds) { VLF("WRN: BISSC Encoder, Cds bit invalid"); errors++; } else
  if (encErr) { VLF("WRN: BISSC Encoder, Error bit set"); errors++; } else errors = 0;
  // todo: solve CRC and return false if it fails

  if (errors > 0) {
    if (errors <= 2) warn = true; else error = true;
    return false;
  }

  #ifndef AS37_SINGLE_TURN
    // combine absolute and 9 low order bits of multi-turn count for a 32 bit count
    position = position | ((encTurns & 0b0111111111) << 23);
  #endif

  return true;
}

#endif
