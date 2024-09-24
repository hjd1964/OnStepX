// -----------------------------------------------------------------------------------
// kalman filter
#pragma once

#include "../../../../../Common.h"
#include "Filter.h"

#if AXIS1_SERVO_FLTR == KALMAN || AXIS2_SERVO_FLTR == KALMAN || AXIS3_SERVO_FLTR == KALMAN || \
    AXIS4_SERVO_FLTR == KALMAN || AXIS5_SERVO_FLTR == KALMAN || AXIS6_SERVO_FLTR == KALMAN || \
    AXIS7_SERVO_FLTR == KALMAN || AXIS8_SERVO_FLTR == KALMAN || AXIS9_SERVO_FLTR == KALMAN

#include <SimpleKalmanFilter.h> // https://github.com/denyssene/SimpleKalmanFilter

class KalmanFilter : public Filter {
  public:
    KalmanFilter(float measurementUncertainty, float variance);

    long update(long encoderCounts, long motorCounts, bool isTracking);

  private:
    bool initialized = false;
    long long result;
 
    SimpleKalmanFilter *kalmanFilter;
};

#endif
