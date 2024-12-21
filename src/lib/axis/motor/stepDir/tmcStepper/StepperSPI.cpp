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

  strcpy(axisPrefix, "MSG: Axis_StepDirTmcSPI, ");
  axisPrefix[9] = '0' + axisNumber;
  strcpy(axisPrefixWarn, "WRN: Axis_StepDirTmcSPI, ");
  axisPrefixWarn[9] = '0' + axisNumber;

  this->Pins = Pins;
  settings = *Settings;
}

// sets driver parameters: microsteps, microsteps goto, hold current, run current, goto current, unused
void StepDirTmcSPI::init(float param1, float param2, float param3, float param4, float param5, float param6) {
  StepDirDriver::init(param1, param2, param3, param4, param5, param6);

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

  VF(axisPrefix);
  if (settings.currentRun == OFF) {
    VLF("current control OFF (set by Vref)");
  } else {
    VF("Ihold="); V(settings.currentHold); VF("mA, ");
    VF("Irun="); V(settings.currentRun); VF("mA, ");
    VF("Igoto="); V(settings.currentGoto); VL("mA");
  }

  if (settings.model == TMC2130) {
    rSense = TMC2130_RSENSE;
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
  if (settings.model == TMC2660) {
    rSense = TMC2660_RSENSE;
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
    rSense = TMC5160_RSENSE;
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
    rSense = TMC5161_RSENSE;
    #ifdef DRIVER_TMC_STEPPER_HW_SPI
      driver = new TMC5161Stepper(Pins->cs, rSense);
    #else
      driver = new TMC5161Stepper(Pins->cs, rSense, Pins->mosi, Pins->miso, Pins->sck);
    #endif
    ((TMC5161Stepper*)driver)->begin();
    ((TMC5161Stepper*)driver)->intpol(settings.intpol);
    modeMicrostepTracking();
    ((TMC5161Stepper*)driver)->en_pwm_mode(false);
  }
  current(settings.currentRun, (float)settings.currentHold/settings.currentRun);

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
}

// validate driver parameters
bool StepDirTmcSPI::validateParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  if (!StepDirDriver::validateParameters(param1, param2, param3, param4, param5, param6)) return false;

  int maxCurrent;
  if (settings.model == TMC2130) maxCurrent = 1500; else
  if (settings.model == TMC2660) maxCurrent = 3000; else
  if (settings.model == TMC5160) maxCurrent = 3000; else
  if (settings.model == TMC5161) maxCurrent = 3500; else
  {
    DF(axisPrefixWarn); DLF("unknown driver model!");
    return false;
  }

  long currentHold = round(param3);
  long currentRun = round(param4);
  long currentGoto = round(param5);
  UNUSED(param6);

  if (currentHold != OFF && (currentHold < 0 || currentHold > maxCurrent)) {
    DF(axisPrefixWarn); DF("bad current hold="); DL(currentHold);
    return false;
  }

  if (currentRun != OFF && (currentRun < 0 || currentRun > maxCurrent)) {
    DF(axisPrefixWarn); DF("bad current run="); DL(currentRun);
    return false;
  }

  if (currentGoto != OFF && (currentGoto < 0 || currentGoto > maxCurrent)) {
    DF(axisPrefixWarn); DF("bad current goto="); DL(currentGoto);
    return false;
  }

  return true;
}

void StepDirTmcSPI::modeMicrostepTracking() {
  int16_t microsteps = 0;
  if (settings.microsteps > 1) microsteps = settings.microsteps;
  switch (settings.model) {
    case TMC2130: ((TMC2130Stepper*)driver)->microsteps(microsteps); break;
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

void StepDirTmcSPI::updateStatus() {
  if (settings.status == ON) {
    if ((long)(millis() - timeLastStatusUpdate) > 200) {

      TMC2130_n::DRV_STATUS_t status_result;
      status_result.sr = 0;
      switch (settings.model) {
        case TMC2130: status_result.sr = ((TMC2130Stepper*)driver)->DRV_STATUS(); break;
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

      // open load indication is not reliable in standstill
      if (status.outputA.shortToGround || status.outputB.shortToGround ||
          status.overTemperatureWarning || status.overTemperature) status.fault = true; else status.fault = false;

      timeLastStatusUpdate = millis();
    }
  } else
  if (settings.status == LOW || settings.status == HIGH) {
    status.fault = digitalReadEx(Pins->fault) == settings.status;
  }

  StepDirDriver::updateStatus();
}

// secondary way to power down not using the enable pin
bool StepDirTmcSPI::enable(bool state) {
  if (state) {
    modeDecayTracking();
  } else {
    setDecayMode(STEALTHCHOP);
    switch (settings.model) {
      case TMC2130: ((TMC2130Stepper*)driver)->ihold(0); break;
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
    VF(axisPrefix); VL("standstill automatic current calibration");
    current(settings.currentRun, 1.0F);
    if (settings.model == TMC2130) {
      ((TMC2130Stepper*)driver)->pwm_autoscale(true);
      ((TMC2130Stepper*)driver)->en_pwm_mode(true);
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
    case TMC5160: ((TMC5160Stepper*)driver)->en_pwm_mode(decayMode != SPREADCYCLE); break;
    case TMC5161: ((TMC5161Stepper*)driver)->en_pwm_mode(decayMode != SPREADCYCLE); break;
  }
}

#endif
