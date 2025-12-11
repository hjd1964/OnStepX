// -----------------------------------------------------------------------------------
// axis servo DC motor driver

#include "DcServoDriver.h"

#if defined(SERVO_PE_PRESENT) || defined(SERVO_EE_PRESENT) || defined(SERVO_TMC2130_DC_PRESENT) || defined(SERVO_TMC5160_DC_PRESENT)

#include "../../../../gpioEx/GpioEx.h"

ServoDcDriver::ServoDcDriver(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings, float pwmMinimum, float pwmMaximum)
                            :ServoDriver(axisNumber, Pins, Settings) {
  if (axisNumber < 1 || axisNumber > 9) return;

  this->pwmMinimum.valueDefault = pwmMinimum;
  this->pwmMaximum.valueDefault = pwmMaximum;
  dirDeadTimeMs.valueDefault = 0.0F;
}

float ServoDcDriver::setMotorVelocity(float velocity) {
  velocity = ServoDriver::setMotorVelocity(velocity);

  // Define "stop" near 0 so we don't treat 0 as DIR_FORWARD
  const float stoppedVelocity = 0.001F;
  int8_t effectiveDirection = 0;
  if (velocity > stoppedVelocity) effectiveDirection = 1;
  if (velocity < -stoppedVelocity) effectiveDirection = -1;

  // Start holdoff only on real + <-> - transitions (ignore stop)
  const unsigned long now = millis();
  if (effectiveDirection != 0 && lastEffectiveDirection != 0 && effectiveDirection != lastEffectiveDirection) {
    const float dtf = dirDeadTimeMs.value;
    const unsigned long dtMs = (dtf > 0.0F) ? (unsigned long)lroundf(dtf) : 0U;
    if (dtMs) dirHoldUntilMs = now + dtMs;
  }
  if (effectiveDirection != 0) lastEffectiveDirection = effectiveDirection;

  // During holdoff: PWM off (so velocityPercent reflects reality)
  if ((long)(dirHoldUntilMs - now) > 0) {
    pwmUpdate(0.0F);
    return 0.0F;
  }

  // Normal PWM
  pwmUpdate(toDuty01(velocity));
  return velocity;
}

#endif
