// -----------------------------------------------------------------------------------
// axis step/dir motor driver

// note: StepperSPI requires MOSI, SCK, CS, and MISO

#include "StepperSPI.h"
 
#if defined(DRIVER_TMC_STEPPER) && defined(STEP_DIR_TMC_SPI_PRESENT)

#include "../../../../gpioEx/GpioEx.h"

// help with pin names
#define mosi m0
#define sck  m1
#define cs   m2
#define miso m3

// constructor
StepDirTmcSPI::StepDirTmcSPI(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings) {
  this->axisNumber = axisNumber;

  strcpy(axisPrefix, " Axis_StepDirTmcSPI, ");
  axisPrefix[5] = '0' + axisNumber;

  this->Pins = Pins;
  settings = *Settings;
}

// setup driver
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

  if (settings.model == TMC2130) {
    if (user_rSense > 0.0F) rSense = user_rSense; else rSense = TMC2130_RSENSE;
    #ifdef DRIVER_TMC_STEPPER_HW_SPI
      driver = new TMC2130Stepper(Pins->cs, rSense);
    #else
      driver = new TMC2130Stepper(Pins->cs, rSense, Pins->mosi, Pins->miso, Pins->sck);
    #endif
    ((TMC2130Stepper*)driver)->begin();
    ((TMC2130Stepper*)driver)->intpol(settings.intpol);
    modeMicrostepTracking();
    ((TMC2130Stepper*)driver)->en_pwm_mode(false);
  } else
  if (settings.model == TMC2160) {
    if (user_rSense > 0.0F) rSense = user_rSense; else rSense = TMC2160_RSENSE;
    #ifdef TMC2160_RSENSE_KRAKEN
      if (axisNumber <= 4) rSense = TMC2160_RSENSE_KRAKEN;
    #endif
    #ifdef DRIVER_TMC_STEPPER_HW_SPI
      driver = new TMC2160Stepper(Pins->cs, rSense);
    #else
      driver = new TMC2160Stepper(Pins->cs, rSense, Pins->mosi, Pins->miso, Pins->sck);
    #endif
    ((TMC2160Stepper*)driver)->begin();
    ((TMC2160Stepper*)driver)->intpol(settings.intpol);
    modeMicrostepTracking();
    ((TMC2160Stepper*)driver)->en_pwm_mode(false);
  } else
  if (settings.model == TMC2660) {
    if (user_rSense > 0.0F) rSense = user_rSense; else rSense = TMC2660_RSENSE;
    #ifdef DRIVER_TMC_STEPPER_HW_SPI
      driver = new TMC2660Stepper(Pins->cs, rSense);
    #else
      driver = new TMC2660Stepper(Pins->cs, rSense, Pins->mosi, Pins->miso, Pins->sck);
    #endif
    ((TMC2660Stepper*)driver)->begin();
    ((TMC2660Stepper*)driver)->toff(5);
    ((TMC2660Stepper*)driver)->intpol(settings.intpol);
    modeMicrostepTracking();
  } else
  if (settings.model == TMC5160) {
    if (user_rSense > 0.0F) rSense = user_rSense; else rSense = TMC5160_RSENSE;
    #ifdef DRIVER_TMC_STEPPER_HW_SPI
      driver = new TMC5160Stepper(Pins->cs, rSense);
    #else
      driver = new TMC5160Stepper(Pins->cs, rSense, Pins->mosi, Pins->miso, Pins->sck);
    #endif
    ((TMC5160Stepper*)driver)->begin();
    ((TMC5160Stepper*)driver)->intpol(settings.intpol);
    modeMicrostepTracking();
    ((TMC5160Stepper*)driver)->en_pwm_mode(false);
  } else
  if (settings.model == TMC5161) {
    if (user_rSense > 0.0F) rSense = user_rSense; else rSense = TMC5161_RSENSE;
    #ifdef DRIVER_TMC_STEPPER_HW_SPI
      driver = new TMC5161Stepper(Pins->cs, rSense);
    #else
      driver = new TMC5161Stepper(Pins->cs, rSense, Pins->mosi, Pins->miso, Pins->sck);
    #endif
    ((TMC5161Stepper*)driver)->begin();
    ((TMC5161Stepper*)driver)->intpol(settings.intpol);
    modeMicrostepTracking();
    ((TMC5161Stepper*)driver)->en_pwm_mode(false);
  } else {
    DF("ERR:"); D(axisPrefix); DLF("unknown driver model exiting!");
    return false;
  }

  // show the selected Rsense
  VF("MSG:"); V(axisPrefix); VF("Rsense="); V(rSense); VL("ohms");

  current(settings.currentRun, (float)settings.currentHold/settings.currentRun);

  // automatically set fault status for known drivers
  status.active = settings.status != OFF;

  // if we can, check to see if the driver is there
  // check to see if the driver is there and ok
  #ifdef MOTOR_DRIVER_DETECT
    #ifndef DRIVER_TMC_STEPPER_HW_SPI
      if (Pins->miso != OFF)
    #endif
    {
      readStatus();
      if (!status.standstill || status.overTemperature) {
        DF("ERR:"); D(axisPrefix); DLF("no motor driver device detected!");
        return false;
      } else { VF("MSG:"); V(axisPrefix); VLF("motor driver device detected"); }
    }
  #endif

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
  if (settings.model == TMC2160) currentMax = TMC2160_MAX_CURRENT; else
  if (settings.model == TMC2660) currentMax = TMC2660_MAX_CURRENT; else
  if (settings.model == TMC5160) currentMax = TMC5160_MAX_CURRENT; else
  if (settings.model == TMC5161) currentMax = TMC5161_MAX_CURRENT; else
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
    DF("WRN:"); D(axisPrefix); DF("bad current run="); D(currentRun); DLF("mA");
    return false;
  }

  if (currentGoto != OFF && (currentGoto < 0 || currentGoto > currentMax)) {
    DF("WRN:"); D(axisPrefix); DF("bad current goto="); D(currentGoto); DLF("mA");
    return false;
  }

  return true;
}

