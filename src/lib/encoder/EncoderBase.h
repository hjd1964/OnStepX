// general purpose encoder class
#pragma once

#include "../../Common.h"

// allow up to 20 errors per minute
#ifndef ENCODER_ERROR_COUNT_THRESHOLD
  #define ENCODER_ERROR_COUNT_THRESHOLD 20
#endif

#ifndef AXIS1_ENCODER
  #define AXIS1_ENCODER OFF
#endif
#ifndef AXIS2_ENCODER
  #define AXIS2_ENCODER OFF
#endif
#ifndef AXIS3_ENCODER
  #define AXIS3_ENCODER OFF
#endif
#ifndef AXIS4_ENCODER
  #define AXIS4_ENCODER OFF
#endif
#ifndef AXIS5_ENCODER
  #define AXIS5_ENCODER OFF
#endif
#ifndef AXIS6_ENCODER
  #define AXIS6_ENCODER OFF
#endif
#ifndef AXIS7_ENCODER
  #define AXIS7_ENCODER OFF
#endif
#ifndef AXIS8_ENCODER
  #define AXIS8_ENCODER OFF
#endif
#ifndef AXIS9_ENCODER
  #define AXIS9_ENCODER OFF
#endif

#if AXIS1_ENCODER == AS37_H39B_B || AXIS2_ENCODER == AS37_H39B_B
  #define HAS_AS37_H39B_B
#endif

#if AXIS1_ENCODER == LIKA_ASC85 || AXIS2_ENCODER == LIKA_ASC85
  #define HAS_LIKA_ASC85
#endif

#if AXIS1_ENCODER == JTW_24BIT || AXIS2_ENCODER == JTW_24BIT
  #define HAS_JTW_24BIT
#endif

#if AXIS1_ENCODER == JTW_26BIT || AXIS2_ENCODER == JTW_26BIT
  #define HAS_JTW_26BIT
#endif

#if defined(HAS_AS37_H39B_B) || defined(HAS_LIKA_ASC85) || defined(HAS_JTW_24BIT) || defined(HAS_JTW_26BIT) 
  #define HAS_BISS_C
#endif

#if AXIS1_ENCODER != OFF || AXIS2_ENCODER != OFF || AXIS3_ENCODER != OFF || \
    AXIS4_ENCODER != OFF || AXIS5_ENCODER != OFF || AXIS6_ENCODER != OFF || \
    AXIS7_ENCODER != OFF || AXIS8_ENCODER != OFF || AXIS9_ENCODER != OFF
    
class Encoder {
  public:
    // get device ready for use
    virtual bool init();

    // set encoder origin
    virtual void setOrigin(uint32_t count);

    // get current position
    virtual int32_t read();

    // set current position to value
    virtual void write(int32_t count);

    // set the virtual encoder velocity in counts per second
    virtual void setVelocity(float countsPerSec) { UNUSED(countsPerSec); }

    // set the virtual encoder direction (-1 is reverse, 1 is forward)
    virtual void setDirection(volatile int8_t *direction) { UNUSED(direction); }

    // check for error state
    virtual bool errorThresholdExceeded();

    // total number of errors
    int32_t totalErrorCount = 0;

    // total number of warnings
    int32_t totalWarningCount = 0;

    // true if encoder count is ready
    bool ready = false;

    // true if this is a virtual encoder
    bool isVirtual = false;

    // raw position count (as last read)
    int32_t count = 0;

    // raw position offset count (as last set)
    int32_t offset = 0;

    // raw origin count (as last set) for absolute encoders
    uint32_t origin = 0;

  protected:
    unsigned long lastMinute = 0;

    // accumulator for error detection
    volatile int32_t error = 0;

    // number of errors (resets once a minute)
    int32_t errorCount = 0;

    // accumulator for warning detection
    volatile int32_t warn = 0;

    // keep track of when the error state changes
    bool lastErrorState = false;

    int16_t axis = 0;
};

#endif