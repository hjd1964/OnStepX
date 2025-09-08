// -----------------------------------------------------------------------------------
// axis servo motor driver
#pragma once

#include "../ServoDriver.h"

#ifdef SERVO_MOTOR_PRESENT

// default settings for any TMC drivers that may be present
#ifndef DRIVER_TMC_STEPPER_AUTOGRAD
  #define DRIVER_TMC_STEPPER_AUTOGRAD true
#endif
#ifndef TMC5160_RSENSE
  #define TMC5160_RSENSE 0.075F
#endif
#ifndef TMC5160_MAX_CURRENT
  #define TMC5160_MAX_CURRENT 4230         // typical module rated at 3.0A RMS
#endif
#ifndef TMC2209_RSENSE
  #define TMC2209_RSENSE 0.11F
#endif
#ifndef TMC2209_MAX_CURRENT
  #define TMC2209_MAX_CURRENT (2820*0.8)   // chip rated at 2.0A RMS, downrated to 80% due to typical step-stick form
#endif

class TmcServoDriver : public ServoDriver {
  public:
    TmcServoDriver(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings, int16_t microsteps, int16_t current, int8_t decay, int8_t decaySlewing);

    // decodes driver model and sets up the pin modes
    virtual bool init(bool reverse);

    // returns the number of axis parameters
    uint8_t getParameterCount() { return numParameters; }

    // returns the specified axis parameter
    AxisParameter* getParameter(uint8_t number) { if (number > numParameters) return &invalid; else return parameter[number]; }

    // set motor velocity
    // \param velocity as needed to reach the target position, in encoder counts per second
    // \param velocityTarget is the desired instantanous velocity at this moment, in encoder counts per second
    // \returns velocity in effect, in encoder counts per second
    virtual float setMotorVelocity(float velocity) { return 0; }

  protected:
    // driver current control
    float rSense = 0.11F;
    int16_t currentMax = 0;

    float iHold, iRun, iGoto, iHoldRatio;

    int16_t normalizedMicrosteps = 256;
    int16_t normalizedDecay;
    int16_t normalizedDecaySlewing;

    // for conversion from counts per second to stepper motor steps per second
    float countsToStepsRatio = 0;

    // runtime adjustable settings
    AxisParameter invalid      = {NAN, NAN, NAN, NAN, NAN, AXP_INVALID, ""};
    AxisParameter currentHold  = {NAN, NAN, NAN, -1, 20000, AXP_INTEGER, AXPN_CURRENT_HOLD};
    AxisParameter currentRun   = {NAN, NAN, NAN, -1, 20000, AXP_INTEGER, AXPN_CURRENT_RUN};
    AxisParameter decay        = {NAN, NAN, NAN, SPREADCYCLE, STEALTHCHOP, AXP_DECAY, AXPN_DECAY_MODE};
    AxisParameter decaySlewing = {NAN, NAN, NAN, SPREADCYCLE, STEALTHCHOP, AXP_DECAY, AXPN_DECAY_MODE_GOTO};

    const int numParameters = 4;
    AxisParameter* parameter[5] = {&invalid, &currentHold, &currentRun, &decay, &decaySlewing};
};

#endif
