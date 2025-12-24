// -----------------------------------------------------------------------------------
// axis servo DC motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../../Common.h"

#ifdef SERVO_PE_PRESENT

#include "../DcServoDriver.h"

class ServoPE : public ServoDcDriver {
  public:
    // constructor
    ServoPE(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings, float pwmMinimum, float pwmMaximum);

    // decodes driver model and sets up the pin modes
    bool init(bool reverse);

    // enable or disable the driver using the enable pin or other method
    void enable(bool state);

    // update status info. for driver
    void updateStatus();

    // get the driver name
    const char* name() {
      if (driverModel == SERVO_PE) return "DC Phase/Enable" ; else
      return "?";
    }

  private:
    // motor control pwm update
    // \param power01 -1.0 to 1.0
    void pwmUpdate(float power01) override;

    inline float off2() { return (Pins->ph2State == HIGH) ? 1.0F : 0.0F; }
};

#endif
