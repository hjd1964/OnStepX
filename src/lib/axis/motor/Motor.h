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
    // sets up the motor identification
    virtual bool init();

    // set motor reverse state
    virtual void setReverse(int8_t state);

    // get motor parameters type code
    virtual char getParameterTypeCode();

    // get motor default parameters
    void getDefaultParameters(float *param1, float *param2, float *param3, float *param4, float *param5, float *param6);

    // set motor default parameters
    void setDefaultParameters(float param1, float param2, float param3, float param4, float param5, float param6);

    // set motor parameters
    virtual void setParameters(float param1, float param2, float param3, float param4, float param5, float param6);

    // validate motor parameters
    virtual bool validateParameters(float param1, float param2, float param3, float param4, float param5, float param6);

    // sets motor enable on/off (if possible)
    virtual void enable(bool value);

    // get the associated stepper motor driver status
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
    virtual float getFrequencySteps();

    // set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
    virtual void setFrequencySteps(float frequency);

    // set backlash frequency in steps per second
    virtual void setBacklashFrequencySteps(float frequency);

    // get tracking mode steps per slewing mode step
    virtual int getStepsPerStepSlewing();

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

    // set slewing state (hint that we are about to slew or are done slewing)
    virtual void setSlewing(bool state);

    // calibrate the motor if required
    virtual void calibrate(float value) { UNUSED(value); }

    // calibrate the motor driver if required
    virtual void calibrateDriver() {}

    // set zero of absolute encoders
    virtual uint32_t encoderZero() { return 0; }

    // return the encoder count, if present
    virtual int32_t getEncoderCount() { return 0; }

    // set origin of absolute encoders
    virtual void encoderSetOrigin(uint32_t origin) {}

    // monitor and respond to motor state as required
    virtual void poll() {}

    int driverType = OFF;
    volatile bool inBacklash = false;          // must be true if within the backlash travel

    volatile uint8_t monitorHandle = 0;        // handle to the axis task monitor

    bool enabled = false;                      // enable/disable logical state

    bool calibrating = false;                  // shadow disable when calibrating

  protected:
    // disable backlash compensation, to work properly there must be an enable call to match
    void disableBacklash();

    // enable backlash compensation, to work properly this must be proceeded by a disable call
    void enableBacklash();

    volatile uint8_t axisNumber = 0;           // axis number for this motor (1 to 9 in OnStepX)
    char axisPrefix[16];                       // prefix for debug messages

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

    float default_param1 = 0, default_param2 = 0, default_param3 = 0, default_param4 = 0, default_param5 = 0, default_param6 = 0;
};

#endif