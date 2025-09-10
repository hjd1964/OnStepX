// -----------------------------------------------------------------------------------
// axis servo TMC2130 DC motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../../../Common.h"

#ifdef SERVO_TMC2130_DC_PRESENT

#include "../../DcServoDriver.h"

#include <TMCStepper.h> // https://github.com/teemuatlut/TMCStepper

class ServoTmc2130DC : public ServoDcDriver {
  public:
    // constructor
    ServoTmc2130DC(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings, float pwmMinimum, float pwmMaximum);

    // decodes driver model and sets up the pin modes
    bool init(bool reverse);

    // enable or disable the driver using the enable pin or other method
    void enable(bool state);

    // get the driver name
    const char* name() { return "DC TMC2130 (SPI)"; }

  private:
    // motor control pwm update
    // \param power in +/-255 units
    void pwmUpdate(long power);
    
    // read status info. from driver
    void readStatus();

    TMC2130Stepper *driver;
};

#endif
