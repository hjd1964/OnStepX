// -----------------------------------------------------------------------------------
// I2C PCF8574 GPIO support
#pragma once

#include "../GpioBase.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == X8574

#include <PCF8574.h> // https://github.com/xreef/PCF8574_library/tree/master

class GpioPcf8574 : public Gpio {
  public:
    // scan for PCF8575 device
    bool init();

    void pinMode(int pin, int mode);

    // one sixteen channel PCF8575 GPIO is supported, this gets the last set value
    int digitalRead(int pin);

    // one sixteen channel PCF8575 GPIO is supported, this sets each output on or off
    void digitalWrite(int pin, int value);

  private:
    bool found = false;

    PCF8574 *pcf[4];

    int mode[32];
    bool state[32];
};

extern GpioPcf8574 gpio;

#endif
