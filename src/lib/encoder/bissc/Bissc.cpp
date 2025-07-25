// BISS-C encoders

#include "Bissc.h"

#ifdef HAS_BISS_C

// get device ready for use
bool Bissc::init() {
  if (ready) return true;
  if (!Encoder::init()) return false;

  pinMode(maPin, OUTPUT);
  digitalWriteF(maPin, LOW);
  pinMode(sloPin, INPUT_PULLUP);

  // see if the encoder is there
  uint32_t temp;
  if (!getCount(temp))
    if (!getCount(temp))
      if (!getCount(temp)) return false;

  ready = true;
  return true;
}

// set encoder origin
void Bissc::setOrigin(uint32_t count) {
  if (!ready) { DF("WRN: Encoder BiSS-C"); D(axis); DLF(" setOrigin(), failed"); return; }

  long temp = index;
  index = 0;
  origin = 0;

  VLF("----------------------------------------------------------------------------------------");
  VF("MSG: Encoder BiSS-C"); V(axis); VLF(", >>> with the mount in the home position <<<");
  VF("MSG: Encoder BiSS-C"); V(axis); VF(", if used AXIS"); V(axis); VF("_ENCODER_OFFSET in counts should be set to "); VL(uint32_t(-read()));
  origin = count;
  uint32_t current;
  getCount(current);
  VF("MSG: Encoder BiSS-C"); V(axis); VF(", counts at home should be 0 and currently are "); VL((int32_t)current);
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
uint32_t Bissc::getCountWithErrorRecovery(bool now) {
  if (now || (long)(millis() - lastValidTime) > 2) {
    uint32_t newCount = 0;
    if (getCount(newCount)) {
      lastValidTime = millis();
      lastValidCount = newCount;
    } else {
      if ((long)(millis() - lastValidTime) > 1000) lastValidCount = INT32_MAX;
    }
  }

  return lastValidCount;
}

#endif
