// -----------------------------------------------------------------------------------
// axis servo DC EE motor driver

#include "EE.h"

#ifdef SERVO_EE_PRESENT

#include "../../../../../gpioEx/GpioEx.h"
#include "../../../../../analog/Analog.h"

ServoEE::ServoEE(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings, float pwmMinimum, float pwmMaximum)
                 :ServoDcDriver(axisNumber, Pins, Settings, pwmMinimum, pwmMaximum) {
  if (axisNumber < 1 || axisNumber > 9) return;

  strcpy(axisPrefix, " Axis_ServoEE, ");
  axisPrefix[5] = '0' + axisNumber;
}

bool ServoEE::init(bool reverse) {
if (!ServoDcDriver::init(reverse)) return false;

  #if DEBUG == VERBOSE
    VF("MSG:"); V(axisPrefix);
    V("pins pwm1="); if (Pins->ph1 == OFF) VF("OFF"); else V(Pins->ph1);
    V(", pwm2="); if (Pins->ph2 == OFF) VLF("OFF"); else VL(Pins->ph2);
  #endif

  // init default driver control pins
  pinModeEx(Pins->ph1, OUTPUT);
  digitalWriteF(Pins->ph1, Pins->ph1State); // either ph1 or direction, state should default to inactive
  pinModeEx(Pins->ph2, OUTPUT);
  digitalWriteF(Pins->ph2, Pins->ph2State); // either ph2 or step (PWM,) state should default to inactive

  // set PWM range & frequency
  AnalogPwmConfig scfg{};
  #ifdef SERVO_ANALOG_WRITE_FREQUENCY
    scfg.hz = SERVO_ANALOG_WRITE_FREQUENCY;
  #endif
  #ifdef SERVO_ANALOG_WRITE_RANGE
    scfg.range = SERVO_ANALOG_WRITE_RANGE;
  #endif
  if (!analog.pwmInit(Pins->ph1, scfg)) return false;
  if (!analog.pwmInit(Pins->ph2, scfg)) return false;

  return true;
}

// enable or disable the driver using the enable pin or other method
void ServoEE::enable(bool state) {
  ServoDriver::enable(state);

  VF("MSG:"); V(axisPrefix);
  if (!enabled) {
    VF("EE outputs off");

    // force both outputs inactive
    analog.write(Pins->ph1, off1());
    analog.write(Pins->ph2, off2());

    VLF("");
  } else {
    VLF("EE enabled");
  }

  ServoDriver::updateStatus();
}

void ServoEE::pwmUpdate(float power01) {
  if (!enabled) return;

  if (reversed) power01 = -power01;
  float duty01 = fabsf(power01);

  if (power01 >= 0.0F) {
    if (Pins->ph2State == HIGH) duty01 = 1.0F - duty01;
    analog.write(Pins->ph1, off1());
    analog.write(Pins->ph2, duty01);
  } else {
    if (Pins->ph1State == HIGH) duty01 = 1.0F - duty01;
    analog.write(Pins->ph1, duty01);
    analog.write(Pins->ph2, off2());
  }
}

// update status info. for driver
void ServoEE::updateStatus() {
  if (statusMode == LOW || statusMode == HIGH) {
    status.fault = digitalReadEx(faultPin) == statusMode;
  }
}

#endif
