// -----------------------------------------------------------------------------------
// I2C MCP23008 GPIO support
#pragma once

#include "../../Common.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == MCP23008

class Mcp23008 {
  public:
    // scan for MCP23008 device on the 1-wire bus
    bool init();

    void pinMode(int pin, int mode);

    // one eight channel MCP23008 GPIO is supported, this gets the last set value
    // index 0 to 7 are auxiliary features #1, #2, etc.
    int digitalRead(int pin);

    // one eight channel MCP23008 GPIO is supported, this sets each output on or off
    // index 0 to 7 are auxiliary features #1, #2, etc.
    void digitalWrite(int pin, bool value);

  private:
    bool found = false;

    int mode[8] = { INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT };
    bool state[8] = { false, false, false, false, false, false, false, false };
};

extern Mcp23008 gpio;

#endif
