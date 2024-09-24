// -----------------------------------------------------------------------------------
// windowing average filter
#pragma once

#include "../../../../../Common.h"
#include "Filter.h"

#if AXIS1_SERVO_FLTR == WINDOWING || AXIS2_SERVO_FLTR == WINDOWING || AXIS3_SERVO_FLTR == WINDOWING || \
    AXIS4_SERVO_FLTR == WINDOWING || AXIS5_SERVO_FLTR == WINDOWING || AXIS6_SERVO_FLTR == WINDOWING || \
    AXIS7_SERVO_FLTR == WINDOWING || AXIS8_SERVO_FLTR == WINDOWING || AXIS9_SERVO_FLTR == WINDOWING

class WindowingFilter : public Filter {
  public:
    WindowingFilter(int size);
    
    long update(long encoderCounts, long motorCounts, bool isTracking);

  private:
    bool initialized = false;
    int size;
    long *values;
    unsigned long index;
    long long sum;
};

#endif