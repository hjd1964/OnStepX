// -----------------------------------------------------------------------------------
// axis servo DC motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#if defined(SERVO_PE_PRESENT) || defined(SERVO_EE_PRESENT) || defined(SERVO_TMC2130_DC_PRESENT) || defined(SERVO_TMC5160_DC_PRESENT)

#include "../ServoDriver.h"

#ifndef SERVO_DC_VPID
  #if ENCODER_VELOCITY == ON
    #define SERVO_DC_VPID ON
  #else
    #define SERVO_DC_VPID OFF
  #endif
#endif

class ServoDcDriver : public ServoDriver {
  public:
    // constructor
    ServoDcDriver(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings, float pwmMinimum, float pwmMaximum);

    // returns the number of axis parameters
    uint8_t getParameterCount() { return numParameters; }

    // returns the specified axis parameter
    AxisParameter* getParameter(uint8_t number) { if (number > numParameters) return &invalid; else return parameter[number]; }

    // set motor velocity
    // \param velocity as needed to reach the target position, in signed encoder counts per second
    // \param encoderVelocity in signed encoder counts per second
    // \returns velocity in effect, in signed encoder counts per second
    float setMotorVelocity(float velocity, float encoderVelocity) override;

  protected:

    // motor control pwm update
    // \param power01 in -1.0 to 1.0 units
    virtual void pwmUpdate(float power01) { }

    // runtime adjustable settings
    AxisParameter pwmMinimum = {NAN, NAN, NAN, 0.0, 100.0, AXP_FLOAT_IMMEDIATE, AXPN_MIN_PWR};
    AxisParameter pwmMaximum = {NAN, NAN, NAN, 1.0, 100.0, AXP_FLOAT_IMMEDIATE, AXPN_MAX_PWR};
    AxisParameter dirDeadTimeMs = {NAN, NAN, NAN, 0.0, 50.0, AXP_FLOAT_IMMEDIATE, "Deadtime, ms"};
    #if SERVO_DC_VPID == ON
      AxisParameter velKp      = {NAN, NAN, NAN, 0.0, 10.0,  AXP_FLOAT_IMMEDIATE, "Vel Kp"};
      AxisParameter velKi      = {NAN, NAN, NAN, 0.0, 10.0,  AXP_FLOAT_IMMEDIATE, "Vel Ki"};
      AxisParameter velIMax    = {NAN, NAN, NAN, 0.0, 1.0,   AXP_FLOAT_IMMEDIATE, "Vel Imax"};
      const int numParameters = 8;
      AxisParameter* parameter[9] = {&invalid, &acceleration, &zeroDeadband, &pwmMinimum, &pwmMaximum, &dirDeadTimeMs, &velKp, &velKi, &velIMax};
    #else
      const int numParameters = 5;
      AxisParameter* parameter[6] = {&invalid, &acceleration, &zeroDeadband, &pwmMinimum, &pwmMaximum, &dirDeadTimeMs};
    #endif

  private:
    float vI = 0.0F;
    uint32_t velLastUs = 0;
    uint32_t dirHoldUntilMs = 0;
    int8_t lastEffectiveDirection = 0; // -1,0,+1
};

#endif
