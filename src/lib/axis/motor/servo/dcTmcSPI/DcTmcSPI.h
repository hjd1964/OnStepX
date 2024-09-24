// -----------------------------------------------------------------------------------
// axis servo TMC2130 and TMC5160 DC motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#ifdef SERVO_DC_TMC_SPI_PRESENT

#include "../ServoDriver.h"

#ifndef DRIVER_TMC_STEPPER_AUTOGRAD
  #define DRIVER_TMC_STEPPER_AUTOGRAD true
#endif

#include <TMCStepper.h> // https://github.com/teemuatlut/TMCStepper

typedef struct ServoDcTmcSpiPins {
  int16_t step;
  int16_t dir;
  int16_t enable;
  uint8_t enabledState;
  int16_t m0;
  int16_t m1;
  int16_t m2;
  int16_t m3;
  int16_t fault;
} ServoDcTmcPins;

typedef struct ServoDcTmcSettings {
  int16_t model;
  int8_t  status;
  int32_t velocityMax;   // maximum velocity in steps/second
  int32_t acceleration;  // acceleration steps/second/second
  int16_t microsteps;
  int16_t current;
} ServoDcTmcSettings;

class ServoDcTmcSPI : public ServoDriver {
  public:
    // constructor
    ServoDcTmcSPI(uint8_t axisNumber, const ServoDcTmcPins *Pins, const ServoDcTmcSettings *TmcSettings);

    // decodes driver model and sets up the pin modes
    void init();

    // enable or disable the driver using the enable pin or other method
    void enable(bool state);

    // power level to the motor
    float setMotorVelocity(float power);

    // update status info. for driver
    void updateStatus();

    const ServoDcTmcSettings *Settings;

  private:

    TMCStepper *driver;

    bool powered = false;
    float currentVelocity = 0.0F;
    float acceleration;
    float accelerationFs;
    const ServoDcTmcSpiPins *Pins;
};

#endif
