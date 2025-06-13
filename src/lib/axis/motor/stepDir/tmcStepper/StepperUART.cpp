// -----------------------------------------------------------------------------------
// axis step/dir motor driver

// note: StepperUART requires UART RX and TX pins to be operational

#include "StepperUART.h"

#if defined(DRIVER_TMC_STEPPER) && defined(STEP_DIR_TMC_UART_PRESENT)

#include "../../../../gpioEx/GpioEx.h"

// help with pin names
#define rx m3
#define tx m2

// provide for using hardware serial
#if SERIAL_TMC == HardSerial
  #include <HardwareSerial.h>
  #undef SERIAL_TMC
  HardwareSerial HWSerialTMC(SERIAL_TMC_RX, SERIAL_TMC_TX);
  #define SERIAL_TMC HWSerialTMC
  #define SERIAL_TMC_RXTX_SET
#endif

// constructor
StepDirTmcUART::StepDirTmcUART(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings) {
  this->axisNumber = axisNumber;

  strcpy(axisPrefix, " Axis_StepDirTmcUART, ");
  axisPrefix[5] = '0' + axisNumber;

  this->Pins = Pins;
  settings = *Settings;
}

// setup driver
bool StepDirTmcUART::init() {

  // get TMC UART driver ready
  pinModeEx(Pins->m0, OUTPUT);
  pinModeEx(Pins->m1, OUTPUT);

  // initialize the serial port
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

    #define SerialTMC SERIAL_TMC
    static bool initialized = false;
    if (!initialized) {
      VF("MSG:"); V(axisPrefix);
      #if defined(SERIAL_TMC_RX) && defined(SERIAL_TMC_TX) && !defined(SERIAL_TMC_RXTX_SET)
        VF("HW UART driver pins rx="); V(SERIAL_TMC_RX); VF(", tx="); V(SERIAL_TMC_TX); VF(", baud="); V(SERIAL_TMC_BAUD); VLF(" bps");
        SerialTMC.begin(SERIAL_TMC_BAUD, SERIAL_8N1, SERIAL_TMC_RX, SERIAL_TMC_TX);
      #else
        VF("HW UART driver pins on port default"); VF(", baud="); V(SERIAL_TMC_BAUD); VLF(" bps");
        SerialTMC.begin(SERIAL_TMC_BAUD);
      #endif
      initialized = true;
    }
  #else
    // pull MS1 and MS2 low for device address 0
    digitalWriteEx(Pins->m0, LOW);
    digitalWriteEx(Pins->m1, LOW);
    VF("SW UART driver pins rx="); V(Pins->rx); VF(", tx="); V(Pins->tx); VF(", baud="); V(SERIAL_TMC_BAUD); VLF(" bps");
    SerialTMC = new SoftwareSerial(Pins->rx, Pins->tx);
    SerialTMC->begin(SERIAL_TMC_BAUD);
  #endif

  // set current defaults for TMC drivers
  if (settings.currentRun != OFF) {
    // automatically set goto and hold current if they are disabled
    if (settings.currentGoto == OFF) settings.currentGoto = settings.currentRun;
    if (settings.currentHold == OFF) settings.currentHold = lround(settings.currentRun/2.0F);
  } else {
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

  // initialize the stepper driver
  if (settings.model == TMC2208) {
    if (user_rSense > 0.0F) rSense = user_rSense; else rSense = TMC2208_RSENSE;
    VF("MSG:"); V(axisPrefix); VF("Rsense="); V(rSense); VL("ohms");
    #if defined(SERIAL_TMC_HARDWARE_UART)
      driver = new TMC2208Stepper(&SerialTMC, rSense);
    #else
      driver = new TMC2208Stepper(SerialTMC, rSense);
    #endif
    ((TMC2208Stepper*)driver)->begin();
    ((TMC2208Stepper*)driver)->intpol(settings.intpol);
    modeMicrostepTracking();
    driver->rms_current(settings.currentRun*0.7071F, settings.currentHold/settings.currentRun);
    ((TMC2208Stepper*)driver)->en_spreadCycle(true);
  } else
  if (settings.model == TMC2209) { // also handles TMC2226
    if (user_rSense > 0.0F) rSense = user_rSense; else rSense = TMC2209_RSENSE;
    VF("MSG:"); V(axisPrefix); VF("Rsense="); V(rSense); VL("ohms");
    #if defined(SERIAL_TMC_HARDWARE_UART)
      driver = new TMC2209Stepper(&SerialTMC, rSense, SERIAL_TMC_ADDRESS_MAP(axisNumber - 1));
    #else
      driver = new TMC2209Stepper(SerialTMC, rSense, SERIAL_TMC_ADDRESS_MAP(axisNumber - 1));
    #endif
    ((TMC2209Stepper*)driver)->begin();
    ((TMC2209Stepper*)driver)->intpol(settings.intpol);
    modeMicrostepTracking();
    driver->rms_current(settings.currentRun*0.7071F, settings.currentHold/settings.currentRun);
    ((TMC2209Stepper*)driver)->en_spreadCycle(true);
  } else {
     DF("ERR:"); D(axisPrefix); DLF("unknown driver model!");
    return false;
  }

  // automatically set fault status for known drivers
  status.active = settings.status != OFF;

  // check to see if the driver is there and ok
  #ifdef MOTOR_DRIVER_DETECT
    readStatus();
    if (!status.standstill || status.overTemperature) {
      DF("ERR:"); D(axisPrefix); DLF("no motor driver device detected!");
      return false;
    } else { VF("MSG:"); V(axisPrefix); VLF("motor driver device detected"); }
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
bool StepDirTmcUART::validateParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  if (!StepDirDriver::validateParameters(param1, param2, param3, param4, param5, param6)) return false;

  if (settings.model == TMC2208) currentMax = TMC2208_MAX_CURRENT; else
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
  if (settings.microsteps == 1) driver->microsteps(0); else driver->microsteps(settings.microsteps);
}

int StepDirTmcUART::modeMicrostepSlewing() {
  if (microstepRatio > 1) {
    if (settings.microstepsSlewing == 1) driver->microsteps(0); else driver->microsteps(settings.microstepsSlewing);
  }
  return microstepRatio;
}

void StepDirTmcUART::modeDecayTracking() {
  setDecayMode(settings.decay);
  driver->rms_current(settings.currentRun*0.7071F, (float)settings.currentHold/(float)settings.currentRun);
}

void StepDirTmcUART::modeDecaySlewing() {
  setDecayMode(settings.decaySlewing);
  int IGOTO = settings.currentGoto;
  if (IGOTO == OFF) IGOTO = settings.currentRun;
  driver->rms_current(IGOTO*0.7071F, 1.0F);
}

// set the decay mode STEALTHCHOP or SPREADCYCLE
void StepDirTmcUART::setDecayMode(int decayMode) {
  if (settings.model == TMC2208) {
    ((TMC2208Stepper*)driver)->en_spreadCycle(decayMode == SPREADCYCLE);
  } else
  if (settings.model == TMC2209) {
    ((TMC2209Stepper*)driver)->en_spreadCycle(decayMode == SPREADCYCLE);
  }
}

void StepDirTmcUART::readStatus() {
  TMC2208_n::DRV_STATUS_t status_result;
  status_result.sr = 0;

  bool crcError = false;
  switch (settings.model) {
    case TMC2208: status_result.sr = ((TMC2208Stepper*)driver)->DRV_STATUS(); crcError = ((TMC2208Stepper*)driver)->CRCerror; break;
    case TMC2209: status_result.sr = ((TMC2209Stepper*)driver)->DRV_STATUS(); crcError = ((TMC2209Stepper*)driver)->CRCerror; break;
  }
  if (crcError) status_result.sr = 0xFFFFFFFF;

  status.outputA.shortToGround  = status_result.s2ga;
  status.outputA.openLoad       = status_result.ola;
  status.outputB.shortToGround  = status_result.s2gb;
  status.outputB.openLoad       = status_result.olb;
  status.overTemperatureWarning = status_result.otpw;
  status.overTemperature        = status_result.ot;
  status.standstill             = status_result.stst;
}

// secondary way to power down not using the enable pin
bool StepDirTmcUART::enable(bool state) {
  if (state) {
    modeDecayTracking();
  } else {
    setDecayMode(STEALTHCHOP);
    driver->ihold(0);
  }
  return true;
}

// calibrate the motor driver if required
void StepDirTmcUART::calibrateDriver() {
  if (settings.decay == STEALTHCHOP || settings.decaySlewing == STEALTHCHOP) {
    VF("MSG:"); V(axisPrefix); VL("standstill automatic current calibration");
    driver->rms_current(settings.currentRun*0.7071F, 1.0F);

    if (settings.model == TMC2208) {
      ((TMC2208Stepper*)driver)->pwm_autograd(DRIVER_TMC_STEPPER_AUTOGRAD);
      ((TMC2208Stepper*)driver)->pwm_autoscale(true);
      ((TMC2208Stepper*)driver)->en_spreadCycle(false);
    } else
    if (settings.model == TMC2209) { // also handles TMC2226
      ((TMC2209Stepper*)driver)->pwm_autograd(DRIVER_TMC_STEPPER_AUTOGRAD);
      ((TMC2209Stepper*)driver)->pwm_autoscale(true);
      ((TMC2209Stepper*)driver)->en_spreadCycle(false);
    }
    delay(1000);
    modeDecayTracking();
  }
}

#endif
