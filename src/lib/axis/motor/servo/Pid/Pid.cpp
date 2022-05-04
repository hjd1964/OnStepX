// -----------------------------------------------------------------------------------
// servo motor PID feedback

#include "Pid.h"

#ifdef SERVO_MOTOR_PRESENT

Pid::Pid(const float P, const float I, const float D, const float P_goto, const float I_goto, const float D_goto) {
  setDefaultParameters(P, I, D, P_goto, I_goto, D_goto);
}

// initialize PID control and parameters
void Pid::init(uint8_t axisNumber, ServoControl *control) {
  Feedback::init(axisNumber, control);

  axisPrefix[8] = '0' + axisNumber;

  p = param1;
  i = param2;
  d = param3;

  V(axisPrefix);
  VF("setting feedback with range +/-");
  VL(ANALOG_WRITE_PWM_RANGE);

  pid = new PID(&control->in, &control->out, &control->set, 0, 0, 0, DIRECT);

  pid->SetSampleTime(1);
  pid->SetOutputLimits(-ANALOG_WRITE_PWM_RANGE, ANALOG_WRITE_PWM_RANGE);
  pid->SetMode(AUTOMATIC);
}

// select PID param set for tracking or slewing
void Pid::selectAlternateParam(bool alternate) {
  V(axisPrefix);
  if (!alternate) {
    p = param1;
    i = param2;
    d = param3;
    VF("setting normal parameters");
  } else {
    p = param4;
    i = param5;
    d = param6;
    VF("setting alternate parameters");
  }
  VF(" P="); V(p); VF(", I="); V(i); VF(", D="); VL(d);
  pid->SetTunings(p, i, d);
}

void Pid::setControlDirection(int8_t state) {
  if (state == ON) pid->SetControllerDirection(REVERSE); else pid->SetControllerDirection(DIRECT);
}

#endif
