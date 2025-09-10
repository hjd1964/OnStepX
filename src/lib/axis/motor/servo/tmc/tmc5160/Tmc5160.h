// -----------------------------------------------------------------------------------
// axis servo TMC5160 stepper motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../../Common.h"

#ifdef SERVO_TMC5160_PRESENT

#include "../TmcServoDriver.h"

#include <TMCStepper.h> // https://github.com/teemuatlut/TMCStepper

class ServoTmc5160 : public TmcServoDriver {
  public:
    // constructor
    ServoTmc5160(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings, float countsToStepsRatio, int16_t microsteps, int16_t current, int8_t decay, int8_t decaySlewing);

    // decodes driver model and sets up the pin modes
    bool init(bool reverse);

    // enable or disable the driver using the enable pin or other method
    void enable(bool state);

    // set motor velocity
    // \param velocity as needed to reach the target position, in encoder counts per second
    // \returns velocity in effect, in encoder counts per second
    float setMotorVelocity(float velocity);

    // calibrate the motor if required
    void calibrateDriver();

    // get the driver name
    const char* name() { return "TMC5160 (SPI)"; }

  private:
    // read status from driver
    void readStatus();

    bool stealthChop() { if (lround(decay.value) == STEALTHCHOP) return true; else return false; }

    TMC5160Stepper *driver;

    // for switching to/from spreadCycle
    int16_t velocityThrs;
};

#endif
