// -----------------------------------------------------------------------------------
// servo motor PID feedback
#pragma once

#include "../Feedback.h"

#ifdef SERVO_MOTOR_PRESENT

#include <QuickPID.h>  // https://github.com/Dlloydev/QuickPID or https://github.com/hjd1964/QuickPID (fix compile fail on ESP32)

class Pid : public Feedback {
  public:
    Pid(const float P, const float I, const float D, const float P_goto, const float I_goto, const float D_goto);

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

    inline void poll() { pid->Compute(); }

  private:
    QuickPID *pid;

    float p, i, d, c;

    unsigned long timeSinceLastUpdate = 0;     // for varaible pid update

    char axisPrefix[14] = "MSG: Pid_, ";       // prefix for debug messages
};

#endif
