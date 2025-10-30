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

// Enable to apply hysteresis around zero velocity
#ifdef SERVO_HYSTERESIS_ENABLE
  // Thresholds in encoder counts/sec
  #ifndef SERVO_HYST_ENTER_CPS
    #define SERVO_HYST_ENTER_CPS 20.0f   // must exceed this to LEAVE zero (1/10 of sidereal speed of 92 counts / sec)
  #endif
  #ifndef SERVO_HYST_EXIT_CPS
    #define SERVO_HYST_EXIT_CPS 10.0f    // drop below this to RETURN to zero (half of the above)
  #endif

  #ifdef SERVO_SIGMA_DELTA_DITHERING
    #include "SigmaDeltaDither.h"
  #endif
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

      #ifdef SERVO_HYSTERESIS_ENABLE
        zeroHoldSign = 0; // ensure immediate exit and clear latch on exact zero
      #endif

      #ifdef SERVO_SIGMA_DELTA_DITHERING
        sigmaDelta.reset(); // reset dithering residue when output is zero
      #endif

      return 0; // early out
    }

    // Clamp to velocityMax to avoid over-range math.
    float vAbs = velocity; // already absolute
    if (vAbs > velocityMaxCached) vAbs = velocityMaxCached;

    #ifdef SERVO_HYSTERESIS_ENABLE
      // Hysteresis around zero: require a larger enter threshold to leave zero,
      // and a smaller "exit" threshold to return to zero. This prevents chatter
      // when the PID jitters around zero
      if (zeroHoldSign == 0) {
        // currently at zero: must exceed enter threshold to start moving
        if (vAbs < SERVO_HYST_ENTER_CPS) return 0;
        zeroHoldSign = (sign >= 0) ? 1 : -1; // latch direction
      } else {
        // currently moving: if we drop below exit threshold, snap back to zero
        if (vAbs < SERVO_HYST_EXIT_CPS) {
          zeroHoldSign = 0;
          #ifdef SERVO_SIGMA_DELTA_DITHERING
            sigmaDelta.reset(); // also reset when snapping back to zero
          #endif
          return 0;
        }
        // allow direction change if command flips while above thresholds
        if ((sign >= 0 ? 1 : -1) != zeroHoldSign) zeroHoldSign = (sign >= 0) ? 1 : -1;
      }
      // use latched direction while moving
      sign = (zeroHoldSign < 0) ? -1 : 1;
    #endif

    // Linear map: counts = countsMin + vAbs * gain
    // fmaf keeps one rounding and is very fast on some FPUS(M7).
    float countsF = fmaf(vAbs, velToCountsGain, (float)countsMinCached);

    #ifdef SERVO_SIGMA_DELTA_DITHERING
      // Dither the floating counts to an integer so the time-average equals countsF
      int32_t power = sigmaDelta.dither_counts(countsF, countsMinCached, countsMaxCached);
    #else
      // Single float→int rounding at the end
      long power = (long)lroundf(countsF);
    #endif

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

        VF("MSG:"); V(axisPrefix); VF("pwmMin="); V(pwmMinPctCached); VLF(" %");
        VF("MSG:"); V(axisPrefix); VF("pwmMax="); V(pwmMaxPctCached); VLF(" %");
        VF("MSG:"); V(axisPrefix); VF("Vmax="); V(velocityMaxCached); VLF(" steps/s");
        VF("MSG:"); V(axisPrefix); VF("pwm units="); V(analogMaxCached); VLF(" pwm Units");

        // Convert % to float counts once, then round once.
        const float minCountsF = (pwmMinPctCached * 0.01f) * (float)analogMaxCached;
        const float maxCountsF = (pwmMaxPctCached * 0.01f) * (float)analogMaxCached;

        countsMinCached = (int32_t)lroundf(minCountsF);
        countsMaxCached = (int32_t)lroundf(maxCountsF);
        if (countsMaxCached < countsMinCached) countsMaxCached = countsMinCached; // safety

        const int span = (int)(countsMaxCached - countsMinCached);
        velToCountsGain = (velocityMaxCached > 0.0f) ? ((float)span / velocityMaxCached) : 0.0f;
        VF("MSG:"); V(axisPrefix); VF("velToCountsGain="); V(velToCountsGain); VLF(" from velocity -> pwm units");
      }
    }

    #ifdef SERVO_HYSTERESIS_ENABLE
      int8_t zeroHoldSign = 0; // 0: at zero; +1 / -1: direction while "moving"
    #endif

};

#endif
