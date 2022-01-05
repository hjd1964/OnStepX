// -----------------------------------------------------------------------------------
// axis step/dir motor driver

#include "StepDirDrivers.h"

#ifdef STEP_DIR_MOTOR_PRESENT

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
 {  0,  0,  0,  0,  0,  0,  0,  0,  0}};  // GENERIC

#if DEBUG != OFF
  const char* DRIVER_NAME[DRIVER_MODEL_COUNT] = {
  "A4988", "DRV8825", "S109", "LV8729", "RAPS128", "TMC2100",
  "TMC2208", "TMC2209", "ST820", "TMC2130", "TMC5160", "GENERIC" };
#endif

// constructor
StepDirDriver::StepDirDriver(uint8_t axisNumber, const DriverModePins *Pins, const DriverSettings *Settings) {
  this->axisNumber = axisNumber;
  this->Pins = Pins;
  settings = *Settings;
}

// decodes driver model/microstep mode into microstep codes (bit patterns or SPI) and sets up the pin modes
void StepDirDriver::setParam(float param1, float param2, float param3, float param4, float param5, float param6) {
  settings.microsteps = round(param1);
  settings.microstepsGoto = round(param2);
  settings.currentHold = round(param3);
  settings.currentRun  = round(param4);
  settings.currentGoto = round(param5);
  UNUSED(param6);

  if (!isTmcSPI()) {
    if (settings.currentHold != OFF || settings.currentRun != OFF || settings.currentGoto != OFF) {
      VF("WRN: StepDvr"); V(axisNumber); VLF(", incorrect model for current control, disabling current settings");
      settings.currentHold = OFF;
      settings.currentRun = OFF;
      settings.currentGoto = OFF;
    }
  }

  if (settings.currentRun != OFF) {
    // automatically set goto and hold current if they are disabled
    if (settings.currentGoto == OFF) settings.currentGoto = settings.currentRun;
    if (settings.currentHold == OFF) settings.currentHold = lround(settings.currentRun/2.0F);
  } else {
    // set current defaults for TMC drivers
    settings.currentRun = 600;
    if (settings.model == TMC2130) settings.currentRun = 2500;
    settings.currentGoto = settings.currentRun;
    settings.currentHold = lround(settings.currentRun/2.0F);
  }

  VF("MSG: StepDvr"); V(axisNumber); VF(", init model "); V(DRIVER_NAME[settings.model]);
  VF(" u-step mode "); if (settings.microsteps == OFF) { VF("OFF (assuming 1X)"); settings.microsteps = 1; } else { V(settings.microsteps); VF("X"); }
  VF(" (goto mode "); if (settings.microstepsGoto == OFF) { VLF("OFF)"); } else { V(settings.microstepsGoto); VL("X)"); }

  if (settings.microstepsGoto == OFF) settings.microstepsGoto = settings.microsteps;
  microstepCode = subdivisionsToCode(settings.microsteps);
  microstepCodeGoto = subdivisionsToCode(settings.microstepsGoto);
  microstepRatio = settings.microsteps/settings.microstepsGoto;

  if (isTmcSPI()) {
    #ifdef TMC_DRIVER_PRESENT
      if (settings.decay == OFF) settings.decay = STEALTHCHOP;
      if (settings.decayGoto == OFF) settings.decayGoto = SPREADCYCLE;
      tmcDriver.init(settings.model, Pins->m0, Pins->m1, Pins->m2, Pins->m3);
      VF("MSG: StepDvr"); V(axisNumber); VF(", TMC ");
      if (settings.currentRun == OFF) {
        VLF("current control OFF (set by Vref)");
      } else {
        VF("Ihold="); V(settings.currentHold); VF("mA, ");
        VF("Irun="); V(settings.currentRun); VF("mA, ");
        VF("Igoto="); V(settings.currentGoto); VL("mA");
      }
      if (settings.decay == STEALTHCHOP || settings.decayGoto == STEALTHCHOP) {
        tmcDriver.mode(true, STEALTHCHOP, microstepCode, settings.currentRun, settings.currentRun);
        VF("MSG: StepDvr"); V(axisNumber); VL(", TMC standstill automatic current calibration");
        delay(100);
      }
      tmcDriver.mode(true, settings.decay, microstepCode, settings.currentRun, settings.currentHold);
    #endif
  } else {
    if (isDecayOnM2()) { decayPin = Pins->m2; m2Pin = OFF; } else { decayPin = Pins->decay; m2Pin = Pins->m2; }
    pinModeEx(decayPin, OUTPUT);
    digitalWriteEx(decayPin, getDecayPinState(settings.decay));

    #if DEBUG == VERBOSE
      VF("MSG: StepDvr"); V(axisNumber);
      V(", pins m0="); if (Pins->m0 == OFF) V("OFF"); else V(Pins->m0);
      V(", m1="); if (Pins->m1 == OFF) VF("OFF"); else V(Pins->m1);
      V(", m2="); if (m2Pin == OFF) VF("OFF"); else V(m2Pin);
      if (!isTmcSPI()) {
        V(", decay="); if (decayPin == OFF) VF("OFF"); else V(decayPin);
        if (settings.status == ON) {
          V(", fault="); if (Pins->fault == OFF) VF("OFF"); else V(Pins->fault);
        }
      }
      VL("");
    #endif

    microstepBitCode = microstepCode;
    microstepBitCodeGoto = microstepCodeGoto;
    pinModeEx(Pins->m0, OUTPUT);
    digitalWriteEx(Pins->m0, bitRead(microstepBitCode, 0));
    pinModeEx(Pins->m1, OUTPUT);
    digitalWriteEx(Pins->m1, bitRead(microstepBitCode, 1));
    pinModeEx(m2Pin, OUTPUT);
    digitalWriteEx(m2Pin, bitRead(microstepBitCode, 2));
  }

  // automatically set fault status for known drivers
  status.active = settings.status != OFF;

  if (settings.status == ON) {
    switch (settings.model) {
      case DRV8825: settings.status = LOW; break;
      case ST820:   settings.status = LOW; break;
      default: break;
    }
  }

  // set fault pin mode
  if (settings.status == LOW) pinModeEx(Pins->fault, INPUT_PULLUP);
  #ifdef PULLDOWN
    if (settings.status == HIGH) pinModeEx(Pins->fault, INPUT_PULLDOWN);
  #else
    if (settings.status == HIGH) pinModeEx(Pins->fault, INPUT);
  #endif
}

