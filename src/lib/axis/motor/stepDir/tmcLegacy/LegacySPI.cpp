// -----------------------------------------------------------------------------------
// axis step/dir motor driver

// note: LegacySPI requires MOSI, SCK, CS but MISO is optional

#include "LegacySPI.h"

#if !defined(DRIVER_TMC_STEPPER) && defined(STEP_DIR_TMC_SPI_PRESENT)

#include "../../../../gpioEx/GpioEx.h"

// help with pin names
#define mosi m0
#define sck  m1
#define cs   m2
#define miso m3

// constructor
StepDirTmcSPI::StepDirTmcSPI(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings) {
  this->axisNumber = axisNumber;

  strcpy(axisPrefix, " Axis_StepDirTmcSPI legacy, ");
  axisPrefix[5] = '0' + axisNumber;

  this->Pins = Pins;
  settings = *Settings;
}

// set up driver and parameters: microsteps, microsteps goto, hold current, run current, goto current, unused
bool StepDirTmcSPI::init() {

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

  if (settings.currentRun == OFF) {
    VF("MSG:"); V(axisPrefix); VLF("current control OFF (set by Vref)");
  } else {
    VF("MSG:"); V(axisPrefix);
    VF("Ihold="); V(settings.currentHold); VF("mA, ");
    VF("Irun="); V(settings.currentRun); VF("mA, ");
    VF("Igoto="); V(settings.currentGoto); VL("mA");
  }

  if (settings.model == TMC5160) {
    if (user_rSense > 0.0F) rSense = user_rSense; else rSense = TMC5160_RSENSE;
  } else
  if (settings.model == TMC2130) {
    if (user_rSense > 0.0F) rSense = user_rSense; else rSense = TMC2130_RSENSE;
  }
  VF("MSG:"); V(axisPrefix); VF("Rsense="); V(rSense); VL("ohms");

  // get TMC SPI ready
  driver.init(settings.model, Pins->m0, Pins->m1, Pins->m2, Pins->m3, axisNumber);

  // get driver ready and check to see if it's there (if possible)
  if (!driver.mode(settings.intpol, settings.decay, microstepCode, settings.currentRun, settings.currentHold, rSense)) {
    DF("ERR:"); D(axisPrefix); DLF("no motor driver device detected!");
    return false;
  } else {
    if (Pins->m3 != OFF) {
      VF("MSG:"); V(axisPrefix); VLF("motor driver device detected");
    }
  }

  // automatically set fault status for known drivers
  status.active = settings.status != OFF;

  // set fault pin mode
  if (settings.status == LOW) pinModeEx(Pins->fault, INPUT_PULLUP);
  #ifdef PULLDOWN
    if (settings.status == HIGH) pinModeEx(Pins->fault, INPUT_PULLDOWN);
  #else
    if (settings.status == HIGH) pinModeEx(Pins->fault, INPUT);
  #endif

  // set mode switching support flags
  // use low speed mode switch for TMC drivers or high speed otherwise
  modeSwitchAllowed = microstepRatio != 1;
  modeSwitchFastAllowed = false;

  return true;
}

// validate driver parameters
bool StepDirTmcSPI::validateParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  if (!StepDirDriver::validateParameters(param1, param2, param3, param4, param5, param6)) return false;

  if (settings.model == TMC2130) currentMax = TMC2130_MAX_CURRENT; else
  if (settings.model == TMC5160) currentMax = TMC5160_MAX_CURRENT; else
  {
    DF("WRN:"); D(axisPrefix); DLF("unknown driver model!");
    return false;
  }

  // override max current with user setting
  if (user_currentMax > 0) currentMax = user_currentMax;

  long currentHold = round(param3);
  long currentRun = round(param4);
  long currentGoto = round(param5);
  UNUSED(param6);

  if (currentHold != OFF && (currentHold < 0 || currentHold > currentMax)) {
    DF("WRN:"); D(axisPrefix); DF("bad current hold="); D(currentHold); DLF("mA");
    return false;
  }

  if (currentRun != OFF && (currentRun < 0 || currentRun > currentMax)) {
    DF("WRN:"); D(axisPrefix); DF(" bad current run="); D(currentRun); DLF("mA");
    return false;
  }

  if (currentGoto != OFF && (currentGoto < 0 || currentGoto > currentMax)) {
    DF("WRN:"); D(axisPrefix); DF("bad current goto="); D(currentGoto); DLF("mA");
    return false;
  }

  return true;
}

void StepDirTmcSPI::modeMicrostepTracking() {
  driver.refresh_CHOPCONF(microstepCode);
}

int StepDirTmcSPI::modeMicrostepSlewing() {
  if (microstepRatio > 1) {
    driver.refresh_CHOPCONF(microstepCodeSlewing);
  }
  return microstepRatio;
}

void StepDirTmcSPI::modeDecayTracking() {
  driver.mode(settings.intpol, settings.decay, microstepCode, settings.currentRun, settings.currentHold, rSense);
}

void StepDirTmcSPI::modeDecaySlewing() {
  int IGOTO = settings.currentGoto;
  if (IGOTO == OFF) IGOTO = settings.currentRun;
  driver.mode(settings.intpol, settings.decaySlewing, microstepCode, IGOTO, settings.currentHold, rSense);
}

void StepDirTmcSPI::readStatus() {
  if (driver.refresh_DRVSTATUS()) {
    status.outputA.shortToGround  = driver.get_DRVSTATUS_s2gA();
    status.outputA.openLoad       = driver.get_DRVSTATUS_olA();
    status.outputB.shortToGround  = driver.get_DRVSTATUS_s2gB();
    status.outputB.openLoad       = driver.get_DRVSTATUS_olB();
    status.overTemperatureWarning = driver.get_DRVSTATUS_otpw();
    status.overTemperature        = driver.get_DRVSTATUS_ot();
    status.standstill             = driver.get_DRVSTATUS_stst();
  }
}

// secondary way to power down not using the enable pin
bool StepDirTmcSPI::enable(bool state) {
  if (state) {
    driver.mode(settings.intpol, settings.decay, microstepCode, settings.currentRun, settings.currentHold, rSense);
  } else {
    driver.mode(settings.intpol, STEALTHCHOP, microstepCode, settings.currentRun, 0, rSense);
  }
  return true;
}

// calibrate the motor driver if required
void StepDirTmcSPI::calibrateDriver() {
  if (settings.decay == STEALTHCHOP || settings.decaySlewing == STEALTHCHOP) {
    VF("MSG:"); V(axisPrefix); VL("TMC standstill automatic current calibration");
    driver.mode(settings.intpol, STEALTHCHOP, microstepCode, settings.currentRun, settings.currentRun, rSense);
    delay(1000);
    driver.mode(settings.intpol, settings.decay, microstepCode, settings.currentRun, settings.currentHold, rSense);
  }
}

#endif
