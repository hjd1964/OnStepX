// -----------------------------------------------------------------------------------
// servo motor PID feedback
#pragma once

#include "../FeedbackBase.h"

#ifdef SERVO_MOTOR_PRESENT

#if AXIS1_SERVO_FEEDBACK == DUAL_PID || AXIS2_SERVO_FEEDBACK == DUAL_PID || AXIS3_SERVO_FEEDBACK == DUAL_PID || \
    AXIS4_SERVO_FEEDBACK == DUAL_PID || AXIS5_SERVO_FEEDBACK == DUAL_PID || AXIS6_SERVO_FEEDBACK == DUAL_PID || \
    AXIS7_SERVO_FEEDBACK == DUAL_PID || AXIS8_SERVO_FEEDBACK == DUAL_PID || AXIS9_SERVO_FEEDBACK == DUAL_PID

#include <QuickPID.h>  // https://github.com/Dlloydev/QuickPID

// various options for QuickPID read about them here https://github.com/Dlloydev/QuickPID
#ifndef PID_SLEWING_TO_TRACKING_TIME_MS
  #define PID_SLEWING_TO_TRACKING_TIME_MS 1000 // time to switch from PID slewing to tracking parameters in milliseconds
#endif
#ifndef PID_SAMPLE_TIME_US
  #define PID_SAMPLE_TIME_US 10000 // PID sample time in microseconds (defaults to 10 milliseconds)
#endif
#ifndef PID_PMODE
  #define PID_PMODE pOnError // http://brettbeauregard.com/blog/2017/06/introducing-proportional-on-measurement/
#endif
#ifndef PID_IMODE
  #define PID_IMODE iAwCondition
#endif
#ifndef PID_DMODE
  #define PID_DMODE dOnMeas
#endif

class DualPid : public Feedback {
  public:
    DualPid(const float P, const float I, const float D, const float P_goto, const float I_goto, const float D_goto, const float sensitivity = 0);

    // initialize PID control and parameters
    void init(uint8_t axisNumber, ServoControl *control, float controlRange);

    // reset feedback control and parameters
    void reset();

    // returns the number of axis parameters
    uint8_t getParameterCount() { return numParameters; }

    // returns the specified axis parameter
    AxisParameter* getParameter(uint8_t number) { if (number > numParameters) return &invalid; else return parameter[number]; }

    // set feedback control direction
    void setControlDirection(int8_t state);

    // select PID param set for tracking
    void selectTrackingParameters();

    // select PID param set for slewing
    void selectSlewingParameters();

    // variable feedback, variable PID params
    void variableParameters(float percent);

    inline void poll() {
      pid->Compute();

      if (manuallySwitchParameters) {
        if ((long)(millis() - nextSelectIncrementTime) > 0) {
          if (trackingSelected) parameterSelectPercent--;
          if (parameterSelectPercent < 0) parameterSelectPercent = 0;
          variableParameters(parameterSelectPercent);
          nextSelectIncrementTime = millis() + round(PID_SLEWING_TO_TRACKING_TIME_MS/100.0F);
        }
      }
    }

  private:
    QuickPID *pid;

    char axisPrefix[26] = " Axis_ServoFeedbackDPID, "; // prefix for debug messages

    float sensitivity;
    float lastP = 0;
    float lastI = 0;
    float lastD = 0;

    unsigned long timeSinceLastUpdate = 0; // for varaible pid update

    int parameterSelectPercent = 0;
    bool trackingSelected = true;
    unsigned long nextSelectIncrementTime = 0;

    // runtime adjustable settings
    AxisParameter trackingP = {NAN, NAN, NAN, 0.0, 10000.0, AXP_FLOAT_IMMEDIATE, AXPN_PID_P};
    AxisParameter trackingI = {NAN, NAN, NAN, 0.0, 10000.0, AXP_FLOAT_IMMEDIATE, AXPN_PID_I};
    AxisParameter trackingD = {NAN, NAN, NAN, 0.0, 10000.0, AXP_FLOAT_IMMEDIATE, AXPN_PID_D};

    AxisParameter slewingP = {NAN, NAN, NAN, 0.0, 10000.0, AXP_FLOAT_IMMEDIATE, AXPN_PID_SLEWING_P};
    AxisParameter slewingI = {NAN, NAN, NAN, 0.0, 10000.0, AXP_FLOAT_IMMEDIATE, AXPN_PID_SLEWING_I};
    AxisParameter slewingD = {NAN, NAN, NAN, 0.0, 10000.0, AXP_FLOAT_IMMEDIATE, AXPN_PID_SLEWING_D};

    const int numParameters = 6;
    AxisParameter* parameter[7] = {&invalid, &trackingP, &trackingI, &trackingD, &slewingP, &slewingI, &slewingD};
};

#endif

#endif
