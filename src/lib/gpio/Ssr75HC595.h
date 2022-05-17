// -----------------------------------------------------------------------------------
// 75HC595 GPIO support
#pragma once

#include "../../Common.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == SSR75HC595

#include "../commands/CommandErrors.h"

#ifndef GPIO_SSR75HC595_LATCH_PIN
#define GPIO_SSR75HC595_LATCH_PIN OFF
#endif
#ifndef GPIO_SSR75HC595_CLOCK_PIN
#define GPIO_SSR75HC595_CLOCK_PIN OFF
#endif
#ifndef GPIO_SSR75HC595_DATA_PIN
#define GPIO_SSR75HC595_DATA_PIN OFF
#endif
#ifndef GPIO_SSR75HC595_COUNT
#define GPIO_SSR75HC595_COUNT 8 // probably 16 (two 8 pin devices) for a gain of 13 pins is the best balance
#endif
#if GPIO_SSR75HC595_COUNT != 8 && GPIO_SSR75HC595_COUNT != 16 && GPIO_SSR75HC595_COUNT != 24 && GPIO_SSR75HC595_COUNT != 32
  #error "GPIO device SSR75HC595 supports GPIO_SSR75HC595_COUNT of 8, 16, 24, or 32 only."
#endif

class Ssr75HC595 {
  public:
    // init for SSR75HC595 device
    bool init();

    // process any gpio commands
    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // set GPIO pin mode for INPUT, INPUT_PULLUP, or OUTPUT (input does nothing always, false)
    void pinMode(int pin, int mode);

    // up to four eight channel 75HC595 GPIOs are supported, this gets the last set value (output only)
    int digitalRead(int pin);

    // up to four eight channel 75HC595 GPIOs are supported, this sets each output on or off
    void digitalWrite(int pin, bool value);

  private:
    uint32_t register_value = 0;
    bool found = false;

    int mode[GPIO_SSR75HC595_COUNT];
    bool state[GPIO_SSR75HC595_COUNT];
};

extern Ssr75HC595 gpio;

#endif
