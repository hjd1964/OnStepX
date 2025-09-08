// -----------------------------------------------------------------------------------
// axis step/dir motor driver, TMC2160Stepper

#include "Tmc2160.h"
 
#if defined(DRIVER_TMC_STEPPER) && defined(TMC2160_PRESENT)

#include "../../../../../../gpioEx/GpioEx.h"

// help with pin names
#define mosi m0
#define sck  m1
#define cs   m2
#define miso m3

// constructor
StepDirTmc2160::
StepDirTmc2160(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings,
               int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t  intpol)
               :TmcStepDirDriver(axisNumber, Pins, Settings, currentHold, currentRun, currentSlewing, intpol) {
  strcpy(axisPrefix, " Axis_Tmc2160StepDir, ");
  axisPrefix[5] = '0' + axisNumber;
}

// setup driver
bool Tmc2160StepDirDriver::init() {
  if (!TmcStepDirDriver::init()) return false;

  #ifdef TMC2160_RSENSE_KRAKEN
    if (axisNumber <= 4) rSense = TMC2160_RSENSE_KRAKEN;
  #endif
  #ifdef DRIVER_TMC_STEPPER_HW_SPI
    driver = new TMC2160Stepper(Pins->cs, rSense);
  #else
    driver = new TMC2160Stepper(Pins->cs, rSense, Pins->mosi, Pins->miso, Pins->sck);
  #endif
  driver->begin();
  driver->intpol(intpol.value == ON);
  modeMicrostepTracking();
  driver->en_pwm_mode(false);

  current(iRun, iHoldRatio);

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

void Tmc2160StepDirDriver::modeMicrostepTracking() {
  int16_t microsteps = 0;
  driver->microsteps(normalizedMicrosteps);
}

int Tmc2160StepDirDriver::modeMicrostepSlewing() {
  if (microstepRatio > 1) {
    driver->microsteps(normalizedMicrostepsSlewing);
  }
  return microstepRatio;
}

void Tmc2160StepDirDriver::modeDecayTracking() {
  setDecayMode(decay.value);
  current(iRun, iHoldRatio);
}

void Tmc2160StepDirDriver::modeDecaySlewing() {
  setDecayMode(decaySlewing.value);
  current(iGoto, 1.0F);
}

void Tmc2160StepDirDriver::readStatus() {
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
bool Tmc2160StepDirDriver::enable(bool state) {
  if (state) {
    modeDecayTracking();
  } else {
    setDecayMode(STEALTHCHOP);
    driver->ihold(0);
  }
  return true;
}

// calibrate the motor driver if required
void Tmc2160StepDirDriver::calibrateDriver() {
  if (decay.value == STEALTHCHOP || decaySlewing.value == STEALTHCHOP) {
    VF("MSG:"); V(axisPrefix); VL("standstill automatic current calibration");
    current(iRun, 1.0F);
    driver->pwm_autoscale(true);
    driver->en_pwm_mode(true);
    delay(1000);
    modeDecayTracking();
  }
}

// set the decay mode STEALTHCHOP or SPREADCYCLE
void Tmc2160StepDirDriver::setDecayMode(int decayMode) {
  driver->en_pwm_mode(decayMode != SPREADCYCLE);
}

#endif
