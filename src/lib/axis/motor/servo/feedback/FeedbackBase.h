// -----------------------------------------------------------------------------------
// servo motor feedback
#pragma once
#include "../../../../../Common.h"

#ifdef SERVO_MOTOR_PRESENT

#include "../../Drivers.h"

#ifndef AXIS1_SERVO_FEEDBACK
  #define AXIS1_SERVO_FEEDBACK DUAL_PID
#endif
#ifndef AXIS2_SERVO_FEEDBACK
  #define AXIS2_SERVO_FEEDBACK DUAL_PID
#endif
#ifndef AXIS3_SERVO_FEEDBACK
  #define AXIS3_SERVO_FEEDBACK DUAL_PID
#endif
#ifndef AXIS4_SERVO_FEEDBACK
  #define AXIS4_SERVO_FEEDBACK DUAL_PID
#endif
#ifndef AXIS5_SERVO_FEEDBACK
  #define AXIS5_SERVO_FEEDBACK DUAL_PID
#endif
#ifndef AXIS6_SERVO_FEEDBACK
  #define AXIS6_SERVO_FEEDBACK DUAL_PID
#endif
#ifndef AXIS7_SERVO_FEEDBACK
  #define AXIS7_SERVO_FEEDBACK DUAL_PID
#endif
#ifndef AXIS8_SERVO_FEEDBACK
  #define AXIS8_SERVO_FEEDBACK DUAL_PID
#endif
#ifndef AXIS9_SERVO_FEEDBACK
  #define AXIS9_SERVO_FEEDBACK DUAL_PID
#endif

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

    // returns the number of axis parameters
    virtual uint8_t getParameterCount() { return 0; }

    // returns the specified axis parameter
    virtual AxisParameter* getParameter(uint8_t number) { return &invalid; }

    // select feedback PID param set for tracking
    virtual void selectTrackingParameters();

    // select feedback PID param set for slewing
    virtual void selectSlewingParameters();

    // variable feedback PID params
    virtual void variableParameters(float percent);

    // set feedback control direction
    virtual void setControlDirection(int8_t state);

    virtual void poll();

    bool manuallySwitchParameters = true;

  protected:
    uint8_t axisNumber = 0;

    ServoControl *control;

    // runtime adjustable settings
    AxisParameter invalid = {NAN, NAN, NAN, NAN, NAN, AXP_INVALID, ""};
};

#endif