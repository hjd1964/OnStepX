// -----------------------------------------------------------------------------------
// servo filter
#pragma once

#include "../../../../../Common.h"

#ifndef AXIS1_SERVO_FLTR
  #define AXIS1_SERVO_FLTR OFF
#endif
#ifndef AXIS2_SERVO_FLTR
  #define AXIS2_SERVO_FLTR OFF
#endif
#ifndef AXIS3_SERVO_FLTR
  #define AXIS3_SERVO_FLTR OFF
#endif
#ifndef AXIS4_SERVO_FLTR
  #define AXIS4_SERVO_FLTR OFF
#endif
#ifndef AXIS5_SERVO_FLTR
  #define AXIS5_SERVO_FLTR OFF
#endif
#ifndef AXIS6_SERVO_FLTR
  #define AXIS6_SERVO_FLTR OFF
#endif
#ifndef AXIS7_SERVO_FLTR
  #define AXIS7_SERVO_FLTR OFF
#endif
#ifndef AXIS8_SERVO_FLTR
  #define AXIS8_SERVO_FLTR OFF
#endif
#ifndef AXIS9_SERVO_FLTR
  #define AXIS9_SERVO_FLTR OFF
#endif

class Filter {
  public:
    virtual long update(long encoderCounts, long motorCounts, bool isTracking) { UNUSED(motorCounts); UNUSED(isTracking); return encoderCounts; }

  private:
    bool initialized = false;
};
