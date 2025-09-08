// -----------------------------------------------------------------------------------
// servo motor PID feedback

#include "Pid.h"

#ifdef SERVO_MOTOR_PRESENT

#if AXIS1_SERVO_FEEDBACK == PID || AXIS2_SERVO_FEEDBACK == PID || AXIS3_SERVO_FEEDBACK == PID || \
    AXIS4_SERVO_FEEDBACK == PID || AXIS5_SERVO_FEEDBACK == PID || AXIS6_SERVO_FEEDBACK == PID || \
    AXIS7_SERVO_FEEDBACK == PID || AXIS8_SERVO_FEEDBACK == PID || AXIS9_SERVO_FEEDBACK == PID

Pid::Pid(const float P, const float I, const float D, const float P_goto, const float I_goto, const float D_goto, const float sensitivity) {
  trackingP.valueDefault = P;
  trackingI.valueDefault = I;
  trackingD.valueDefault = D;

  autoScaleParameters = (sensitivity == 0);
  if (autoScaleParameters) this->sensitivity = 100; else this->sensitivity = sensitivity;
}

// initialize PID control and parameters
void DualPid::init(uint8_t axisNumber, ServoControl *control, float controlRange) {
  Feedback::init(axisNumber, control);

  axisPrefix[5] = '0' + axisNumber;

  VF("MSG:"); V(axisPrefix); VF("setting feedback with range +/-"); VL(controlRange);
  VF("MSG:"); V(axisPrefix); if (autoScaleParameters) { VL("using auto parameter scaling"); } else { VL("using manual parameter scaling"); } 

  pid = new QuickPID(&control->in, &control->out, &control->set,
                     0, 0, 0,
                     QuickPID::pMode::PID_PMODE, QuickPID::dMode::PID_DMODE, QuickPID::iAwMode::PID_IMODE,
                     QuickPID::Action::direct);
  pid->SetSampleTimeUs(PID_SAMPLE_TIME_US);
  pid->SetOutputLimits(-controlRange, controlRange);
  pid->Initialize();
  pid->SetMode(QuickPID::Control::automatic);

  autoScaleParameters = false;
}

// reset feedback control and parameters
void Pid::reset() {
  VF("MSG:"); V(axisPrefix); VLF("reset");
  control->in = 0;
  control->set = 0;
  control->out = 0;
  trackingSelected = false;
  selectTrackingParameters();
}

void DualPid::setControlDirection(int8_t state) {
  if (state == ON) pid->SetControllerDirection(QuickPID::Action::reverse); else pid->SetControllerDirection(QuickPID::Action::direct);
}

// select PID param set for slewing
void Pid::selectTrackingParameters() {
  if (!trackingSelected) {
    VF("MSG:"); V(axisPrefix); VL("tracking selected");
    trackingSelected = true;
    pid->Reset();
    pid->SetTunings(P.value, I.value, D.value);
  }
}

// select PID param set for slewing
void Pid::selectSlewingParameters() {
  if (trackingSelected) {
    VF("MSG:"); V(axisPrefix); VL("slewing selected");
    trackingSelected = false;
    pid->Reset();
    pid->SetTunings(P.value, I.value, D.value);
  }
}

// variable PID params isn't used
void Pid::variableParameters(float percent) {
  UNUSED(percent);
}

#endif

#endif
