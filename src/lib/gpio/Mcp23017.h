// -----------------------------------------------------------------------------------
// I2C MCP23017 GPIO support
#pragma once

#include "../../Common.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == MCP23017

class Mcp23017 {
  public:
    // scan for MCP23017 device on the 1-wire bus
    bool init();

    void pinMode(int pin, int mode);

    // one sixteen channel MCP23017 GPIO is supported, this gets the last set value
    // index 0 to 15 are auxiliary features #1, #2, etc.
    int digitalRead(int pin);

    // one sixteen channel MCP23017 GPIO is supported, this sets each output on or off
    // index 0 to 15 are auxiliary features #1, #2, etc.
    void digitalWrite(int pin, bool value);

  private:
    bool found = false;

    int mode[16] = { INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT };
    bool state[16] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
};

extern Mcp23017 gpio;

#endif