// validate driver parameters
bool StepDirDriver::validateParam(float param1, float param2, float param3, float param4, float param5, float param6) {
  int index = axisNumber - 1;
  if (index > 3) index = 3;

  int maxCurrent;
  if (settings.model == TMC2130) maxCurrent = 1500; else
  if (settings.model == TMC5160) maxCurrent = 3000; else maxCurrent = OFF;

  long subdivisions = round(param1);
  long subdivisionsGoto = round(param2);
  long currentHold = round(param3);
  long currentRun = round(param4);
  long currentGoto = round(param5);
  UNUSED(param6);

  if (subdivisions == OFF) {
    VF("WRN, StepDirDrivers::validateParam(): Axis"); V(axisNumber); VLF(" subdivisions OFF (assuming 1X)");
    subdivisions = 1;
  }

  if (subdivisions <= subdivisionsGoto) {
    DF("ERR, StepDirDrivers::validateParam(): Axis"); D(axisNumber); DLF(" subdivisions must be > subdivisionsGoto");
    return false;
  }

  if (subdivisions != OFF && (subdivisionsToCode(subdivisions) == OFF)) {
    DF("ERR, StepDirDrivers::validateParam(): Axis"); D(axisNumber); DF(" bad subdivisions="); DL(subdivisions);
    return false;
  }

  if (subdivisionsGoto != OFF && (subdivisionsToCode(subdivisionsGoto) == OFF)) {
    DF("ERR, StepDirDrivers::validateParam(): Axis"); D(axisNumber); DF(" bad subdivisionsGoto="); DL(subdivisionsGoto);
    return false;
  }

  if (isTmcSPI()) {
    if (currentHold != OFF && (currentHold < 0 || currentHold > maxCurrent)) {
      DF("ERR, StepDirDrivers::validateParam(): Axis"); D(axisNumber); DF(" bad current hold="); DL(currentHold);
      return false;
    }

    if (currentRun != OFF && (currentRun < 0 || currentRun > maxCurrent)) {
      DF("ERR, StepDirDrivers::validateParam(): Axis"); D(axisNumber); DF(" bad current run="); DL(currentRun);
      return false;
    }

    if (currentGoto != OFF && (currentGoto < 0 || currentGoto > maxCurrent)) {
      DF("ERR, StepDirDrivers::validateParam(): Axis"); D(axisNumber); DF(" bad current goto="); DL(currentGoto);
      return false;
    }
  }

  return true;
}

bool StepDirDriver::modeSwitchAllowed() {
  return microstepRatio != 1;  
}

