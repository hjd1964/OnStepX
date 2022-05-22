// -----------------------------------------------------------------------------------
// axis servo motor driver
#pragma once

#include <Arduino.h>
#include "../../../../Common.h"

// SERVO DRIVER (usually for DC motors equipped with encoders)
#define DRIVER_SERVO_MODEL_COUNT 2

#ifndef SERVO_DRIVER_FIRST
  #define SERVO_DRIVER_FIRST        100
  #define SERVO_PE                  100    // SERVO, direction and pwm connections
  #define SERVO_II                  101    // SERVO, dual pwm input connections
  #define SERVO_DRIVER_LAST         101
#endif

#include "../Drivers.h"
#include "../Motor.h"

#ifdef SERVO_MOTOR_PRESENT

typedef struct ServoDriverSettings {
  int16_t model;
  int8_t  status;
} ServoDriverSettings;

typedef struct ServoDriverPins {
  int16_t in1;
  uint8_t inState1;
  int16_t in2;
  uint8_t inState2;
  int16_t enable;
  uint8_t enabledState;
  int16_t fault;
} ServoDriverPins;

class ServoDriver {
  public:
    // constructor
    ServoDriver(uint8_t axisNumber, const ServoDriverPins *Pins, const ServoDriverSettings *Settings);

    // decodes driver model and sets up the pin modes
    void init();

    // update status info. for driver
    // this is a required method for the Axis class
    void updateStatus();

    // get status info.
    // this is a required method for the Axis class
    DriverStatus getStatus();

    // secondary way to power down not using the enable pin
    // this is a required method for the Axis class, even if it does nothing
    void power(bool state);

    // get the microsteps goto
    // this is a required method for the Axis class, even if it only ever returns 1
    inline int getSubdivisionsGoto() { return 1; }

    // power level to the motor (-ANALOG_WRITE_PWM_RANGE to ANALOG_WRITE_PWM_RANGE, negative for reverse)
    void setMotorPower(int power);

    // returns motor direction (DIR_FORMWARD or DIR_REVERSE)
    Direction getMotorDirection() { return motorDir; };

    ServoDriverSettings settings;

  private:
    // sets motor direction (DIR_FORMWARD or DIR_REVERSE)
    void setMotorDirection(Direction dir);

    void update();

    int axisNumber;
    bool powered = false;

    int motorPwr = 0;
    Direction motorDir = DIR_FORWARD;

    DriverStatus status = { false, {false, false}, {false, false}, false, false, false, false };

    const ServoDriverPins *Pins;
};

#endif
