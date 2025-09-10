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
}

float ServoDcDriver::setMotorVelocity(float velocity)  {
  velocity = ServoDriver::setMotorVelocity(velocity);

  pwmUpdate(fabs(toAnalogRange(velocity)));

  return velocity;
}

#endif
