// -----------------------------------------------------------------------------------
// servo motor feedback
#pragma once
#include "../../../../../Common.h"

#ifdef SERVO_MOTOR_PRESENT

typedef struct ServoControl {
  float in;
  float out;
  float set;
  volatile int8_t directionHint;
} ServoControl;

class Feedback {
  public:
    // initialize feedback control and parameters, controlRange is +/- 255 default
    virtual void init(uint8_t axisNumber, ServoControl *control, float controlRange = 255);

    // reset feedback control and parameters
    virtual void reset();

    // get driver type code
    virtual char getParameterTypeCode();

    // get default feedback parameters
    virtual void getDefaultParameters(float *param1, float *param2, float *param3, float *param4, float *param5, float *param6);

    // set default feedback parameters
    virtual void setDefaultParameters(float param1, float param2, float param3, float param4, float param5, float param6);

    // set feedback parameters
    virtual void setParameters(float param1, float param2, float param3, float param4, float param5, float param6);

    // validate driver parameters
    virtual bool validateParameters(float param1, float param2, float param3, float param4, float param5, float param6);

    // select feedback PID param set for tracking
    virtual void selectTrackingParameters();

    // select feedback PID param set for slewing
    virtual void selectSlewingParameters();

    // variable feedback PID params
    virtual void variableParameters(float percent);

    // set feedback control direction
    virtual void setControlDirection(int8_t state);

    virtual void poll();

    bool autoScaleParameters = true;

  protected:
    float default_param1 = 0, default_param2 = 0, default_param3 = 0, default_param4 = 0, default_param5 = 0, default_param6 = 0;
    float param1 = 0, param2 = 0, param3 = 0, param4 = 0, param5 = 0, param6 = 0;
    ServoControl *control;
    uint8_t axisNumber = 0;
};

#endif