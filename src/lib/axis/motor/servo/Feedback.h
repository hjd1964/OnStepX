// -----------------------------------------------------------------------------------
// servo motor feedback
#pragma once
#include "../../../../Common.h"

#ifdef SERVO_MOTOR_PRESENT

typedef struct ServoControl {
  double in;
  double out;
  double set;
  volatile int8_t directionHint;
} ServoControl;

class Feedback {
  public:
    // initialize feedback control and parameters
    virtual void init(uint8_t axisNumber, ServoControl *control);

    // get driver type code
    virtual char getParamTypeCode();

    // validate driver parameters
    virtual bool validateParam(float param1, float param2, float param3, float param4, float param5, float param6);

    // set feedback parameters
    virtual bool setParam(float param1, float param2, float param3, float param4, float param5, float param6);

    // select feedback PID param set
    virtual void selectAlternateParam(bool alternate);

    // set feedback control direction
    virtual void setControlDirection(int8_t state);

    virtual void poll();

  protected:
    float param1, param2, param3, param4, param5, param6;
    ServoControl *control;
    uint8_t axisNumber = 0; // axis number for this feedback (1 to 9 in OnStepX)
};

#endif