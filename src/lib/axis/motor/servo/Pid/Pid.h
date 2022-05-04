// -----------------------------------------------------------------------------------
// servo motor PID feedback
#pragma once

#include "../Feedback.h"

#ifdef SERVO_MOTOR_PRESENT

#include <PID_v1.h>  // https://github.com/hjd1964/Arduino-PID-Library

class Pid : public Feedback {
  public:
    Pid(const float P, const float I, const float D, const float P_goto, const float I_goto, const float D_goto);

    // initialize PID control and parameters
    void init(uint8_t axisNumber, ServoControl *control);

    // get driver type code so clients understand the use of the six parameters
    char getParameterTypeCode() { return 'P'; }

    // select PID param set for tracking or slewing
    void selectAlternateParam(bool alternate);

    // set feedback control direction
    void setControlDirection(int8_t state);

    inline void poll() { pid->Compute(); }

  private:
    PID *pid;

    float p, i, d;

    char axisPrefix[14] = "MSG: Pid_, ";       // prefix for debug messages
};

#endif
