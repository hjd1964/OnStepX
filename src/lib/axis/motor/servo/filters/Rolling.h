// -----------------------------------------------------------------------------------
// rolling average filter
#pragma once

#include "../../../../../Common.h"
#include "Filter.h"

#if AXIS1_SERVO_FLTR == ROLLING || AXIS2_SERVO_FLTR == ROLLING || AXIS3_SERVO_FLTR == ROLLING || \
    AXIS4_SERVO_FLTR == ROLLING || AXIS5_SERVO_FLTR == ROLLING || AXIS6_SERVO_FLTR == ROLLING || \
    AXIS7_SERVO_FLTR == ROLLING || AXIS8_SERVO_FLTR == ROLLING || AXIS9_SERVO_FLTR == ROLLING

class RollingFilter : public Filter {
  public:
    RollingFilter(int size);

    long update(long encoderCounts, long motorCounts, bool isTracking);

  private:
    bool initialized = false;
    int size;
    long long result;
};

#endif
