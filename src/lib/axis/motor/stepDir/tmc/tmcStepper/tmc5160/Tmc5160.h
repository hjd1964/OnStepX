// -----------------------------------------------------------------------------------
// axis step/dir motor driver, TMC5160Stepper 
#pragma once

#include <Arduino.h>
#include "../../../../../../../Common.h"

#if defined(DRIVER_TMC_STEPPER) && defined(TMC5160_PRESENT)

#include <TMCStepper.h> // https://github.com/teemuatlut/TMCStepper

#include "../../../../Drivers.h"
#include "../../TmcStepDirDriver.h"

class StepDirTmc5160 : public TmcStepDirDriver {
  public:
    // constructor
    StepDirTmc5160(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings, int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t  intpol);

    // setup driver
    bool init();

    // set microstep mode for tracking
    void modeMicrostepTracking();

    // set microstep mode for slewing
    int modeMicrostepSlewing();

    // set decay mode for tracking
    void modeDecayTracking();

    // set decay mode for slewing
    void modeDecaySlewing();

    // secondary way to power down not using the enable pin
    bool enable(bool state);

    // calibrate the motor driver if required
    void calibrateDriver();

  private:
    // read status from driver
    void readStatus();

    // set the decay mode STEALTH_CHOP or SPREAD_CYCLE
    void setDecayMode(int decayMode);

    // set peak current and hold current multiplier
    inline void current(float mA, float mult) { driver->rms_current(mA*0.7071F, mult); }

    TMC5160Stepper *driver;
};

#endif
