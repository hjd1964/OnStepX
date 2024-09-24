// -----------------------------------------------------------------------------------
// rolling average filter

#include "Rolling.h"

#if AXIS1_SERVO_FLTR == ROLLING || AXIS2_SERVO_FLTR == ROLLING || AXIS3_SERVO_FLTR == ROLLING || \
    AXIS4_SERVO_FLTR == ROLLING || AXIS5_SERVO_FLTR == ROLLING || AXIS6_SERVO_FLTR == ROLLING || \
    AXIS7_SERVO_FLTR == ROLLING || AXIS8_SERVO_FLTR == ROLLING || AXIS9_SERVO_FLTR == ROLLING

RollingFilter::RollingFilter(int size) {
  this->size = size;
}

long RollingFilter::update(long encoderCounts, long motorCounts, bool isTracking) {
  UNUSED(isTracking);

  long delta = encoderCounts - motorCounts;

  if (!initialized) {
     result = encoderCounts - motorCounts;
     initialized = true;
  }

  result = (result*(size - 1) + delta)/size;
  return result + motorCounts;
}

#endif
