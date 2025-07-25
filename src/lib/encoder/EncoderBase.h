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

// OFF is disabled, ON disregards unexpected quadrature encoder signals, or 
// a value > 0 (nanoseconds) disregards repeat signal events for that timer period  
#ifndef ENCODER_FILTER
  #define ENCODER_FILTER OFF
#endif

// these should allow time for an encoder signal to stabalize
#if ENCODER_FILTER > 0
  // once a signal state changes don't allow the ISR to run again for ENCODER_FILTER nanoseconds
  // it would be even better to create a low-res millis counter outside of this routine and just access the variable here
  #define ENCODER_FILTER_UNTIL(n) \
    uint32_t nsNow = nanoseconds(); \
    if ((long)(msNow - nsInvalidMillis) < 0 && (long)(nsNow - nsNext) < 0) return; \
    nsNext = nsNow + n; \
    nsInvalidMillis = msNow + 1000;

  // or, a less optimal alternative when a reasonably functional delayNanoseconds() is available...
  // once a signal state changes wait in the ISR for ENCODER_FILTER nanoseconds to let the signal stabalize
  // #define ENCODER_FILTER_UNTIL(n) delayNanoseconds(n);
#endif

// signal mode for pulse and cw/ccw encoders, default CHANGE or RISING or FALLING
// quadrature encoders
#ifndef ENCODER_SIGNAL_MODE
  #define ENCODER_SIGNAL_MODE CHANGE
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

    // set current position
    virtual void write(int32_t position);

    // set the virtual encoder velocity in counts per second
    virtual void setVelocity(float countsPerSec) { UNUSED(countsPerSec); }

    // set the virtual encoder direction (-1 is reverse, 1 is forward)
    virtual void setDirection(volatile int8_t *direction) { UNUSED(direction); }

    // check error state
    virtual bool errorThresholdExceeded() { return errorState; }

    // update encoder status
    void poll();

    // total number of errors
    uint32_t totalErrorCount = 0;

    // total number of warnings
    uint32_t totalWarningCount = 0;

    // true if encoder count is ready
    bool ready = false;

    // true if this is a virtual encoder
    bool isVirtual = false;

    // raw count as last read (includes origin for absolute encoders)
    int32_t count = 0;

    // raw index as last set
    int32_t index = 0;

    // raw origin as last set (for absolute encoders)
    uint32_t origin = 0;

  protected:
    // axis number from 1 to 9
    int16_t axis = 0;

    // accumulator for warning detection
    volatile uint32_t warn = 0;

    // accumulator for error detection
    volatile uint32_t error = 0;

    // number of errors over the last minute
    uint16_t errorCount[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // keep track of the error state
    bool errorState = false;
    bool lastErrorState = false;
    uint16_t tick = UINT16_MAX;
    uint16_t errorCountIndex = UINT16_MAX;

    #if ENCODER_FILTER > 0
      // approximate time keeping for filtering
      volatile uint32_t msNow = 0;
      volatile uint32_t nsNext = 0;
      volatile uint32_t nsInvalidMillis = 0;
    #endif
};

#endif