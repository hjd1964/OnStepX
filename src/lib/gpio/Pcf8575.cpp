// -----------------------------------------------------------------------------------
// I2C PCF8575 GPIO support

#include "Pcf8575.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == X8575

#ifndef GPIO_PCF8575_I2C_ADDRESS
  #define GPIO_PCF8575_I2C_ADDRESS 0x20
#endif

#include "../tasks/OnTask.h"

#include <PCF8575.h> // https://www.arduino.cc/reference/en/libraries/pcf8575/

PCF8575 pcf(GPIO_PCF8575_I2C_ADDRESS, &HAL_Wire); // might need to change this I2C Address?

// check for PCF8575 device on the I2C bus
bool Pcf8575::init() {
  static bool initialized = false;
  if (initialized) return found;

  if (pcf.begin()) {
    found = true;
  } else { found = false; DF("WRN: Gpio.init(), PCF8575 (I2C 0x"); if (DEBUG != OFF) SERIAL_DEBUG.print(GPIO_PCF8575_I2C_ADDRESS, HEX); DLF(") not found"); }
  #ifdef HAL_WIRE_CLOCK
    HAL_Wire.setClock(HAL_WIRE_CLOCK);
  #endif

  return found;
}

// no command processing
bool Pcf8575::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  UNUSED(reply);
  UNUSED(command);
  UNUSED(parameter);
  UNUSED(supressFrame);
  UNUSED(numericReply);
  UNUSED(commandError);
  return false;
}

// set GPIO pin (0 to 15) mode for INPUT, INPUT_PULLUP, or OUTPUT
void Pcf8575::pinMode(int pin, int mode) {
  if (found && pin >= 0 && pin <= 15) {
    #ifdef INPUT_PULLDOWN
      if (mode == INPUT_PULLDOWN) mode = INPUT;
    #endif
    if (mode == INPUT_PULLUP) mode = INPUT;
    // no pinMode() seems to exist for the PCF8575, I assume reading sets input mode and writing sets output mode automatically
    this->mode[pin] = mode;
  }
}

// one sixteen channel Pcf8575 GPIO is supported, this gets the last set value
int Pcf8575::digitalRead(int pin) {
  if (found && pin >= 0 && pin <= 15) {
    if (mode[pin] == INPUT || mode[pin] == INPUT_PULLUP) {
      return pcf.read(pin);
    } else return state[pin];
  } else return 0;
}

// one sixteen channel Pcf8575 GPIO is supported, this sets each output on or off
void Pcf8575::digitalWrite(int pin, bool value) {
  if (found && pin >= 0 && pin <= 15) {
    state[pin] = value;
    if (mode[pin] == OUTPUT) {
      pcf.write(pin, value);
    } else {
      if (value == HIGH) pinMode(pin, INPUT_PULLUP); else pinMode(pin, INPUT);
    }
  } else return;
}

Pcf8575 gpio;

#endif
