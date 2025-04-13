// -----------------------------------------------------------------------------------
// 74HC595 GPIO support
#pragma once

#include "../GpioBase.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == SSR74HC595

class GpioSsr74HC595 : public Gpio {
  public:
    // init for SSR74HC595 device
    bool init();

    // set GPIO pin mode for INPUT, INPUT_PULLUP, or OUTPUT (input does nothing always, false)
    void pinMode(int pin, int mode);

    // up to four eight channel 74HC595 GPIOs are supported, this gets the last set value (output only)
    int digitalRead(int pin);

    // up to four eight channel 74HC595 GPIOs are supported, this sets each output on or off
    void digitalWrite(int pin, int value);

  private:
    uint32_t register_value = 0;
    bool found = false;

    int mode[32];
    bool state[32];
};

extern GpioSsr74HC595 gpio;

#endif
