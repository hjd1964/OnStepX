// -----------------------------------------------------------------------------------
// I2C TCA9555 GPIO support
#pragma once

#include "../../Common.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == X9555

class Tca9555 {
  public:
    // scan for TCA9555 device on the 1-wire bus
    bool init();

    void pinMode(int pin, int mode);

    // one sixteen channel TCA9555 GPIO is supported, this gets the last set value
    // index 0 to 15 are auxiliary features #1, #2, etc.
    int digitalRead(int pin);

    // one sixteen channel TCA9555 GPIO is supported, this sets each output on or off
    // index 0 to 15 are auxiliary features #1, #2, etc.
    void digitalWrite(int pin, bool value);

  private:
    bool found = false;

    int mode[16] = { INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT };
    bool state[16] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
};

extern Tca9555 gpio;

#endif
