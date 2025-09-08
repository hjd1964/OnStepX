// -----------------------------------------------------------------------------------
// axis servo DC motor driver

#include "DcServoDriver.h"

#if defined(SERVO_PE_PRESENT) || defined(SERVO_EE_PRESENT) || defined(SERVO_TMC2130_DC_PRESENT) || defined(SERVO_TMC5160_DC_PRESENT)

#include "../../../../gpioEx/GpioEx.h"

ServoDcDriver::ServoDcDriver(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings)
                            :ServoDriver(axisNumber, Pins, Settings) {
  if (axisNumber < 1 || axisNumber > 9) return;

  switch (axisNumber) {
    case 1:
      pwmMinimum.valueDefault = AXIS1_ANALOG_WRITE_MIN;
      pwmMaximum.valueDefault = AXIS1_ANALOG_WRITE_MAX;
  break;
    case 2:
      pwmMinimum.valueDefault = AXIS2_ANALOG_WRITE_MIN;
      pwmMaximum.valueDefault = AXIS2_ANALOG_WRITE_MAX;
    break;
    case 3:
      pwmMinimum.valueDefault = AXIS3_ANALOG_WRITE_MIN;
      pwmMaximum.valueDefault = AXIS3_ANALOG_WRITE_MAX;
    break;
    case 4:
      pwmMinimum.valueDefault = AXIS4_ANALOG_WRITE_MIN;
      pwmMaximum.valueDefault = AXIS4_ANALOG_WRITE_MAX;
    break;
    case 5:
      pwmMinimum.valueDefault = AXIS5_ANALOG_WRITE_MIN;
      pwmMaximum.valueDefault = AXIS5_ANALOG_WRITE_MAX;
    break;
    case 6:
      pwmMinimum.valueDefault = AXIS6_ANALOG_WRITE_MIN;
      pwmMaximum.valueDefault = AXIS6_ANALOG_WRITE_MAX;
    break;
    case 7:
      pwmMinimum.valueDefault = AXIS7_ANALOG_WRITE_MIN;
      pwmMaximum.valueDefault = AXIS7_ANALOG_WRITE_MAX;
    break;
    case 8:
      pwmMinimum.valueDefault = AXIS8_ANALOG_WRITE_MIN;
      pwmMaximum.valueDefault = AXIS8_ANALOG_WRITE_MAX;
    break;
    case 9:
      pwmMinimum.valueDefault = AXIS9_ANALOG_WRITE_MIN;
      pwmMaximum.valueDefault = AXIS9_ANALOG_WRITE_MAX;
    break;
  }
}

float ServoDcDriver::setMotorVelocity(float velocity)  {
  if (!enabled) velocity = 0.0F;

  if (velocity > velocityMax) velocity = velocityMax; else
  if (velocity < -velocityMax) velocity = -velocityMax;

  if (velocity > velocityRamp) {
    velocityRamp += accelerationFs;
    if (velocityRamp > velocity) velocityRamp = velocity;
  } else
  if (velocity < velocityRamp) {
    velocityRamp -= accelerationFs;
    if (velocityRamp < velocity) velocityRamp = velocity;
  }
  if (velocityRamp >= 0) motorDirection = DIR_FORWARD; else motorDirection = DIR_REVERSE;

  pwmUpdate(fabs(toAnalogRange(velocityRamp)));

  return velocityRamp;
}

#endif
