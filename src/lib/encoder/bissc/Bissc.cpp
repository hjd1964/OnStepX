// BISS-C encoders

#include "Bissc.h"

#ifdef HAS_BISS_C

// get device ready for use
void Bissc::init() {
  if (initialized) { VF("WRN: Encoder BiSS-C"); V(axis); VLF(" init(), already initialized!"); return; }

  pinMode(maPin, OUTPUT);
  digitalWriteF(maPin, LOW);
  pinMode(sloPin, INPUT_PULLUP);

  initialized = true;
}

// set encoder origin
void Bissc::setOrigin(uint32_t count) {
  if (!initialized) { VF("WRN: Encoder BiSS-C"); V(axis); VLF(" setOrigin(), not initialized!"); return; }

  long temp = offset;
  offset = 0;
  origin = 0;

  VLF("----------------------------------------------------------------------------------------");
  VF("MSG: Encoder BiSS-C"); V(axis); VLF(", >>> with the mount in the home position <<<");
  VF("MSG: Encoder BiSS-C"); V(axis); VF(", if used AXIS"); V(axis); VF("_ENCODER_OFFSET in counts should be set to "); VL(uint32_t(-read()));
  origin = count;
  uint32_t current;
  readEnc(current);
  VF("MSG: Encoder BiSS-C"); V(axis); VF(", counts at home should be 0 and currently are "); VL((int32_t)current);
  VLF("----------------------------------------------------------------------------------------");

  offset = temp;
}

// read encoder count
int32_t Bissc::read() {
  if (!initialized) { VF("WRN: Encoder BiSS-C"); V(axis); VLF(" read(), not initialized!"); return 0; }

  uint32_t temp;
  if (readEncLatest(temp)) {
    count = (int32_t)temp;
    return count + offset;
  } else return INT32_MAX;
}

// write encoder count
void Bissc::write(int32_t count) {
  if (!initialized) { VF("WRN: Encoder BiSS-C"); V(axis); VLF(" write(), not initialized!"); return; }

  if (count != INT32_MAX) {
    uint32_t temp;
    if (readEncLatest(temp)) {
      offset = count - (int32_t)temp;
    }
  }
}

// read encoder count with (1 second) error recovery
bool Bissc::readEncLatest(uint32_t &position) {
  if ((long)(millis() - lastValidTime) < 2) {
    position = lastValidPosition;
    return true;
  }

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

#endif
