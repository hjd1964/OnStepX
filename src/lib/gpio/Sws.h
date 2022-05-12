// -----------------------------------------------------------------------------------
// Smart Web Server GPIO support (optional when SWS Encoders are not used)
#pragma once

#include "../../Common.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == SWS

#include "../commands/CommandErrors.h"

class SwsGpio {
  public:
    // scan for SWS device on the 1-wire bus
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
    bool swsPolling = true;

    bool virtualRead[4] = { false, false, false, false };
    bool virtualWrite[4] = { false, false, false, false };
    char virtualMode[4] = { 'X', 'X', 'X', 'X' };

    int mode[4] = { OFF, OFF, OFF, OFF };
    bool state[4] = { false, false, false, false };
};

extern SwsGpio gpio;

#endif
