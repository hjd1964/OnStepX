// -----------------------------------------------------------------------------------
// I2C PCF8575 GPIO support
#pragma once

#include "../GpioBase.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == X8575

#ifndef GPIO_PCF8575_I2C_NUM_DEVICES
  #define GPIO_PCF8575_I2C_NUM_DEVICES 1
#endif

#ifndef GPIO_PCF8575_I2C_ADDRESS1
  #define GPIO_PCF8575_I2C_ADDRESS1 0x20
#endif
#ifndef GPIO_PCF8575_I2C_ADDRESS2
  #define GPIO_PCF8575_I2C_ADDRESS2 0x21
#endif

class GpioPcf8575 : public Gpio {
  public:
    // scan for PCF8575 device
    bool init();

    void pinMode(int pin, int mode);

    int digitalRead(int pin);

    void digitalWrite(int pin, int value);

  private:
    bool found = false;

    uint8_t iicAddress[2] = {GPIO_PCF8575_I2C_ADDRESS1, GPIO_PCF8575_I2C_ADDRESS2};

    int mode[32];
    uint16_t state[2] = {0, 0};
};

extern GpioPcf8575 gpio;

#endif
