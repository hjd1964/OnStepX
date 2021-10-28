// -----------------------------------------------------------------------------------
// axis step/dir motor
#pragma once

#include "../../../../Common.h"

#ifdef SD_DRIVER_PRESENT

#include "StepDirDrivers.h"
#include "../Motor.h"

#define DirSetRev 253
#define DirSetFwd 254
#define DirNone 255

typedef struct StepDirPins {
  int16_t   step;
  uint8_t   stepState;
  int16_t   dir;
  int16_t   enable;
  uint8_t   enabledState;
} StepDirPins;

enum MicrostepModeControl: uint8_t {MMC_TRACKING, MMC_SLEWING, MMC_SLEWING_REQUEST, MMC_SLEWING_PAUSE, MMC_SLEWING_READY, MMC_TRACKING_READY};

class StepDirMotor : public Motor {
  public:
    // constructor
    StepDirMotor(uint8_t axisNumber, const StepDirPins *Pins, StepDirDriver *driver);

    // sets up the driver step/dir/enable pins
    bool init(void (*volatile move)(), void (*volatile moveFF)() = NULL, void (*volatile moveFR)() = NULL);

    // set driver default reverse state
    void setReverse(int8_t state);

    // set default driver microsteps and current
    void setParam(int16_t microsteps, int16_t current);

    // sets motor power on/off (if possible)
    void power(bool value);

    // get the associated stepper driver status
    DriverStatus getDriverStatus();

    // get movement frequency in steps per second
    float getFrequencySteps();

    // set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
    void setFrequencySteps(float frequency);

    // get tracking mode steps per slewing mode step
    inline int getStepsPerStepSlewing() { return driver->getMicrostepRatio(); }

    // switch microstep modes as needed
    void modeSwitch();

    // swaps in/out fast unidirectional ISR for slewing 
    bool enableMoveFast(const bool state);

    // set slewing state (hint that we are about to slew or are done slewing)
    void setSlewing(bool state);

    // monitor and respond to motor state as required
    void poll();

    // change motor direction
    void updateMotorDirection();

    // sets dir as required and moves coord toward target at setFrequencySteps() rate
    void move(const int8_t stepPin);

    // fast forward axis movement, no backlash, no mode switching
    void moveFF(const int8_t stepPin);

    // fast reverse axis movement, no backlash, no mode switching
    void moveFR(const int8_t stepPin);

    // a stepper motor driver, should not be used outside of the StepDir class
    StepDirDriver *driver;

  private:
    const StepDirPins *Pins;

    uint8_t taskHandle = 0;
    volatile uint8_t mtrHandle = 0;

    #ifdef DRIVER_STEP_DEFAULTS
      #define stepClr LOW               // pin state to reset driver before taking a step
      #define stepSet HIGH              // pin state to take a step
    #else
      volatile uint8_t stepClr = LOW;   // pin state to reset driver before taking a step
      volatile uint8_t stepSet = HIGH;  // pin state to take a step
    #endif
    volatile uint8_t dirFwd = LOW;      // pin state for forward direction
    volatile uint8_t dirRev = HIGH;     // pin state for reverse direction
    volatile uint8_t direction = LOW;   // current direction in use

    volatile int  homeSteps = 1;        // step count for microstep sequence between home positions (driver indexer)
    volatile int  slewStep = 1;         // step size during slews (for micro-step mode switching)
    volatile bool takeStep = false;     // should we take a step

    float currentFrequency = 0.0F;      // last frequency set 
    float lastFrequency = 0.0F;         // last frequency requested
    unsigned long lastPeriod = 0;       // last timer period (in sub-micros)

    volatile MicrostepModeControl microstepModeControl = MMC_TRACKING;

    void (*_move)() = NULL;
    void (*_moveFF)() = NULL;
    void (*_moveFR)() = NULL;
};

#endif