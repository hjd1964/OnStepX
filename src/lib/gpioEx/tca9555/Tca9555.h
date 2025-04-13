// -----------------------------------------------------------------------------------
// I2C TCA9555 GPIO support
#pragma once

#include "../GpioBase.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == X9555

class GpioTca9555 : public Gpio {
  public:
    // scan for TCA9555 device
    bool init();

    void pinMode(int pin, int mode);

    // one sixteen channel TCA9555 GPIO is supported, this gets the last set value
    int digitalRead(int pin);

    // one sixteen channel TCA9555 GPIO is supported, this sets each output on or off
    void digitalWrite(int pin, int value);

  private:
    bool found = false;

    int mode[16] = { INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT };
    bool state[16] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
};

extern GpioTca9555 gpio;

#endif
