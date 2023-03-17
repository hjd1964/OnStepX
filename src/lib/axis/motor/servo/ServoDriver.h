// -----------------------------------------------------------------------------------
// axis servo motor driver
#pragma once

#include <Arduino.h>
#include "../../../../Common.h"

#include "../Drivers.h"
#include "../Motor.h"

#ifdef SERVO_MOTOR_PRESENT

#ifndef ANALOG_WRITE_RANGE
  #define ANALOG_WRITE_RANGE 255
#endif

class ServoDriver {
  public:
    // decodes driver model and sets up the pin modes
    virtual void init();

    // enable or disable the driver using the enable pin or other method
    virtual void enable(bool state) { UNUSED(state); }

    // get the control range to the motor (-velocityMax to velocityMax) defaults to ANALOG_WRITE_RANGE
    // must be ready at object creation!
    virtual float getMotorControlRange() { return velocityMax; }

    // set motor velocity, returns velocity actually set
    virtual float setMotorVelocity(float velocity);

    // returns motor direction (DIR_FORMWARD or DIR_REVERSE)
    Direction getMotorDirection() { return motorDirection; };

    // update status info. for driver
    // this is a required method for the Axis class
    virtual void updateStatus();

    // get status info.
    // this is a required method for the Axis class
    DriverStatus getStatus() { return status; }
   
    // calibrate the motor if required
    virtual void calibrate() {}

  protected:
    int axisNumber;
    DriverStatus status = { false, {false, false}, {false, false}, false, false, false, false };
    #if DEBUG != OFF
      DriverStatus lastStatus = {false, {false, false}, {false, false}, false, false, false, false};
    #endif
    unsigned long timeLastStatusUpdate = 0;

    int16_t model = OFF;
    int16_t statusMode = OFF;

    float velocityMax = ANALOG_WRITE_RANGE;
    Direction motorDirection = DIR_FORWARD;

    int16_t enablePin = OFF;
    uint8_t enabledState = LOW;
    bool enabled = false;
    int16_t faultPin = OFF;
};

#endif
