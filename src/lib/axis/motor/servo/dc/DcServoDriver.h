// -----------------------------------------------------------------------------------
// axis servo DC motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#if defined(SERVO_PE_PRESENT) || defined(SERVO_EE_PRESENT) || defined(SERVO_TMC2130_DC_PRESENT) || defined(SERVO_TMC5160_DC_PRESENT)

#ifndef ANALOG_WRITE_RANGE
  #define ANALOG_WRITE_RANGE 255
#endif

#include "../ServoDriver.h"

class ServoDcDriver : public ServoDriver {
  public:
    // constructor
    ServoDcDriver(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings, float pwmMinimum, float pwmMaximum);

    // returns the number of axis parameters
    uint8_t getParameterCount() { return numParameters; }

    // returns the specified axis parameter
    AxisParameter* getParameter(uint8_t number) { if (number > numParameters) return &invalid; else return parameter[number]; }

    // set motor velocity
    // \param velocity as needed to reach the target position, in encoder counts per second
    // \returns velocity in effect, in encoder counts per second
    float setMotorVelocity(float velocity);

  protected:
    // convert from encoder counts/sec to normalized duty 0..1
    float toDuty01(float velocity) {
      if (velocity == 0.0F) return 0.0F;
      if (velocity < 0.0F) velocity = -velocity;

      float d = velocity/velocityMax;
      if (isinf(d)) return 0.0F;
      clamp01(d);

      // apply min/max % limits
      float dMin = pwmMinimum.value/100.0F;
      float dMax = pwmMaximum.value/100.0F;
      if (dMax < dMin) { float t = dMax; dMax = dMin; dMin = t; }

      return dMin + d*(dMax - dMin);
    }

    // motor control update (normalized duty)
    virtual void pwmUpdate(float duty01) { }

    static inline void clamp01(float &x) {
      if (isnan(x) || isinf(x)) { x = 0.0F; return; }
      if (x < 0.0F) { x = 0.0F; return; }
      if (x > 1.0F) { x = 1.0F; return; }
    }

    // runtime adjustable settings
    AxisParameter pwmMinimum = {NAN, NAN, NAN, 0.0, 100.0, AXP_FLOAT_IMMEDIATE, AXPN_MIN_PWR};
    AxisParameter pwmMaximum = {NAN, NAN, NAN, 0.0, 100.0, AXP_FLOAT_IMMEDIATE, AXPN_MAX_PWR};
    AxisParameter dirDeadTimeMs = {NAN, NAN, NAN, 0.0, 50.0, AXP_FLOAT_IMMEDIATE, "Deadtime, ms"};

    const int numParameters = 5;
    AxisParameter* parameter[6] = {&invalid, &acceleration, &zeroDeadband, &pwmMinimum, &pwmMaximum, &dirDeadTimeMs};

  private:
    uint32_t dirHoldUntilMs = 0;
    int8_t lastEffectiveDirection = 0; // -1,0,+1
};

#endif
