// -----------------------------------------------------------------------------------
// Generic motor controller
#pragma once

#include <Arduino.h>
#include "../../../Common.h"
#include "Drivers.h"

#ifdef MOTOR_PRESENT

enum Direction: uint8_t {DIR_NONE, DIR_FORWARD, DIR_REVERSE, DIR_BOTH};

#ifndef STEP_WAVE_FORM
#define STEP_WAVE_FORM SQUARE
#endif

class Motor {
  public:
    Motor(uint8_t axisNumber, int8_t reverse);

    // sets up the motor identification
    virtual bool init();

    // returns the number of parameters from the motor and motor driver
    virtual uint8_t getParameterCount() { return numParameters; }

    // returns the specified axis parameter
    virtual AxisParameter* getParameter(uint8_t number) { if (number > numParameters) return &invalid; else return parameter[number]; }

    // check if parameter is valid
    virtual bool parameterIsValid(AxisParameter* parameter, bool next = false);

    // returns the specified axis parameter by name
    AxisParameter* getParameterByName(const char* name);

    // sets reversal of axis directions
    // \param state: true reverses the direction behavior specified in settings
    virtual void setReverse(bool state) { reversed = state; }

    // sets motor enable on/off (if possible)
    virtual void enable(bool value) {}

    // get the associated stepper motor driver status
    virtual DriverStatus getDriverStatus() { return errorStatus; }

    // resets motor and target angular position in steps, also zeros backlash and index 
    virtual void resetPositionSteps(long value);

    // resets target position to the motor position
    void resetTargetToMotorPosition();

    // get motor position in steps (including backlash)
    long getMotorPositionSteps();

    // get index position in steps
    inline long getIndexPositionSteps() { return indexSteps; }

    // get instrument coordinate, in steps
    virtual long getInstrumentCoordinateSteps();

    // set instrument coordinate, in steps
    virtual void setInstrumentCoordinateSteps(long value);

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
    virtual long getTargetDistanceSteps();

    // distance to origin or target, whichever is closer, in steps
    long getOriginOrTargetDistanceSteps();

    // returns 1 if distance to origin is closer else -1 if target is closer
    int getRampDirection();

    // get movement frequency in steps per second
    virtual float getFrequencySteps() { return 0; }

    // set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
    virtual void setFrequencySteps(float frequency) {}

    // set backlash frequency in steps per second
    virtual void setBacklashFrequencySteps(float frequency);

    // get tracking mode steps per slewing mode step
    virtual int getStepsPerStepSlewing() { return 1; }

    // get synchronized state (automatic movement of target at setFrequencySteps() rate)
    inline bool getSynchronized() { return sync; }

    // set synchronized state (automatic movement of target at setFrequencySteps() rate)
    virtual inline void setSynchronized(bool state) {
      if (state) {
        noInterrupts();
        sync = state;
        targetSteps = motorSteps;
        interrupts();
      } else sync = state;
    }

    // get the current direction of motion
    Direction getDirection();

    // microstep sequencer steps
    int getSequencerSteps() { return 1; };

    // set slewing state (hint that we are about to slew or are done slewing)
    virtual void setSlewing(bool state) {}

    // calibrate the motor if required
    virtual void calibrate(float value) { UNUSED(value); }

    // calibrate the motor driver if required
    virtual void calibrateDriver() {}

    // set zero of absolute encoders
    virtual uint32_t encoderZero() { return 0; }

    // return the encoder count, if present
    virtual int32_t getEncoderCount() { return 0; }

    // set origin of absolute encoders
    virtual void encoderSetOrigin(uint32_t origin) { UNUSED(origin); }

    // get the motor name
    virtual const char* name() { return NULL; }

    // monitor and respond to motor state as required
    virtual void poll() {}

    int driverType = OFF;
    volatile bool inBacklash = false;          // must be true if within the backlash travel

    volatile uint8_t monitorHandle = 0;        // handle to the axis task monitor

    bool enabled = false;                      // enable/disable logical state

    bool ready = false;                        // set to true after successful init

    bool calibrating = false;                  // shadow disable when calibrating

  protected:
    // disable backlash compensation, to work properly there must be an enable call to match
    void disableBacklash();

    // enable backlash compensation, to work properly this must be proceeded by a disable call
    void enableBacklash();

    volatile uint8_t axisNumber = 0;           // axis number for this motor (1 to 9 in OnStepX)
    char axisPrefix[32];                       // prefix for debug messages
    char nameStr[40];                          // name of this motor/driver

    volatile bool sync = true;                 // locks movement of axis target with timer rate
    bool limitsCheck = true;                   // enable/disable numeric range limits (doesn't apply to limit switches)

    uint8_t homeSenseHandle = 0;               // home sensor handle
    uint8_t minSenseHandle = 0;                // limit min sensor handle
    uint8_t maxSenseHandle = 0;                // limit max sensor handle

    float backlashFrequency = 0.0F;
    volatile uint16_t backlashSteps = 0;       // count of motor steps taken for backlash
    uint16_t backlashStepsStore;               // temporary storage for the position in backlash
    volatile uint16_t backlashAmountSteps = 0; // the amount of backlash travel
    uint16_t backlashAmountStepsStore;         // temporary storage for the amount of backlash travel

    long originSteps = 0;                      // start position for an autoGoto()
    volatile long targetSteps = 0;             // where we want the motor
    volatile long motorSteps = 0;              // where the motor is not counting backlash
    volatile long indexSteps = 0;              // for absolute motor position to axis position
    volatile long step = 1;                    // step size, and for direction control

    // a fault return status
    const DriverStatus errorStatus = {false, {false, false}, {false, false}, false, false, false, true};

    // reverses the set direction of motion
    bool reversed = false;

    // set direction of motion
    bool normalizedReverse = false;

    // runtime adjustable settings
    const int numParameters = 1;
    AxisParameter invalid = {NAN, NAN, NAN, NAN, NAN, AXP_INVALID, ""};
    AxisParameter reverse = {NAN, NAN, NAN, 0, 1, AXP_BOOLEAN, AXPN_REVERSE};

    AxisParameter* parameter[2] = {&invalid, &reverse};
};

#endif