void StepDirDriver::modeMicrostepTracking() {
  if (isTmcSPI()) {
    #ifdef TMC_DRIVER_PRESENT
      tmcDriver.refresh_CHOPCONF(microstepCode);
    #endif
  } else {
    noInterrupts();
    digitalWriteEx(Pins->m0, bitRead(microstepBitCode, 0));
    digitalWriteEx(Pins->m1, bitRead(microstepBitCode, 1));
    digitalWriteEx(m2Pin, bitRead(microstepBitCode, 2));
    interrupts();
  }
}

void StepDirDriver::modeDecayTracking() {
  if (isTmcSPI()) {
    #ifdef TMC_DRIVER_PRESENT
      tmcDriver.mode(true, settings.decay, microstepCode, settings.currentRun, settings.currentHold);
    #endif
  } else {
    if (settings.decay == OFF) return;
    int8_t state = getDecayPinState(settings.decay);
    noInterrupts();
    digitalWriteEx(decayPin, state);
    interrupts();
  }
}

// get microstep ratio for slewing
int StepDirDriver::getMicrostepRatio() {
  return microstepRatio;
}

int StepDirDriver::modeMicrostepSlewing() {
  if (microstepRatio > 1) {
    if (isTmcSPI()) {
      #ifdef TMC_DRIVER_PRESENT
        tmcDriver.refresh_CHOPCONF(microstepCodeGoto);
      #endif
    } else {
      noInterrupts();
      digitalWriteEx(Pins->m0, bitRead(microstepBitCodeGoto, 0));
      digitalWriteEx(Pins->m1, bitRead(microstepBitCodeGoto, 1));
      digitalWriteEx(m2Pin, bitRead(microstepBitCodeGoto, 2));
      interrupts();
    }
  }
  return microstepRatio;
}

void StepDirDriver::modeDecaySlewing() {
  if (isTmcSPI()) {
    #ifdef TMC_DRIVER_PRESENT
      int IGOTO = settings.currentGoto;
      if (IGOTO == OFF) IGOTO = settings.currentRun;
      tmcDriver.mode(true, settings.decayGoto, microstepCode, IGOTO, settings.currentHold);
    #endif
  } else {
    if (settings.decayGoto == OFF) return;
    int8_t state = getDecayPinState(settings.decayGoto);
    noInterrupts();
    if (state != OFF) digitalWriteEx(decayPin, state);
    interrupts();
  }
}

void StepDirDriver::updateStatus() {
  #ifdef TMC_DRIVER_PRESENT
    if (settings.status == ON) {
      if (tmcDriver.refresh_DRVSTATUS()) {
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

      } else {
        status.outputA.shortToGround = true;
        status.outputA.openLoad      = true;
        status.outputB.shortToGround = true;
        status.outputB.openLoad      = true;
        status.overTemperaturePreWarning = true;
        status.overTemperature       = true;
        status.standstill            = true;
        status.fault                 = true;
      }
    } else
  #endif
  if (settings.status == LOW || settings.status == HIGH) {
    status.fault = digitalReadEx(Pins->fault) == settings.status;
  }
}

DriverStatus StepDirDriver::getStatus() {
  return status;
}

int8_t StepDirDriver::getDecayPinState(int8_t decay) {
  uint8_t state = OFF;
  if (decay == SPREADCYCLE) state = HIGH; else
  if (decay == STEALTHCHOP) state = LOW;  else
  if (decay == MIXED)       state = LOW;  else
  if (decay == FAST)        state = HIGH;
  return state;
}

// secondary way to power down not using the enable pin
void StepDirDriver::power(bool state) {
  #ifdef TMC_DRIVER_PRESENT
    int I_run = 0, I_hold = 0;
    if (state) { I_run = settings.currentRun; I_hold = settings.currentHold; }
    tmcDriver.mode(true, settings.decay, microstepCode, I_run, I_hold);
  #else
    state = state;
  #endif
}

// checks for TMC SPI driver
bool StepDirDriver::isTmcSPI() {
  #ifdef TMC_DRIVER_PRESENT
    if (settings.model == TMC2130 || settings.model == TMC5160) return true; else return false;
  #else
    return false;
  #endif
}

bool StepDirDriver::isDecayOnM2() {
  if (settings.model == TMC2209) return true; else return false;
}

// different models of stepper drivers have different bit settings for microsteps
// translate the human readable microsteps in the configuration to mode bit settings
// returns bit code (0 to 7) or OFF if microsteps is not supported or unknown
int StepDirDriver::subdivisionsToCode(long microsteps) {
  int allowed[9] = {1,2,4,8,16,32,64,128,256};
  if (settings.model >= DRIVER_MODEL_COUNT) return OFF;
  for (int i = 0; i < 9; i++) {
    if (microsteps == allowed[i]) return steps[settings.model][i];
  }
  return OFF;
}

#endif
