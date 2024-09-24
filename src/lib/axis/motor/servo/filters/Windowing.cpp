// -----------------------------------------------------------------------------------
// windowing average filter

#include "Windowing.h"

#if AXIS1_SERVO_FLTR == WINDOWING || AXIS2_SERVO_FLTR == WINDOWING || AXIS3_SERVO_FLTR == WINDOWING || \
    AXIS4_SERVO_FLTR == WINDOWING || AXIS5_SERVO_FLTR == WINDOWING || AXIS6_SERVO_FLTR == WINDOWING || \
    AXIS7_SERVO_FLTR == WINDOWING || AXIS8_SERVO_FLTR == WINDOWING || AXIS9_SERVO_FLTR == WINDOWING

WindowingFilter::WindowingFilter(int size) {
  this->size = size;
  values = new long[size]; 
}

long WindowingFilter::update(long encoderCounts, long motorCounts, bool isTracking) {
  UNUSED(isTracking);

  long delta = encoderCounts - motorCounts;

  if (!initialized) {
    sum = delta*size;
    index = 0;
    for (int i = 0; i < size; i++) values[i] = delta;
    initialized = true;
  }

  sum -= values[index % size];
  values[index % size] = delta;
  sum += delta;
  index++;

  return (sum/size) + motorCounts;
}

#endif
