// -----------------------------------------------------------------------------------
// 74HC595 GPIO support
#pragma once

#include "../../Common.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == SSR74HC595

#include "../commands/CommandErrors.h"

class Ssr74HC595 {
  public:
    // init for SSR74HC595 device
    bool init();

    // process any gpio commands
    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // set GPIO pin mode for INPUT, INPUT_PULLUP, or OUTPUT (input does nothing always, false)
    void pinMode(int pin, int mode);

    // up to four eight channel 74HC595 GPIOs are supported, this gets the last set value (output only)
    int digitalRead(int pin);

    // up to four eight channel 74HC595 GPIOs are supported, this sets each output on or off
    void digitalWrite(int pin, bool value);

  private:
    uint32_t register_value = 0;
    bool found = false;

    int mode[32];
    bool state[32];
};

extern Ssr74HC595 gpio;

#endif
