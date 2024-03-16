// -----------------------------------------------------------------------------------
// axis step/dir motor driver

// note: LegacySPI requires MOSI, SCK, CS but MISO is optional

#include "LegacySPI.h"

#if !defined(DRIVER_TMC_STEPPER) && defined(STEP_DIR_TMC_SPI_PRESENT)

// help with pin names
#define mosi m0
#define sck  m1
#define cs   m2
#define miso m3

// constructor
StepDirTmcSPI::StepDirTmcSPI(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings) {
  this->axisNumber = axisNumber;
  this->Pins = Pins;
  settings = *Settings;
}

// set up driver and parameters: microsteps, microsteps goto, hold current, run current, goto current, unused
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

  VF("MSG: StepDirDriver"); V(axisNumber); VF(", TMC ");
  if (settings.currentRun == OFF) {
    VLF("current control OFF (set by Vref)");
  } else {
    VF("Ihold="); V(settings.currentHold); VF("mA, ");
    VF("Irun="); V(settings.currentRun); VF("mA, ");
    VF("Igoto="); V(settings.currentGoto); VL("mA");
  }

  // get TMC SPI driver ready
  driver.init(settings.model, Pins->m0, Pins->m1, Pins->m2, Pins->m3, axisNumber);
  driver.mode(settings.intpol, settings.decay, microstepCode, settings.currentRun, settings.currentHold);

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
  if (settings.model == TMC5160) maxCurrent = 3000; else
  {
    DF("ERR: StepDirDriver::validateParameters(), Axis"); D(axisNumber); DLF(" unknown driver model!");
    return false;
  }

  long currentHold = round(param3);
  long currentRun = round(param4);
  long currentGoto = round(param5);
  UNUSED(param6);

  if (currentHold != OFF && (currentHold < 0 || currentHold > maxCurrent)) {
    DF("ERR: StepDirDriver::validateParameters(), Axis"); D(axisNumber); DF(" bad current hold="); DL(currentHold);
    return false;
  }

  if (currentRun != OFF && (currentRun < 0 || currentRun > maxCurrent)) {
    DF("ERR: StepDirDriver::validateParameters(), Axis"); D(axisNumber); DF(" bad current run="); DL(currentRun);
    return false;
  }

  if (currentGoto != OFF && (currentGoto < 0 || currentGoto > maxCurrent)) {
    DF("ERR: StepDirDriver::validateParameters(), Axis"); D(axisNumber); DF(" bad current goto="); DL(currentGoto);
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
  driver.mode(settings.intpol, settings.decay, microstepCode, settings.currentRun, settings.currentHold);
}

void StepDirTmcSPI::modeDecaySlewing() {
  int IGOTO = settings.currentGoto;
  if (IGOTO == OFF) IGOTO = settings.currentRun;
  driver.mode(settings.intpol, settings.decaySlewing, microstepCode, IGOTO, settings.currentHold);
}

void StepDirTmcSPI::updateStatus() {
  if (settings.status == ON) {
    if ((long)(millis() - timeLastStatusUpdate) > 200) {
      if (driver.refresh_DRVSTATUS()) {
        status.outputA.shortToGround = driver.get_DRVSTATUS_s2gA();
        status.outputA.openLoad      = driver.get_DRVSTATUS_olA();
        status.outputB.shortToGround = driver.get_DRVSTATUS_s2gB();
        status.outputB.openLoad      = driver.get_DRVSTATUS_olB();
        status.overTemperatureWarning = driver.get_DRVSTATUS_otpw();
        status.overTemperature       = driver.get_DRVSTATUS_ot();
        status.standstill            = driver.get_DRVSTATUS_stst();

        // open load indication is not reliable in standstill
        if (
          status.outputA.shortToGround ||
          status.outputB.shortToGround ||
          status.overTemperatureWarning ||
          status.overTemperature
        ) status.fault = true; else status.fault = false;
      } else {
        status.outputA.shortToGround = true;
        status.outputA.openLoad      = true;
        status.outputB.shortToGround = true;
        status.outputB.openLoad      = true;
        status.overTemperatureWarning = true;
        status.overTemperature       = true;
        status.standstill            = true;
        status.fault                 = true;
      }

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
    driver.mode(settings.intpol, settings.decay, microstepCode, settings.currentRun, settings.currentHold);
  } else {
    driver.mode(settings.intpol, STEALTHCHOP, microstepCode, settings.currentRun, 0);
  }

  return true;
}

// calibrate the motor driver if required
void StepDirTmcSPI::calibrateDriver() {
  if (settings.decay == STEALTHCHOP || settings.decaySlewing == STEALTHCHOP) {
    VF("MSG: StepDirDriver"); V(axisNumber); VL(", TMC standstill automatic current calibration");
    driver.mode(settings.intpol, STEALTHCHOP, microstepCode, settings.currentRun, settings.currentRun);
    delay(1000);
    driver.mode(settings.intpol, settings.decay, microstepCode, settings.currentRun, settings.currentHold);
  }
}

#endif
