// -----------------------------------------------------------------------------------
// I2C PCF8575 GPIO support
#pragma once

#include "../GpioBase.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == X8575

class GpioPcf8575 : public Gpio {
  public:
    // scan for PCF8575 device
    bool init();

    // process any gpio commands
    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    void pinMode(int pin, int mode);

    // one sixteen channel PCF8575 GPIO is supported, this gets the last set value
    int digitalRead(int pin);

    // one sixteen channel PCF8575 GPIO is supported, this sets each output on or off
    void digitalWrite(int pin, int value);

  private:
    bool found = false;

    int mode[16] = { INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT };
    bool state[16] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
};

extern GpioPcf8575 gpio;

#endif
