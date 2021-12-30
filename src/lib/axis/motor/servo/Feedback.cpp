// -----------------------------------------------------------------------------------
// servo motor feedback

#include "Feedback.h"

#ifdef SERVO_MOTOR_PRESENT

// initialize feedback control and parameters
void Feedback::init(uint8_t axisNumber, ServoControl *control) {
  this->axisNumber = axisNumber;

  this->control = control;
  control->in = 0;
  control->set = 0;

  param1 = 0;
  param2 = 0;
  param3 = 0;
  param4 = 0;
  param5 = 0;
  param6 = 0;
}

// validate driver parameters
bool Feedback::validateParam(float param1, float param2, float param3, float param4, float param5, float param6) {
  return true;
}

// set feedback parameters
bool Feedback::setParam(float param1, float param2, float param3, float param4, float param5, float param6) {
  this->param1 = param1;
  this->param2 = param2;
  this->param3 = param3;
  this->param4 = param4;
  this->param5 = param5;
  this->param6 = param6;
}

#endif