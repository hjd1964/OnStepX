// -----------------------------------------------------------------------------------
// axis step/dir motor driver, TMC2160Stepper
#pragma once

#include <Arduino.h>
#include "../../../../../../../Common.h"

#if defined(DRIVER_TMC_STEPPER) && defined(TMC2160_PRESENT)

#include <TMCStepper.h> // https://github.com/teemuatlut/TMCStepper

#include "../../../../Drivers.h"
#include "../../TmcStepDirDriverSG.h"

class StepDirTmc2160 : public TmcStepDirDriverSG {
  public:
    // constructor
    StepDirTmc2160(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings, int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t  intpol);

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

    // read the 32bit stallguard register
    uint32_t readSgRegister() override { return driver->DRV_STATUS(); }

    // SG only meaningful in spreadCycle (not stealthChop)
    // and SG is only active while slewing so we assume this is representative of the state
    bool stallGuardModeActive() const override { return decaySlewing.value == SPREADCYCLE; }

    // apply normalized sensitivity percent (0..100) to the concrete chip register
    // (e.g., 2209: SGTHRS, 2130: SGT). Called only when % changes.
    void applySgSensitivityPercent(uint8_t pct) override {
      int sgt = 63 - (((int)pct * 127 + 50) / 100);   // yields +63..-64
      driver->sgt((int8_t)sgt);
      sgAppliedReg = (int16_t)sgt;
    }

    // calibrate the motor driver if required
    void calibrateDriver();

  private:
    // read status from driver
    void readStatus();

    // set the decay mode STEALTH_CHOP or SPREAD_CYCLE
    void setDecayMode(int decayMode);

    // set peak current and hold current multiplier
    inline void current(float mA, float mult) { driver->rms_current(mA*0.7071F, mult); }

    TMC2160Stepper *driver;
};

#endif
