// -----------------------------------------------------------------------------------
// servo motor PID feedback
#pragma once

#include "../Feedback.h"

#ifdef SERVO_MOTOR_PRESENT

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

      if (autoScaleParameters) {
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

    float p, i, d, c, sensitivity;
    float lastP = 0;
    float lastI = 0;
    float lastD = 0;

    unsigned long timeSinceLastUpdate = 0;     // for varaible pid update

    char axisPrefix[14] = "MSG: Pid_, ";       // prefix for debug messages

    int parameterSelectPercent = 0;
    bool trackingSelected = true;
    unsigned long nextSelectIncrementTime = 0;
};

#endif
