// -----------------------------------------------------------------------------------
// servo motor feedback

#include "Feedback.h"

#ifdef SERVO_MOTOR_PRESENT

// initialize feedback control and parameters
void Feedback::init(uint8_t axisNumber, ServoControl *control, float controlRange) {
  this->axisNumber = axisNumber;

  this->control = control;
  control->in = 0;
  control->set = 0;
}

// get default feedback parameters
void Feedback::getDefaultParameters(float *param1, float *param2, float *param3, float *param4, float *param5, float *param6) {
  *param1 = default_param1;
  *param2 = default_param2;
  *param3 = default_param3;
  *param4 = default_param4;
  *param5 = default_param5;
  *param6 = default_param6;
}

// set default feedback parameters
void Feedback::setDefaultParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  default_param1 = param1;
  default_param2 = param2;
  default_param3 = param3;
  default_param4 = param4;
  default_param5 = param5;
  default_param6 = param6;
  setParameters(param1, param2, param3, param4, param5, param6);
}

// set feedback parameters
void Feedback::setParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  this->param1 = param1;
  this->param2 = param2;
  this->param3 = param3;
  this->param4 = param4;
  this->param5 = param5;
  this->param6 = param6;
}

// validate driver parameters
bool Feedback::validateParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  return true;
}

#endif