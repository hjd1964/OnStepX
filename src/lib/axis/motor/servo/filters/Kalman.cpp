// -----------------------------------------------------------------------------------
// kalman filter

#include "Kalman.h"

#if AXIS1_SERVO_FLTR == KALMAN || AXIS2_SERVO_FLTR == KALMAN || AXIS3_SERVO_FLTR == KALMAN || \
    AXIS4_SERVO_FLTR == KALMAN || AXIS5_SERVO_FLTR == KALMAN || AXIS6_SERVO_FLTR == KALMAN || \
    AXIS7_SERVO_FLTR == KALMAN || AXIS8_SERVO_FLTR == KALMAN || AXIS9_SERVO_FLTR == KALMAN

KalmanFilter::KalmanFilter(float measurementUncertainty, float variance) {
  kalmanFilter = new SimpleKalmanFilter(measurementUncertainty, measurementUncertainty, variance);
}

long KalmanFilter::update(long encoderCounts, long motorCounts, bool isTracking) {
  UNUSED(isTracking);

  long delta = encoderCounts - motorCounts;

  if (!initialized) {
    result = encoderCounts - motorCounts;
    initialized = true;
  } else {
    result = kalmanFilter->updateEstimate(delta);
  }

  return result + motorCounts;
}

#endif
