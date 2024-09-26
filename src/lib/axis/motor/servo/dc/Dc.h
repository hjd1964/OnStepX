// -----------------------------------------------------------------------------------
// axis servo DC motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#ifdef SERVO_DC_PRESENT

#include "../ServoDriver.h"

typedef struct ServoDcPins {
  int16_t in1;
  uint8_t inState1;
  int16_t in2;
  uint8_t inState2;
  int16_t enable;
  uint8_t enabledState;
  int16_t fault;
} ServoDcPins;

typedef struct ServoDcSettings {
  int16_t model;
  int8_t  status;
  int32_t velocityMax;   // in % of max power
  int32_t acceleration;  // in %/second/second
} ServoDcSettings;

class ServoDc : public ServoDriver {
  public:
    // constructor
    ServoDc(uint8_t axisNumber, const ServoDcPins *Pins, const ServoDcSettings *Settings);

    // decodes driver model and sets up the pin modes
    void init();

    // enable or disable the driver using the enable pin or other method
    void enable(bool state);

    // set motor velocity by adjusting power (0 to SERVO_ANALOG_WRITE_RANGE for 0 to 100% power)
    float setMotorVelocity(float power);

    // update status info. for driver
    void updateStatus();

    const ServoDcSettings *Settings;

  private:
    // motor control update
    void pwmUpdate(float power);

    const ServoDcPins *Pins;

    float currentVelocity = 0.0F;
    float acceleration;
    float accelerationFs;
};

#endif
