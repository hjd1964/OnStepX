// -----------------------------------------------------------------------------------
// axis servo motor driver encoder filters

#include "Servo.h"

#ifdef SERVO_MOTOR_PRESENT

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

#if AXIS1_SERVO_FLTR == KALMAN || AXIS2_SERVO_FLTR == KALMAN || AXIS3_SERVO_FLTR == KALMAN || \
    AXIS4_SERVO_FLTR == KALMAN || AXIS5_SERVO_FLTR == KALMAN || AXIS6_SERVO_FLTR == KALMAN || \
    AXIS7_SERVO_FLTR == KALMAN || AXIS8_SERVO_FLTR == KALMAN || AXIS9_SERVO_FLTR == KALMAN
  #include <SimpleKalmanFilter.h> // https://github.com/denyssene/SimpleKalmanFilter
#endif

#if AXIS1_SERVO_FLTR == KALMAN
  SimpleKalmanFilter axis1EncoderKalmanFilter(AXIS1_SERVO_FLTR_MEAS_U, AXIS1_SERVO_FLTR_MEAS_U, AXIS1_SERVO_FLTR_VARIANCE);
#endif
#if AXIS2_SERVO_FLTR == KALMAN
  SimpleKalmanFilter axis2EncoderKalmanFilter(AXIS2_SERVO_FLTR_MEAS_U, AXIS2_SERVO_FLTR_MEAS_U, AXIS2_SERVO_FLTR_VARIANCE);
#endif
#if AXIS3_SERVO_FLTR == KALMAN
  SimpleKalmanFilter axis3EncoderKalmanFilter(AXIS3_SERVO_FLTR_MEAS_U, AXIS3_SERVO_FLTR_MEAS_U, AXIS3_SERVO_FLTR_VARIANCE);
#endif
#if AXIS4_SERVO_FLTR == KALMAN
  SimpleKalmanFilter axis4EncoderKalmanFilter(AXIS4_SERVO_FLTR_MEAS_U, AXIS4_SERVO_FLTR_MEAS_U, AXIS4_SERVO_FLTR_VARIANCE);
#endif
#if AXIS5_SERVO_FLTR == KALMAN
  SimpleKalmanFilter axis5EncoderKalmanFilter(AXIS5_SERVO_FLTR_MEAS_U, AXIS5_SERVO_FLTR_MEAS_U, AXIS5_SERVO_FLTR_VARIANCE);
#endif
#if AXIS6_SERVO_FLTR == KALMAN
  SimpleKalmanFilter axis6EncoderKalmanFilter(AXIS6_SERVO_FLTR_MEAS_U, AXIS6_SERVO_FLTR_MEAS_U, AXIS6_SERVO_FLTR_VARIANCE);
#endif
#if AXIS7_SERVO_FLTR == KALMAN
  SimpleKalmanFilter axis7EncoderKalmanFilter(AXIS7_SERVO_FLTR_MEAS_U, AXIS7_SERVO_FLTR_MEAS_U, AXIS7_SERVO_FLTR_VARIANCE);
#endif
#if AXIS8_SERVO_FLTR == KALMAN
  SimpleKalmanFilter axis8EncoderKalmanFilter(AXIS8_SERVO_FLTR_MEAS_U, AXIS8_SERVO_FLTR_MEAS_U, AXIS8_SERVO_FLTR_VARIANCE);
#endif
#if AXIS9_SERVO_FLTR == KALMAN
  SimpleKalmanFilter axis9EncoderKalmanFilter(AXIS9_SERVO_FLTR_MEAS_U, AXIS9_SERVO_FLTR_MEAS_U, AXIS9_SERVO_FLTR_VARIANCE);
#endif

#if AXIS1_SERVO_FLTR == ROLLING || AXIS2_SERVO_FLTR == ROLLING || AXIS3_SERVO_FLTR == ROLLING || \
    AXIS4_SERVO_FLTR == ROLLING || AXIS5_SERVO_FLTR == ROLLING || AXIS6_SERVO_FLTR == ROLLING || \
    AXIS7_SERVO_FLTR == ROLLING || AXIS8_SERVO_FLTR == ROLLING || AXIS9_SERVO_FLTR == ROLLING
  #include "filters/rolling.h"
