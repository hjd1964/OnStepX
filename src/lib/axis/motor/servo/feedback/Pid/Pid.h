// -----------------------------------------------------------------------------------
// servo motor PID feedback
#pragma once

#include "../Feedback.h"

#ifdef SERVO_MOTOR_PRESENT

#include <QuickPID.h>  // https://github.com/Dlloydev/QuickPID or https://github.com/hjd1964/QuickPID (fix compile fail on ESP32)

#ifndef SERVO_SLEWING_TO_TRACKING_TIME
  #define SERVO_SLEWING_TO_TRACKING_TIME 1000 // time to switch from PID slewing to tracking parameters in ms
#endif

class Pid : public Feedback {
  public:
    Pid(const float P, const float I, const float D, const float P_goto, const float I_goto, const float D_goto, const float sensitivity = 0);

    // initialize PID control and parameters
    void init(uint8_t axisNumber, ServoControl *control, float controlRange);

    // reset feedback control and parameters
    void reset();

    // get driver type code so clients understand the use of the six parameters
    char getParameterTypeCode() { return 'P'; }

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

      if (!useVariableParameters) {
        if ((long)(millis() - nextSelectIncrementTime) > 0) {
          if (trackingSelected) parameterSelect--;
          if (parameterSelect < 0) parameterSelect = 0;
          variableParameters(parameterSelect);
          nextSelectIncrementTime = millis() + round(SERVO_SLEWING_TO_TRACKING_TIME/100.0F);
        }
      }
    }

  private:
    QuickPID *pid;

    float p, i, d, c, sensitivity;
    float lastP = 0;
    float lastI = 0;
    float lastD = 0;

    unsigned long timeSinceLastUpdate = 0;     // for varaible pid update

    char axisPrefix[14] = "MSG: Pid_, ";       // prefix for debug messages

    int parameterSelect = 0;
    bool trackingSelected = true;
    unsigned long nextSelectIncrementTime = 0;
};

#endif
