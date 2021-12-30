// -----------------------------------------------------------------------------------
// servo motor PID feedback
#pragma once

#include "..\Feedback.h"

#ifdef SERVO_MOTOR_PRESENT

#include <PID_v1.h>  // https://github.com/hjd1964/Arduino-PID-Library

class Pid : public Feedback {
  public:
    void init(uint8_t axisNumber, ServoControl *control);
    void selectAlternateParam(bool alternate);
    void setControlDirection(int8_t state);
    inline void poll() { pid->Compute(); }

  private:
    PID *pid;

    float p, i, d;

    char axisPrefix[14] = "MSG: Pid_, ";       // prefix for debug messages
};

#endif
