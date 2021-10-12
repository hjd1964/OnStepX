// -----------------------------------------------------------------------------------
// standard external GPIO library

#include "Gpio.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == OFF

bool Gpio::init() {
  return true;
}

void Gpio::pinMode(int pin, int mode) {
  pin = pin;
  mode = mode;
}

int Gpio::digitalRead(int pin) {
  pin = pin;
  return 0;
}

void Gpio::digitalWrite(int pin, int value) {
  pin = pin;
  value = value;
}

Gpio gpio;

#endif
