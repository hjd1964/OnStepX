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
  int16_t microsteps;
  int16_t microstepsSlewing;
  int16_t currentHold;
  int16_t currentRun;
  int16_t currentGoto;
  int8_t  intpol;
  int8_t  decay;
  int8_t  decaySlewing;
  int8_t  status;
} StepDirDriverSettings;

#ifndef TMC2130_RSENSE
#define TMC2130_RSENSE 0.11F
#endif
#ifndef TMC2130_MAX_CURRENT
#define TMC2130_MAX_CURRENT (1700*0.8) // chip typically rated at 1.2A RMS, downrated to 80% due to typical step-stick form
#endif

#ifndef TMC2160_RSENSE
#define TMC2160_RSENSE 0.075F
#endif
#ifndef TMC2160_MAX_CURRENT
#define TMC2160_MAX_CURRENT 4230       // module typically rated at 3.0A RMS
#endif

#ifndef TMC2208_RSENSE
#define TMC2208_RSENSE 0.11F
#endif
#ifndef TMC2208_MAX_CURRENT
#define TMC2208_MAX_CURRENT (1974*0.8) // chip rated at 1.4A RMS, downrated to 80% due to typical step-stick form
#endif

#ifndef TMC2209_RSENSE
#define TMC2209_RSENSE 0.11F
#endif
#ifndef TMC2209_MAX_CURRENT
#define TMC2209_MAX_CURRENT (2820*0.8) // chip rated at 2.0A RMS, downrated to 80% due to typical step-stick form
#endif

#ifndef TMC2660_RSENSE
#define TMC2660_RSENSE 0.075F
#endif
#ifndef TMC2660_MAX_CURRENT
#define TMC2660_MAX_CURRENT 2820       // module/chip rated at 2.0A RMS
#endif

#ifndef TMC5160_RSENSE
#define TMC5160_RSENSE 0.075F
#endif
#ifndef TMC5160_MAX_CURRENT
#define TMC5160_MAX_CURRENT 4230       // typical module rated at 3.0A RMS
#endif

#ifndef TMC5161_RSENSE
#define TMC5161_RSENSE 0.075F
#endif
#ifndef TMC5161_MAX_CURRENT
#define TMC5161_MAX_CURRENT (4935*0.8) // chip rated at 3.5A RMS, downrated to 80% due to typical step-stick form
#endif

class StepDirDriver {
  public:
    // get driver type code
    virtual char getParameterTypeCode() { return 'X'; }

    // set up driver
    virtual bool init();

    // set up parameters: microsteps, microsteps goto, hold current, run current, goto current, unused
    virtual bool setParameters(float param1, float param2, float param3, float param4, float param5, float param6);

    // validate driver parameters
    virtual bool validateParameters(float param1, float param2, float param3, float param4, float param5, float param6);

    // set microstep mode for tracking
    virtual void modeMicrostepTracking();

    // set microstep mode for slewing
    virtual int modeMicrostepSlewing();

    // set decay mode for tracking
    virtual void modeDecayTracking();

    // set decay mode for slewing
    virtual void modeDecaySlewing();

    // get microstep ratio for slewing
    inline int getMicrostepRatio() { return microstepRatio; }

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

    // true if switching microstep modes at low speed is allowed
    bool modeSwitchAllowed = false;

    // true if switching microstep modes at high speed is allowed
    bool modeSwitchFastAllowed = false;

    StepDirDriverSettings settings;

  protected:
    // read status from driver
    virtual void readStatus();

    uint8_t axisNumber;
    char axisPrefix[32]; // prefix for debug messages

    int16_t currentMax = 0;
    float rSense = 0.11F;
    int16_t user_currentMax = 0;
    float user_rSense = 0.0F;

    DriverStatus status = {false, {false, false}, {false, false}, false, false, false, false};
    #if DEBUG != OFF
      DriverStatus lastStatus = {false, {false, false}, {false, false}, false, false, false, false};
    #endif
    unsigned long timeLastStatusUpdate = 0;

    const int16_t* microsteps;
    int16_t microstepRatio = 1;
    int16_t microstepCode = OFF;
    int16_t microstepCodeSlewing = OFF;

    const StepDirDriverPins *Pins;

  private:
    // different models of stepper drivers have different bit settings for microsteps
    // translate the human readable microsteps in the configuration to mode bit settings
    // returns bit code (0 to 7) or OFF if microsteps is not supported or unknown
    int subdivisionsToCode(long microsteps);
};

#endif
