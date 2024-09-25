// -----------------------------------------------------------------------------------
// servo motor PID feedback

#include "Pid.h"

#ifdef SERVO_MOTOR_PRESENT

Pid::Pid(const float P, const float I, const float D, const float P_goto, const float I_goto, const float D_goto, const float sensitivity) {
  setDefaultParameters(P, I, D, P_goto, I_goto, D_goto);
  autoScaleParameters = (sensitivity == 0);
  if (autoScaleParameters) this->sensitivity = 100; else this->sensitivity = sensitivity;
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
  V(axisPrefix); if (autoScaleParameters) { VL("using auto parameter scaling"); } else { VL("using manual parameter scaling"); } 

  pid = new QuickPID(&control->in, &control->out, &control->set,
                     0, 0, 0,
                     QuickPID::pMode::PID_PMODE, QuickPID::dMode::PID_DMODE, QuickPID::iAwMode::PID_IMODE,
                     QuickPID::Action::direct);
  pid->SetSampleTimeUs(PID_SAMPLE_TIME_US);
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
  trackingSelected = true;
  selectSlewingParameters();
}

void Pid::setControlDirection(int8_t state) {
  if (state == ON) pid->SetControllerDirection(QuickPID::Action::reverse); else pid->SetControllerDirection(QuickPID::Action::direct);
}

// select PID param set for slewing
void Pid::selectTrackingParameters() {
  if (!trackingSelected) {
    pid->SetMode(QuickPID::Control::manual);
    pid->SetMode(QuickPID::Control::automatic);
    V(axisPrefix); VL("tracking selected");
    trackingSelected = true;
  }
}

// select PID param set for slewing
void Pid::selectSlewingParameters() {
  if (trackingSelected) {
    pid->SetMode(QuickPID::Control::manual);
    pid->SetMode(QuickPID::Control::automatic);
    V(axisPrefix); VL("slewing selected");
    trackingSelected = false;
    parameterSelectPercent = 100;
    p = param4;
    i = param5;
    d = param6;
    pid->SetTunings(p, i, d);
    lastP = p;
    lastI = i;
    lastD = d;
  }
}

// manage feedback, variable PID params
void Pid::variableParameters(float percent) {
  float s = percent/sensitivity;
  if (s < 0.0F) s = 0.0F;
  if (s > 1.0F) s = 1.0F;
  p = param1 + (param4 - param1)*s;
  i = param2 + (param5 - param2)*s;
  d = param3 + (param6 - param3)*s;
  if (lastP != p || lastI != i || lastD != d) {
//    V(axisPrefix);
//    V("variable parameters");
//    V(" P="); V(p); V(", I="); V(i); V(", D="); VL(d);
    pid->SetTunings(p, i, d);
    lastP = p;
    lastI = i;
    lastD = d;
  }
}

#endif
