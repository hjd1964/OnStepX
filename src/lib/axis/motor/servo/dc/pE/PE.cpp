// -----------------------------------------------------------------------------------
// axis servo DC PE motor driver

#include "PE.h"

#ifdef SERVO_PE_PRESENT

// for a Teensy4.1 you can have one additional PWM channel on a pin (for example pin 38)
// for example in Config.h:
//
// #define PWM_CUSTOM_PIN 38
// #define PWM_CUSTOM_ANALOG_WRITE_RANGE 4095
// #define PWM_CUSTOM_ANALOG_WRITE_FREQUENCY 10000
//
// then use the AXISn_SERVO_PHn_PIN as you normally would (AXIS1_SERVO_PH2_PIN 38, for example)

#include "../../../../../gpioEx/GpioEx.h"
#include "../../../../../analog/Analog.h"

ServoPE::ServoPE(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings, float pwmMinimum, float pwmMaximum)
                 :ServoDcDriver(axisNumber, Pins, Settings, pwmMinimum, pwmMaximum) {
  if (axisNumber < 1 || axisNumber > 9) return;

  strcpy(axisPrefix, " Axis_ServoPE, ");
  axisPrefix[5] = '0' + axisNumber;
}

bool ServoPE::init(bool reverse) {
  if (!ServoDcDriver::init(reverse)) return false;

  #if DEBUG == VERBOSE
    VF("MSG:"); V(axisPrefix);
    V("pins dir="); if (Pins->ph1 == OFF) VF("OFF"); else V(Pins->ph1);
    V(", pwm="); if (Pins->ph2 == OFF) VLF("OFF"); else VL(Pins->ph2);
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
  if (!analog.pwmInit(Pins->ph2, scfg)) return false;

  return true;
}

// enable or disable the driver using the enable pin or other method
void ServoPE::enable(bool state) {
  ServoDriver::enable(state);

  VF("MSG:"); V(axisPrefix);
  if (!enabled) {
    VF("PE outputs off");

    // force outputs inactive
    digitalWriteF(Pins->ph1, Pins->ph1State);
    analog.write(Pins->ph2, off2());

    VLF("");
  } else {
    VLF("PE enabled");
  }

  ServoDriver::updateStatus();
}

void ServoPE::pwmUpdate(float power01) {
  if (!enabled) return;

  if (reversed) power01 = -power01;
  float duty01 = fabsf(power01);

  digitalWriteF(Pins->ph1, power01 >= 0.0F ? Pins->ph1State : !Pins->ph1State);

  if (Pins->ph2State == HIGH) duty01 = 1.0F - duty01;
  analog.write(Pins->ph2, duty01);
}

// update status info. for driver
void ServoPE::updateStatus() {
  if (statusMode == LOW || statusMode == HIGH) {
    status.fault = digitalReadEx(faultPin) == statusMode;
  }
}

#endif