#endif

#if AXIS1_SERVO_FLTR == ROLLING
  RollingFilter axis1EncoderRollingFilter(AXIS1_SERVO_FLTR_WSIZE);
#endif
#if AXIS2_SERVO_FLTR == ROLLING
  RollingFilter axis2EncoderRollingFilter(AXIS2_SERVO_FLTR_WSIZE);
#endif
#if AXIS3_SERVO_FLTR == ROLLING
  RollingFilter axis3EncoderRollingFilter(AXIS3_SERVO_FLTR_WSIZE);
#endif
#if AXIS4_SERVO_FLTR == ROLLING
  RollingFilter axis4EncoderRollingFilter(AXIS4_SERVO_FLTR_WSIZE);
#endif
#if AXIS5_SERVO_FLTR == ROLLING
  RollingFilter axis5EncoderRollingFilter(AXIS5_SERVO_FLTR_WSIZE);
#endif
#if AXIS6_SERVO_FLTR == ROLLING
  RollingFilter axis6EncoderRollingFilter(AXIS6_SERVO_FLTR_WSIZE);
#endif
#if AXIS7_SERVO_FLTR == ROLLING
  RollingFilter axis7EncoderRollingFilter(AXIS7_SERVO_FLTR_WSIZE);
#endif
#if AXIS8_SERVO_FLTR == ROLLING
  RollingFilter axis8EncoderRollingFilter(AXIS8_SERVO_FLTR_WSIZE);
#endif
#if AXIS9_SERVO_FLTR == ROLLING
  RollingFilter axis9EncoderRollingFilter(AXIS9_SERVO_FLTR_WSIZE);
#endif

#if AXIS1_SERVO_FLTR == WINDOWING || AXIS2_SERVO_FLTR == WINDOWING || AXIS3_SERVO_FLTR == WINDOWING || \
    AXIS4_SERVO_FLTR == WINDOWING || AXIS5_SERVO_FLTR == WINDOWING || AXIS6_SERVO_FLTR == WINDOWING || \
    AXIS7_SERVO_FLTR == WINDOWING || AXIS8_SERVO_FLTR == WINDOWING || AXIS9_SERVO_FLTR == WINDOWING
  #include "filters/windowing.h"
#endif

#if AXIS1_SERVO_FLTR == WINDOWING
  WindowingFilter axis1EncoderWindowingFilter(AXIS1_SERVO_FLTR_WSIZE);
#endif
#if AXIS2_SERVO_FLTR == WINDOWING
  WindowingFilter axis2EncoderWindowingFilter(AXIS2_SERVO_FLTR_WSIZE);
#endif
#if AXIS3_SERVO_FLTR == WINDOWING
  WindowingFilter axis3EncoderWindowingFilter(AXIS3_SERVO_FLTR_WSIZE);
#endif
#if AXIS4_SERVO_FLTR == WINDOWING
  WindowingFilter axis4EncoderWindowingFilter(AXIS4_SERVO_FLTR_WSIZE);
#endif
#if AXIS5_SERVO_FLTR == WINDOWING
  WindowingFilter axis5EncoderWindowingFilter(AXIS5_SERVO_FLTR_WSIZE);
#endif
#if AXIS6_SERVO_FLTR == WINDOWING
  WindowingFilter axis6EncoderWindowingFilter(AXIS6_SERVO_FLTR_WSIZE);
#endif
#if AXIS7_SERVO_FLTR == WINDOWING
  WindowingFilter axis7EncoderWindowingFilter(AXIS7_SERVO_FLTR_WSIZE);
#endif
#if AXIS8_SERVO_FLTR == WINDOWING
  WindowingFilter axis8EncoderWindowingFilter(AXIS8_SERVO_FLTR_WSIZE);
#endif
#if AXIS9_SERVO_FLTR == WINDOWING
  WindowingFilter axis9EncoderWindowingFilter(AXIS9_SERVO_FLTR_WSIZE);
#endif

