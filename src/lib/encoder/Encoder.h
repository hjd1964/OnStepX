// general purpose encoder class
#pragma once

#include "../../Common.h"

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

#if AXIS1_ENCODER == AS37_H39B_B || AXIS2_ENCODER == AS37_H39B_B || AXIS3_ENCODER == AS37_H39B_B || \
    AXIS4_ENCODER == AS37_H39B_B || AXIS5_ENCODER == AS37_H39B_B || AXIS6_ENCODER == AS37_H39B_B || \
    AXIS7_ENCODER == AS37_H39B_B || AXIS8_ENCODER == AS37_H39B_B || AXIS9_ENCODER == AS37_H39B_B
  #define HAS_AS37_H39B_B
#endif

#if AXIS1_ENCODER == JTW_24BIT || AXIS2_ENCODER == JTW_24BIT || AXIS3_ENCODER == JTW_24BIT || \
    AXIS4_ENCODER == JTW_24BIT || AXIS5_ENCODER == JTW_24BIT || AXIS6_ENCODER == JTW_24BIT || \
    AXIS7_ENCODER == JTW_24BIT || AXIS8_ENCODER == JTW_24BIT || AXIS9_ENCODER == JTW_24BIT
  #define HAS_JTW_24BIT
#endif

#if AXIS1_ENCODER == JTW_26BIT || AXIS2_ENCODER == JTW_26BIT || AXIS3_ENCODER == JTW_26BIT || \
    AXIS4_ENCODER == JTW_26BIT || AXIS5_ENCODER == JTW_26BIT || AXIS6_ENCODER == JTW_26BIT || \
    AXIS7_ENCODER == JTW_26BIT || AXIS8_ENCODER == JTW_26BIT || AXIS9_ENCODER == JTW_26BIT
  #define HAS_JTW_26BIT
#endif

#if defined(HAS_AS37_H39B_B) || defined(HAS_JTW_24BIT) || defined(HAS_JTW_26BIT)
  #define HAS_BISS_C
#endif

class Encoder {
  public:
    // get device ready for use
    virtual void init();

    // set encoder origin
    virtual void setOrigin(uint32_t count);

    // get current position
    virtual int32_t read();

    // set current position to value
    virtual void write(int32_t count);

    // set the virtual encoder velocity in counts per second
    virtual void setVelocity(float countsPerSec) { /* normally does nothing */ }

    // set the virtual encoder direction (-1 is reverse, 1 is forward)
    virtual void setDirection(volatile int8_t *direction) { /* normally does nothing */ }

    // true if this is a virtual encoder
    bool isVirtual = false;

    // true if encoder count is ready
    bool ready = true;

    // true if errors were detected
    bool error = false;

    // true if issues with operation were detected
    bool warn = false;

    // index offset (r/w)
    int32_t offset = 0;

    // origin for absolute encoders
    uint32_t origin = 0;

    int32_t count = 0;

  protected:
    bool initialized = false;

    int16_t axis = 0;
};