void StepDirTmcSPI::modeMicrostepTracking() {
  int16_t microsteps = 0;
  if (settings.microsteps > 1) microsteps = settings.microsteps;
  switch (settings.model) {
    case TMC2130: ((TMC2130Stepper*)driver)->microsteps(microsteps); break;
    case TMC2160: ((TMC2160Stepper*)driver)->microsteps(microsteps); break;
    case TMC2660: ((TMC2660Stepper*)driver)->microsteps(microsteps); break;
    case TMC5160: ((TMC5160Stepper*)driver)->microsteps(microsteps); break;
    case TMC5161: ((TMC5161Stepper*)driver)->microsteps(microsteps); break;
  }
}

int StepDirTmcSPI::modeMicrostepSlewing() {
  if (microstepRatio > 1) {
    int16_t microsteps = 0;
    if (settings.microstepsSlewing > 1) microsteps = settings.microstepsSlewing;
    switch (settings.model) {
      case TMC2130: ((TMC2130Stepper*)driver)->microsteps(microsteps); break;
      case TMC2160: ((TMC2160Stepper*)driver)->microsteps(microsteps); break;
      case TMC2660: ((TMC2660Stepper*)driver)->microsteps(microsteps); break;
      case TMC5160: ((TMC5160Stepper*)driver)->microsteps(microsteps); break;
      case TMC5161: ((TMC5161Stepper*)driver)->microsteps(microsteps); break;
    }
  }
  return microstepRatio;
}

void StepDirTmcSPI::modeDecayTracking() {
  setDecayMode(settings.decay);
  current(settings.currentRun, (float)settings.currentHold/settings.currentRun);
}

void StepDirTmcSPI::modeDecaySlewing() {
  setDecayMode(settings.decaySlewing);
  int IGOTO = settings.currentGoto;
  if (IGOTO == OFF) IGOTO = settings.currentRun;
  current(IGOTO, 1.0F);
}

