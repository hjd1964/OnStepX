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

class Encoder {
  public:
    // get device ready for use
    virtual void init();

    // get current count
    virtual int32_t read();

    // set current count to value
    virtual void write(int32_t count);

    // true if errors were detected
    bool error = false;

    // true if issues with operation were detected
    bool warn = false;

    // index offset (r/w)
    int32_t offset = 0;

  protected:
    volatile int32_t count = 0;
    bool initialized = false;

};
