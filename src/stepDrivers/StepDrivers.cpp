// -----------------------------------------------------------------------------------
// stepper driver control
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"

#ifdef HAS_TMC_DRIVER
  #include "TmcDrivers.h"
#endif
#include "StepDrivers.h"

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

void StepDriver::init() {
  VF("MSG: StepDriver::init, model "); V(Settings.model);
  VF(", usteps "); if (Settings.microsteps == OFF) VF("OFF"); else V(Settings.microsteps);
  VF(", ustepsGoto "); if (Settings.microstepsGoto == OFF) VLF("OFF"); else VL(Settings.microstepsGoto);

  microstepCode     = microstepsToCode(Settings.model, Settings.microsteps);
  microstepCodeGoto = microstepsToCode(Settings.model, Settings.microstepsGoto);
  microstepRatio    = Settings.microsteps/Settings.microstepsGoto;

  if (isTmcSPI()) {
    #ifdef HAS_TMC_DRIVER
      tmcDriver.init(Settings.model);
    #endif
  } else {
    if (isDecayOnM2()) { decayPin = Pins.m2; m2Pin = OFF; } else { decayPin = Pins.decay; m2Pin = Pins.m2; }
    pinModeInitEx(decayPin, OUTPUT, getDecayPinState(Settings.decay));

    microstepBitCode     = microstepCode;
    microstepBitCodeGoto = microstepCodeGoto;
    pinModeInitEx(Pins.m0, OUTPUT, bitRead(microstepBitCode, 0));
    pinModeInitEx(Pins.m1, OUTPUT, bitRead(microstepBitCode, 1));
    pinModeInitEx(m2Pin,   OUTPUT, bitRead(microstepBitCode, 2));
    pinModeEx    (Pins.m3, INPUT);
  }
}

void StepDriver::modeTracking() {
  if (isTmcSPI()) {
    #ifdef HAS_TMC_DRIVER
      tmcDriver.refresh_CHOPCONF(microstepCode);
    #endif
  } else {
    noInterrupts();
    digitalWriteEx(Pins.m0, bitRead(microstepBitCode, 0));
    digitalWriteEx(Pins.m1, bitRead(microstepBitCode, 1));
    digitalWriteEx(Pins.m2, bitRead(microstepBitCode, 2));
    interrupts();
  }
}

void StepDriver::modeDecayTracking() {
  if (isTmcSPI()) {
    #ifdef HAS_TMC_DRIVER
      tmcDriver.mode(true, Settings.decay, microstepCode, Settings.currentRun, Settings.currentHold);
    #endif
  } else {
    if (Settings.decay == OFF) return;
    int8_t state = getDecayPinState(Settings.decay);
    noInterrupts();
    if (state != OFF) digitalWriteEx(decayPin,state);
    interrupts();
  }
}

uint8_t StepDriver::modeGoto() {
  if (isTmcSPI()) {
    #ifdef HAS_TMC_DRIVER
      tmcDriver.refresh_CHOPCONF(microstepCodeGoto);
    #endif
  } else {
    noInterrupts();
    digitalWriteEx(Pins.m0, bitRead(microstepBitCodeGoto, 0));
    digitalWriteEx(Pins.m1, bitRead(microstepBitCodeGoto, 1));
    digitalWriteEx(Pins.m2, bitRead(microstepBitCodeGoto, 2));
    interrupts();
  }
  return microstepRatio;
}

void StepDriver::modeDecayGoto() {
  if (isTmcSPI()) {
    #ifdef HAS_TMC_DRIVER
      int IRUN = Settings.currentGoto;
      if (IRUN == OFF) IRUN = Settings.currentRun;
      tmcDriver.mode(true, Settings.decayGoto, microstepCodeGoto, IRUN, Settings.currentHold);
    #endif
  } else {
    if (Settings.decayGoto == OFF) return;
    int8_t state = getDecayPinState(Settings.decayGoto);
    noInterrupts();
    if (state != OFF) digitalWriteEx(decayPin, state);
    interrupts();
  }
}

int8_t StepDriver::getDecayPinState(int8_t decay) {
  uint8_t state = OFF;
  if (decay == SPREADCYCLE) state = LOW;  else
  if (decay == STEALTHCHOP) state = HIGH; else
  if (decay == MIXED)       state = LOW;  else
  if (decay == FAST)        state = HIGH;
  return state;
}

bool StepDriver::isTmcSPI() {
  #ifdef HAS_TMC_DRIVER
    if (Settings.model == TMC2130 || Settings.model == TMC5160) return true; else return false;
  #else
    return false;
  #endif
}

bool StepDriver::isDecayOnM2() {
  if (Settings.model == TMC2209) return true; else return false;
}

// different models of stepper drivers have different bit settings for microsteps
// translate the human readable microsteps in the configuration to mode bit settings
// returns bit code (0 to 7) or OFF if microsteps is not supported or unknown
int StepDriver::microstepsToCode(uint8_t driverModel, uint8_t microsteps) {
  int allowed[9] = {1,2,4,8,16,32,64,128,256};
  if (driverModel >= DRIVER_MODEL_COUNT) return OFF;
  for (int i = 0; i < 9; i++) {
    if (microsteps == allowed[i]) return steps[driverModel][i];
  }
  return OFF;
}
