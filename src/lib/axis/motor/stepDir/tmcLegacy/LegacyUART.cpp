// -----------------------------------------------------------------------------------
// axis step/dir motor driver

// note: LegacyUART requires UART RX but TX is optional

#include "LegacyUART.h"

#if !defined(DRIVER_TMC_STEPPER) && defined(STEP_DIR_TMC_UART_PRESENT)

#include "../../../../gpioEx/GpioEx.h"

// help with pin names
#define rx m3
#define tx m2

#define CurrentToPercent(current) ((current/1000.0)/(0.325/(rSense + 0.02)))*100.0

// constructor
StepDirTmcUART::StepDirTmcUART(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings) {
  this->axisNumber = axisNumber;

  strcpy(axisPrefix, " Axis_StepDirTmcUART legacy, ");
  axisPrefix[5] = '0' + axisNumber;

  this->Pins = Pins;
  settings = *Settings;
}

// setup driver
bool StepDirTmcUART::init() {

  if (settings.currentRun != OFF) {
    // automatically set goto and hold current if they are disabled
    if (settings.currentGoto == OFF) settings.currentGoto = settings.currentRun;
    if (settings.currentHold == OFF) settings.currentHold = lround(settings.currentRun/2.0F);
  } else {
    // set current defaults for TMC drivers
    settings.currentRun = 300;
    settings.currentGoto = settings.currentRun;
    settings.currentHold = lround(settings.currentRun/2.0F);
  }

  if (settings.currentRun == OFF) {
    VF("MSG:"); V(axisPrefix); VLF("current control OFF (300mA)");
  } else {
    VF("MSG:"); V(axisPrefix);
    VF("Ihold="); V(settings.currentHold); VF("mA, ");
    VF("Irun="); V(settings.currentRun); VF("mA, ");
    VF("Igoto="); V(settings.currentGoto); VL("mA");
  }

  if (user_rSense > 0.0F) rSense = user_rSense; else rSense = TMC2209_RSENSE;
  VF("MSG:"); V(axisPrefix); VF("Rsense="); V(rSense); VL("ohms");

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
  if (!settings.intpol) {
    VF("MSG:"); V(axisPrefix); VLF("driver interpolation control not supported");
  }

  modeMicrostepTracking();
  driver->setRunCurrent(CurrentToPercent(settings.currentRun));
  driver->setHoldCurrent(CurrentToPercent(settings.currentHold));
  driver->disableStealthChop();

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
bool StepDirTmcUART::validateParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  if (!StepDirDriver::validateParameters(param1, param2, param3, param4, param5, param6)) return false;

  if (settings.model == TMC2209) currentMax = TMC2209_MAX_CURRENT; else // both TMC2209 and TMC2226
  {
    DF("WRN:"); D(axisPrefix); DLF("unknown driver model!");
    return false;
  }

  // override max current with user setting
  if (user_currentMax != 0) currentMax = user_currentMax;

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

void StepDirTmcUART::modeMicrostepTracking() {
  driver->setMicrostepsPerStep(settings.microsteps);
}

int StepDirTmcUART::modeMicrostepSlewing() {
  if (microstepRatio > 1) {
    driver->setMicrostepsPerStep(settings.microstepsSlewing);
  }
  return microstepRatio;
}

void StepDirTmcUART::modeDecayTracking() {
  if (settings.decay == SPREADCYCLE) driver->disableStealthChop(); else driver->enableStealthChop();
  driver->setRunCurrent(CurrentToPercent(settings.currentRun));
  driver->setHoldCurrent(CurrentToPercent(settings.currentHold));
}  

void StepDirTmcUART::modeDecaySlewing() {
  int IGOTO = settings.currentGoto;
  if (IGOTO == OFF) IGOTO = settings.currentRun;
  if (settings.decaySlewing == SPREADCYCLE) driver->disableStealthChop(); else driver->enableStealthChop();
  driver->setRunCurrent(CurrentToPercent(IGOTO));
  driver->setHoldCurrent(CurrentToPercent(settings.currentHold));
}

void StepDirTmcUART::readStatus() {
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
bool StepDirTmcUART::enable(bool state) {
  if (state) {
    modeDecayTracking();
  } else {
    driver->enableStealthChop();
    driver->setHoldCurrent(0);
  }

  return true;
}

// calibrate the motor driver if required
void StepDirTmcUART::calibrateDriver() {
  if (settings.decay == STEALTHCHOP || settings.decaySlewing == STEALTHCHOP) {
    VF("MSG: StepDirDriver"); V(axisNumber); VL(", TMC standstill automatic current calibration");
    driver->setRunCurrent(CurrentToPercent(settings.currentRun));
    driver->setHoldCurrent(CurrentToPercent(settings.currentRun));
    driver->enableStealthChop();
    delay(1000);
    driver->setRunCurrent(CurrentToPercent(settings.currentRun));
    driver->setHoldCurrent(CurrentToPercent(settings.currentHold));
    driver->disableStealthChop();
  }
}

#endif
