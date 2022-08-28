// -----------------------------------------------------------------------------------
// axis servo motor driver
#pragma once

#include <Arduino.h>
#include "../../../../Common.h"

#include "../Drivers.h"
#include "../Motor.h"

#ifdef SERVO_MOTOR_PRESENT

class ServoDriver {
  public:
    // decodes driver model and sets up the pin modes
    virtual void init();

    // secondary way to enable down not using the enable pin
    // this is a required method for the Axis class, even if it does nothing
    virtual void enable(bool state);

    // get the control range to the motor (-motorPwrMax to motorPwrMax) defaults to ANALOG_WRITE_PWM_RANGE
    // must be ready at object creation!
    virtual float getMotorControlRange() { return motorPwrMax; }

    // power level to the motor
    virtual void setMotorPower(float power);

    // returns motor direction (DIR_FORMWARD or DIR_REVERSE)
    Direction getMotorDirection() { return motorDir; };

    // update status info. for driver
    // this is a required method for the Axis class
    virtual void updateStatus();

    // get status info.
    // this is a required method for the Axis class
    DriverStatus getStatus() { return status; }

  protected:
    int axisNumber;
    DriverStatus status = { false, {false, false}, {false, false}, false, false, false, false };
    #if DEBUG != OFF
      DriverStatus lastStatus = {false, {false, false}, {false, false}, false, false, false, false};
    #endif
    unsigned long timeLastStatusUpdate = 0;

    int16_t model = OFF;
    int16_t statusMode = OFF;

    float motorPwr = 0.0F;
    float motorPwrMax = ANALOG_WRITE_PWM_RANGE;
    Direction motorDir = DIR_FORWARD;

    int16_t enablePin = OFF;
    uint8_t enabledState = LOW;
    bool enabled = false;
    int16_t faultPin = OFF;
};

#endif
