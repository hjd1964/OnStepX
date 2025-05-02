// -----------------------------------------------------------------------------------
// axis step/dir motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#if defined(DRIVER_TMC_STEPPER) && defined(STEP_DIR_TMC_SPI_PRESENT)

#include <TMCStepper.h> // https://github.com/teemuatlut/TMCStepper

#include "../../Drivers.h"
#include "../StepDirDriver.h"

#ifndef DRIVER_TMC_STEPPER_AUTOGRAD
  #define DRIVER_TMC_STEPPER_AUTOGRAD false
#endif

class StepDirTmcSPI : public StepDirDriver {
  public:
    // constructor
    StepDirTmcSPI(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings);

    // get driver type code
    inline char getParameterTypeCode() { return 'T'; }

    // setup driver
    bool init();

    // validate driver parameters
    bool validateParameters(float param1, float param2, float param3, float param4, float param5, float param6);

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

    void *driver;

  private:
    // read status from driver
    void readStatus();

    // checks if decay pin should be HIGH/LOW for a given decay setting
    int8_t getDecayPinState(int8_t decay);

    // set the decay mode STEALTH_CHOP or SPREAD_CYCLE
    void setDecayMode(int decayMode);

    // set peak current and hold current multiplier
    inline void current(float mA, float mult) {
      if (settings.model == TMC2130) { ((TMC2130Stepper*)driver)->rms_current(mA*0.7071F, mult); } else
      if (settings.model == TMC2660) { ((TMC2660Stepper*)driver)->rms_current(mA*0.7071F); } else
      if (settings.model == TMC5160) { ((TMC5160Stepper*)driver)->rms_current(mA*0.7071F, mult); } else
      if (settings.model == TMC5161) { ((TMC5161Stepper*)driver)->rms_current(mA*0.7071F, mult); }
    }
};

#endif
