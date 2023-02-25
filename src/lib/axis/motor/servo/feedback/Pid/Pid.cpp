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

  pid = new QuickPID(&control->in, &control->out, &control->set,
                     0, 0, 0,
                     QuickPID::pMode::pOnError, QuickPID::dMode::dOnError, QuickPID::iAwMode::iAwCondition,
                     QuickPID::Action::direct);

  pid->SetSampleTimeUs(1000);
  pid->SetOutputLimits(-controlRange, controlRange);
  pid->SetMode(QuickPID::Control::automatic);
}

// reset feedback control and parameters
void Pid::reset() {
  V(axisPrefix); VLF("reset");
  pid->SetMode(QuickPID::Control::manual);
  control->in = 0;
  control->set = 0;
  control->out = 0;
  pid->SetMode(QuickPID::Control::automatic);
  selectTrackingParameters();
}

void Pid::setControlDirection(int8_t state) {
  if (state == ON) pid->SetControllerDirection(QuickPID::Action::reverse); else pid->SetControllerDirection(QuickPID::Action::direct);
}

// select PID param set for slewing
void Pid::selectTrackingParameters() {
  V(axisPrefix);
  p = param1;
  i = param2;
  d = param3;
  VF("select tracking parameters");
  VF(" P="); V(p); VF(", I="); V(i); VF(", D="); VL(d);
  pid->SetTunings(p, i, d);
}

// select PID param set for slewing
void Pid::selectSlewingParameters() {
  V(axisPrefix);
  p = param4;
  i = param5;
  d = param6;
  VF("select slewing parameters");
  VF(" P="); V(p); VF(", I="); V(i); VF(", D="); VL(d);
  pid->SetTunings(p, i, d);
}

// manage feedback, variable PID params
void Pid::variableParameters(float percent) {
  if ((long)(millis() - timeSinceLastUpdate) < 1000) return;
  timeSinceLastUpdate = millis();

  if (percent < 0.0F) percent = 0.0F;
  if (percent > 100.0F) percent = 100.0F;

  p = param1 + (param4 - param1)*percent/100.0F;
  i = param2 + (param5 - param2)*percent/100.0F;
  d = param3 + (param6 - param3)*percent/100.0F;

  pid->SetTunings(p, i, d);
}

#endif
