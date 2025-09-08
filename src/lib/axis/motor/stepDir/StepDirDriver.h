// -----------------------------------------------------------------------------------
// axis step/dir motor driver
#pragma once

#include <Arduino.h>
#include "../../../../Common.h"

#ifdef STEP_DIR_MOTOR_PRESENT

#include "../Drivers.h"
#include "StepDirDriver.h"

typedef struct StepDirDriverPins {
  int16_t m0;
  int16_t m1;
  int16_t m2;
  uint8_t m2State;
  int16_t m3;
  int16_t decay;
  int16_t fault;
} StepDirDriverPins;

typedef struct StepDirDriverSettings {
  int16_t model;
  int8_t  status;
  int16_t microsteps;
  int16_t microstepsSlewing;
  int8_t  decay;
  int8_t  decaySlewing;
} StepDirDriverSettings;

class StepDirDriver {
  public:
    StepDirDriver(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings);

    // set up driver
    virtual bool init();

    // returns the number of axis parameters
    virtual uint8_t getParameterCount() { return 0; }

    // returns the specified axis parameter
    virtual AxisParameter* getParameter(uint8_t number) { return &invalid; }

    // check if axis parameter is valid
    virtual bool parameterIsValid(AxisParameter* parameter, bool next = false);

    // set microstep mode for tracking
    virtual void modeMicrostepTracking() {}

    // set microstep mode for slewing
    virtual int modeMicrostepSlewing() { return 1; }

    // set decay mode for tracking
    virtual void modeDecayTracking() {}

    // set decay mode for slewing
    virtual void modeDecaySlewing() {}

    // get microstep ratio for slewing
    inline int getMicrostepRatio() { return microstepRatio; }

    // get microstep mode used for tracking
    inline int getMicrostepMode() { return normalizedMicrosteps; }

    // update status info. for driver
    virtual void updateStatus();

    // get status info.
    inline DriverStatus getStatus() { return status; }

    // secondary way to power down not using the enable pin
    virtual bool enable(bool state) { UNUSED(state); return false; }

    // calibrate the motor driver if required
    virtual void calibrateDriver() {}

    // get the pulse width in nanoseconds, if unknown (-1) returns 2000 nanoseconds
    long getPulseWidth();

    // get the driver name
    virtual const char* name();

    // true if switching microstep modes at low speed is allowed
    bool modeSwitchAllowed = false;

    // true if switching microstep modes at high speed is allowed
    bool modeSwitchFastAllowed = false;

  protected:
    // read status from driver
    virtual void readStatus() {}

    // different models of stepper drivers have different bit settings for microsteps
    // translate the human readable microsteps in the configuration to mode bit settings
    // returns bit code (0 to 7) or OFF if microsteps is not supported or unknown
    int subdivisionsToCode(long microsteps);

    uint8_t axisNumber;
    char axisPrefix[32]; // prefix for debug messages

    DriverStatus status = {false, {false, false}, {false, false}, false, false, false, false};
    #if DEBUG != OFF
      DriverStatus lastStatus = {false, {false, false}, {false, false}, false, false, false, false};
    #endif
    unsigned long timeLastStatusUpdate = 0;

    const StepDirDriverPins *Pins;

    int16_t driverModel = 0;
    int16_t statusMode;
    int16_t normalizedMicrosteps;
    int16_t normalizedMicrostepsSlewing;
    int16_t normalizedDecay;
    int16_t normalizedDecaySlewing;
    int16_t microstepRatio = 1;
    int16_t microstepCode = OFF;
    int16_t microstepCodeSlewing = OFF;

    // runtime adjustable settings
    AxisParameter invalid           = {NAN, NAN, NAN, NAN, NAN, AXP_INVALID, ""};
    AxisParameter microsteps        = {NAN, NAN, NAN, 1, 256, AXP_POW2, AXPN_MICROSTEPS};
    AxisParameter microstepsSlewing = {NAN, NAN, NAN, 1, 256, AXP_POW2, AXPN_MICROSTEPS_GOTO};
    AxisParameter decay             = {NAN, NAN, NAN, DRIVER_DECAY_MODE_FIRST, DRIVER_DECAY_MODE_LAST, AXP_DECAY, AXPN_DECAY_MODE};
    AxisParameter decaySlewing      = {NAN, NAN, NAN, DRIVER_DECAY_MODE_FIRST, DRIVER_DECAY_MODE_LAST, AXP_DECAY, AXPN_DECAY_MODE_GOTO};
};

#endif
