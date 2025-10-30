// -----------------------------------------------------------------------------------
// axis servo motor driver
#pragma once

#include <Arduino.h>
#include "../../../../Common.h"

#include "../Drivers.h"
#include "../Motor.h"

#ifdef SERVO_MOTOR_PRESENT

#ifdef SERVO_SIGMA_DELTA_DITHERING
  #include "dc/SigmaDeltaDither.h"
#endif

typedef struct ServoPins {
  int16_t ph1; // step
  int16_t ph1State;
  int16_t ph2; // dir
  int16_t ph2State;
  int16_t enable;
  uint8_t enabledState;
  int16_t m0;
  int16_t m1;
  int16_t m2;
  int16_t m3;
  int16_t fault;
} ServoPins;

typedef struct ServoSettings {
  int16_t model;
  int8_t  status;
  int32_t acceleration;  // acceleration in encoder %/s/s
} ServoSettings;

class ServoDriver {
  public:
    ServoDriver(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings);

    // decodes driver model and sets up the pin modes
    virtual bool init(bool reverse);

    // returns the number of axis parameters
    virtual uint8_t getParameterCount() { return numParameters; }

    // returns the specified axis parameter
    virtual AxisParameter* getParameter(uint8_t number) { if (number > numParameters) return &invalid; else return parameter[number]; }

    // check if axis parameter is valid
    virtual bool parameterIsValid(AxisParameter* parameter, bool next = false) { return true; }

    // alternate mode for movement
    virtual void alternateMode(bool state) { UNUSED(state); }

    // enable or disable the driver using the enable pin or other method
    virtual void enable(bool state) { UNUSED(state); }

    // sets overall maximum frequency
    // \param frequency: rate of motion in steps (counts) per second
    void setFrequencyMax(float frequency);

    // set motor velocity
    // \param velocity as needed to reach the target position, in encoder counts per second
    // \returns velocity in effect, in encoder counts per second
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

    // get the driver name
    virtual const char* name() { return NULL; }

  protected:
    virtual void readStatus() {}

    int axisNumber;
    char axisPrefix[32]; // prefix for debug messages

    int16_t driverModel;
    int16_t statusMode;

    DriverStatus status = { false, {false, false}, {false, false}, false, false, false, false };
    #if DEBUG != OFF
      DriverStatus lastStatus = {false, {false, false}, {false, false}, false, false, false, false};
    #endif
    unsigned long timeLastStatusUpdate = 0;

    float normalizedAcceleration; // in encoder counts/s/s
    float accelerationFs;         // in encoder counts/s/fs
    float velocityRamp = 0.0F;    // regulate velocity changes
    float velocityMax = 0.0F;     // in encoder counts/s

    Direction motorDirection = DIR_FORWARD;
    bool reversed = false;

    int16_t enablePin = OFF;
    uint8_t enabledState = LOW;
    bool enabled = false;
    int16_t faultPin = OFF;

    const ServoPins *Pins;
    const ServoSettings *Settings;

    // runtime adjustable settings
    AxisParameter invalid = {NAN, NAN, NAN, NAN, NAN, AXP_INVALID, ""};
    AxisParameter acceleration = {NAN, NAN, NAN, 0, 100000, AXP_FLOAT, AXPN_MAX_ACCEL};

    const int numParameters = 2;
    AxisParameter* parameter[2] = {&invalid, &acceleration};

    #ifdef SERVO_SIGMA_DELTA_DITHERING
      SigmaDeltaDither sigmaDelta;  // carries fractional residue between ticks
    #endif

};

#endif
