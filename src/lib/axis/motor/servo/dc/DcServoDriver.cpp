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
  // initialize caches up front
  recomputeScalingIfNeeded();

  #ifdef SERVO_HYSTERESIS_ENABLE
    VF("MSG:"); V(axisPrefix); VF("Hysteresis Enabled ENTER="); V((float)SERVO_HYST_ENTER_CPS);
    VF(" cps, EXIT="); V((float)SERVO_HYST_EXIT_CPS); VLF(" cps");
  #else
    VF("MSG:"); V(axisPrefix); VLF("Hysteresis Disabled");
  #endif

  #ifdef SERVO_SIGMA_DELTA_DITHERING
    VF("MSG:"); V(axisPrefix); VLF("Sigma-Delta dither: enabled");
  #else
    VF("MSG:"); V(axisPrefix); VLF("Sigma-Delta dither: disabled");
  #endif
}

float ServoDcDriver::setMotorVelocity(float velocity)  {
  velocity = ServoDriver::setMotorVelocity(velocity);

  pwmUpdate(labs(toAnalogRange(velocity)));

  return velocity;
}

#endif
