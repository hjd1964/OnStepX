// -----------------------------------------------------------------------------------
// stepper driver control

#include "../../../Common.h"
#include "StepDrivers.h"

const static int8_t steps[DRIVER_MODEL_COUNT][9] =
//  1   2   4   8  16  32  64 128 256x
{{  0,  1,  2,  3,  7,OFF,OFF,OFF,OFF},   // A4988
 {  0,  1,  2,  3,  4,  5,OFF,OFF,OFF},   // DRV8825
 {  4,  2,  6,  5,  3,  7,OFF,OFF,OFF},   // S109
 {  0,  1,  2,  3,  4,  5,  6,  7,OFF},   // LV8729
 {  0,  1,  2,  3,  4,  5,  6,  7,OFF},   // RAPS128
 {  0,  1,  2,OFF,  3,OFF,OFF,OFF,OFF},   // TMC2100
 {OFF,  1,  2,  0,  3,OFF,OFF,OFF,OFF},   // TMC2208
 {OFF,OFF,OFF,  0,  3,  1,  2,OFF,OFF},   // TMC2209
 {  0,  1,  2,  3,  4,  5,OFF,  6,  7},   // ST820
 {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC2130
 {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC5160
 {  0,  0,  0,  0,  0,  0,  0,  0,  0},   // GENERIC
 {  0,  1,  1,  1,  1,  1,  1,  1,  1}};  // SERVO

#if DEBUG_MODE != OFF
  const char* DRIVER_NAME[DRIVER_MODEL_COUNT] = {
  "A4988","DRV8825","S109","LV8729","RAPS128",
  "TMC2100","TMC2208","TMC2209","ST820","TMC2130",
  "TMC5160","GENERIC","SERVO" };
#endif

#if AXIS1_DRIVER_MODEL != OFF
  const DriverPins     Axis1DriverModePins     = {AXIS1_M0_PIN, AXIS1_M1_PIN, AXIS1_M2_PIN, AXIS1_M3_PIN, AXIS1_DECAY_PIN, AXIS1_FAULT_PIN};
  const DriverSettings Axis1DriverModeSettings = {AXIS1_DRIVER_MODEL, AXIS1_DRIVER_MICROSTEPS, AXIS1_DRIVER_MICROSTEPS_GOTO,
                                                  AXIS1_DRIVER_IHOLD, AXIS1_DRIVER_IRUN, AXIS1_DRIVER_IGOTO,
                                                  AXIS1_DRIVER_DECAY, AXIS1_DRIVER_DECAY_GOTO, AXIS1_DRIVER_STATUS};
#endif
#if AXIS2_DRIVER_MODEL != OFF
  const DriverPins     Axis2DriverModePins     = {AXIS2_M0_PIN, AXIS2_M1_PIN, AXIS2_M2_PIN, AXIS2_M3_PIN, AXIS2_DECAY_PIN, AXIS2_FAULT_PIN};
  const DriverSettings Axis2DriverModeSettings = {AXIS2_DRIVER_MODEL, AXIS2_DRIVER_MICROSTEPS, AXIS2_DRIVER_MICROSTEPS_GOTO,
                                                  AXIS2_DRIVER_IHOLD, AXIS2_DRIVER_IRUN, AXIS2_DRIVER_IGOTO,
                                                  AXIS2_DRIVER_DECAY, AXIS2_DRIVER_DECAY_GOTO, AXIS2_DRIVER_STATUS};
#endif
#if AXIS3_DRIVER_MODEL != OFF
  const DriverPins     Axis3DriverModePins     = {AXIS3_M0_PIN, AXIS3_M1_PIN, AXIS3_M2_PIN, AXIS3_M3_PIN, AXIS3_DECAY_PIN, AXIS3_FAULT_PIN};
  const DriverSettings Axis3DriverModeSettings = {AXIS3_DRIVER_MODEL, AXIS3_DRIVER_MICROSTEPS, AXIS3_DRIVER_MICROSTEPS_GOTO,
                                                  AXIS3_DRIVER_IHOLD, AXIS3_DRIVER_IRUN, AXIS3_DRIVER_IGOTO,
                                                  AXIS3_DRIVER_DECAY, AXIS3_DRIVER_DECAY_GOTO, AXIS3_DRIVER_STATUS};
#endif
#if AXIS4_DRIVER_MODEL != OFF
  const DriverPins     Axis4DriverModePins     = {AXIS4_M0_PIN, AXIS4_M1_PIN, AXIS4_M2_PIN, AXIS4_M3_PIN, AXIS4_DECAY_PIN, AXIS4_FAULT_PIN};
  const DriverSettings Axis4DriverModeSettings = {AXIS4_DRIVER_MODEL, AXIS4_DRIVER_MICROSTEPS, AXIS4_DRIVER_MICROSTEPS_GOTO,
                                                  AXIS4_DRIVER_IHOLD, AXIS4_DRIVER_IRUN, AXIS4_DRIVER_IGOTO,
                                                  AXIS4_DRIVER_DECAY, AXIS4_DRIVER_DECAY_GOTO, AXIS4_DRIVER_STATUS};
#endif
#if AXIS5_DRIVER_MODEL != OFF
  const DriverPins     Axis5DriverModePins     = {AXIS5_M0_PIN, AXIS5_M1_PIN, AXIS5_M2_PIN, AXIS5_M3_PIN, AXIS5_DECAY_PIN, AXIS5_FAULT_PIN};
  const DriverSettings Axis5DriverModeSettings = {AXIS5_DRIVER_MODEL, AXIS5_DRIVER_MICROSTEPS, AXIS5_DRIVER_MICROSTEPS_GOTO,
                                                  AXIS5_DRIVER_IHOLD, AXIS5_DRIVER_IRUN, AXIS5_DRIVER_IGOTO,
                                                  AXIS5_DRIVER_DECAY, AXIS5_DRIVER_DECAY_GOTO, AXIS5_DRIVER_STATUS};
#endif
#if AXIS6_DRIVER_MODEL != OFF
  const DriverPins     Axis6DriverModePins     = {AXIS6_M0_PIN, AXIS6_M1_PIN, AXIS6_M2_PIN, AXIS6_M3_PIN, AXIS6_DECAY_PIN, AXIS6_FAULT_PIN};
  const DriverSettings Axis6DriverModeSettings = {AXIS6_DRIVER_MODEL, AXIS6_DRIVER_MICROSTEPS, AXIS6_DRIVER_MICROSTEPS_GOTO,
                                                  AXIS6_DRIVER_IHOLD, AXIS6_DRIVER_IRUN, AXIS6_DRIVER_IGOTO,
                                                  AXIS6_DRIVER_DECAY, AXIS6_DRIVER_DECAY_GOTO, AXIS6_DRIVER_STATUS};
#endif

void StepDriver::init(uint8_t axisNumber) {
  this->axisNumber = axisNumber;
  #if AXIS1_DRIVER_MODEL != OFF
    if (axisNumber == 1) { pins = Axis1DriverModePins; settings = Axis1DriverModeSettings; }
  #endif
  #if AXIS2_DRIVER_MODEL != OFF
    if (axisNumber == 2) { pins = Axis2DriverModePins; settings = Axis2DriverModeSettings; }
  #endif
  #if AXIS3_DRIVER_MODEL != OFF
    if (axisNumber == 3) { pins = Axis3DriverModePins; settings = Axis3DriverModeSettings; }
  #endif
  #if AXIS4_DRIVER_MODEL != OFF
    if (axisNumber == 4) { pins = Axis4DriverModePins; settings = Axis4DriverModeSettings; }
  #endif
  #if AXIS5_DRIVER_MODEL != OFF
    if (axisNumber == 5) { pins = Axis5DriverModePins; settings = Axis5DriverModeSettings; }
  #endif
  #if AXIS6_DRIVER_MODEL != OFF
    if (axisNumber == 6) { pins = Axis6DriverModePins; settings = Axis6DriverModeSettings; }
  #endif

  VF("MSG: StepDriver, init model "); V(DRIVER_NAME[settings.model]);
  VF(" u-step mode "); if (settings.microsteps == OFF) VF("OFF"); else { V(settings.microsteps); V("X"); }
  VF(" (goto mode "); if (settings.microstepsGoto == SAME) VLF("SAME)"); else { V(settings.microstepsGoto); VL("X)"); }
  if (settings.microstepsGoto == SAME) settings.microstepsGoto = settings.microsteps;

  microstepCode = microstepsToCode(settings.model, settings.microsteps);
  microstepCodeGoto = microstepsToCode(settings.model, settings.microstepsGoto);
  microstepRatio = settings.microsteps/settings.microstepsGoto;

  if (isTmcSPI()) {
    #ifdef HAS_TMC_DRIVER
      tmcDriver.init(settings.model, pins);
      if (settings.decay == STEALTHCHOP || settings.decayGoto == STEALTHCHOP) {
        tmcDriver.mode(true, STEALTHCHOP, microstepCode, settings.currentRun, settings.currentRun);
        VLF("MSG: StepDriver, TMC driver stealthChop 100ms pause for standstill automatic current calibration");
        delay(100);
      }
      tmcDriver.mode(true, settings.decay, microstepCode, settings.currentRun, settings.currentHold);
    #endif
  } else {
    if (isDecayOnM2()) { decayPin = pins.m2; m2Pin = OFF; } else { decayPin = pins.decay; m2Pin = pins.m2; }
    pinModeEx(decayPin, OUTPUT);
    digitalWriteEx(decayPin, getDecayPinState(settings.decay));

    microstepBitCode = microstepCode;
    microstepBitCodeGoto = microstepCodeGoto;
    pinModeEx(pins.m0, OUTPUT);
    digitalWriteEx(pins.m0, bitRead(microstepBitCode, 0));
    pinModeEx(pins.m1, OUTPUT);
    digitalWriteEx(pins.m1, bitRead(microstepBitCode, 1));
    pinModeEx(m2Pin, OUTPUT);
    digitalWriteEx(m2Pin, bitRead(microstepBitCode, 2));
    pinModeEx(pins.m3, INPUT);
  }

  // automatically set fault status for known drivers
  if (settings.status == ON) {
    switch (settings.model) {
      case DRV8825: settings.status = LOW; break;
      case ST820:   settings.status = LOW; break;
      default:      settings.status = LOW;
    }
  }
}

bool StepDriver::modeSwitchAllowed() {
  return microstepRatio != 1;  
}

void StepDriver::modeMicrostepTracking() {
  if (isTmcSPI()) {
    #ifdef HAS_TMC_DRIVER
      tmcDriver.refresh_CHOPCONF(microstepCode);
    #endif
  } else {
    noInterrupts();
    digitalWriteEx(pins.m0, bitRead(microstepBitCode, 0));
    digitalWriteEx(pins.m1, bitRead(microstepBitCode, 1));
    digitalWriteEx(pins.m2, bitRead(microstepBitCode, 2));
    interrupts();
  }
}

void StepDriver::modeDecayTracking() {
  if (isTmcSPI()) {
    #ifdef HAS_TMC_DRIVER
      tmcDriver.mode(true, settings.decay, microstepCode, settings.currentRun, settings.currentHold);
    #endif
  } else {
    if (settings.decay == OFF) return;
    int8_t state = getDecayPinState(settings.decay);
    noInterrupts();
    if (state != OFF) digitalWriteEx(decayPin,state);
    interrupts();
  }
}

// get microstep ratio for slewing
int StepDriver::getMicrostepRatio() {
  return microstepRatio;
}

int StepDriver::modeMicrostepSlewing() {
  if (microstepRatio > 1) {
    if (isTmcSPI()) {
      #ifdef HAS_TMC_DRIVER
        tmcDriver.refresh_CHOPCONF(microstepCodeGoto);
      #endif
    } else {
      noInterrupts();
      digitalWriteEx(pins.m0, bitRead(microstepBitCodeGoto, 0));
      digitalWriteEx(pins.m1, bitRead(microstepBitCodeGoto, 1));
      digitalWriteEx(pins.m2, bitRead(microstepBitCodeGoto, 2));
      interrupts();
    }
  }
  return microstepRatio;
}

void StepDriver::modeDecaySlewing() {
  if (isTmcSPI()) {
    #ifdef HAS_TMC_DRIVER
      int IRUN = settings.currentGoto;
      if (IRUN == OFF) IRUN = settings.currentRun;
      tmcDriver.mode(true, settings.decayGoto, microstepCodeGoto, IRUN, settings.currentHold);
    #endif
  } else {
    if (settings.decayGoto == OFF) return;
    int8_t state = getDecayPinState(settings.decayGoto);
    noInterrupts();
    if (state != OFF) digitalWriteEx(decayPin, state);
    interrupts();
  }
}

// set the driver IRUN current in mA
void StepDriver::setCurrent(int16_t current) {
  if (settings.currentRun != OFF && settings.currentRun != current) {
    settings.currentRun  = current;
    settings.currentGoto = current;
    settings.currentHold = current/2;
  }
}

void StepDriver::updateStatus() {
  #ifdef HAS_TMC_DRIVER
    if (settings.status == ON) {
        tmcDriver.refresh_DRVSTATUS();
        status.outputA.shortToGround = tmcDriver.get_DRVSTATUS_s2gA();
        status.outputA.openLoad      = tmcDriver.get_DRVSTATUS_olA();
        status.outputB.shortToGround = tmcDriver.get_DRVSTATUS_s2gB();
        status.outputB.openLoad      = tmcDriver.get_DRVSTATUS_olB();
        status.overTemperaturePreWarning = tmcDriver.get_DRVSTATUS_otpw();
        status.overTemperature       = tmcDriver.get_DRVSTATUS_ot();
        status.standstill            = tmcDriver.get_DRVSTATUS_stst();

        // open load indication is not reliable in standstill
        if (
          status.outputA.shortToGround ||
          (status.outputA.openLoad && !status.standstill) ||
          status.outputB.shortToGround ||
          (status.outputB.openLoad && !status.standstill) ||
          status.overTemperaturePreWarning ||
          status.overTemperature
        ) status.fault = true; else status.fault = false;
    } else
  #endif

  if (settings.status == LOW || settings.status == HIGH) {
    status.fault = digitalReadEx(pins.fault) == settings.status;
  }
}

DriverStatus StepDriver::getStatus() {
  return status;
}

int8_t StepDriver::getDecayPinState(int8_t decay) {
  uint8_t state = OFF;
  if (decay == SPREADCYCLE) state = LOW;  else
  if (decay == STEALTHCHOP) state = HIGH; else
  if (decay == MIXED)       state = LOW;  else
  if (decay == FAST)        state = HIGH;
  return state;
}

// secondary way to power down not using the enable pin
void StepDriver::power(bool state) {
  #ifdef HAS_TMC_DRIVER
    int I_run = 0, I_hold = 0;
    if (state) { I_run = settings.currentRun; I_hold = settings.currentHold; }
    tmcDriver.mode(true, settings.decay, microstepCode, I_run, I_hold);
  #else
    state = state;
  #endif
}

// checks for TMC SPI driver
bool StepDriver::isTmcSPI() {
  #ifdef HAS_TMC_DRIVER
    if (settings.model == TMC2130 || settings.model == TMC5160) return true; else return false;
  #else
    return false;
  #endif
}

bool StepDriver::isDecayOnM2() {
  if (settings.model == TMC2209) return true; else return false;
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
