// -----------------------------------------------------------------------------------
// stepper driver control

#include "StepDrivers.h"

#ifdef AXIS_PRESENT

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

const DriverPins ModePins[] = {
  #if AXIS1_DRIVER_MODEL != OFF
    { 1, AXIS1_M0_PIN, AXIS1_M1_PIN, AXIS1_M2_PIN, AXIS1_M3_PIN, AXIS1_DECAY_PIN, AXIS1_FAULT_PIN },
  #endif
  #if AXIS2_DRIVER_MODEL != OFF
    { 2, AXIS2_M0_PIN, AXIS2_M1_PIN, AXIS2_M2_PIN, AXIS2_M3_PIN, AXIS2_DECAY_PIN, AXIS2_FAULT_PIN },
  #endif
  #if AXIS3_DRIVER_MODEL != OFF
    { 3, AXIS3_M0_PIN, AXIS3_M1_PIN, AXIS3_M2_PIN, AXIS3_M3_PIN, AXIS3_DECAY_PIN, AXIS3_FAULT_PIN },
  #endif
  #if AXIS4_DRIVER_MODEL != OFF
    { 4, AXIS4_M0_PIN, AXIS4_M1_PIN, AXIS4_M2_PIN, AXIS4_M3_PIN, AXIS4_DECAY_PIN, AXIS4_FAULT_PIN },
  #endif
  #if AXIS5_DRIVER_MODEL != OFF
    { 5, AXIS5_M0_PIN, AXIS5_M1_PIN, AXIS5_M2_PIN, AXIS5_M3_PIN, AXIS5_DECAY_PIN, AXIS5_FAULT_PIN },
  #endif
  #if AXIS6_DRIVER_MODEL != OFF
    { 6, AXIS6_M0_PIN, AXIS6_M1_PIN, AXIS6_M2_PIN, AXIS6_M3_PIN, AXIS6_DECAY_PIN, AXIS6_FAULT_PIN },
  #endif
  #if AXIS7_DRIVER_MODEL != OFF
    { 7, AXIS7_M0_PIN, AXIS7_M1_PIN, AXIS7_M2_PIN, AXIS7_M3_PIN, AXIS7_DECAY_PIN, AXIS7_FAULT_PIN },
  #endif
  #if AXIS8_DRIVER_MODEL != OFF
    { 8, AXIS8_M0_PIN, AXIS8_M1_PIN, AXIS8_M2_PIN, AXIS8_M3_PIN, AXIS8_DECAY_PIN, AXIS8_FAULT_PIN },
  #endif
  #if AXIS9_DRIVER_MODEL != OFF
    { 9, AXIS9_M0_PIN, AXIS9_M1_PIN, AXIS9_M2_PIN, AXIS9_M3_PIN, AXIS9_DECAY_PIN, AXIS9_FAULT_PIN },
  #endif
};

