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
} ServoDcSettings;

class ServoDc : public ServoDriver {
  public:
    // constructor
    ServoDc(uint8_t axisNumber, const ServoDcPins *Pins, const ServoDcSettings *Settings);

    // decodes driver model and sets up the pin modes
    void init();

    // power level to the motor
    void setMotorPower(float power);

    // update status info. for driver
    void updateStatus();

    const ServoDcSettings *Settings;

  private:
    // sets motor direction (DIR_FORMWARD or DIR_REVERSE)
    void setMotorDirection(Direction dir);

    // motor control update
    void pwmUpdate();

    const ServoDcPins *Pins;
};

#endif
