// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire DS2413 device support
#pragma once

#include "../../Common.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == DS2413

class Ds2413 {
  public:
    // scan for DS2413 devices on the 1-wire bus
    bool init();

    // set GPIO pin (0 or 1) mode for INPUT or OUTPUT (both pins must be in the same mode)
    void pinMode(int pin, int mode);

    // get GPIO pin (0 or 1) state
    int digitalRead(int pin);

    // set GPIO pin (0 or 1) state
    void digitalWrite(int pin, int value);

    // update the DS2413, designed for a 20ms polling interval
    void poll();

  private:
    bool found = false;
    uint8_t deviceCount = 0;
    uint8_t address[8];

    int mode = INPUT;
    uint8_t state[2] = { false, false };
    int16_t lastState[2] = { INVALID, INVALID };

    unsigned long goodUntil = 0;
};

extern Ds2413 gpio;

#endif
