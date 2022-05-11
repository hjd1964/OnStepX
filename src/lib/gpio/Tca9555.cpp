// -----------------------------------------------------------------------------------
// I2C TCA9555 GPIO support

#include "Tca9555.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == X9555

#include "../tasks/OnTask.h"

#include <TCA9555.h>
TCA9555 tca(0x27, &HAL_Wire); // might need to change this I2C Address?

// check for TCA9555 device on the I2C bus
bool Tca9555::init() {
  static bool initialized = false;
  if (initialized) return found;

  if (tca.begin()) {
    found = true;
    for (int i = 0; i < 16; i++) { tca.pinMode(i, INPUT); }
  } else { found = false; DLF("WRN: Gpio.init(), Tca9555 (I2C 0x27) not found"); }

  return found;
}

// set GPIO pin (0 to 15) mode for INPUT, INPUT_PULLUP, or OUTPUT
void Tca9555::pinMode(int pin, int mode) {
  if (found && pin >= 0 && pin <= 15) {
    #ifdef INPUT_PULLDOWN
      if (mode == INPUT_PULLDOWN) mode = INPUT;
    #endif
    if (mode == INPUT_PULLUP) mode = INPUT;
    tca.pinMode(pin, mode);
    this->mode[pin] = mode;
  }
}

// one sixteen channel Tca9555 GPIO is supported, this gets the last set value
// index 0 to 15 are auxiliary features #1, #2, etc.
int Tca9555::digitalRead(int pin) {
  if (found && pin >= 0 && pin <= 15) {
    if (mode[pin] == INPUT || mode[pin] == INPUT_PULLUP) {
      return tca.digitalRead(pin);
    } else return state[pin]; 
  } else return 0;
}

// one sixteen channel Tca9555 GPIO is supported, this sets each output on or off
// index 0 to 15 are auxiliary features #1, #2, etc.
void Tca9555::digitalWrite(int pin, bool value) {
  if (found && pin >= 0 && pin <= 15) {
    state[pin] = value;
    if (mode[pin] == OUTPUT) {
      tca.digitalWrite(pin, value);
    } else {
      if (value == HIGH) pinMode(pin, INPUT_PULLUP); else pinMode(pin, INPUT);
    }
  } else return;
}

Tca9555 gpio;

#endif
