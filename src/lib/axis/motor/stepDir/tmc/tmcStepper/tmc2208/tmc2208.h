// -----------------------------------------------------------------------------------
// axis step/dir motor driver, TMC2208Stepper
#pragma once

#include <Arduino.h>
#include "../../../../../../../Common.h"

#if defined(DRIVER_TMC_STEPPER) && defined(TMC2208_PRESENT)

#include <TMCStepper.h> // https://github.com/teemuatlut/TMCStepper

#include "../../../../Drivers.h"
#include "../../TmcStepDirDriver.h"

#if SERIAL_TMC == SoftSerial
  #include <SoftwareSerial.h> // must be built into the board libraries
#endif

class StepDirTmc2208 : public TmcStepDirDriver {
  public:
    // constructor
    StepDirTmc2208(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings, int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t  intpol);

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

    TMC2208Stepper *driver;
};

#endif
