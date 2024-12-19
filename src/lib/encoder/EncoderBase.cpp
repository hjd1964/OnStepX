// general purpose encoder class

#include "EncoderBase.h"

// get device ready for use
void Encoder::init() {
}

// set encoder origin
void Encoder::setOrigin(uint32_t count) {
  origin = count;
}

bool Encoder::errorThresholdExceeded() {
  unsigned long minute = millis()/60000;

  if (minute != lastMinute) {
    errorCount = 0;

    // simulate an encoder error event
    //if (minute == 2) error = 30; 
  
    lastMinute = minute;
  }

  errorCount += error;
  #ifdef ENCODER_WARN_AS_ERROR
    errorCount += warn;
  #endif

  totalErrorCount += error;
  error = 0;

  totalWarningCount += warn;
  warn = 0;

  bool errorState = errorCount > ENCODER_ERROR_COUNT_THRESHOLD;

  if (errorState != lastErrorState) {
    if (errorState) {
      VF("WRN: Encoder"); V(axis); VF(" error, exceeded threshold/minute at "); VL(errorCount);
    } else {
      VF("MSG: Encoder"); V(axis); VF(" error, cleared");
    }
    lastErrorState = errorState;
  }

   return errorState;
}
