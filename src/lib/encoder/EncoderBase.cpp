// general purpose encoder class

#include "EncoderBase.h"

#if AXIS1_ENCODER != OFF || AXIS2_ENCODER != OFF || AXIS3_ENCODER != OFF || \
    AXIS4_ENCODER != OFF || AXIS5_ENCODER != OFF || AXIS6_ENCODER != OFF || \
    AXIS7_ENCODER != OFF || AXIS8_ENCODER != OFF || AXIS9_ENCODER != OFF

#include "Arduino.h"

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

  // look at how often warnings occur on a given axis
  // if (axis == 1) { D1("WRN: Encoder"); D1(axis); D1(" warning, count at "); DL1(totalWarningCount); }

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

IRAM_ATTR uint32_t Encoder::nanoseconds() {
  int64_t ns = 0;
  #if defined(CONFIG_IDF_TARGET_ESP32S)
    ns = xthal_get_ccount()*4;
  #elif defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)
    ns = ARM_DWT_CYCCNT*(1E9/F_CPU);
  #endif
  return ns & 0x0000FFFF;
}

#endif
