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

long ServoMotor::encoderApplyFilter(long encoderCounts) {

  // apply Kalaman filter if enabled
  switch (axisNumber) {
    case 1:
      #if AXIS1_SERVO_FLTR == KALMAN
        encoderCounts = round(axis1EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
    break;
    case 2:
      #if AXIS2_SERVO_FLTR == KALMAN
        encoderCounts = round(axis2EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
    break;
    case 3:
      #if AXIS3_SERVO_FLTR == KALMAN
        encoderCounts = round(axis3EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
    break;
    case 4:
      #if AXIS4_SERVO_FLTR == KALMAN
        encoderCounts = round(axis4EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
    break;
    case 5:
      #if AXIS5_SERVO_FLTR == KALMAN
        encoderCounts = round(axis5EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
    break;
    case 6:
      #if AXIS6_SERVO_FLTR == KALMAN
        encoderCounts = round(axis6EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
    break;
    case 7:
      #if AXIS7_SERVO_FLTR == KALMAN
        encoderCounts = round(axis7EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
    break;
    case 8:
      #if AXIS8_SERVO_FLTR == KALMAN
        encoderCounts = round(axis8EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
    break;
    case 9:
      #if AXIS9_SERVO_FLTR == KALMAN
        encoderCounts = round(axis9EncoderKalmanFilter.updateEstimate(encoderCounts));
      #endif
    break;
  }
  return encoderCounts;
}

#endif
