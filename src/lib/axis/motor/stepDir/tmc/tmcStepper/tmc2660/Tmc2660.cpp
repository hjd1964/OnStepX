// -----------------------------------------------------------------------------------
// axis step/dir motor driver, TMC2660Stepper

#include "Tmc2660.h"
 
#if defined(DRIVER_TMC_STEPPER) && defined(TMC2260_PRESENT)

#include "../../../../../../gpioEx/GpioEx.h"

// help with pin names
#define mosi m0
#define sck  m1
#define cs   m2
#define miso m3

// constructor
StepDirTmc2260::
StepDirTmc2260(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings,
               int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t  intpol)
               :TmcStepDirDriver(axisNumber, Pins, Settings, currentHold, currentRun, currentSlewing, intpol) {
  strcpy(axisPrefix, " Axis_Tmc2660StepDir, ");
  axisPrefix[5] = '0' + axisNumber;
}

// setup driver
bool StepDirTmc2260::init() {
  if (!TmcStepDirDriver::init()) return false;

  #ifdef DRIVER_TMC_STEPPER_HW_SPI
    driver = new TMC2660Stepper(Pins->cs, rSense);
  #else
    driver = new TMC2660Stepper(Pins->cs, rSense, Pins->mosi, Pins->miso, Pins->sck);
  #endif
  driver->begin();
  driver->toff(5);
  driver->intpol(intpol.value == ON);
  modeMicrostepTracking();
  current(iRun);

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

  return true;
}

void StepDirTmc2260::modeMicrostepTracking() {
  int16_t microsteps = 0;
  driver->microsteps(normalizedMicrosteps);
}

int StepDirTmc2260::modeMicrostepSlewing() {
  if (microstepRatio > 1) {
    driver->microsteps(normalizedMicrostepsSlewing);
  }
  return microstepRatio;
}

void StepDirTmc2260::modeDecayTracking() {
  current(iRun);
}

void StepDirTmc2260::modeDecaySlewing() {
  current(iGoto);
}

void StepDirTmc2260::readStatus() {
  TMC2130_n::DRV_STATUS_t status_result;
  status_result.sr = driver->DRV_STATUS();

  status.outputA.shortToGround = status_result.s2ga;
  status.outputA.openLoad      = status_result.ola;
  status.outputB.shortToGround = status_result.s2gb;
  status.outputB.openLoad      = status_result.olb;
  status.overTemperatureWarning= status_result.otpw;
  status.overTemperature       = status_result.ot;
  status.standstill            = status_result.stst;
}

// secondary way to power down not using the enable pin
bool StepDirTmc2260::enable(bool state) {
  if (state) {
    modeDecayTracking();
  } else {
    driver->rms_current(0);
  }
  return true;
}

#endif
