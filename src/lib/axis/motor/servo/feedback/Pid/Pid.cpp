// -----------------------------------------------------------------------------------
// servo motor PID feedback

#include "Pid.h"

#ifdef SERVO_MOTOR_PRESENT

Pid::Pid(const float P, const float I, const float D, const float P_goto, const float I_goto, const float D_goto) {
  setDefaultParameters(P, I, D, P_goto, I_goto, D_goto);
}

// initialize PID control and parameters
void Pid::init(uint8_t axisNumber, ServoControl *control, float controlRange) {
  Feedback::init(axisNumber, control);

  axisPrefix[8] = '0' + axisNumber;

  p = param1;
  i = param2;
  d = param3;
  c = controlRange;

  V(axisPrefix); VF("setting feedback with range +/-"); VL(controlRange);

  pid = new PID(&control->in, &control->out, &control->set, 0, 0, 0, DIRECT);

  pid->SetSampleTime(1);
  pid->SetOutputLimits(-controlRange, controlRange);
  pid->SetMode(AUTOMATIC);
}

// reset feedback control and parameters
void Pid::reset() {
  V(axisPrefix); VLF("reset");
  pid->SetMode(MANUAL);
  control->in = 0;
  control->set = 0;
  control->out = 0;
  pid->SetMode(AUTOMATIC);
  selectAlternateParameters(false);
}

// select PID param set for tracking or slewing
void Pid::selectAlternateParameters(bool alternate) {
  V(axisPrefix);
  if (!alternate) {
    p = param1;
    i = param2;
    d = param3;
    VF("select tracking parameters");
  } else {
    p = param4;
    i = param5;
    d = param6;
    VF("select slewing parameters");
  }
  VF(" P="); V(p); VF(", I="); V(i); VF(", D="); VL(d);
  pid->SetTunings(p, i, d);
}

void Pid::setControlDirection(int8_t state) {
  if (state == ON) pid->SetControllerDirection(REVERSE); else pid->SetControllerDirection(DIRECT);
}

// manage feedback, variable PID params
void Pid::variableParameters(float percent) {
  if (percent < 0.0F) percent = 0.0F;
  if (percent > 100.0F) percent = 100.0F;

  p = param1 + (param4 - param1)*percent/100.0F;
  i = param2 + (param5 - param2)*percent/100.0F;
  d = param3 + (param6 - param3)*percent/100.0F;

  pid->SetTunings(p, i, d);
}

#endif
