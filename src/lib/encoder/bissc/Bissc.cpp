// BISS-C encoders

#include "Bissc.h"

#ifdef HAS_BISS_C

// get device ready for use
bool Bissc::init() {
  if (ready) return true;
  if (!Encoder::init()) return false;
  if (encoderBits < 8 || encoderBits > 31) return false;

  pinMode(maPin, OUTPUT);
  digitalWriteF(maPin, LOW);
  pinMode(sloPin, INPUT_PULLUP);

  // get the counts/mask ready
  encoderCounts = (uint32_t)(1UL << encoderBits);
  encoderHalfCounts = (int32_t)(encoderCounts >> 1);

  const uint8_t lowTurnBits = (encoderBits >= 32) ? 0 : (uint8_t)(32U - encoderBits);
  encoderMultiTurnMask = (lowTurnBits >= 32) ? 0xFFFFFFFFUL : ((1UL << lowTurnBits) - 1UL);

  VF("MSG: Encoder "); V(encoderName); VLF(", confirming attempt 1...");

  // see if the encoder is there
  uint32_t temp;
  if (!getCount(temp)) {
    VF("MSG: Encoder "); V(encoderName); VLF(", confirming attempt 2...");
    if (!getCount(temp)) {
      VF("MSG: Encoder "); V(encoderName); VLF(", confirming attempt 3...");
      if (!getCount(temp)) return false;
    }
  }

  VF("MSG: Encoder "); V(encoderName); VLF(", confirmation success");

  ready = true;
  return true;
}

// set encoder origin
void Bissc::setOrigin(int32_t counts) {
  if (!ready) { DF("WRN: Encoder "); V(encoderName); DLF(", setOrigin(); failed not ready"); return; }

  // origin must always remains in the of range +/- encoderCounts/2 which is the
  // normalized absolute encoder range (prior to applying the origin and any index)
  if (counts < -encoderHalfCounts || counts >= encoderHalfCounts) {
    DF("WRN: Encoder "); V(encoderName); DLF(", setOrigin(); failed counts out of range"); return; 
  }

  const int32_t temp = index;
  index = 0;
  origin = 0;

  VLF("----------------------------------------------------------------------------------------");
  VF("MSG: Encoder "); V(encoderName); VLF(", >>> with the mount in the home position <<<");
  VF("MSG: Encoder "); V(encoderName); VF(", if used AXIS"); V(axis); VF("_ENCODER_OFFSET in counts should be set to "); VL(-read());
  origin = counts;
  
  VF("MSG: Encoder "); V(encoderName);
  uint32_t currentCounts;
  if (getCount(currentCounts)) {
    VF(", counts at home should be 0 and currently are "); VL((int32_t)currentCounts);
  } else {
    VLF(", getCount read FAILED!");
  }
  VLF("----------------------------------------------------------------------------------------");

  index = temp;
}

// read encoder position
int32_t Bissc::read() {
  if (!ready) return 0;

  int32_t newCount = getCountWithErrorRecovery();
  
  if (newCount != INT32_MAX) return newCount + index; else return INT32_MAX;
}

// write encoder position
void Bissc::write(int32_t position) {
  if (!ready) return;

  if (position != INT32_MAX) {
    int32_t newCount;
    newCount = getCountWithErrorRecovery(true);
    if (newCount == INT32_MAX) {
      error = ENCODER_ERROR_COUNT_THRESHOLD + 1;
      return;
    }
    
    index = position - newCount;
  }
}

// read encoder count with 1 second error recovery
// returns encoder count or INT32_MAX on error
int32_t Bissc::getCountWithErrorRecovery(bool immediate) {
  const unsigned long now = millis();
  if (immediate || now - lastValidTime > 2) {
    uint32_t newCount = 0;
    if (getCount(newCount)) {
      lastValidTime = now;
      lastValidCount = newCount;

      #if ENCODER_VELOCITY == ON
        velNoteSampledCount((int32_t)newCount);
      #endif

    } else {
      if (now - lastValidTime > 1000U) lastValidCount = INT32_MAX;
    }
  }

  return lastValidCount;
}

