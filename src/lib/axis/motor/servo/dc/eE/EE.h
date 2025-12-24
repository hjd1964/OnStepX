// -----------------------------------------------------------------------------------
// axis servo DC EE motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../../Common.h"

#ifdef SERVO_EE_PRESENT

#include "../DcServoDriver.h"

class ServoEE : public ServoDcDriver {
  public:
    // constructor
    ServoEE(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings, float pwmMinimum, float pwmMaximum);

    // decodes driver model and sets up the pin modes
    bool init(bool reverse);

    // enable or disable the driver using the enable pin or other method
    void enable(bool state);

    // update status info. for driver
    void updateStatus();

    // get the driver name
    const char* name() {
      if (driverModel == SERVO_EE) return "DC Enable/Enable"; else
      return "?";
    }

  private:
    // motor control pwm update
    // \param power01 in -1.0 to 1.0 units
    void pwmUpdate(float power01) override;

    inline float off1() { return (Pins->ph1State == HIGH) ? 1.0F : 0.0F; }
    inline float off2() { return (Pins->ph2State == HIGH) ? 1.0F : 0.0F; }
};

#endif
