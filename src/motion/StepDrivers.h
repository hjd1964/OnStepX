// -----------------------------------------------------------------------------------
// stepper driver control
#pragma once
#include <Arduino.h>

#include "Pins.h"
#include "TmcDrivers.h"

// the various microsteps for different driver models, with the bit modes for each
#define DRIVER_MODEL_COUNT 13
#define A4988    0  // step/dir stepper driver, allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x
#define DRV8825  1  // step/dir stepper driver, allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x
#define S109     2  // step/dir stepper driver, allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x
#define LV8729   3  // step/dir stepper driver, allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,64x,128x
#define RAPS128  4  // step/dir stepper driver, allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,64x,128x
#define TMC2100  5  // step/dir stepper driver, allows M0,M1    bit patterens for 1x,2x,4x,16x   (spreadCycle only, no 256x intpol)
#define TMC2208  6  // step/dir stepper driver, allows M0,M1    bit patterens for 2x,4x,8x,16x   (stealthChop default, uses 256x intpol)
#define TMC2209  7  // step/dir stepper driver, allows M0,M1    bit patterens for 8x,16x,32x,64x (M2 sets spreadCycle/stealthChop, uses 256x intpol)
#define ST820    8  // step/dir stepper driver, allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,128x,256x
#define TMC2130  9  // step/dir stepper driver, uses TMC protocol SPI comms   for 1x,2x...,256x  (SPI sets spreadCycle/stealthChop etc.)
#define TMC5160  10 // step/dir stepper driver, uses TMC protocol SPI comms   for 1x,2x...,256x  (SPI sets spreadCycle/stealthChop etc.)
#define GENERIC  11 // step/dir stepper driver, allows                        for 1x,2x,4x,8x,16x,32x,64x,128x,256x (no mode switching)
#define SERVO    12 // step/dir servo   driver, allows M0 bit pattern for LOW = native mode & goto HIGH = 2x,4x,8x,16x,32x,64x, or 128x *larger* steps

#define MIXED         0
#define FAST          1
#define SLOW          2
#define SPREADCYCLE   3
#define STEALTHCHOP   4

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

typedef struct DriverOutputStatus {
  bool shortToGround;
  bool openLoad;
} DriverOutputStatus;

typedef struct DriverStatus {
  DriverOutputStatus outputA;
  DriverOutputStatus outputB;
  bool overTemperaturePreWarning;
  bool overTemperature;
  bool standstill;
  bool fault;
} DriverStatus;

// check/flag TMC stepper drivers
#if AXIS1_DRIVER_MODEL == TMC2130 || AXIS1_DRIVER_MODEL == TMC5160
  #define AXIS1_DRIVER_TMC_SPI
#endif
#if AXIS2_DRIVER_MODEL == TMC2130 || AXIS2_DRIVER_MODEL == TMC5160
  #define AXIS2_DRIVER_TMC_SPI
#endif
#if AXIS3_DRIVER_MODEL == TMC2130 || AXIS3_DRIVER_MODEL == TMC5160
  #define AXIS3_DRIVER_TMC_SPI
#endif
#if AXIS4_DRIVER_MODEL == TMC2130 || AXIS4_DRIVER_MODEL == TMC5160
  #define AXIS4_DRIVER_TMC_SPI
#endif
#if AXIS5_DRIVER_MODEL == TMC2130 || AXIS5_DRIVER_MODEL == TMC5160
  #define AXIS5_DRIVER_TMC_SPI
#endif
#if AXIS6_DRIVER_MODEL == TMC2130 || AXIS6_DRIVER_MODEL == TMC5160
  #define AXIS6_DRIVER_TMC_SPI
#endif

#if AXIS1_DRIVER_MODEL == TMC2130 || AXIS1_DRIVER_MODEL == TMC5160 || \
    AXIS2_DRIVER_MODEL == TMC2130 || AXIS2_DRIVER_MODEL == TMC5160 || \
    AXIS3_DRIVER_MODEL == TMC2130 || AXIS3_DRIVER_MODEL == TMC5160 || \
    AXIS4_DRIVER_MODEL == TMC2130 || AXIS4_DRIVER_MODEL == TMC5160 || \
    AXIS5_DRIVER_MODEL == TMC2130 || AXIS5_DRIVER_MODEL == TMC5160 || \
    AXIS6_DRIVER_MODEL == TMC2130 || AXIS6_DRIVER_MODEL == TMC5160
  #define HAS_TMC_DRIVER
#endif

class StepDriver {
  public:
    // decodes driver model/microstep mode into microstep codes (bit patterns or SPI)
    // and sets up the pin modes
    void init(uint8_t axisNumber);

    // true if switching microstep modes is allowed
    bool modeSwitchAllowed();
    // set microstep mode for tracking
    void modeMicrostepTracking();
    // set decay mode for tracking
    void modeDecayTracking();

    // set microstep mode for slewing
    int modeMicrostepSlewing();
    // set decay mode for slewing
    void modeDecaySlewing();

    // update status info. for driver
    void updateStatus();
    // get status info.
    DriverStatus getStatus();

    // different models of stepper drivers have different bit settings for microsteps
    // translate the human readable microsteps in the configuration to mode bit settings
    // returns bit code (0 to 7) or OFF if microsteps is not supported or unknown
    int microstepsToCode(uint8_t driverModel, uint8_t microsteps);

    #ifdef HAS_TMC_DRIVER
      TmcDriver tmcDriver;
    #endif

    DriverSettings settings;

  private:
    // checks if decay pin should be HIGH/LOW for a given decay setting
    int8_t getDecayPinState(int8_t decay);

    // checks if this is a TMC SPI driver
    bool isTmcSPI();

    // checkes if decay control is on the M2 pin
    bool isDecayOnM2();

    int axisNumber;
    DriverPins pins;
    DriverStatus status = {{false, false}, {false, false}, false, false, false, false};

    int     microstepRatio        = 1;
    int     microstepCode         = OFF;
    int     microstepCodeGoto     = OFF;
    uint8_t microstepBitCode      = 0;
    uint8_t microstepBitCodeGoto  = 0;
    int8_t  m2Pin                 = OFF;
    int8_t  decayPin              = OFF;
};
