// -----------------------------------------------------------------------------------
// axis step/dir motor driver
#pragma once

#include <Arduino.h>
#include "../../../../Common.h"

#ifdef STEP_DIR_MOTOR_PRESENT

// the various microsteps for different driver models, with the bit modes for each
#define DRIVER_MODEL_COUNT 14

#include "../Drivers.h"
#include "TmcDrivers.h"

#define mosi_pin m0
#define sck_pin  m1
#define cs_pin   m2
#define miso_pin m3

#pragma pack(1)
typedef struct StepDirDriverPins {
  int16_t step;
  uint8_t stepState;
  int16_t dir;
  int16_t enable;
  uint8_t enabledState;
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
  int16_t microstepsGoto;
  int16_t currentHold;
  int16_t currentRun;
  int16_t currentGoto;
  int8_t  decay;
  int8_t  decayGoto;
  int8_t  status;
} StepDirDriverSettings;
#pragma pack()

class StepDirDriver {
  public:
    // constructor
    StepDirDriver(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings);

    // get driver type code
    inline char getParameterTypeCode() { if (isTmcSPI() || isTmcUART()) return 'T'; else return 'S'; }

    // sets driver parameters: microsteps, microsteps goto, hold current, run current, goto current, unused
    void setParameters(float param1, float param2, float param3, float param4, float param5, float param6);

    // validate driver parameters
    bool validateParameters(float param1, float param2, float param3, float param4, float param5, float param6);

    // true if switching microstep modes is allowed
    bool modeSwitchAllowed();

    // set microstep mode for tracking
    void modeMicrostepTracking();

    // set decay mode for tracking
    void modeDecayTracking();

    // get microstep ratio for slewing
    int getMicrostepRatio();

    // set microstep mode for slewing
    int modeMicrostepSlewing();

    // set decay mode for slewing
    void modeDecaySlewing();

    // update status info. for driver
    void updateStatus();

    // get status info.
    DriverStatus getStatus();

    // secondary way to power down not using the enable pin
    void power(bool state);

    // checks for TMC SPI driver
    bool isTmcSPI();

    // checks for TMC UART driver
    bool isTmcUART();

    // get the pulse width in microseconds, if unknown (-1) uses 2 microseconds
    long getPulseWidth();

    // get the microsteps
    // this is a required method for the Axis class, even if it only ever returns 1
    inline int getSubdivisions() { return settings.microsteps; }

    // get the microsteps goto
    // this is a required method for the Axis class, even if it only ever returns 1
    inline int getSubdivisionsGoto() { return settings.microstepsGoto; }

    // different models of stepper drivers have different bit settings for microsteps
    // translate the human readable microsteps in the configuration to mode bit settings
    // returns bit code (0 to 7) or OFF if microsteps is not supported or unknown
    // this is a required method for the Axis class, even if it only ever returns 1
    int subdivisionsToCode(long microsteps);

    #if defined(TMC_SPI_DRIVER_PRESENT) || defined(TMC_UART_DRIVER_PRESENT)
      TmcDriver tmcDriver;
    #endif

    StepDirDriverSettings settings;

  private:
    // checks if decay pin should be HIGH/LOW for a given decay setting
    int8_t getDecayPinState(int8_t decay);

    // checkes if decay control is on the M2 pin
    bool isDecayOnM2();

    uint8_t axisNumber;
    DriverStatus status = {false, {false, false}, {false, false}, false, false, false, false};
    #if DEBUG != OFF
      DriverStatus lastStatus = {false, {false, false}, {false, false}, false, false, false, false};
    #endif

    const int16_t* microsteps;
    int16_t microstepRatio        = 1;
    int16_t microstepCode         = OFF;
    int16_t microstepCodeGoto     = OFF;
    uint8_t microstepBitCode      = 0;
    uint8_t microstepBitCodeGoto  = 0;
    int16_t m2Pin                 = OFF;
    int16_t decayPin              = OFF;
    const StepDirDriverPins *Pins;

    unsigned long timeLastStatusUpdate = 0;
};

#endif
