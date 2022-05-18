// -----------------------------------------------------------------------------------
// I2C TCA9555 GPIO support
#pragma once

#include "../../Common.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == X9555

#include "../commands/CommandErrors.h"

class Tca9555 {
  public:
    // scan for TCA9555 device
    bool init();

    // process any gpio commands
    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    void pinMode(int pin, int mode);

    // one sixteen channel TCA9555 GPIO is supported, this gets the last set value
    int digitalRead(int pin);

    // one sixteen channel TCA9555 GPIO is supported, this sets each output on or off
    void digitalWrite(int pin, bool value);

  private:
    bool found = false;

    int mode[16] = { INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT, INPUT };
    bool state[16] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
};

extern Tca9555 gpio;

#endif
