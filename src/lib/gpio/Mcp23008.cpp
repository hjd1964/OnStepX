// -----------------------------------------------------------------------------------
// I2C MCP23008 GPIO support

#include "Mcp23008.h"

#if GPIO_DEVICE == MCP23008

#include "../../tasks/OnTask.h"
extern Tasks tasks;

// needs: https://github.com/adafruit/Adafruit-MCP23008-library and https://github.com/adafruit/Adafruit_BusIO
#include "Adafruit_MCP23008.h"
Adafruit_MCP23008 mcp;

// check for MCP23008 device on the I2C bus
bool Mcp23008::init() {
  static bool initialized = false;
  if (initialized) return found;

  if (mcp.begin(0x20, &HAL_Wire)) {
    found = true;
    for (int i = 0; i < 8; i++) { mcp.pinMode(i, INPUT); }
  } else found = false;

  return found;
}

// set GPIO pin (0 to 7) mode for INPUT, INPUT_PULLUP, or OUTPUT
void Mcp23008::pinMode(int pin, int mode) {
  if (found && pin >= 0 && pin <= 7) {
    this->mode[pin] = mode;
    if (mode == INPUT_PULLUP) this->mode[pin] = INPUT;
    #ifdef INPUT_PULLDOWN
      if (mode == INPUT_PULLDOWN) this->mode[pin] = INPUT;
    #endif
    mcp.pinMode(pin, mode);
    if (mode == INPUT_PULLUP) mcp.pullUp(pin, HIGH); else mcp.pullUp(pin, LOW);
  }
}

// one eight channel MCP23008 GPIO is supported, this gets the last set value
// index 0 to 7 are auxiliary features #1, #2, etc.
int Mcp23008::digitalRead(int pin) {
  if (found && pin >= 0 && pin <= 7) {
    if (mode[pin] == INPUT) {
      return mcp.digitalRead(pin);
    } else return state[pin]; 
  } else return 0;
}

// one eight channel MCP23008 GPIO is supported, this sets each output on or off
// index 0 to 7 are auxiliary features #1, #2, etc.
void Mcp23008::digitalWrite(int pin, bool value) {
  if (found && pin >= 0 && pin <= 7) {
    state[pin] = value;
    if (mode[pin] == OUTPUT) mcp.digitalWrite(pin, value); else mcp.pullUp(pin, value);
  } else return;
}

Mcp23008 gpio;

#endif
