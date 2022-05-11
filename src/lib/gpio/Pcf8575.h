// -----------------------------------------------------------------------------------
// I2C PCF8575 GPIO support
#pragma once

#include "../../Common.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == X8575

class Pcf8575 {
  public:
    // scan for PCF8575 device on the 1-wire bus
    bool init();

    void pinMode(int pin, int mode);

    // one sixteen channel PCF8575 GPIO is supported, this gets the last set value
    int digitalRead(int pin);

    // one sixteen channel PCF8575 GPIO is supported, this sets each output on or off
    void digitalWrite(int pin, bool value);

  private:
    bool found = false;

    int mode[16] = { INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT };
    bool state[16] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
};

extern Pcf8575 gpio;

#endif
