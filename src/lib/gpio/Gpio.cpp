// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire ds2413 device support

#include "Gpio.h"

#ifdef GPIO_PRESENT

bool Gpio::init() {
  return true;
}

int Gpio::getChannel(int index) {
  index = index;
  return 0;
}

void Gpio::setChannel(int index, bool state) {
  index = index;
  state = state;
}

bool Gpio::failure(int index) {
  return true;
}

Gpio gpio;

#endif
