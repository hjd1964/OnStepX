// -----------------------------------------------------------------------------------
// servo motor PID feedback

#include "DualPid.h"

#ifdef SERVO_MOTOR_PRESENT

#if AXIS1_SERVO_FEEDBACK == DUAL_PID || AXIS2_SERVO_FEEDBACK == DUAL_PID || AXIS3_SERVO_FEEDBACK == DUAL_PID || \
    AXIS4_SERVO_FEEDBACK == DUAL_PID || AXIS5_SERVO_FEEDBACK == DUAL_PID || AXIS6_SERVO_FEEDBACK == DUAL_PID || \
    AXIS7_SERVO_FEEDBACK == DUAL_PID || AXIS8_SERVO_FEEDBACK == DUAL_PID || AXIS9_SERVO_FEEDBACK == DUAL_PID

DualPid::DualPid(const float P, const float I, const float D, const float P_goto, const float I_goto, const float D_goto, const float sensitivity) {
  trackingP.valueDefault = P;
  trackingI.valueDefault = I;
  trackingD.valueDefault = D;

  slewingP.valueDefault = P_goto;
  slewingI.valueDefault = I_goto;
  slewingD.valueDefault = D_goto;

  manuallySwitchParameters = (sensitivity == 0);
  if (manuallySwitchParameters) this->sensitivity = 100; else this->sensitivity = sensitivity;
}

// initialize PID control and parameters
void DualPid::init(uint8_t axisNumber, ServoControl *control, float controlRange) {
  Feedback::init(axisNumber, control);

  axisPrefix[5] = '0' + axisNumber;

  VF("MSG:"); V(axisPrefix); VF("setting feedback with range +/-"); VL(controlRange);
  VF("MSG:"); V(axisPrefix); if (manuallySwitchParameters) { VL("using manual parameter switching"); } else { VL("using auto parameter scaling"); } 

  pid = new QuickPID(&control->in, &control->out, &control->set,
                     0, 0, 0,
                     QuickPID::pMode::PID_PMODE, QuickPID::dMode::PID_DMODE, QuickPID::iAwMode::PID_IMODE,
                     QuickPID::Action::direct);
  pid->SetSampleTimeUs(PID_SAMPLE_TIME_US);
  pid->SetOutputLimits(-controlRange, controlRange);
  pid->Initialize();
  pid->SetMode(QuickPID::Control::automatic);
}

// reset feedback control and parameters
void DualPid::reset() {
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
void DualPid::selectTrackingParameters() {
  if (!trackingSelected) {
    VF("MSG:"); V(axisPrefix); VL("tracking selected");
    trackingSelected = true;
    parameterSelectPercent = 0;
    pid->Reset();
    pid->SetTunings(trackingP.value, trackingI.value, trackingD.value);
    lastP = trackingP.value;
    lastI = trackingI.value;
    lastD = trackingD.value;
  }
}

// select PID param set for slewing
void DualPid::selectSlewingParameters() {
  if (trackingSelected) {
    VF("MSG:"); V(axisPrefix); VL("slewing selected");
    trackingSelected = false;
    parameterSelectPercent = 100;
    pid->Reset();
    pid->SetTunings(slewingP.value, slewingI.value, slewingD.value);
    lastP = slewingP.value;
    lastI = slewingI.value;
    lastD = slewingD.value;
  }
}

// manage feedback, variable PID params
void DualPid::variableParameters(float percent) {
  float s = percent/sensitivity;
  if (s < 0.0F) s = 0.0F;
  if (s > 1.0F) s = 1.0F;
  float p = trackingP.value + (slewingP.value - trackingP.value)*s;
  float i = trackingI.value + (slewingI.value - trackingI.value)*s;
  float d = trackingD.value + (slewingD.value - trackingD.value)*s;
  if (lastP != p || lastI != i || lastD != d) {
//    VF("MSG:"); V(axisPrefix);
//    V("variable parameters");
//    V(" P="); V(p); V(", I="); V(i); V(", D="); VL(d);
    pid->SetTunings(p, i, d);
    lastP = p;
    lastI = i;
    lastD = d;
  }
}

#endif

#endif
