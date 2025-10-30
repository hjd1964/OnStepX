// -----------------------------------------------------------------------------------
// axis servo DC motor driver
#pragma once

#include <Arduino.h>
#include <math.h>            // fabsf, lroundf, fmaf
#include "../../../../../Common.h"

#if defined(SERVO_PE_PRESENT) || defined(SERVO_EE_PRESENT) || defined(SERVO_TMC2130_DC_PRESENT) || defined(SERVO_TMC5160_DC_PRESENT)

#ifndef ANALOG_WRITE_RANGE
  #define ANALOG_WRITE_RANGE 255
#endif
#ifndef SERVO_ANALOG_WRITE_RANGE
  #define SERVO_ANALOG_WRITE_RANGE ANALOG_WRITE_RANGE
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
    // convert from encoder counts per second to analogWriteRange units to roughly match velocity
    // we expect a linear scaling for the motors (maybe in the future we can be smarter?)
    // uses cached min/max counts and a precomputed gain
    // only float ops since usually there is no double FPU
    long toAnalogRange(float velocity) {
      // Update caches only if inputs changed
      recomputeScalingIfNeeded();

      // Extract sign and work with magnitude
      int sign = 1;
      if (velocity < 0.0F) { velocity = -velocity; sign = -1; }

      if (velocity == 0.0F || velocityMaxCached <= 0.0f) {
        return 0; // early outvelocityMaxCached
      }

      // Clamp to velocityMax to avoid over-range math.
      float vAbs = velocity;
      if (vAbs > velocityMaxCached) vAbs = velocityMaxCached;

      // Linear map: counts = countsMin + vAbs * gain
      // fmaf keeps one rounding and is very fast on some FPUS(M7).
      // Currenty we always set the power to overcome static friction to the min counts
      // even for near zero velocity where the PID jitters around zero
      // TODO! Consider deadband handling here
      float countsF = fmaf(vAbs, velToCountsGain, (float)countsMinCached);

      // Single float→int rounding at the end
      long power = (long)lroundf(countsF);

      return power * sign;
    }

    // motor control update
    virtual void pwmUpdate(long power) { }

    long analogWriteRange = SERVO_ANALOG_WRITE_RANGE;

    // runtime adjustable settings (percent 0..100)
    AxisParameter pwmMinimum = {NAN, NAN, NAN, 0.0, 100.0, AXP_FLOAT_IMMEDIATE, AXPN_MIN_PWR};
    AxisParameter pwmMaximum = {NAN, NAN, NAN, 0.0, 100.0, AXP_FLOAT_IMMEDIATE, AXPN_MAX_PWR};

    const int numParameters = 3;
    AxisParameter* parameter[4] = {&invalid, &acceleration, &pwmMinimum, &pwmMaximum};

  private:
    // Cached scaling (recomputed only when inputs change)
    // Detect changes to pwmMinimum.value, pwmMaximum.value, velocityMax, analogWriteRange.
    float   pwmMinPctCached   = -1.0f;
    float   pwmMaxPctCached   = -1.0f;
    float   velocityMaxCached = 0.0f;
    long    analogMaxCached   = -1;

    int32_t countsMinCached   = 0;      // integer min duty in counts
    int32_t countsMaxCached   = 0;      // integer max duty in counts
    float   velToCountsGain   = 0.0f;   // counts per (encoder count/s)

    // Recompute cache if any dependency changed.
    inline void recomputeScalingIfNeeded() {
      const long analogMaxNow = (analogWriteRange - 1);

      if (pwmMinPctCached   != pwmMinimum.value ||
          pwmMaxPctCached   != pwmMaximum.value ||
          velocityMaxCached != velocityMax ||
          analogMaxCached   != analogMaxNow)
      {
        pwmMinPctCached   = pwmMinimum.value;   // percent 0..100
        pwmMaxPctCached   = pwmMaximum.value;   // percent 0..100
        velocityMaxCached = velocityMax;
        analogMaxCached   = analogMaxNow;

        // Convert % to float counts once, then round once.
        const float minCountsF = (pwmMinPctCached * 0.01f) * (float)analogMaxCached;
        const float maxCountsF = (pwmMaxPctCached * 0.01f) * (float)analogMaxCached;

        countsMinCached = (int32_t)lroundf(minCountsF);
        countsMaxCached = (int32_t)lroundf(maxCountsF);
        if (countsMaxCached < countsMinCached) countsMaxCached = countsMinCached; // safety

        const int span = (int)(countsMaxCached - countsMinCached);
        velToCountsGain = (velocityMaxCached > 0.0f) ? ((float)span / velocityMaxCached) : 0.0f;
      }
    }
};

#endif
