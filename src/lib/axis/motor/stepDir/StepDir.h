// -----------------------------------------------------------------------------------
// axis step/dir motor
#pragma once

#include "../../../../Common.h"

#ifdef STEP_DIR_MOTOR_PRESENT

#if defined(GPIO_DIRECTION_PINS) && defined(SHARED_DIRECTION_PINS)
  #error "Configuration (Config.h): Having both GPIO_DIRECTION_PINS and SHARED_DIRECTION_PINS is not allowed"
#endif

#include "../../../gpioEx/GpioEx.h"

#include "generic/Generic.h"
#include "tmc/legacy/tmc2130/Tmc2130.h"
#include "tmc/legacy/tmc2209/Tmc2209.h"
#include "tmc/legacy/tmc5160/Tmc5160.h"
#include "tmc/tmcStepper/tmc2130/Tmc2130.h"
#include "tmc/tmcStepper/tmc2160/Tmc2160.h"
#include "tmc/tmcStepper/tmc2208/tmc2208.h"
#include "tmc/tmcStepper/tmc2209/Tmc2209.h"
#include "tmc/tmcStepper/tmc2660/Tmc2660.h"
#include "tmc/tmcStepper/tmc5160/Tmc5160.h"
#include "tmc/tmcStepper/tmc5161/Tmc5161.h"
#include "../Motor.h"

typedef struct StepDirPins {
  int16_t step;
  uint8_t stepState;
  int16_t dir;
  int16_t enable;
  uint8_t enabledState;
} StepDirPins;

#define DirNone 253
#define DirSetRev 254
#define DirSetFwd 255

enum MicrostepModeControl: uint8_t {MMC_TRACKING, MMC_SLEWING, MMC_SLEWING_REQUEST, MMC_SLEWING_PAUSE, MMC_SLEWING_READY, MMC_TRACKING_READY};

class StepDirMotor : public Motor {
  public:
    // constructor
    StepDirMotor(uint8_t axisNumber, int8_t reverse, const StepDirPins *Pins, StepDirDriver *Driver, bool useFastHardwareTimers = true);

    // sets up the driver step/dir/enable pins
    bool init();

    // returns the number of parameters from the motor/driver
    uint8_t getParameterCount() { return Motor::getParameterCount() + driver->getParameterCount(); }

    // returns the specified axis parameter
    AxisParameter* getParameter(uint8_t number) {
      if (number > Motor::getParameterCount()) return driver->getParameter(number - Motor::getParameterCount()); else
      if (number >= 1 && number <= Motor::getParameterCount()) return Motor::getParameter(number); else
      return &invalid;
    }

    // check if parameter is valid
    bool parameterIsValid(AxisParameter* parameter, bool next = false) { 
      if (!Motor::parameterIsValid(parameter, next)) return false; else return driver->parameterIsValid(parameter, next);
    }
    
    // sets reversal of axis directions
    // \param state: true reverses the direction behavior specified in settings
    void setReverse(bool state);

    // sets motor enable on/off (if possible)
    void enable(bool value);

    // get the associated stepper driver status
    DriverStatus getDriverStatus() { if (ready) { driver->updateStatus(); return driver->getStatus(); } else return errorStatus; }

    // get movement frequency in steps per second
    float getFrequencySteps();

    // set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
    void setFrequencySteps(float frequency);

    // get tracking mode steps per slewing mode step
    inline int getStepsPerStepSlewing() { return driver->getMicrostepRatio(); }

    // microstep sequencer steps
    int getSequencerSteps() { return driver->getMicrostepMode(); };

    // set slewing state (hint that we are about to slew or are done slewing)
    void setSlewing(bool state);

    // calibrate stealthChop then return to tracking mode
    void calibrateDriver() {
      if (!ready) return;
      digitalWriteEx(Pins->enable, Pins->enabledState);
      driver->calibrateDriver();
      digitalWriteEx(Pins->enable, !Pins->enabledState);
    }
  
    #if defined(GPIO_DIRECTION_PINS)
      // monitor and respond to motor state as required
      void poll() { updateMotorDirection(); }

      // change motor direction on request by polling
      void updateMotorDirection();
    #endif

    // sets dir as required and moves coord toward target at setFrequencySteps() rate
    void move(const int16_t stepPin);

    // fast forward axis movement, no backlash, no mode switching
    void moveFF(const int16_t stepPin);

    // fast reverse axis movement, no backlash, no mode switching
    void moveFR(const int16_t stepPin);

    // get the motor name
    const char* name() { strcpy(nameStr, "Step/Dir, "); strcat(nameStr, driver->name()); return nameStr; }

    // a stepper motor driver, should not be used above the StepDir class
    StepDirDriver *driver;

    // the stepper motor driver pins, should not be used above the StepDir class
    const StepDirPins *Pins;

  private:
    // switch microstep modes as needed
    void modeSwitch();

    // swaps in/out fast unidirectional ISR for slewing 
    bool enableMoveFast(const bool state);

    uint8_t taskHandle = 0;

    #ifdef DRIVER_STEP_DEFAULTS
      #define stepClr LOW                // pin state to reset driver before taking a step
      #define stepSet HIGH               // pin state to take a step
    #else
      volatile uint8_t stepClr = LOW;    // pin state to reset driver before taking a step
      volatile uint8_t stepSet = HIGH;   // pin state to take a step
    #endif
    volatile uint8_t dirFwd = LOW;       // pin state for forward direction
    volatile uint8_t dirRev = HIGH;      // pin state for reverse direction
    volatile uint8_t direction = LOW;    // current direction in use
    volatile uint32_t pulseWidth = 2000; // step/dir driver pulse width in nanoseconds

    volatile int16_t stepSize = 1;       // step size during slews (for micro-step mode switching)
    volatile bool takeStep = false;      // should we take a step

    float currentFrequency = 0.0F;       // last frequency set 
    float lastFrequency = 0.0F;          // last frequency requested
    unsigned long lastPeriod = 0;        // last timer period (in sub-micros)
    unsigned long lastPeriodSet = 0;     // last timer period actually set (in sub-micros)
    unsigned long switchStartTimeMs;     // log time to switch microstep mode and do ISR swap

    volatile MicrostepModeControl microstepModeControl = MMC_TRACKING;

    bool useFastHardwareTimers = true;

    void (*callback)() = NULL;
    void (*callbackFF)() = NULL;
    void (*callbackFR)() = NULL;
};

#endif