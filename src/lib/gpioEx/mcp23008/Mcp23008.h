// -----------------------------------------------------------------------------------
// I2C MCP23008 GPIO support
#pragma once

#include "../GpioBase.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == MCP23008

class GpioMcp23008 : public Gpio {
  public:
    // scan for MCP23008 device
    bool init();

    void pinMode(int pin, int mode);

    // one eight channel MCP23008 GPIO is supported, this gets the last set value
    int digitalRead(int pin);

    // one eight channel MCP23008 GPIO is supported, this sets each output on or off
    void digitalWrite(int pin, int value);

  private:
    bool found = false;

    int mode[8] = { INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT };
    bool state[8] = { false, false, false, false, false, false, false, false };
};

extern GpioMcp23008 gpio;

#endif
