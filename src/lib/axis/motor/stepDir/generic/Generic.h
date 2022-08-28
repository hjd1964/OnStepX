// -----------------------------------------------------------------------------------
// axis step/dir motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#ifdef STEP_DIR_LEGACY_PRESENT

#include "../../Drivers.h"
#include "../StepDirDriver.h"

class StepDirGeneric : StepDirDriver {
  public:
    // constructor
    StepDirGeneric(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings);

    // get driver type code
    inline char getParameterTypeCode() { return 'S'; }

    // set up driver and parameters: microsteps, microsteps goto
    void init(float param1, float param2, float param3, float param4, float param5, float param6);

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

  private:
    // checks if decay pin should be HIGH/LOW for a given decay setting
    int8_t getDecayPinState(int8_t decay);

    // checkes if decay control is on the M2 pin
    bool isDecayOnM2();

    uint8_t microstepBitCode       = 0;
    uint8_t microstepBitCodeM0     = 0;
    uint8_t microstepBitCodeM1     = 0;
    uint8_t microstepBitCodeM2     = 0;
    uint8_t microstepBitCodeGoto   = 0;
    uint8_t microstepBitCodeGotoM0 = 0;
    uint8_t microstepBitCodeGotoM1 = 0;
    uint8_t microstepBitCodeGotoM2 = 0;
    int16_t m0Pin                  = OFF;
    int16_t m1Pin                  = OFF;
    int16_t m2Pin                  = OFF;
    int16_t decayPin               = OFF;
};

#endif
