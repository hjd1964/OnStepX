// -----------------------------------------------------------------------------------
// axis servo KTech motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#ifdef SERVO_KTECH_PRESENT

#include "../ServoDriver.h"

#if !defined(CAN_PLUS) || CAN_PLUS == OFF
  #error "No KTECH motor CAN interface!"
#endif

// KTECH status rate default 1Hz
#ifndef KTECH_STATUS_MS
  #define KTECH_STATUS_MS 1000
#endif

class ServoKTech : public ServoDriver {
  public:
    // constructor
    ServoKTech(uint8_t axisNumber, const ServoSettings *Settings, float countsToDegreesRatio);

    // decodes driver model and sets up the pin modes
    bool init(bool reverse);

    // returns the number of axis parameters
    uint8_t getParameterCount() { return numParameters; }

    // returns the specified axis parameter
    AxisParameter* getParameter(uint8_t number) { if (number > numParameters) return &invalid; else return parameter[number]; }

    // enable or disable the driver using the enable pin or other method
    void enable(bool state);

    // set motor velocity
    // \param velocity as needed to reach the target position, in encoder counts per second
    // \param velocityTarget is the desired instantanous velocity at this moment, in encoder counts per second
    // \returns velocity in effect, in encoder counts per second
    float setMotorVelocity(float velocity);

    // request driver status from CAN
    void requestStatus();

    // read the associated driver status from CAN
    void requestStatusCallback(uint8_t data[8]);

    // get the driver name
    const char* name() { return "KTECH"; }

  private:
    // read status info. from driver
    void readStatus();

    int canID;
    unsigned long lastVelocityUpdateTime = 0;
    unsigned long lastStatusUpdateTime = 0;

    void (*callback)() = NULL;

    // regulate velocity changes
    int32_t velocityLast = 0;

    // runtime adjustable settings
    AxisParameter countsToStepsRatio = {NAN, NAN, NAN, -1, 20000, AXP_FLOAT, "Count/Step ratio"};

    const int numParameters = 3;
    AxisParameter* parameter[4] = {&invalid, &velocityMax, &acceleration, &countsToStepsRatio};

};

#endif
