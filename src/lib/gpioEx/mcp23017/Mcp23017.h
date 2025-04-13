// -----------------------------------------------------------------------------------
// I2C MCP23017 GPIO support
#pragma once

#include "../GpioBase.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == MCP23017

class GpioMcp23017 : public Gpio {
  public:
    // scan for MCP23017 device
    bool init();

    void pinMode(int pin, int mode);

    // one sixteen channel MCP23017 GPIO is supported, this gets the last set value
    int digitalRead(int pin);

    // one sixteen channel MCP23017 GPIO is supported, this sets each output on or off
    void digitalWrite(int pin, int value);

  private:
    bool found = false;

    int mode[16] = { INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT };
    bool state[16] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
};

extern GpioMcp23017 gpio;

#endif