void StepDirTmcSPI::readStatus() {
  TMC2130_n::DRV_STATUS_t status_result;
  status_result.sr = 0;

  switch (settings.model) {
    case TMC2130: status_result.sr = ((TMC2130Stepper*)driver)->DRV_STATUS(); break;
    case TMC2160: status_result.sr = ((TMC2160Stepper*)driver)->DRV_STATUS(); break;
    case TMC2660: status_result.sr = ((TMC2660Stepper*)driver)->DRV_STATUS(); break;
    case TMC5160: status_result.sr = ((TMC5160Stepper*)driver)->DRV_STATUS(); break;
    case TMC5161: status_result.sr = ((TMC5161Stepper*)driver)->DRV_STATUS(); break;
  }

  status.outputA.shortToGround = status_result.s2ga;
  status.outputA.openLoad      = status_result.ola;
  status.outputB.shortToGround = status_result.s2gb;
  status.outputB.openLoad      = status_result.olb;
  status.overTemperatureWarning= status_result.otpw;
  status.overTemperature       = status_result.ot;
  status.standstill            = status_result.stst;
}

// secondary way to power down not using the enable pin
bool StepDirTmcSPI::enable(bool state) {
  if (state) {
    modeDecayTracking();
  } else {
    setDecayMode(STEALTHCHOP);
    switch (settings.model) {
      case TMC2130: ((TMC2130Stepper*)driver)->ihold(0); break;
      case TMC2160: ((TMC2160Stepper*)driver)->ihold(0); break;
      case TMC2660: ((TMC2660Stepper*)driver)->rms_current(0); break;
      case TMC5160: ((TMC5160Stepper*)driver)->ihold(0); break;
      case TMC5161: ((TMC5161Stepper*)driver)->ihold(0); break;
    }
  }
  return true;
}

// calibrate the motor driver if required
void StepDirTmcSPI::calibrateDriver() {
  if (settings.decay == STEALTHCHOP || settings.decaySlewing == STEALTHCHOP) {
    VF("MSG:"); V(axisPrefix); VL("standstill automatic current calibration");
    current(settings.currentRun, 1.0F);
    if (settings.model == TMC2130) {
      ((TMC2130Stepper*)driver)->pwm_autoscale(true);
      ((TMC2130Stepper*)driver)->en_pwm_mode(true);
    } else
    if (settings.model == TMC2160) {
      ((TMC2160Stepper*)driver)->pwm_autoscale(true);
      ((TMC2160Stepper*)driver)->en_pwm_mode(true);
    } else
    if (settings.model == TMC5160) {
      ((TMC5160Stepper*)driver)->pwm_autograd(DRIVER_TMC_STEPPER_AUTOGRAD);
      ((TMC5160Stepper*)driver)->pwm_autoscale(true);
      ((TMC5160Stepper*)driver)->en_pwm_mode(true);
    } else
    if (settings.model == TMC5161) {
      ((TMC5161Stepper*)driver)->pwm_autograd(DRIVER_TMC_STEPPER_AUTOGRAD);
      ((TMC5161Stepper*)driver)->pwm_autoscale(true);
      ((TMC5161Stepper*)driver)->en_pwm_mode(true);
    }
    delay(1000);
    modeDecayTracking();
  }
}

// set the decay mode STEALTHCHOP or SPREADCYCLE
void StepDirTmcSPI::setDecayMode(int decayMode) {
  switch (settings.model) {
    case TMC2130: ((TMC2130Stepper*)driver)->en_pwm_mode(decayMode != SPREADCYCLE); break;
    case TMC2160: ((TMC2160Stepper*)driver)->en_pwm_mode(decayMode != SPREADCYCLE); break;
    case TMC5160: ((TMC5160Stepper*)driver)->en_pwm_mode(decayMode != SPREADCYCLE); break;
    case TMC5161: ((TMC5161Stepper*)driver)->en_pwm_mode(decayMode != SPREADCYCLE); break;
  }
}

#endif
