// -----------------------------------------------------------------------------------
// axis step/dir motor driver, TMC2209Stepper

// note: LegacyUART requires UART RX but TX is optional

#include "Tmc2209.h"

#if !defined(DRIVER_TMC_STEPPER) && defined(TMC2209_PRESENT)

#include "../../../../../../gpioEx/GpioEx.h"

// help with pin names
#define rx m3
#define tx m2

#define CurrentToPercent(current) ((current/1000.0)/(0.325/(rSense + 0.02)))*100.0

// constructor
StepDirTmc2209::StepDirTmc2209(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings,
                               int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t intpol)
                               :TmcStepDirDriver(axisNumber, Pins, Settings, currentHold, currentRun, currentSlewing, intpol) {
  strcpy(axisPrefix, " Axis_Tmc2209StepDir legacy, ");
  axisPrefix[5] = '0' + axisNumber;
}

// setup driver
bool StepDirTmc2209::init() {
  if (!TmcStepDirDriver::init()) return false;

  // get TMC UART driver ready
  pinModeEx(Pins->m0, OUTPUT);
  pinModeEx(Pins->m1, OUTPUT);

  driver = new TMC2209Stepper();
  if (driver == NULL) return false; 

  int16_t rxPin = Pins->rx;

  delay(1);
  #if defined(SERIAL_TMC_HARDWARE_UART)
    #if defined(DEDICATED_MODE_PINS)
      // program the device address 0,1,2,3 since M0 and M1 are all unique
      int deviceAddress = SERIAL_TMC_ADDRESS_MAP(axisNumber - 1);
      digitalWriteEx(Pins->m0, bitRead(deviceAddress, 0));
      digitalWriteEx(Pins->m1, bitRead(deviceAddress, 1));
    #else
      // help user hard code the device address 0,1,2,3 by cutting pins
      digitalWriteEx(Pins->m0, HIGH);
      digitalWriteEx(Pins->m1, HIGH);
    #endif

    VF("MSG:"); V(axisPrefix);
    VF("HW UART driver pins rx="); V(SERIAL_TMC_RX); VF(", tx="); V(SERIAL_TMC_TX); VF(", baud="); V(SERIAL_TMC_BAUD); VLF(" bps");
    #if SERIAL_TMC_INVERT == ON
      driver->setup(SERIAL_TMC, SERIAL_TMC_BAUD, SERIAL_TMC_ADDRESS_MAP(axisNumber - 1), SERIAL_TMC_RX, SERIAL_TMC_TX, true);
    #else
      driver->setup(SERIAL_TMC, SERIAL_TMC_BAUD, SERIAL_TMC_ADDRESS_MAP(axisNumber - 1), SERIAL_TMC_RX, SERIAL_TMC_TX);
    #endif
    rxPin = SERIAL_TMC_RX;
  #else
    // pull MS1 and MS2 low for device address 0
    digitalWriteEx(Pins->m0, LOW);
    digitalWriteEx(Pins->m1, LOW);

    #if SERIAL_TMC_RX_DISABLE == true
      rxPin = OFF;
    #endif
    VF("MSG:"); V(axisPrefix);
    VF("SW UART driver pins rx="); V(rxPin); VF(", tx="); V(Pins->tx); VF(", baud="); V(SERIAL_TMC_BAUD); VLF(" bps");
    driver->setup(SERIAL_TMC_BAUD, SERIAL_TMC_ADDRESS_MAP(axisNumber - 1), rxPin, Pins->tx);
  #endif

  // this driver automatically switches to one-way communications, even if a RX pin is set
  // so the following only returns false if communications are "half working"
  #ifdef MOTOR_DRIVER_DETECT
    if (rxPin != OFF) {
      if (!driver->isSetupAndCommunicating()) {
        DF("ERR:"); D(axisPrefix); DLF("no motor driver device detected!");
        return false;
      } else { VF("MSG:"); V(axisPrefix); VLF("motor driver device detected"); }
    }
  #endif

  driver->useExternalSenseResistors();
  driver->enableAnalogCurrentScaling();

  driver->enable();
  driver->moveUsingStepDirInterface();
  driver->setPwmOffset(pc_pwm_ofs);
  driver->setPwmGradient(pc_pwm_grad);
  if (pc_pwm_auto) driver->enableAutomaticCurrentScaling();
  if (!intpol.value) {
    VF("MSG:"); V(axisPrefix); VLF("driver interpolation control not supported");
  }

  modeMicrostepTracking();
  driver->setRunCurrent(CurrentToPercent(iRun));
  driver->setHoldCurrent(CurrentToPercent(iHold));
  driver->disableStealthChop();

  return true;
}

void StepDirTmc2209::modeMicrostepTracking() {
  driver->setMicrostepsPerStep(normalizedMicrosteps);
}

int StepDirTmc2209::modeMicrostepSlewing() {
  if (microstepRatio > 1) {
    driver->setMicrostepsPerStep(normalizedMicrostepsSlewing);
  }
  return microstepRatio;
}

void StepDirTmc2209::modeDecayTracking() {
  if (normalizedDecay == SPREADCYCLE) driver->disableStealthChop(); else driver->enableStealthChop();
  driver->setRunCurrent(CurrentToPercent(iRun));
  driver->setHoldCurrent(CurrentToPercent(iHold));
}  

void StepDirTmc2209::modeDecaySlewing() {
  if (decaySlewing.value == SPREADCYCLE) driver->disableStealthChop(); else driver->enableStealthChop();
  driver->setRunCurrent(CurrentToPercent(iGoto));
  driver->setHoldCurrent(CurrentToPercent(iHold));
}

void StepDirTmc2209::readStatus() {
  TMC2209Stepper::Status status_result = driver->getStatus();
  status.outputA.shortToGround  = (bool)status_result.short_to_ground_a || (bool)status_result.low_side_short_a;
  status.outputA.openLoad       = (bool)status_result.open_load_a;
  status.outputB.shortToGround  = (bool)status_result.short_to_ground_b || (bool)status_result.low_side_short_b;
  status.outputB.openLoad       = (bool)status_result.open_load_b;
  status.overTemperatureWarning = (bool)status_result.over_temperature_warning;
  status.overTemperature        = (bool)status_result.over_temperature_shutdown;
  status.standstill             = (bool)status_result.standstill;
}

// secondary way to power down not using the enable pin
bool StepDirTmc2209::enable(bool state) {
  if (state) {
    modeDecayTracking();
  } else {
    driver->enableStealthChop();
    driver->setHoldCurrent(0);
  }

  return true;
}

// calibrate the motor driver if required
void StepDirTmc2209::calibrateDriver() {
  if (decay.value == STEALTHCHOP || decaySlewing.value == STEALTHCHOP) {
    VF("MSG: StepDirDriver"); V(axisNumber); VL(", TMC standstill automatic current calibration");
    driver->setRunCurrent(CurrentToPercent(iRun));
    driver->setHoldCurrent(CurrentToPercent(iRun));
    driver->enableStealthChop();
    delay(1000);
    driver->setRunCurrent(CurrentToPercent(iRun));
    driver->setHoldCurrent(CurrentToPercent(iHold));
    driver->disableStealthChop();
  }
}

#endif
