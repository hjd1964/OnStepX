// -----------------------------------------------------------------------------------
// I2C TCA9555 GPIO support

#include "Tca9555.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == X9555

#ifndef GPIO_TCA9555_I2C_ADDRESS
  #define GPIO_TCA9555_I2C_ADDRESS 0x27
#endif

#include <TCA9555.h> // https://www.arduino.cc/reference/en/libraries/tca9555/

TCA9555 tca(GPIO_TCA9555_I2C_ADDRESS, &HAL_WIRE); // might need to change this I2C Address?

// check for TCA9555 device on the I2C bus
bool GpioTca9555::init() {
  static bool initialized = false;
  if (initialized) return found;

  if (tca.begin()) {
    found = true;
    for (int i = 0; i < 16; i++) { tca.pinMode1(i, INPUT); }
  } else { found = false; DLF("WRN: Gpio.init(), TCA9555 (I2C 0x"); if (DEBUG != OFF) SERIAL_DEBUG.print(GPIO_TCA9555_I2C_ADDRESS, HEX); DLF(") not found"); }
  HAL_WIRE_SET_CLOCK();

  return found;
}

// set GPIO pin (0 to 15) mode for INPUT, INPUT_PULLUP, or OUTPUT
void GpioTca9555::pinMode(int pin, int mode) {
  if (found && pin >= 0 && pin <= 15) {
    #ifdef INPUT_PULLDOWN
      if (mode == INPUT_PULLDOWN) mode = INPUT;
    #endif
    if (mode == INPUT_PULLUP) mode = INPUT;
    tca.pinMode1(pin, mode);
    this->mode[pin] = mode;
  }
}

// one sixteen channel Tca9555 GPIO is supported, this gets the last set value
int GpioTca9555::digitalRead(int pin) {
  if (found && pin >= 0 && pin <= 15) {
    if (mode[pin] == INPUT || mode[pin] == INPUT_PULLUP) {
      return tca.read1(pin);
    } else return state[pin]; 
  } else return 0;
}

// one sixteen channel Tca9555 GPIO is supported, this sets each output on or off
void GpioTca9555::digitalWrite(int pin, int value) {
  if (found && pin >= 0 && pin <= 15) {
    state[pin] = value;
    if (mode[pin] == OUTPUT) {
      tca.write1(pin, value);
    } else {
      if (value == HIGH) tca.pinMode1(pin, INPUT_PULLUP); else tca.pinMode1(pin, INPUT);
    }
  } else return;
}

GpioTca9555 gpio;

#endif