const DriverSettings ModeSettings[] = {
  #if AXIS1_DRIVER_MODEL != OFF
    { AXIS1_DRIVER_MODEL, AXIS1_DRIVER_MICROSTEPS, AXIS1_DRIVER_MICROSTEPS_GOTO, AXIS1_DRIVER_IHOLD, AXIS1_DRIVER_IRUN, AXIS1_DRIVER_IGOTO, AXIS1_DRIVER_DECAY, AXIS1_DRIVER_DECAY_GOTO, AXIS1_DRIVER_STATUS },
  #endif
  #if AXIS2_DRIVER_MODEL != OFF
    { AXIS2_DRIVER_MODEL, AXIS2_DRIVER_MICROSTEPS, AXIS2_DRIVER_MICROSTEPS_GOTO, AXIS2_DRIVER_IHOLD, AXIS2_DRIVER_IRUN, AXIS2_DRIVER_IGOTO, AXIS2_DRIVER_DECAY, AXIS2_DRIVER_DECAY_GOTO, AXIS2_DRIVER_STATUS },
  #endif
  #if AXIS3_DRIVER_MODEL != OFF
    { AXIS3_DRIVER_MODEL, AXIS3_DRIVER_MICROSTEPS, AXIS3_DRIVER_MICROSTEPS_GOTO, AXIS3_DRIVER_IHOLD, AXIS3_DRIVER_IRUN, AXIS3_DRIVER_IGOTO, AXIS3_DRIVER_DECAY, AXIS3_DRIVER_DECAY_GOTO, AXIS3_DRIVER_STATUS },
  #endif
  #if AXIS4_DRIVER_MODEL != OFF
    { AXIS4_DRIVER_MODEL, AXIS4_DRIVER_MICROSTEPS, AXIS4_DRIVER_MICROSTEPS_GOTO, AXIS4_DRIVER_IHOLD, AXIS4_DRIVER_IRUN, AXIS4_DRIVER_IGOTO, AXIS4_DRIVER_DECAY, AXIS4_DRIVER_DECAY_GOTO, AXIS4_DRIVER_STATUS },
  #endif
  #if AXIS5_DRIVER_MODEL != OFF
    { AXIS5_DRIVER_MODEL, AXIS5_DRIVER_MICROSTEPS, AXIS5_DRIVER_MICROSTEPS_GOTO, AXIS5_DRIVER_IHOLD, AXIS5_DRIVER_IRUN, AXIS5_DRIVER_IGOTO, AXIS5_DRIVER_DECAY, AXIS5_DRIVER_DECAY_GOTO, AXIS5_DRIVER_STATUS },
  #endif
  #if AXIS6_DRIVER_MODEL != OFF
    { AXIS6_DRIVER_MODEL, AXIS6_DRIVER_MICROSTEPS, AXIS6_DRIVER_MICROSTEPS_GOTO, AXIS6_DRIVER_IHOLD, AXIS6_DRIVER_IRUN, AXIS6_DRIVER_IGOTO, AXIS6_DRIVER_DECAY, AXIS6_DRIVER_DECAY_GOTO, AXIS6_DRIVER_STATUS },
  #endif
  #if AXIS7_DRIVER_MODEL != OFF
    { AXIS7_DRIVER_MODEL, AXIS7_DRIVER_MICROSTEPS, AXIS7_DRIVER_MICROSTEPS_GOTO, AXIS7_DRIVER_IHOLD, AXIS7_DRIVER_IRUN, AXIS7_DRIVER_IGOTO, AXIS7_DRIVER_DECAY, AXIS7_DRIVER_DECAY_GOTO, AXIS7_DRIVER_STATUS },
  #endif
  #if AXIS8_DRIVER_MODEL != OFF
    { AXIS8_DRIVER_MODEL, AXIS8_DRIVER_MICROSTEPS, AXIS8_DRIVER_MICROSTEPS_GOTO, AXIS8_DRIVER_IHOLD, AXIS8_DRIVER_IRUN, AXIS8_DRIVER_IGOTO, AXIS8_DRIVER_DECAY, AXIS8_DRIVER_DECAY_GOTO, AXIS8_DRIVER_STATUS },
  #endif
  #if AXIS9_DRIVER_MODEL != OFF
    { AXIS9_DRIVER_MODEL, AXIS9_DRIVER_MICROSTEPS, AXIS9_DRIVER_MICROSTEPS_GOTO, AXIS9_DRIVER_IHOLD, AXIS9_DRIVER_IRUN, AXIS9_DRIVER_IGOTO, AXIS9_DRIVER_DECAY, AXIS9_DRIVER_DECAY_GOTO, AXIS9_DRIVER_STATUS },
  #endif
};

