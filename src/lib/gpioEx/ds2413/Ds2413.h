// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire DS2413 device support
#pragma once

#include "../GpioBase.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == DS2413

#define DS2413_MAX_DEVICES 2

class GpioDs2413 : public Gpio {
  public:
    // scan for DS2413 devices on the 1-wire bus
    bool init();

    // set GPIO pin mode for INPUT or OUTPUT (both pins of any device must be in the same mode)
    void pinMode(int pin, int mode);

    // get GPIO pin state
    int digitalRead(int pin);

    // set GPIO pin state
    void digitalWrite(int pin, int value);

    // update the DS2413
    void poll();

  private:
    bool found = false;
    uint8_t deviceCount = 0;
    int lastValidPin = -1;

    uint8_t address[DS2413_MAX_DEVICES][8];

    int mode[DS2413_MAX_DEVICES] = {INPUT, INPUT};
    uint8_t state[DS2413_MAX_DEVICES*2] = {false, false, false, false};
    int16_t lastState[DS2413_MAX_DEVICES*2] = {INVALID, INVALID, INVALID, INVALID};

    unsigned long goodUntil[DS2413_MAX_DEVICES] = {0, 0};
};

extern GpioDs2413 gpio;

#endif
