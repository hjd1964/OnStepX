// -----------------------------------------------------------------------------------
// servo motor feedback

#include "FeedbackBase.h"

#ifdef SERVO_MOTOR_PRESENT

// initialize feedback control and parameters
void Feedback::init(uint8_t axisNumber, ServoControl *control, float controlRange) {
  this->axisNumber = axisNumber;

  this->control = control;
  control->in = 0;
  control->set = 0;
}

#endif