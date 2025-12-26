// -----------------------------------------------------------------------------------
// axis servo DC motor driver

#include "DcServoDriver.h"

#if defined(SERVO_PE_PRESENT) || defined(SERVO_EE_PRESENT) || \
    defined(SERVO_TMC2130_DC_PRESENT) || defined(SERVO_TMC5160_DC_PRESENT)

#include "../../../../gpioEx/GpioEx.h"

// constructor
ServoDcDriver::ServoDcDriver(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings,
                             float pwmMinimum, float pwmMaximum)
                            :ServoDriver(axisNumber, Pins, Settings) {
  if (axisNumber < 1 || axisNumber > 9) return;

  this->pwmMinimum.valueDefault = pwmMinimum;
  this->pwmMaximum.valueDefault = pwmMaximum;

  dirDeadTimeMs.valueDefault = 0.0F;
  #if SERVO_DC_VPID == ON
    velKp.valueDefault   = 0.2F;
    velKi.valueDefault   = 0.5F;
    velIMax.valueDefault = 0.2F;
  #endif
}

#if SERVO_DC_VPID == ON

  float ServoDcDriver::setMotorVelocity(float velocity, float encoderVelocity) {
    velocity = ServoDriver::setMotorVelocity(velocity);

    if (velocity == 0.0F) {
      pwmUpdate(0.0F);
      vI = 0.0F;
      velLastUs = 0;
      lastEffectiveDirection = 0;
      return 0.0F;
    }

    const float vmag = fabsf(velocity);
    const int8_t dir = (velocity > 0.0F) ? 1 : -1;

    const unsigned long nowMs = millis();
    if (lastEffectiveDirection != 0 && dir != lastEffectiveDirection) {
      const unsigned long dtMs = (unsigned long)lroundf(dirDeadTimeMs.value);
      if (dtMs) dirHoldUntilMs = nowMs + dtMs;
      vI = 0.0F;
      velLastUs = 0;
    }
    lastEffectiveDirection = dir;

    if ((long)(dirHoldUntilMs - nowMs) > 0) {
      pwmUpdate(0.0F);
      return 0.0F;
    }

    const float dMin  = pwmMinimum.value * 0.01F;
    const float dMax0 = pwmMaximum.value * 0.01F;
    const float dMax  = (dMax0 >= dMin) ? dMax0 : dMin;
    const float dSpan = dMax - dMin;

    float vcmd01 = vmag * InvVelocityMax;
    if (vcmd01 > 1.0F) vcmd01 = 1.0F;

    const uint32_t nowUs = micros();
    uint32_t dUs = nowUs - velLastUs;
    if (velLastUs == 0) dUs = 0;
    velLastUs = nowUs;

    // measured velocity projected onto commanded direction and normalized
    encoderVelocity *= (float)dir;
    const float vmeas01 = encoderVelocity * InvVelocityMax;
    const float error01 = vcmd01 - vmeas01;

    float u01 = vcmd01 + (velKp.value * error01) + vI;

    float uSat = u01;
    if (uSat < 0.0F) uSat = 0.0F;
    else if (uSat > 1.0F) uSat = 1.0F;

    if (velKi.value > 0.0F && dUs > 0 && dUs <= 200000U) {
      const bool atLo = (uSat <= 0.0F);
      const bool atHi = (uSat >= 1.0F);
      const bool allowI =
        (!atLo && !atHi) ||
        (atLo && error01 > 0.0F) ||
        (atHi && error01 < 0.0F);

      if (allowI) {
        vI += velKi.value * error01 * (dUs * 0.000001F);

        const float imax = velIMax.value;
        if (vI >  imax) vI =  imax;
        else if (vI < -imax) vI = -imax;

        u01  = vcmd01 + (velKp.value * error01) + vI;
        uSat = u01;
        if (uSat < 0.0F) uSat = 0.0F;
        else if (uSat > 1.0F) uSat = 1.0F;
      }
    } else if (dUs > 200000U) {
      vI = 0.0F;
    }

    float dutyMag01 = 0.0F;
    if (uSat > 0.0F) {
      dutyMag01 = dMin + dSpan * uSat;
      if (dutyMag01 > dMax) dutyMag01 = dMax;
    }

    pwmUpdate(dutyMag01 * (float)dir);
    return velocity;
  }

#else

  float ServoDcDriver::setMotorVelocity(float velocity, float /*encoderVelocity*/) {
    velocity = ServoDriver::setMotorVelocity(velocity);

    if (velocity == 0.0F) {
      pwmUpdate(0.0F);
      velLastUs = 0;
      lastEffectiveDirection = 0;
      return 0.0F;
    }

    const float vmag = fabsf(velocity);
    const int8_t dir = (velocity > 0.0F) ? 1 : -1;

    const unsigned long nowMs = millis();
    if (lastEffectiveDirection != 0 && dir != lastEffectiveDirection) {
      const unsigned long dtMs = (unsigned long)lroundf(dirDeadTimeMs.value);
      if (dtMs) dirHoldUntilMs = nowMs + dtMs;
      velLastUs = 0;
    }
    lastEffectiveDirection = dir;

    if ((long)(dirHoldUntilMs - nowMs) > 0) {
      pwmUpdate(0.0F);
      return 0.0F;
    }

    const float dMin  = pwmMinimum.value * 0.01F;
    const float dMax0 = pwmMaximum.value * 0.01F;
    const float dMax  = (dMax0 >= dMin) ? dMax0 : dMin;
    const float dSpan = dMax - dMin;

    float vcmd01 = vmag * InvVelocityMax;
    if (vcmd01 > 1.0F) vcmd01 = 1.0F;

    // feedforward-only: uSat = vcmd01
    const float uSat = vcmd01;

    float dutyMag01 = 0.0F;
    if (uSat > 0.0F) {
      dutyMag01 = dMin + dSpan * uSat;
      if (dutyMag01 > dMax) dutyMag01 = dMax;
    }

    pwmUpdate(dutyMag01 * (float)dir);
    return velocity;
  }

#endif

#endif
