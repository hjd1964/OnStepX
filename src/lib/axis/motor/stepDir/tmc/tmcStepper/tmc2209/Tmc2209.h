// -----------------------------------------------------------------------------------
// axis step/dir motor driver, TMC2209Stepper
#pragma once

#include <Arduino.h>
#include "../../../../../../../Common.h"

#if defined(DRIVER_TMC_STEPPER) && defined(TMC2209_PRESENT)

#include <TMCStepper.h> // https://github.com/teemuatlut/TMCStepper

#include "../../../../Drivers.h"
#include "../../TmcStepDirDriverSG.h"

class StepDirTmc2209 : public TmcStepDirDriverSG {
  public:
    // constructor
    StepDirTmc2209(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings, int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t  intpol);

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

    // set the decay mode STEALTH_CHOP or SPREAD_CYCLE
    void setDecayMode(int decayMode);

    // secondary way to power down not using the enable pin
    bool enable(bool state);

    // read the 32bit stallguard register
    uint32_t readSgRegister() override { return (uint32_t)(driver->SG_RESULT() & 0x03FF); }

    // SG only meaningful in stealthChop (not spreadCycle)
    // and SG is only active while slewing so we assume this is representative of the state
    bool stallGuardModeActive() const override { return decaySlewing.value == STEALTHCHOP; }

    // apply normalized sensitivity percent (0..100) to the concrete chip register
    // (e.g., 2209: SGTHRS, 2130: SGT). Called only when % changes.
    void applySgSensitivityPercent(uint8_t pct) override {
      uint8_t sgthrs = (uint8_t)(((int)pct * 255 + 50) / 100);
      driver->SGTHRS(sgthrs);
      sgAppliedReg = (int16_t)sgthrs;
    }

    // calibrate the motor driver if required
    void calibrateDriver();

  private:
    // read status from driver
    void readStatus();

    // set peak current and hold current multiplier
    inline void current(float mA, float mult) { driver->rms_current(round(mA*0.7071F), mult); }

    #if SERIAL_TMC == SoftSerial
      SoftwareSerial *SerialTMC;
    #endif

    TMC2209Stepper *driver;
};

#endif
