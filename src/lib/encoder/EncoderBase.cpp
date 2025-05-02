// general purpose encoder class

#include "EncoderBase.h"

#if AXIS1_ENCODER != OFF || AXIS2_ENCODER != OFF || AXIS3_ENCODER != OFF || \
    AXIS4_ENCODER != OFF || AXIS5_ENCODER != OFF || AXIS6_ENCODER != OFF || \
    AXIS7_ENCODER != OFF || AXIS8_ENCODER != OFF || AXIS9_ENCODER != OFF

// get device ready for use
bool Encoder::init() {
  return true;
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
      DF("WRN: Encoder"); D(axis); DF(" error, exceeded threshold/minute at "); DL(errorCount);
    } else {
      VF("MSG: Encoder"); V(axis); VF(" error, cleared");
    }
    lastErrorState = errorState;
  }

   return errorState;
}

#endif
