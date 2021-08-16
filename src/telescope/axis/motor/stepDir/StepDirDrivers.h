// -----------------------------------------------------------------------------------
// step/dir driver control
#pragma once

#include <Arduino.h>
#include "../../../../Common.h"

#include "StepDir.defaults.h"

// the various microsteps for different driver models, with the bit modes for each
#define DRIVER_MODEL_COUNT 13

#ifdef SD_DRIVER_PRESENT

#include "../Drivers.h"
#include "TmcDrivers.h"

#pragma pack(1)
#define StepDriverSettingsSize 15
typedef struct DriverSettings {
  int16_t model;
  int16_t microsteps;
  int16_t microstepsGoto;
  int16_t currentHold;
  int16_t currentRun;
  int16_t currentGoto;
  int8_t  decay;
  int8_t  decayGoto;
  int8_t  status;
} DriverSettings;
#pragma pack()

typedef struct DriverModePins {
  int16_t m0;
  int16_t m1;
  int16_t m2;
  int16_t m3;
  int16_t decay;
  int16_t fault;
} DriverModePins;

#define mosi_pin m0
#define sck_pin  m1
#define cs_pin   m2
#define miso_pin m3

class StepDirDriver {
  public:
    // constructor
    StepDirDriver(uint8_t axisNumber, const DriverModePins *Pins, const DriverSettings *Settings);

    // decodes driver model/microstep mode into microstep codes (bit patterns or SPI)
    // and sets up the pin modes
    void init(int16_t microsteps, int16_t current);

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
    int subdivisionsToCode(uint8_t microsteps);

    #ifdef TMC_DRIVER_PRESENT
      TmcDriver tmcDriver;
    #endif

    DriverSettings settings;

  private:
    // checks if decay pin should be HIGH/LOW for a given decay setting
    int8_t getDecayPinState(int8_t decay);

    // checkes if decay control is on the M2 pin
    bool isDecayOnM2();

    int axisNumber;
    DriverStatus status = {{false, false}, {false, false}, false, false, false, false};

    int     microstepRatio        = 1;
    int     microstepCode         = OFF;
    int     microstepCodeGoto     = OFF;
    uint8_t microstepBitCode      = 0;
    uint8_t microstepBitCodeGoto  = 0;
    int16_t m2Pin                 = OFF;
    int16_t decayPin              = OFF;
    const DriverModePins *Pins;
};

#ifdef AXIS1_DRIVER_SD
  extern StepDirDriver stepDirDriver1;
#endif
#ifdef AXIS2_DRIVER_SD
  extern StepDirDriver stepDirDriver2;
#endif
#ifdef AXIS3_DRIVER_SD
  extern StepDirDriver stepDirDriver3;
#endif
#ifdef AXIS4_DRIVER_SD
  extern StepDirDriver stepDirDriver4;
#endif
#ifdef AXIS5_DRIVER_SD
  extern StepDirDriver stepDirDriver5;
#endif
#ifdef AXIS6_DRIVER_SD
  extern StepDirDriver stepDirDriver6;
#endif
#ifdef AXIS7_DRIVER_SD
  extern StepDirDriver stepDirDriver7;
#endif
#ifdef AXIS8_DRIVER_SD
  extern StepDirDriver stepDirDriver8;
#endif
#ifdef AXIS9_DRIVER_SD
  extern StepDirDriver stepDirDriver9;
#endif

#endif