void StepDriver::init(uint8_t axisNumber, int16_t microsteps, int16_t current) {
  this->axisNumber = axisNumber;

  // load constants for this axis
  for (uint8_t i = 0; i < 10; i++) { if (ModePins[i].axis == axisNumber) { index = i; settings = ModeSettings[i]; break; } if (i == 9) { VLF("ERR: StepDriver::init(); indexing failed!"); return; } }

  // update the current from initialization setting
  if (settings.currentRun != OFF && settings.currentRun != current) {
    settings.currentRun = current;
    settings.currentGoto = current;
    settings.currentHold = current/2;
  }

  // update the microsteps from the initialization setting
  settings.microsteps = microsteps;

  #if DEBUG == VERBOSE
    VF("MSG: StepDriver, init model "); V(DRIVER_NAME[settings.model]);
    VF(" u-step mode "); if (settings.microsteps == OFF) VF("OFF"); else { V(settings.microsteps); V("X"); }
    VF(" (goto mode "); if (settings.microstepsGoto == SAME) VF("SAME)"); else { V(settings.microstepsGoto); V("X)"); }
    if (settings.model == TMC2130 || settings.model == TMC5160) { V(" Irun="); V(current); VL("mA"); } else VL("");
  #endif

  if (settings.microstepsGoto == SAME) settings.microstepsGoto = settings.microsteps;
  microstepCode = subdivisionsToCode(settings.microsteps);
  microstepCodeGoto = subdivisionsToCode(settings.microstepsGoto);
  microstepRatio = settings.microsteps/settings.microstepsGoto;

  if (isTmcSPI()) {
    #ifdef HAS_TMC_DRIVER
      if (settings.decay == OFF) settings.decay = STEALTHCHOP;
      if (settings.decayGoto == OFF) settings.decayGoto = SPREADCYCLE;
      tmcDriver.init(settings.model, ModePins[index].m0, ModePins[index].m1, ModePins[index].m2, ModePins[index].m3);
      if (settings.decay == STEALTHCHOP || settings.decayGoto == STEALTHCHOP) {
        tmcDriver.mode(true, STEALTHCHOP, microstepCode, settings.currentRun, settings.currentRun);
        VLF("MSG: StepDriver, TMC standstill automatic current calibration");
        delay(100);
      }
      tmcDriver.mode(true, settings.decay, microstepCode, settings.currentRun, settings.currentHold);
    #endif
  } else {
    if (isDecayOnM2()) { decayPin = ModePins[index].m2; m2Pin = OFF; } else { decayPin = ModePins[index].decay; m2Pin = ModePins[index].m2; }
    pinModeEx(decayPin, OUTPUT);
    digitalWriteEx(decayPin, getDecayPinState(settings.decay));

    microstepBitCode = microstepCode;
    microstepBitCodeGoto = microstepCodeGoto;
    pinModeEx(ModePins[index].m0, OUTPUT);
    digitalWriteEx(ModePins[index].m0, bitRead(microstepBitCode, 0));
    pinModeEx(ModePins[index].m1, OUTPUT);
    digitalWriteEx(ModePins[index].m1, bitRead(microstepBitCode, 1));
    pinModeEx(m2Pin, OUTPUT);
    digitalWriteEx(m2Pin, bitRead(microstepBitCode, 2));
    pinModeEx(ModePins[index].m3, INPUT);
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
    digitalWriteEx(ModePins[index].m0, bitRead(microstepBitCode, 0));
    digitalWriteEx(ModePins[index].m1, bitRead(microstepBitCode, 1));
    digitalWriteEx(ModePins[index].m2, bitRead(microstepBitCode, 2));
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
      digitalWriteEx(ModePins[index].m0, bitRead(microstepBitCodeGoto, 0));
      digitalWriteEx(ModePins[index].m1, bitRead(microstepBitCodeGoto, 1));
      digitalWriteEx(ModePins[index].m2, bitRead(microstepBitCodeGoto, 2));
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
    status.fault = digitalReadEx(ModePins[index].fault) == settings.status;
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
int StepDriver::subdivisionsToCode(uint8_t microsteps) {
  int allowed[9] = {1,2,4,8,16,32,64,128,256};
  if (settings.model >= DRIVER_MODEL_COUNT) return OFF;
  for (int i = 0; i < 9; i++) {
    if (microsteps == allowed[i]) return steps[settings.model][i];
  }
  return OFF;
}

#endif