// get encoder count relative to origin
bool Bissc::getCount(uint32_t &count) {

  // prepare for a reading
  foundAck = false;
  foundStart = false;
  foundCds = false;

  nErr = 1;
  nWrn = 1;
  frameCrc = 0;

  turns = 0;
  count = 0;

  #ifdef ESP32
    static portMUX_TYPE bisscMutex = portMUX_INITIALIZER_UNLOCKED;
    taskENTER_CRITICAL(&bisscMutex);
  #else
    noInterrupts();
  #endif

  getCountBitBang(count);

  #ifdef ESP32
    taskEXIT_CRITICAL(&bisscMutex);
  #else
    interrupts();
  #endif

  // trap errors
  int16_t errors = 0;

  uint64_t encData = (uint64_t)count | ((uint64_t)turns << encoderBits);
  encData = (encData << 1) | nErr;
  encData = (encData << 1) | nWrn;
  if (foundAck && foundStart && foundCds) {
    if (crc6(encData) == frameCrc) {
      if (encoderWrnInvert ? !nWrn : nWrn) {
        D1("WRN: Encoder "); D1(encoderName); DL1(", Warn bit set");
        warn++;
      }
      if (encoderErrInvert ? !nErr : nErr) {
        D1("WRN: Encoder "); D1(encoderName); DL1(", Error bit set");
        errors++;
      } else
        good++;
    } else {
      bad++;
      #if DEBUG != OFF
        float goodBadRatio = (float)bad/(good + bad);
        D1("WRN: Encoder "); D1(encoderName); D1(", Crc invalid (overall "); D1(goodBadRatio*100.0F); D1("%"); DL1(")");
      #endif
      errors++;
    }
  } else {
    D1("WRN: Encoder "); D1(encoderName);
    if (!foundAck)   { D1(", Ack"); }   else
    if (!foundStart) { D1(", Start"); } else
    if (!foundCds)   { D1(", Cds"); }   else
    { DL1(" bit invalid"); }
    errors++;
  }

  if (errors > 0) {
    error++;
    return false;
  }

  #if BISSC_SINGLE_TURN != ON
    // combine absolute and low order bits of multi-turn count for a 32 bit count
    count = count | ((turns & encoderMultiTurnMask) << encoderBits);
  #endif

  // apply origin
  count += origin;

  // center count about the +/- half counts range (a signed value stored unsigned)
  // note: instead shifts an multiTurn count but for historical reasons thats preferred
  count = count - (encoderCounts >> 1);

  #ifdef BISSC_RESOLUTION_DIVISOR
    count = (int32_t)count/(int32_t)(BISSC_RESOLUTION_DIVISOR);
  #endif

  return true;
}

IRAM_ATTR void Bissc::getCountBitBang(uint32_t &count) {

  // sync phase
  for (int i = 0; i < BISSC_SYNC_PHASE; i++) {
    digitalWriteF(maPin, LOW_MA);
    delayNanoseconds(tSample);
    if (digitalReadF(sloPin) == LOW_SLO) foundAck = true;
    delayNanoseconds(rate - tSample);
    digitalWriteF(maPin, HIGH_MA);
    delayNanoseconds(rate);
    if (foundAck) break;
  }

  // if we have an Ack
  if (foundAck) {
    for (int i = 0; i < BISSC_ACK_PHASE; i++) {
      digitalWriteF(maPin, LOW_MA);
      delayNanoseconds(tSample);
      if (digitalReadF(sloPin) == HIGH_SLO) foundStart = true;
      delayNanoseconds(rate - tSample);
      digitalWriteF(maPin, HIGH_MA);
      delayNanoseconds(rate);
      if (foundStart) break;
    }

    // if we have an Start
    if (foundStart) {
      digitalWriteF(maPin, LOW_MA);
      delayNanoseconds(tSample);
      if (digitalReadF(sloPin) == LOW_SLO) foundCds = true;
      delayNanoseconds(rate - tSample);
      digitalWriteF(maPin, HIGH_MA);
      delayNanoseconds(rate);

      // if we have an Cds, read the data
      if (foundCds) {

        // the first n bits are the multi-turn count
        for (int i = 1; i <= encoderMultiTurnBits; i++) {
          digitalWriteF(maPin, LOW_MA);
          delayNanoseconds(tSample);
          if (digitalReadF(sloPin) == HIGH_SLO) bitSet(turns, encoderMultiTurnBits - i);
          delayNanoseconds(rate - tSample);
          digitalWriteF(maPin, HIGH_MA);
          delayNanoseconds(rate);
        }
        
        // the next n bits are the encoder absolute count
        for (int i = 1; i <= encoderBits; i++) {
          digitalWriteF(maPin, LOW_MA);
          delayNanoseconds(tSample);
          if (digitalReadF(sloPin) == HIGH_SLO) bitSet(count, encoderBits - i);
          delayNanoseconds(rate - tSample);
          digitalWriteF(maPin, HIGH_MA);
          delayNanoseconds(rate);
        }

        // the Err bit
        digitalWriteF(maPin, LOW_MA);
        delayNanoseconds(tSample);
        nErr = (digitalReadF(sloPin) == HIGH_SLO);
        delayNanoseconds(rate - tSample);
        digitalWriteF(maPin, HIGH_MA);
        delayNanoseconds(rate);

        // the Wrn bit
        digitalWriteF(maPin, LOW_MA);
        delayNanoseconds(tSample);
        nWrn = (digitalReadF(sloPin) == HIGH_SLO);
        delayNanoseconds(rate - tSample);
        digitalWriteF(maPin, HIGH_MA);
        delayNanoseconds(rate);

        // the last 6 bits are the CRC
        for (int i = 0; i < 6; i++) {
          digitalWriteF(maPin, LOW_MA);
          delayNanoseconds(tSample);
          if (digitalReadF(sloPin) == HIGH_SLO) bitSet(frameCrc, 5 - i);
          delayNanoseconds(rate - tSample);
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
}

#endif
