// -----------------------------------------------------------------------------------
// Generic motor controller
#pragma once

#include <Arduino.h>
#include "../../../Common.h"
#include "Drivers.h"

#define SERVO    -1  // general purpose flag for a SERVO driver motor
#define STEP_DIR -2  // general purpose flag for a STEP_DIR driver motor

enum Direction: uint8_t {DIR_NONE, DIR_FORWARD, DIR_REVERSE, DIR_BOTH};

class Motor {
  public:
    // sets up the motor identification
    virtual bool init(void (*volatile move)(), void (*volatile moveFF)() = NULL, void (*volatile moveFR)() = NULL);

    // set driver reverse state
    virtual void setReverse(int8_t state);

    // set default driver parameters (microsteps and current for SD drivers)
    virtual void setParam(int16_t param1, int16_t param2);

    // sets motor power on/off (if possible)
    virtual void power(bool value);

    // get the associated stepper driver status
    virtual DriverStatus getDriverStatus();

    // resets motor and target angular position in steps, also zeros backlash and index 
    virtual void resetPositionSteps(long value);

    // resets target position to the motor position
    void resetTargetToMotorPosition();

    // get motor position in steps (including backlash)
    long getMotorPositionSteps();

    // get index position in steps
    inline long getIndexPositionSteps() { return indexSteps; }

    // get instrument coordinate, in steps
    long getInstrumentCoordinateSteps();

    // set instrument coordinate, in steps
    void setInstrumentCoordinateSteps(long value);

    // set instrument park coordinate, in steps
    // should only be called when the axis is not moving
    void setInstrumentCoordinateParkSteps(long value, int modulo);

    // get target coordinate (with index), in steps
    long getTargetCoordinateSteps();

    // set target coordinate (with index), in steps
    void setTargetCoordinateSteps(long value);

    // set target park coordinate, in steps (taking into account stepper motor cogging when powered off)
    // should only be called when the axis is not moving
    void setTargetCoordinateParkSteps(long value, int modulo);

    // get backlash amount in steps
    long getBacklashSteps();

    // set backlash amount in steps
    void setBacklashSteps(long value);

    // mark origin coordinate at current location
    void markOriginCoordinateSteps();

    // distance to target in steps (+/-)
    long getTargetDistanceSteps();

    // distance to origin or target, whichever is closer, in steps
    long getOriginOrTargetDistanceSteps();

    // returns 1 if distance to origin is closer else -1 if target is closer
    int getRampDirection();

    // get movement frequency in steps per second
    virtual float getFrequencySteps();

    // set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
    virtual void setFrequencySteps(float frequency);

    // get tracking mode steps per slewing mode step
    virtual int getStepsPerStepSlewing();

    // set backlash frequency in steps per second
    void setBacklashFrequencySteps(float frequency);

    // get synchronized state (automatic movement of target at setFrequencySteps() rate)
    inline bool getSynchronized() { return synchronized; }

    // set synchronized state (automatic movement of target at setFrequencySteps() rate)
    inline void setSynchronized(bool state) { synchronized = state; }

    // get the current direction of motion
    Direction getDirection();

    // set slewing state (hint that we are about to slew or are done slewing)
    virtual void setSlewing(bool state);

    int driverType = OFF;
    volatile bool inBacklash = false;          // must be true if within the backlash travel

  protected:
    // disable backlash compensation, to work properly there must be an enable call to match
    void disableBacklash();

    // enable backlash compensation, to work properly this must be proceeded by a disable call
    void enableBacklash();

    volatile uint8_t axisNumber = 0;           // axis number for this motor (1 to 9 in OnStepX)
    char axisPrefix[14] = "MSG: Motor_, ";     // prefix for debug messages

    bool enabled = false;                      // enable/disable logical state (disabled is powered down)
    bool synchronized = true;                  // locks movement of axis target with timer rate
    bool limitsCheck = true;                   // enable/disable numeric range limits (doesn't apply to limit switches)

    uint8_t homeSenseHandle = 0;               // home sensor handle
    uint8_t minSenseHandle = 0;                // limit min sensor handle
    uint8_t maxSenseHandle = 0;                // limit max sensor handle

    float backlashFrequency = 0.0F;
    volatile uint16_t backlashSteps = 0;       // count of motor steps taken for backlash
    uint16_t backlashStepsStore;               // temporary storage for the position in backlash
    volatile uint16_t backlashAmountSteps = 0; // the amount of backlash travel
    uint16_t backlashAmountStepsStore;         // temporary storage for the amount of backlash travel

    long originSteps = 0;                      // start position for an autoSlewRateByDistance()
    volatile long targetSteps = 0;             // where we want the motor
    volatile long motorSteps = 0;              // where the motor is not counting backlash
    volatile long indexSteps = 0;              // for absolute motor position to axis position
    volatile int  step = 1;                    // step size, and for direction control

    bool poweredDown = false;

};
