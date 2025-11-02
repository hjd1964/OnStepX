// -----------------------------------------------------------------------------------
// axis step/dir motor driver, TMC5160Stepper

// note: LegacySPI requires MOSI, SCK, CS but MISO is optional

#include "Tmc5160.h"

#if !defined(DRIVER_TMC_STEPPER) && defined(TMC5160_PRESENT)

#include "../../../../../../gpioEx/GpioEx.h"

// help with pin names
#define mosi m0
#define sck  m1
#define cs   m2
#define miso m3

// constructor
StepDirTmc5160::StepDirTmc5160(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings,
                               int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t intpol)
                               :TmcStepDirDriver(axisNumber, Pins, Settings, currentHold, currentRun, currentSlewing, intpol) {
  strcpy(axisPrefix, " Axis_Tmc5160StepDir legacy, ");
  axisPrefix[5] = '0' + axisNumber;
}

// set up driver and parameters: microsteps, microsteps goto, hold current, run current, goto current, unused
bool StepDirTmc5160::init() {
  if (!TmcStepDirDriver::init()) return false;

  // get TMC SPI ready
  driver.init(driverModel, Pins->m0, Pins->m1, Pins->m2, Pins->m3, axisNumber);

  // get driver ready and check to see if it's there (if possible)
  if (!driver.mode(intpol.value == ON, decay.value, microstepCode, iRun, iHold, rSense)) {
    DF("ERR:"); D(axisPrefix); DLF("no motor driver device detected!");
    return false;
  } else {
    if (Pins->m3 != OFF) {
      VF("MSG:"); V(axisPrefix); VLF("motor driver device detected");
    }
  }

  return true;
}

void StepDirTmc5160::modeMicrostepTracking() {
  driver.refresh_CHOPCONF(microstepCode);
}

int StepDirTmc5160::modeMicrostepSlewing() {
  if (microstepRatio > 1) {
    driver.refresh_CHOPCONF(microstepCodeSlewing);
  }
  return microstepRatio;
}

void StepDirTmc5160::modeDecayTracking() {
  driver.mode(intpol.value == ON, decay.value, microstepCode, iRun, iHold, rSense);
}

void StepDirTmc5160::modeDecaySlewing() {
  driver.mode(intpol.value == ON, decaySlewing.value, microstepCode, iGoto, iHold, rSense);
}

void StepDirTmc5160::readStatus() {
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
bool StepDirTmc5160::enable(bool state) {
  if (state) {
    driver.mode(intpol.value == ON, decay.value, microstepCode, iRun, iHold, rSense);
  } else {
    driver.mode(intpol.value == ON, STEALTHCHOP, microstepCode, iRun, 0, rSense);
  }
  return true;
}

// calibrate the motor driver if required
void StepDirTmc5160::calibrateDriver() {
  if (decay.value == STEALTHCHOP || decaySlewing.value == STEALTHCHOP) {
    VF("MSG:"); V(axisPrefix); VL("TMC standstill automatic current calibration");
    driver.mode(intpol.value == ON, STEALTHCHOP, microstepCode, iRun, iRun, rSense);
    delay(1000);
    driver.mode(intpol.value == ON, decay.value, microstepCode, iRun, iHold, rSense);
  }
}

#endif
