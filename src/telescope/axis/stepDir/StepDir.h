// -----------------------------------------------------------------------------------
// Step Dir motor control
#pragma once

#include <Arduino.h>
#include "../../../Common.h"
#include "../../../commands/ProcessCmds.h"
#include "StepDrivers.h"

#if AXIS1_DRIVER_STEP == AXIS2_DRIVER_STEP == AXIS3_DRIVER_STEP == AXIS1_DRIVER_STEP == AXIS1_DRIVER_STEP == AXIS1_DRIVER_STEP == AXIS1_DRIVER_STEP == AXIS1_DRIVER_STEP == AXIS1_DRIVER_STEP == HIGH
  #define DRIVER_STEP_DEFAULTS
#endif

typedef struct StepDirPins {
  int16_t   step;
  uint8_t   stepState;
  int16_t   dir;
  int16_t   enable;
  uint8_t   enabledState;
} StepDirPins;

enum MicrostepModeControl: uint8_t {MMC_TRACKING, MMC_SLEWING_REQUEST, MMC_SLEWING_READY, MMC_SLEWING};
enum Direction: uint8_t {DIR_NONE, DIR_FORWARD, DIR_REVERSE, DIR_BOTH};

class StepDir {
  public:
    // sets up the driver step/dir/enable pins and any associated stepDriver mode control
    bool init(uint8_t axisNumber, int8_t reverse, int16_t microsteps, int16_t currentRun);

    // sets motor power on/off (if possible)
    void power(bool value);

    // set backlash in steps
    void setBacklashSteps(long value);
    // get backlash in steps
    long getBacklashSteps();

    // sets motor and target coordinates in steps, also zeros backlash and index 
    void setMotorCoordinateSteps(long value);
    // get motor coordinate in steps
    long getMotorCoordinateSteps();

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
    void setTargetCoordinateParkSteps(long value, int modulo);

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

    // monitor movement
    void poll();

    // sets dir as required and moves coord toward target at setFrequencySteps() rate
    void move(const int8_t stepPin, const int8_t dirPin);
    // fast forward axis movement, no backlash, no mode switching
    void moveFF(const int8_t stepPin);
    // fast reverse axis movement, no backlash, no mode switching
    void moveFR(const int8_t stepPin);

    // swaps in/out fast unidirectional ISR for slewing 
    bool enableMoveFast(const bool state);

    StepDriver stepDriver;

  private:
    // disable backlash compensation, to work properly there must be an enable call to match
    void disableBacklash();
    // enable backlash compensation, to work properly this must be proceeded by a disable call
    void enableBacklash();
    // returns true if traveling through backlash
    bool inBacklash();
    
    StepDirPins pins;

    uint8_t taskHandle = 0;
    uint8_t axisNumber = 0;
    char axisPrefix[14] = "MSG: Motor_, ";

    bool enabled = false;        // enable/disable logical state (disabled is powered down)
    bool tracking = true;        // locks movement of axis target with timer rate
    bool limitsCheck = true;     // enable/disable numeric position range limits (doesn't apply to limit switches)

    uint8_t homeSenseHandle = 0; // home sensor handle
    uint8_t minSenseHandle = 0;  // min sensor handle
    uint8_t maxSenseHandle = 0;  // max sensor handle

    uint16_t backlashStepsStore;
    volatile uint16_t backlashSteps = 0;
    volatile uint16_t backlashAmountSteps = 0;

    #ifdef DRIVER_STEP_DEFAULTS
      #define stepClr LOW
      #define stepSet HIGH
    #else
      volatile uint8_t stepClr = LOW;
      volatile uint8_t stepSet = HIGH;
    #endif
    volatile uint8_t dirFwd = LOW;
    volatile uint8_t dirRev = HIGH;

    double target = 0.0;
    long originSteps = 0;
    volatile long targetSteps = 0;
    volatile long motorSteps = 0;
    volatile long indexSteps = 0;
    volatile int  trackingStep = 1;
    volatile int  switchStep = 1;
    volatile int  slewStep = 1;
    volatile int  step = 1;
    volatile bool takeStep = false;
    volatile Direction direction = DIR_NONE;

    bool poweredDown = false;

    float backlashFrequency = 0.0F;
    float lastFrequency = 0.0F;

    float minPeriodMicros;
    unsigned long lastPeriod = 0;

    volatile MicrostepModeControl microstepModeControl = MMC_TRACKING;

    void (*_move)() = NULL;
    void (*_moveFF)() = NULL;
    void (*_moveFR)() = NULL;
};
