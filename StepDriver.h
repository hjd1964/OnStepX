// -----------------------------------------------------------------------------------
// stepper driver control
#pragma once

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

const static int8_t steps[13][9] =
//  1   2   4   8  16  32  64 128 256x
{{  0,  1,  2,  3,  7,OFF,OFF,OFF,OFF},   // A4988
 {  0,  1,  2,  3,  4,  5,OFF,OFF,OFF},   // DRV8825
 {  4,  2,  6,  5,  3,  7,OFF,OFF,OFF},   // S109
 {  0,  1,  2,  3,  4,  5,  6,  7,OFF},   // LV8729
 {  0,  1,  2,  3,  4,  5,  6,  7,OFF},   // RAPS128
 {  0,  1,  2,OFF,  3,OFF,OFF,OFF,OFF},   // TCM2100
 {OFF,  1,  2,  0,  3,OFF,OFF,OFF,OFF},   // TCM2208
 {OFF,OFF,OFF,  0,  3,  1,  2,OFF,OFF},   // TCM2209
 {  0,  1,  2,  3,  4,  5,OFF,  6,  7},   // ST820
 {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TCM2130
 {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TCM5160
 {  0,  0,  0,  0,  0,  0,  0,  0,  0},   // GENERIC
 {  0,  1,  1,  1,  1,  1,  1,  1,  1}};  // SERVO

#define MIXED         0
#define FAST          1
#define SLOW          2
#define SPREAD_CYCLE  3
#define STEALTH_CHOP  4

#pragma pack(1)
typedef struct DriverSettings {
  int16_t       model;
  int16_t       microsteps;
  int16_t       microstepsGoto;
  int16_t       currentHold;
  int16_t       currentRun;
  int16_t       currentGoto;
  int8_t        decay;
  int8_t        decayGoto;
} DriverSettings;
#pragma pack()

#define mosiPin m0Pin
#define sckPin  m1Pin
#define csPin   m2Pin
#define misoPin m3Pin

// check for TMC stepper drivers
#if AXIS1_DRIVER == TMC2130 || AXIS1_DRIVER == TMC5160 || \
    AXIS2_DRIVER == TMC2130 || AXIS2_DRIVER == TMC5160 || \
    AXIS3_DRIVER == TMC2130 || AXIS3_DRIVER == TMC5160 || \
    AXIS4_DRIVER == TMC2130 || AXIS4_DRIVER == TMC5160 || \
    AXIS5_DRIVER == TMC2130 || AXIS5_DRIVER == TMC5160 || \
    AXIS6_DRIVER == TMC2130 || AXIS6_DRIVER == TMC5160
  #define HAS_TMC_DRIVER
#endif

#ifdef HAS_TMC_DRIVER
  #include "TmcDriver.h"
#endif

class StepDriver {
  public:    
    StepDriver(int8_t m0Pin, int8_t m1Pin, int8_t m2Pin, int8_t m3Pin, int8_t decayPin) :
      m0Pin{ m0Pin }, m1Pin{ m1Pin }, m2Pin{ m2Pin }, m3Pin{ m3Pin }, decayPin{ decayPin } {}

    void init(DriverSettings settings) {
      this->settings = settings;

      microstepCode     = microstepsToCode(settings.model,settings.microsteps);
      microstepCodeGoto = microstepsToCode(settings.model,settings.microstepsGoto);
      microstepRatio    = settings.microsteps/settings.microstepsGoto;

      if (isTmcSPI()) {
#ifdef HAS_TMC_DRIVER
        if (settings.currentGoto == OFF) settings.currentGoto=settings.currentRun;
        if (settings.currentHold == OFF) settings.currentHold=settings.currentRun/2;
        tmcDriver.init(settings.model, mosiPin, sckPin, csPin, misoPin);
#endif
      } else {
        if (isDecayOnM2()) { decayPin = m2Pin; m2Pin = OFF; }

        noInterrupts();
        pinModeInit(m0Pin, OUTPUT, bitRead(microstepCode,0));
        pinModeInit(m1Pin, OUTPUT, bitRead(microstepCode,1));
        pinModeInit(m2Pin, OUTPUT, bitRead(microstepCode,2));
        pinModeEx(m3Pin,INPUT);
        interrupts();
  
        int8_t state = getDecayPinState(settings.decay);
        noInterrupts();
        if (state != OFF) pinModeInit(decayPin, OUTPUT, state);
        interrupts();
      }
    }

    void modeTracking() {
      if (isTmcSPI()) {
#ifdef HAS_TMC_DRIVER
        tmcDriver.refresh_CHOPCONF(microstepCode);
#endif
      } else {
        uint8_t initCode = microstepCode;
        noInterrupts();
        digitalWriteEx(m0Pin, bitRead(initCode, 0));
        digitalWriteEx(m1Pin, bitRead(initCode, 1));
        digitalWriteEx(m2Pin, bitRead(initCode, 2));
        interrupts();
      }
    }

    void modeDecayTracking() {
      if (isTmcSPI()) {
#ifdef HAS_TMC_DRIVER
        tmcDriver.setup(true, settings.decay, microstepCode, settings.currentRun, settings.currentHold);
#endif
      } else {
        if (settings.decay == OFF) return;
        int8_t state = getDecayPinState(settings.decay);
        noInterrupts();
        if (state != OFF) digitalWriteEx(decayPin,state);
        interrupts();
      }
    }

    uint8_t modeGoto() {
      if (isTmcSPI()) {
#ifdef HAS_TMC_DRIVER
        tmcDriver.refresh_CHOPCONF(microstepCodeGoto);
#endif
      } else {
        uint8_t initCode = microstepCodeGoto;
        noInterrupts();
        digitalWriteEx(m0Pin, bitRead(initCode, 0));
        digitalWriteEx(m1Pin, bitRead(initCode, 1));
        digitalWriteEx(m2Pin, bitRead(initCode, 2));
        interrupts();
      }
      return microstepRatio;
    }

    void modeDecayGoto() {
      if (isTmcSPI()) {
#ifdef HAS_TMC_DRIVER
        tmcDriver.setup(true, settings.decayGoto, microstepCodeGoto, settings.currentGoto, settings.currentHold);
#endif
      } else {
        if (settings.decayGoto == OFF) return;
        int8_t state = getDecayPinState(settings.decayGoto);
        noInterrupts();
        if (state != OFF) digitalWriteEx(decayPin,state);
        interrupts();
      }
    }

  private:
    int8_t getDecayPinState(int8_t decay) {
      uint8_t state = OFF;
      if (decay == SPREAD_CYCLE) state = LOW;  else
      if (decay == STEALTH_CHOP) state = HIGH; else
      if (decay == MIXED)        state = LOW;  else
      if (decay == FAST)         state = HIGH;
      return state;
    }

    bool isTmcSPI() {
#ifdef HAS_TMC_DRIVER
      if (settings.model == TMC2130 || settings.model == TMC5160) return true; else return false;
#else
      return false;
#endif
    }

    bool isDecayOnM2() {
      if (settings.model == TMC2209) return true; else return false;
    }

    // different models of stepper drivers have different bit settings for microsteps
    // translate the human readable microsteps in the configuration to mode bit settings
    // returns bit code (0 to 7) or OFF if microsteps is not supported or unknown
    int microstepsToCode(uint8_t driverModel, uint8_t microsteps) {
      int allowed[9] = {1,2,4,8,16,32,64,128,256};
      if (driverModel >= DRIVER_MODEL_COUNT) return OFF;
      for (int i = 0; i < 9; i++) {
        if (microsteps == allowed[i]) return steps[driverModel][i];
      }
      return OFF;
    }
    
    const int8_t m0Pin, m1Pin, m3Pin;
    int8_t m2Pin, decayPin;

#ifdef HAS_TMC_DRIVER
    TmcDriver tmcDriver;
#endif

    DriverSettings settings = {OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF};

    uint8_t microstepRatio = 1;
    int microstepCode      = OFF;
    int microstepCodeGoto  = OFF;
};
