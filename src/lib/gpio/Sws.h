// -----------------------------------------------------------------------------------
// Smart Web Server GPIO support (optional when SWS Encoders are not used)
#pragma once

#include "../../Common.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == SWS

#include "../commands/CommandErrors.h"

class SwsGpio {
  public:
    // scan for SWS device
    bool init();

    // process any gpio commands
    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    void pinMode(int pin, int mode);

    // one eight channel SWS GPIO is supported, this gets the last set value
    int digitalRead(int pin);

    // one eight channel SWS GPIO is supported, this sets each output on or off
    void digitalWrite(int pin, bool value);

    // one eight channel SWS GPIO is supported
    void analogWrite(int pin, int value);

  private:
    bool found = false;

    bool virtualRead[8] = {false, false, false, false, false, false, false, false};
    int virtualWrite[8] = {false, false, false, false, false, false, false, false};
    char virtualMode[8] = {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X' };

    int mode[8] = {OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF};
    int state[8] = {false, false, false, false, false, false, false, false};
};

extern SwsGpio gpio;

#endif