long ServoMotor::encoderApplyFilter(long encoderCounts) {
  static bool firstRun = true;

  // apply filter if enabled
  switch (axisNumber) {
    case 1:
      #if AXIS1_SERVO_FLTR == KALMAN
        encoderCounts = round(axis1EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
      #if AXIS1_SERVO_FLTR == ROLLING
        encoderCounts = axis1EncoderRollingFilter.update(encoderCounts);
      #endif
      #if AXIS1_SERVO_FLTR == WINDOWING
        encoderCounts = axis1EncoderWindowingFilter.update(encoderCounts);
      #endif
    break;
    case 2:
      #if AXIS2_SERVO_FLTR == KALMAN
        encoderCounts = round(axis2EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
      #if AXIS2_SERVO_FLTR == ROLLING
        encoderCounts = axis2EncoderRollingFilter.update(encoderCounts);
      #endif
      #if AXIS2_SERVO_FLTR == WINDOWING
        encoderCounts = axis2EncoderWindowingFilter.update(encoderCounts);
      #endif
    break;
    case 3:
      #if AXIS3_SERVO_FLTR == KALMAN
        encoderCounts = round(axis3EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
      #if AXIS3_SERVO_FLTR == ROLLING
        encoderCounts = axis3EncoderRollingFilter.update(encoderCounts);
      #endif
      #if AXIS3_SERVO_FLTR == WINDOWING
        encoderCounts = axis3EncoderWindowingFilter.update(encoderCounts);
      #endif
    break;
    case 4:
      #if AXIS4_SERVO_FLTR == KALMAN
        encoderCounts = round(axis4EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
      #if AXIS4_SERVO_FLTR == ROLLING
        encoderCounts = axis4EncoderRollingFilter.update(encoderCounts);
      #endif
      #if AXIS4_SERVO_FLTR == WINDOWING
        encoderCounts = axis4EncoderWindowingFilter.update(encoderCounts);
      #endif
    break;
    case 5:
      #if AXIS5_SERVO_FLTR == KALMAN
        encoderCounts = round(axis5EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
      #if AXIS5_SERVO_FLTR == ROLLING
        encoderCounts = axis5EncoderRollingFilter.update(encoderCounts);
      #endif
      #if AXIS5_SERVO_FLTR == WINDOWING
        encoderCounts = axis5EncoderWindowingFilter.update(encoderCounts);
      #endif
    break;
    case 6:
      #if AXIS6_SERVO_FLTR == KALMAN
        encoderCounts = round(axis6EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
      #if AXIS6_SERVO_FLTR == ROLLING
        encoderCounts = axis6EncoderRollingFilter.update(encoderCounts);
      #endif
      #if AXIS6_SERVO_FLTR == WINDOWING
        encoderCounts = axis6EncoderWindowingFilter.update(encoderCounts);
      #endif
    break;
    case 7:
      #if AXIS7_SERVO_FLTR == KALMAN
        encoderCounts = round(axis7EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
      #if AXIS7_SERVO_FLTR == ROLLING
        encoderCounts = axis7EncoderRollingFilter.update(encoderCounts);
      #endif
      #if AXIS7_SERVO_FLTR == WINDOWING
        encoderCounts = axis7EncoderWindowingFilter.update(encoderCounts);
      #endif
    break;
    case 8:
      #if AXIS8_SERVO_FLTR == KALMAN
        encoderCounts = round(axis8EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
      #if AXIS8_SERVO_FLTR == ROLLING
        encoderCounts = axis8EncoderRollingFilter.update(encoderCounts);
      #endif
      #if AXIS8_SERVO_FLTR == WINDOWING
        encoderCounts = axis8EncoderWindowingFilter.update(encoderCounts);
      #endif
    break;
    case 9:
      #if AXIS9_SERVO_FLTR == KALMAN
        encoderCounts = round(axis9EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
      #if AXIS9_SERVO_FLTR == ROLLING
        encoderCounts = axis9EncoderRollingFilter.update(encoderCounts);
      #endif
      #if AXIS9_SERVO_FLTR == WINDOWING
        encoderCounts = axis9EncoderWindowingFilter.update(encoderCounts);
      #endif
    break;
  }
  return encoderCounts;
}

#endif
