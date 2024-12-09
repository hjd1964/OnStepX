// general purpose encoder class

#include "Encoder.h"

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

  if (errorCount > ENCODER_ERROR_COUNT_THRESHOLD) {
    return true;
   } else return false;
}
