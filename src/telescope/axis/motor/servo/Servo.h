// -----------------------------------------------------------------------------------
// axis servo driver motion
#pragma once

#include <Arduino.h>
#include "../../../../Common.h"

#include "Servo.defaults.h"

#ifdef SERVO_DRIVER_PRESENT

#include <Encoder.h> // https://github.com/PaulStoffregen/Encoder

#include "../../../../commands/ProcessCmds.h"
#include "ServoDrivers.h"
#include "../Motor.h"

class ServoMotor : public Motor {
  public:
    // sets up the servo pins and any associated driver
    bool init(uint8_t axisNumber, int8_t reverse, int16_t integral, int16_t porportional);

    // sets motor power on/off (if possible)
    void power(bool value);

    // get movement frequency in steps per second
    float getFrequencySteps();

    // set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
    void setFrequencySteps(float frequency);

    // get the associated stepper driver status
    DriverStatus getDriverStatus();

    // set slewing state (hint that we are about to slew or are done slewing)
    void setSlewing(bool state);

    // sets dir as required and moves coord toward target at setFrequencySteps() rate
    void move();

    // a DC motor driver
    DcDriver driver;

  private:
    uint8_t taskHandle = 0;

    bool enabled = false;               // enable/disable logical state (disabled is powered down)
    bool synchronized = true;           // locks movement of axis target with timer rate
    bool limitsCheck = true;            // enable/disable numeric position range limits (doesn't apply to limit switches)

    volatile Direction direction = DIR_NONE;
    #ifdef DRIVER_STEP_DEFAULTS
      #define stepClr LOW               // pin state to reset driver before taking a step
      #define stepSet HIGH              // pin state to take a step
    #else
      volatile uint8_t stepClr = LOW;   // pin state to reset driver before taking a step
      volatile uint8_t stepSet = HIGH;  // pin state to take a step
    #endif
    volatile uint8_t dirFwd = LOW;      // pin state for forward direction
    volatile uint8_t dirRev = HIGH;     // pin state for reverse direction

    volatile int  homeSteps = 1;        // step count for microstep sequence between home positions (driver indexer)
    volatile int  slewStep = 1;         // step size during slews (for micro-step mode switching)
    volatile bool takeStep = false;     // should we take a step

    float currentFrequency = 0.0F;      // last frequency set 
    float lastFrequency = 0.0F;         // last frequency requested
    unsigned long lastPeriod = 0;       // last timer period (in sub-micros)

    void (*_move)() = NULL;

    Encoder *enc = NULL;
};

#ifdef AXIS1_DRIVER_SERVO
  extern ServoMotor motor1;
#endif
#ifdef AXIS2_DRIVER_SERVO
  extern ServoMotor motor2;
#endif
#ifdef AXIS3_DRIVER_SERVO
  extern ServoMotor motor3;
#endif
#ifdef AXIS4_DRIVER_SERVO
  extern ServoMotor motor4;
#endif
#ifdef AXIS5_DRIVER_SERVO
  extern ServoMotor motor5;
#endif
#ifdef AXIS6_DRIVER_SERVO
  extern ServoMotor motor6;
#endif
#ifdef AXIS7_DRIVER_SERVO
  extern ServoMotor motor7;
#endif
#ifdef AXIS8_DRIVER_SERVO
  extern ServoMotor motor8;
#endif
#ifdef AXIS9_DRIVER_SERVO
  extern ServoMotor motor9;
#endif

#endif