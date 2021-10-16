// -----------------------------------------------------------------------------------
// I2C MCP23017 GPIO support

#include "Mcp23017.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == MCP23017

#include "../tasks/OnTask.h"

// needs: https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library and https://github.com/adafruit/Adafruit_BusIO
#include "Adafruit_MCP23X17.h"
Adafruit_MCP23X17 mcp;

// check for MCP23017 device on the I2C bus
bool Mcp23017::init() {
  static bool initialized = false;
  if (initialized) return found;

  if (mcp.begin_I2C(0x20, &HAL_Wire)) {
    found = true;
    for (int i = 0; i < 16; i++) { mcp.pinMode(i, INPUT); }
  } else { found = false; DLF("WRN: Gpio.init(), Mcp23017 (I2C 0x20) not found"); }

  return found;
}

// set GPIO pin (0 to 15) mode for INPUT, INPUT_PULLUP, or OUTPUT
void Mcp23017::pinMode(int pin, int mode) {
  if (found && pin >= 0 && pin <= 16) {
    #ifdef INPUT_PULLDOWN
      if (mode == INPUT_PULLDOWN) mode = INPUT;
    #endif
    mcp.pinMode(pin, mode);
    this->mode[pin] = mode;
  }
}

// one sixteen channel MCP23017 GPIO is supported, this gets the last set value
// index 0 to 15 are auxiliary features #1, #2, etc.
int Mcp23017::digitalRead(int pin) {
  if (found && pin >= 0 && pin <= 15) {
    if (mode[pin] == INPUT || mode[pin] == INPUT_PULLUP) {
      return mcp.digitalRead(pin);
    } else return state[pin]; 
  } else return 0;
}

// one sixteen channel MCP23017 GPIO is supported, this sets each output on or off
// index 0 to 15 are auxiliary features #1, #2, etc.
void Mcp23017::digitalWrite(int pin, bool value) {
  if (found && pin >= 0 && pin <= 15) {
    state[pin] = value;
    if (mode[pin] == OUTPUT) {
      mcp.digitalWrite(pin, value);
    } else {
      if (value == HIGH) pinMode(pin, INPUT_PULLUP); else pinMode(pin, INPUT);
    }
  } else return;
}

Mcp23017 gpio;

#endif
