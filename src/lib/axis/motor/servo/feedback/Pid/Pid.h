// -----------------------------------------------------------------------------------
// servo motor PID feedback
#pragma once

#include "../FeedbackBase.h"

#ifdef SERVO_MOTOR_PRESENT

#if AXIS1_SERVO_FEEDBACK == PID || AXIS2_SERVO_FEEDBACK == PID || AXIS3_SERVO_FEEDBACK == PID || \
    AXIS4_SERVO_FEEDBACK == PID || AXIS5_SERVO_FEEDBACK == PID || AXIS6_SERVO_FEEDBACK == PID || \
    AXIS7_SERVO_FEEDBACK == PID || AXIS8_SERVO_FEEDBACK == PID || AXIS9_SERVO_FEEDBACK == PID

#include <QuickPID.h>  // https://github.com/Dlloydev/QuickPID

// various options for QuickPID read about them here https://github.com/Dlloydev/QuickPID
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
    Pid(const float P, const float I, const float D);

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

    inline void poll() { }

  private:
    QuickPID *pid;

    char axisPrefix[26] = " Axis_ServoFeedbackPID, "; // prefix for debug messages

    bool trackingSelected = true;

    // runtime adjustable settings
    AxisParameter P = {NAN, NAN, NAN, 0.0, 10000.0, AXP_FLOAT_IMMEDIATE, "P"};
    AxisParameter I = {NAN, NAN, NAN, 0.0, 10000.0, AXP_FLOAT_IMMEDIATE, "I"};
    AxisParameter D = {NAN, NAN, NAN, 0.0, 10000.0, AXP_FLOAT_IMMEDIATE, "D"};

    const int numParameters = 3;
    AxisParameter* parameter[4] = {&invalid, &P, &I, &D};
};

#endif

#endif
