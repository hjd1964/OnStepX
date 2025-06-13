// -----------------------------------------------------------------------------------
// I2C PCF8574 GPIO support
#pragma once

#include "../GpioBase.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == X8574

#ifndef GPIO_PCF8574_I2C_NUM_DEVICES
  #define GPIO_PCF8574_I2C_NUM_DEVICES 1
#endif

#ifndef GPIO_PCF8574_I2C_ADDRESS1
  #define GPIO_PCF8574_I2C_ADDRESS1 0x39
#endif
#ifndef GPIO_PCF8574_I2C_ADDRESS2
  #define GPIO_PCF8574_I2C_ADDRESS2 0x3A
#endif
#ifndef GPIO_PCF8574_I2C_ADDRESS3
  #define GPIO_PCF8574_I2C_ADDRESS3 0x3B
#endif
#ifndef GPIO_PCF8574_I2C_ADDRESS4
  #define GPIO_PCF8574_I2C_ADDRESS4 0x3C
#endif

class GpioPcf8574 : public Gpio {
  public:
    // scan for PCF8574 device
    bool init();

    void pinMode(int pin, int mode);

    int digitalRead(int pin);

    void digitalWrite(int pin, int value);

  private:
    bool found = false;

    uint8_t iicAddress[4] = {GPIO_PCF8574_I2C_ADDRESS1, GPIO_PCF8574_I2C_ADDRESS2, GPIO_PCF8574_I2C_ADDRESS3, GPIO_PCF8574_I2C_ADDRESS4};

    int mode[32];
    uint8_t state[4] = {0, 0, 0, 0};
};

extern GpioPcf8574 gpio;

#endif
