// -----------------------------------------------------------------------------------
// axis servo DC EE motor driver

#include "EE.h"

#ifdef SERVO_EE_PRESENT

#include "../../../../../gpioEx/GpioEx.h"

ServoEE::ServoEE(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings, float pwmMinimum, float pwmMaximum)
                 :ServoDcDriver(axisNumber, Pins, Settings, pwmMinimum, pwmMaximum) {
  if (axisNumber < 1 || axisNumber > 9) return;

  strcpy(axisPrefix, " Axis_ServoEE, ");
  axisPrefix[5] = '0' + axisNumber;
}

bool ServoEE::init(bool reverse) {
  if (!ServoDriver::init(reverse)) return false;

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

  // set PWM frequency
  #ifdef SERVO_ANALOG_WRITE_FREQUENCY
    VF("MSG:"); V(axisPrefix); VF("setting control pins analog frequency "); VL(SERVO_ANALOG_WRITE_FREQUENCY);
    analogWriteFrequency(Pins->ph2, SERVO_ANALOG_WRITE_FREQUENCY);
  #endif

  // set PWM bits
  #ifdef SERVO_ANALOG_WRITE_RESOLUTION
    VF("MSG:"); V(axisPrefix); VF("setting control pins analog bits "); VL(SERVO_ANALOG_WRITE_RESOLUTION);
    analogWriteResolution(Pins->ph2, SERVO_ANALOG_WRITE_RESOLUTION);
  #endif

  return true;
}

// enable or disable the driver using the enable pin or other method
void ServoEE::enable(bool state) {
  enabled = state;

  VF("MSG:"); V(axisPrefix);
  if (!enabled) {
    VF("EE outputs off");
    if (Pins->ph1State == HIGH) analogWrite(Pins->ph1, SERVO_ANALOG_WRITE_RANGE); else analogWrite(Pins->ph1, 0);
    if (Pins->ph2State == HIGH) analogWrite(Pins->ph2, SERVO_ANALOG_WRITE_RANGE); else analogWrite(Pins->ph2, 0);

    if (enablePin != SHARED) {
      VF(" and powered down using enable pin");
      digitalWriteF(enablePin, !enabledState);
    }
    VLF("");
  } else {
    if (enablePin != SHARED) {
      VLF("powered up using enable pin");
      digitalWriteF(enablePin, enabledState);
    }
  }

  velocityRamp = 0.0F;

  ServoDriver::updateStatus();
}

void ServoEE::pwmUpdate(long power) {
  if (!enabled) return;

  if (motorDirection == (reversed ? DIR_REVERSE : DIR_FORWARD)) {
    if (Pins->ph1State == HIGH) analogWrite(Pins->ph1, SERVO_ANALOG_WRITE_RANGE); else analogWrite(Pins->ph1, 0);
    if (Pins->ph2State == HIGH) power = SERVO_ANALOG_WRITE_RANGE - power;
    analogWrite(Pins->ph2, power);
  } else
  if (motorDirection == (reversed ? DIR_FORWARD : DIR_REVERSE)) {
    if (Pins->ph1State == HIGH) power = SERVO_ANALOG_WRITE_RANGE - power;
    analogWrite(Pins->ph1, power);
    if (Pins->ph2State == HIGH) analogWrite(Pins->ph2, SERVO_ANALOG_WRITE_RANGE); else analogWrite(Pins->ph2, 0);
  } else {
    if (Pins->ph1State == HIGH) analogWrite(Pins->ph1, SERVO_ANALOG_WRITE_RANGE); else analogWrite(Pins->ph1, 0);
    if (Pins->ph2State == HIGH) analogWrite(Pins->ph2, SERVO_ANALOG_WRITE_RANGE); else analogWrite(Pins->ph2, 0);
  }
}

// update status info. for driver
void ServoDc::updateStatus() {
  if (statusMode == LOW || statusMode == HIGH) {
    status.fault = digitalReadEx(faultPin) == statusMode;
  }
}

#endif