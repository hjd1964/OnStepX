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

typedef struct ServoKTechSettings {
  int16_t model;
  int8_t  status;
  int32_t velocityMax;   // maximum velocity in steps/second
  int32_t acceleration;  // acceleration steps/second/second
} ServoKTechSettings;

class ServoKTech : public ServoDriver {
  public:
    // constructor
    ServoKTech(uint8_t axisNumber, const ServoKTechSettings *KTechSettings);

    // decodes driver model and sets up the pin modes
    bool init();

    // enable or disable the driver using the enable pin or other method
    void enable(bool state);

    // power level to the motor
    float setMotorVelocity(float power);

    // request driver status from CAN
    void requestStatus();

    // read the associated driver status from CAN
    void requestStatusCallback(uint8_t data[8]);

    const ServoKTechSettings *Settings;

  private:
    // read status info. from driver
    void readStatus();

    int canID;
    unsigned long lastVelocityUpdateTime = 0;
    unsigned long lastStatusUpdateTime = 0;

    void (*callback)() = NULL;

    bool powered = false;
    bool sdMode = false;
    float currentVelocity = 0.0F;
    float acceleration;
    float accelerationFs;
    bool statusRequestSent = false;

    long lastV = 0;
};

#endif
