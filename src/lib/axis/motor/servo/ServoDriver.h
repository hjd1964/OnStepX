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
#ifndef SERVO_ANALOG_WRITE_RANGE
  #define SERVO_ANALOG_WRITE_RANGE ANALOG_WRITE_RANGE
#endif

#ifndef AXIS1_SERVO_VELOCITY_FACTOR
  #define AXIS1_SERVO_VELOCITY_FACTOR 0.0F
#endif
#ifndef AXIS2_SERVO_VELOCITY_FACTOR
  #define AXIS2_SERVO_VELOCITY_FACTOR 0.0F
#endif
#ifndef AXIS3_SERVO_VELOCITY_FACTOR
  #define AXIS3_SERVO_VELOCITY_FACTOR 0.0F
#endif
#ifndef AXIS4_SERVO_VELOCITY_FACTOR
  #define AXIS4_SERVO_VELOCITY_FACTOR 0.0F
#endif
#ifndef AXIS5_SERVO_VELOCITY_FACTOR
  #define AXIS5_SERVO_VELOCITY_FACTOR 0.0F
#endif
#ifndef AXIS6_SERVO_VELOCITY_FACTOR
  #define AXIS6_SERVO_VELOCITY_FACTOR 0.0F
#endif
#ifndef AXIS7_SERVO_VELOCITY_FACTOR
  #define AXIS7_SERVO_VELOCITY_FACTOR 0.0F
#endif
#ifndef AXIS8_SERVO_VELOCITY_FACTOR
  #define AXIS8_SERVO_VELOCITY_FACTOR 0.0F
#endif
#ifndef AXIS9_SERVO_VELOCITY_FACTOR
  #define AXIS9_SERVO_VELOCITY_FACTOR 0.0F
#endif

#ifndef AXIS1_SERVO_VELOCITY_SCALE
  #define AXIS1_SERVO_VELOCITY_SCALE 1.0F
#endif
#ifndef AXIS2_SERVO_VELOCITY_SCALE
  #define AXIS2_SERVO_VELOCITY_SCALE 1.0F
#endif
#ifndef AXIS3_SERVO_VELOCITY_SCALE
  #define AXIS3_SERVO_VELOCITY_SCALE 1.0F
#endif
#ifndef AXIS4_SERVO_VELOCITY_SCALE
  #define AXIS4_SERVO_VELOCITY_SCALE 1.0F
#endif
#ifndef AXIS5_SERVO_VELOCITY_SCALE
  #define AXIS5_SERVO_VELOCITY_SCALE 1.0F
#endif
#ifndef AXIS6_SERVO_VELOCITY_SCALE
  #define AXIS6_SERVO_VELOCITY_SCALE 1.0F
#endif
#ifndef AXIS7_SERVO_VELOCITY_SCALE
  #define AXIS7_SERVO_VELOCITY_SCALE 1.0F
#endif
#ifndef AXIS8_SERVO_VELOCITY_SCALE
  #define AXIS8_SERVO_VELOCITY_SCALE 1.0F
#endif
#ifndef AXIS9_SERVO_VELOCITY_SCALE
  #define AXIS9_SERVO_VELOCITY_SCALE 1.0F
#endif

class ServoDriver {
  public:
    // decodes driver model and sets up the pin modes
    virtual void init();

    // alternate mode for movement
    virtual void alternateMode(bool state) { UNUSED(state); }

    // enable or disable the driver using the enable pin or other method
    virtual void enable(bool state) { UNUSED(state); }

    // get the control range to the motor (-velocityMax to velocityMax) defaults to ANALOG_WRITE_RANGE
    // must be ready at object creation!
    virtual float getMotorControlRange() { return velocityMax; }

    // set motor velocity, returns velocity actually set
    virtual float setMotorVelocity(float velocity);

    // returns motor direction (DIR_FORWARD or DIR_REVERSE)
    Direction getMotorDirection() { return motorDirection; };

    // update status info. for driver
    // this is a required method for the Axis class
    virtual void updateStatus();

    // get status info.
    // this is a required method for the Axis class
    DriverStatus getStatus() { return status; }
   
    // calibrate the motor if required
    virtual void calibrateDriver() {}

    // return the velocity estimate
    virtual float getVelocityEstimate(float frequency) {
      UNUSED(frequency);
      switch (axisNumber) {
        case 1: return AXIS1_SERVO_VELOCITY_FACTOR;
        case 2: return AXIS2_SERVO_VELOCITY_FACTOR;
        case 3: return AXIS3_SERVO_VELOCITY_FACTOR;
        case 4: return AXIS4_SERVO_VELOCITY_FACTOR;
        case 5: return AXIS5_SERVO_VELOCITY_FACTOR;
        case 6: return AXIS6_SERVO_VELOCITY_FACTOR;
        case 7: return AXIS7_SERVO_VELOCITY_FACTOR;
        case 8: return AXIS8_SERVO_VELOCITY_FACTOR;
        case 9: return AXIS9_SERVO_VELOCITY_FACTOR;
        default: return 0;
      }
    }

    // return the velocity scale factor
    virtual float getVelocityScale() {
      switch (axisNumber) {
        case 1: return AXIS1_SERVO_VELOCITY_SCALE;
        case 2: return AXIS2_SERVO_VELOCITY_SCALE;
        case 3: return AXIS3_SERVO_VELOCITY_SCALE;
        case 4: return AXIS4_SERVO_VELOCITY_SCALE;
        case 5: return AXIS5_SERVO_VELOCITY_SCALE;
        case 6: return AXIS6_SERVO_VELOCITY_SCALE;
        case 7: return AXIS7_SERVO_VELOCITY_SCALE;
        case 8: return AXIS8_SERVO_VELOCITY_SCALE;
        case 9: return AXIS9_SERVO_VELOCITY_SCALE;
        default: return 0;
      }
    }

  protected:
    int axisNumber;
    DriverStatus status = { false, {false, false}, {false, false}, false, false, false, false };
    #if DEBUG != OFF
      DriverStatus lastStatus = {false, {false, false}, {false, false}, false, false, false, false};
    #endif
    unsigned long timeLastStatusUpdate = 0;

    int16_t model = OFF;
    int16_t statusMode = OFF;

    float velocityMax = SERVO_ANALOG_WRITE_RANGE;

    Direction motorDirection = DIR_FORWARD;

    int16_t enablePin = OFF;
    uint8_t enabledState = LOW;
    bool enabled = false;
    int16_t faultPin = OFF;
};

#endif
