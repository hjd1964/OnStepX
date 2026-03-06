// -----------------------------------------------------------------------------------
// axis step/dir motor driver, TMC2660Stepper
#pragma once

#include <Arduino.h>
#include "../../../../../../../Common.h"

#if defined(DRIVER_TMC_STEPPER) && defined(TMC2660_PRESENT)

#include <TMCStepper.h> // https://github.com/teemuatlut/TMCStepper

#include "../../../../Drivers.h"
#include "../../TmcStepDirDriverNSG.h"

class StepDirTmc2660 : public TmcStepDirDriver {
  public:
    // constructor
    StepDirTmc2660(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings, int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t  intpol);

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
    void calibrateDriver() {}

  private:
    // read status from driver
    void readStatus();

    // set peak current current multiplier
    inline void current(float mA) { driver->rms_current(mA*0.7071F); }

    TMC2660Stepper *driver;
};

#endif
