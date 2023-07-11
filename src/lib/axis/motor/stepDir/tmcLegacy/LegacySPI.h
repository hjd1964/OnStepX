// -----------------------------------------------------------------------------------
// axis step/dir motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#if !defined(DRIVER_TMC_STEPPER) && defined(STEP_DIR_TMC_SPI_PRESENT)

#include "TmcSPI.h"
#include "../StepDirDriver.h"

class StepDirTmcSPI : public StepDirDriver {
  public:
    // constructor
    StepDirTmcSPI(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings);

    // get driver type code
    inline char getParameterTypeCode() { return 'T'; }

    // set up driver and parameters: microsteps, microsteps goto, hold current, run current, goto current, unused
    void init(float param1, float param2, float param3, float param4, float param5, float param6);

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

    // update status info. for driver
    void updateStatus();

    // secondary way to power down not using the enable pin
    bool enable(bool state);

    // calibrate the motor driver if required
    void calibrateDriver();

    TmcSPI driver;

  private:
    // checks if decay pin should be HIGH/LOW for a given decay setting
    int8_t getDecayPinState(int8_t decay);
};

#endif
