// -----------------------------------------------------------------------------------
// I2C MCP23008 GPIO support

#include "Mcp23008.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == MCP23008

#ifndef GPIO_MCP23008_I2C_ADDRESS
  #define GPIO_MCP23008_I2C_ADDRESS 0x20
#endif

// needs: https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library and https://github.com/adafruit/Adafruit_BusIO
#include "Adafruit_MCP23X08.h"
Adafruit_MCP23X08 mcp;

// check for MCP23008 device on the I2C bus
bool GpioMcp23008::init() {
  static bool initialized = false;
  if (initialized) return found;

  if (mcp.begin_I2C(GPIO_MCP23008_I2C_ADDRESS, &HAL_WIRE)) {
    found = true;
    for (int i = 0; i < 8; i++) { mcp.pinMode(i, INPUT); }
  } else { found = false; DF("WRN: Gpio.init(), MCP23008 (I2C 0x"); if (DEBUG != OFF) SERIAL_DEBUG.print(GPIO_MCP23008_I2C_ADDRESS, HEX); DLF(") not found"); }
  HAL_WIRE_SET_CLOCK();
  
  return found;
}

// set GPIO pin (0 to 7) mode for INPUT, INPUT_PULLUP, or OUTPUT
void GpioMcp23008::pinMode(int pin, int mode) {
  if (found && pin >= 0 && pin <= 7) {
    #ifdef INPUT_PULLDOWN
      if (mode == INPUT_PULLDOWN) mode = INPUT;
    #endif
    mcp.pinMode(pin, mode);
    this->mode[pin] = mode;
  }
}

// one eight channel MCP23008 GPIO is supported, this gets the last set value
int GpioMcp23008::digitalRead(int pin) {
  if (found && pin >= 0 && pin <= 7) {
    if (mode[pin] == INPUT || mode[pin] == INPUT_PULLUP) {
      return mcp.digitalRead(pin);
    } else return state[pin]; 
  } else return 0;
}

// one eight channel MCP23008 GPIO is supported, this sets each output on or off
void GpioMcp23008::digitalWrite(int pin, int value) {
  if (found && pin >= 0 && pin <= 7) {
    state[pin] = value;
    if (mode[pin] == OUTPUT) {
      mcp.digitalWrite(pin, value);
    } else {
      if (value == HIGH) pinMode(pin, INPUT_PULLUP); else pinMode(pin, INPUT);
    }
  } else return;
}

GpioMcp23008 gpio;

#endif
