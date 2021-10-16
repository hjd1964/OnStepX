// -----------------------------------------------------------------------------------
// I2C MCP23008 GPIO support

#include "Mcp23008.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == MCP23008

#include "../tasks/OnTask.h"

// needs: https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library and https://github.com/adafruit/Adafruit_BusIO
#include "Adafruit_MCP23X08.h"
Adafruit_MCP23X08 mcp;

// check for MCP23008 device on the I2C bus
bool Mcp23008::init() {
  static bool initialized = false;
  if (initialized) return found;

  if (mcp.begin_I2C(0x20, &HAL_Wire)) {
    found = true;
    for (int i = 0; i < 8; i++) { mcp.pinMode(i, INPUT); }
  } else { found = false; DLF("WRN: Gpio.init(), Mcp23008 (I2C 0x20) not found"); }
  return found;
}

// set GPIO pin (0 to 7) mode for INPUT, INPUT_PULLUP, or OUTPUT
void Mcp23008::pinMode(int pin, int mode) {
  if (found && pin >= 0 && pin <= 7) {
    #ifdef INPUT_PULLDOWN
      if (mode == INPUT_PULLDOWN) mode = INPUT;
    #endif
    mcp.pinMode(pin, mode);
    this->mode[pin] = mode;
  }
}

// one eight channel MCP23008 GPIO is supported, this gets the last set value
// index 0 to 7 are auxiliary features #1, #2, etc.
int Mcp23008::digitalRead(int pin) {
  if (found && pin >= 0 && pin <= 7) {
    if (mode[pin] == INPUT || mode[pin] == INPUT_PULLUP) {
      return mcp.digitalRead(pin);
    } else return state[pin]; 
  } else return 0;
}

// one eight channel MCP23008 GPIO is supported, this sets each output on or off
// index 0 to 7 are auxiliary features #1, #2, etc.
void Mcp23008::digitalWrite(int pin, bool value) {
  if (found && pin >= 0 && pin <= 7) {
    state[pin] = value;
    if (mode[pin] == OUTPUT) {
      mcp.digitalWrite(pin, value);
    } else {
      if (value == HIGH) pinMode(pin, INPUT_PULLUP); else pinMode(pin, INPUT);
    }
  } else return;
}

Mcp23008 gpio;

#endif
