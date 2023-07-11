// -----------------------------------------------------------------------------------
// axis step/dir motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#if defined(DRIVER_TMC_STEPPER) && defined(STEP_DIR_TMC_UART_PRESENT)

#include <TMCStepper.h> // https://github.com/teemuatlut/TMCStepper

#include "../../Drivers.h"
#include "../StepDirDriver.h"

#ifndef DRIVER_TMC_STEPPER_AUTOGRAD
  #define DRIVER_TMC_STEPPER_AUTOGRAD true
#endif

// default settings for any TMC UART drivers that may be present
#ifndef SERIAL_TMC
  #define SERIAL_TMC                  SoftSerial     // Use software serial w/ TX on M3 (CS) and RX on M4 (MISO) of each axis
#endif
#ifndef SERIAL_TMC_BAUD
  #define SERIAL_TMC_BAUD             115200         // Baud rate
#endif
#ifndef SERIAL_TMC_ADDRESS_MAP
  #define SERIAL_TMC_ADDRESS_MAP(x)   (0)            // driver addresses are 0 for all axes
#endif

#if SERIAL_TMC == SoftSerial
  #include <SoftwareSerial.h> // must be built into the board libraries
#endif

class StepDirTmcUART : public StepDirDriver {
  public:
    // constructor
    StepDirTmcUART(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings);

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

    // set the decay mode STEALTH_CHOP or SPREAD_CYCLE
    void setDecayMode(int decayMode);

    // update status info. for driver
    void updateStatus();

    // secondary way to power down not using the enable pin
    bool enable(bool state);

    // calibrate the motor driver if required
    void calibrateDriver();

  private:
    #if SERIAL_TMC == SoftSerial
      SoftwareSerial SerialTMC;
    #endif

    TMCStepper *driver;

    // checks if decay pin should be HIGH/LOW for a given decay setting
    int8_t getDecayPinState(int8_t decay);

};

#endif
