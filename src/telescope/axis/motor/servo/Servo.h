// -----------------------------------------------------------------------------------
// axis servo driver motion
#pragma once

#include <Arduino.h>
#include "../../../../Common.h"

#include "Servo.defaults.h"

#ifdef SERVO_DRIVER_PRESENT

#include <Encoder.h> // https://github.com/hjd1964/Encoder (for AB, CW/CCW, PULSE/DIR, PULSE ONLY)
                     // or use https://github.com/PaulStoffregen/Encoder for AB encoders only

#include <PID_v1.h>  // https://github.com/hjd1964/Arduino-PID-Library

#include "../../../../commands/ProcessCmds.h"
#include "ServoDrivers.h"
#include "../Motor.h"

typedef struct PidControl {
  double in;
  double out;
  double set;
} PidSettings;

class ServoMotor : public Motor {
  public:
    // constructor
    ServoMotor(uint8_t axisNumber, Encoder *enc, PID *pid, PidControl *pidControl, ServoDriver *driver, void (*volatile move)());

    // sets up the servo pins and any associated driver
    bool init(int8_t reverse, int16_t integral, int16_t porportional);

    // sets motor power on/off (if possible)
    void power(bool value);

    // get the associated stepper driver status
    DriverStatus getDriverStatus();

    // resets motor and target angular position in steps, also zeros backlash and index 
    void resetPositionSteps(long value);

    // get tracking mode steps per slewing mode step
    inline int getStepsPerStepSlewing() { return 64; }

    // get movement frequency in steps per second
    float getFrequencySteps();

    // set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
    void setFrequencySteps(float frequency);

    // set slewing state (hint that we are about to slew or are done slewing)
    void setSlewing(bool state);

    // updates PID and sets servo motor power/direction
    void poll();

    // sets dir as required and moves coord toward target at setFrequencySteps() rate
    void move();

    // DC servo motor driver
    ServoDriver *driver;

    // for Pulse only encoders
    volatile int8_t directionHint = 1;

  private:
    uint8_t servoMonitorHandle = 0;
    uint8_t taskHandle = 0;

    int  stepSize = 1;                  // step size
    volatile int  homeSteps = 1;        // step count for microstep sequence between home positions (driver indexer)
    volatile bool takeStep = false;     // should we take a step

    float currentFrequency = 0.0F;      // last frequency set 
    float lastFrequency = 0.0F;         // last frequency requested
    unsigned long lastPeriod = 0;       // last timer period (in sub-micros)

    volatile int  absStep = 1;          // absolute step size (unsigned)

    void (*_move)() = NULL;

    Encoder *enc;
    PID *pid = NULL;
    PidControl *pidControl;
};

#ifdef AXIS1_SERVO
  extern ServoMotor motor1;
#endif
#ifdef AXIS2_SERVO
  extern ServoMotor motor2;
#endif
#ifdef AXIS3_SERVO
  extern ServoMotor motor3;
#endif
#ifdef AXIS4_SERVO
  extern ServoMotor motor4;
#endif
#ifdef AXIS5_SERVO
  extern ServoMotor motor5;
#endif
#ifdef AXIS6_SERVO
  extern ServoMotor motor6;
#endif
#ifdef AXIS7_SERVO
  extern ServoMotor motor7;
#endif
#ifdef AXIS8_SERVO
  extern ServoMotor motor8;
#endif
#ifdef AXIS9_SERVO
  extern ServoMotor motor9;
#endif

#endif