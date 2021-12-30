// -----------------------------------------------------------------------------------
// axis servo motor
#pragma once
#include "../../../../Common.h"

#ifdef SERVO_MOTOR_PRESENT

#include <Encoder.h> // https://github.com/hjd1964/Encoder (for AB, CW/CCW, PULSE/DIR, PULSE ONLY)
                     // or use https://github.com/PaulStoffregen/Encoder for AB encoders only

#include "ServoDrivers.h"

#include "Pid/Pid.h"

class ServoMotor : public Motor {
  public:
    // constructor
    ServoMotor(uint8_t axisNumber, Encoder *enc, Feedback *feedback, ServoDriver *driver, ServoControl *control);

    // sets up the servo motor
    bool init(void (*volatile move)(), void (*volatile moveFF)() = NULL, void (*volatile moveFR)() = NULL);

    // set driver reverse state
    void setReverse(int8_t state);

    // get driver type code
    inline char getParamTypeCode() { return feedback->getParamTypeCode(); }

    // set driver parameters
    void setParam(float param1, float param2, float param3, float param4, float param5, float param6);

    // validate driver parameters
    bool validateParam(float param1, float param2, float param3, float param4, float param5, float param6);

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
    Feedback *feedback;
    ServoControl *control;

    bool isSlewing = false;
};

#endif