// -----------------------------------------------------------------------------------
// Step Dir motor control
#pragma once

#include <Arduino.h>
#include "../../../Common.h"

#include "DcDrivers.h"

#ifdef SERVO_DRIVER_PRESENT

#include <Encoder.h> // https://github.com/PaulStoffregen/Encoder

#include "../../../commands/ProcessCmds.h"
#include "Servo.defaults.h"

class Servo {
  public:
    // sets up the driver step/dir/enable pins and any associated stepDriver mode control
    bool init(uint8_t axisNumber, int8_t reverse, int16_t microsteps, int16_t current);

    // sets motor power on/off (if possible)
    void power(bool value);

    // set backlash amount in steps
    void setBacklashSteps(long value);
    // get backlash amount in steps
    long getBacklashSteps();

    // resets motor and target angular position in steps, also zeros backlash and index 
    void resetPositionSteps(long value);
    // resets target position to the motor position
    void resetTargetToMotorPosition();
    // get motor angular position in steps
    long getMotorPositionSteps();

    // get instrument coordinate, in steps
    long getInstrumentCoordinateSteps();
    // set instrument coordinate, in steps
    void setInstrumentCoordinateSteps(long value);

    // mark origin coordinate at current location
    void setOriginCoordinateSteps();
    // set target coordinate (with index), in steps
    void setTargetCoordinateSteps(long value);
    // get target coordinate (with index), in steps
    long getTargetCoordinateSteps();
    // set target park coordinate, in steps (taking into account stepper motor cogging when powered off)
    // should only be called when the axis is not moving
    void setTargetCoordinateParkSteps(long value, int modulo);
    // set instrument park coordinate, in steps
    // should only be called when the axis is not moving
    void setInstrumentCoordinateParkSteps(long value, int modulo);

    // distance to target in steps (+/-)
    long getTargetDistanceSteps();
    // distance to origin or target, whichever is closer, in steps
    long getOriginOrTargetDistanceSteps();

    // set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
    void setFrequencySteps(float frequency);
    // get movement frequency in steps per second
    float getFrequencySteps();

    // set synchronized state (automatic movement of target at setFrequencySteps() rate)
    void setSynchronized(bool state);
    // get synchronized state (automatic movement of target at setFrequencySteps() rate)
    bool getSynchronized();

    // set slewing state (hint that we are about to slew or are done slewing)
    void setSlewing(bool state);

    // set backlash frequency in steps per second
    void setBacklashFrequencySteps(float frequency);

    // get the current direction of motion
    Direction getDirection();

    // get the associated stepper driver status
    DriverStatus getDriverStatus();

    // sets dir as required and moves coord toward target at setFrequencySteps() rate
    void move();

    // a DC motor driver
    DcDriver driver;

  private:
    // disable backlash compensation, to work properly there must be an enable call to match
    void disableBacklash();
    // enable backlash compensation, to work properly this must be proceeded by a disable call
    void enableBacklash();

    uint8_t taskHandle = 0;
    uint8_t axisNumber = 0;
    char axisPrefix[14] = "MSG: Servo_, ";

    bool enabled = false;        // enable/disable logical state (disabled is powered down)
    bool synchronized = true;    // locks movement of axis target with timer rate
    bool limitsCheck = true;     // enable/disable numeric position range limits (doesn't apply to limit switches)

    uint8_t homeSenseHandle = 0; // home sensor handle
    uint8_t minSenseHandle = 0;  // min sensor handle
    uint8_t maxSenseHandle = 0;  // max sensor handle

    uint16_t backlashStepsStore;
    uint16_t backlashAmountStepsStore;
    volatile uint16_t backlashSteps = 0;
    volatile uint16_t backlashAmountSteps = 0;
    volatile bool inBacklash = false; // true if within the backlash travel

    double target = 0.0;
    long originSteps = 0;
    volatile long targetSteps = 0;
    volatile long motorSteps = 0;
    volatile long indexSteps = 0;
    volatile int  homeSteps = 1;
    volatile int  step = 1;
    volatile int  slewStep = 1;
    volatile bool takeStep = false;
    volatile Direction direction = DIR_NONE;

    bool poweredDown = false;

    float backlashFrequency = 0.0F;
    float currentFrequency = 0.0F; // last frequency set 
    float lastFrequency = 0.0F; // last frequency requested

    unsigned long lastPeriod = 0;

    void (*_move)() = NULL;

    Encoder *enc = NULL;
};

#endif