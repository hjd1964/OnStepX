// -----------------------------------------------------------------------------------
// axis step/dir motor driver

// note: StepperUART requires UART RX and TX pins to be operational

#include "StepperUART.h"

#if defined(DRIVER_TMC_STEPPER) && defined(STEP_DIR_TMC_UART_PRESENT)

// help with pin names
#define rx m2
#define tx m3

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
  this->Pins = Pins;
  settings = *Settings;
}

// set up driver and parameters: microsteps, microsteps goto, hold current, run current, goto current, unused
void StepDirTmcUART::init(float param1, float param2, float param3, float param4, float param5, float param6) {
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

  if (settings.decay == OFF) settings.decay = STEALTHCHOP;
  if (settings.decaySlewing == OFF) settings.decaySlewing = SPREADCYCLE;

  VF("MSG: StepDirDriver"); V(axisNumber); VF(", TMC ");
  if (settings.currentRun == OFF) {
    VLF("current control OFF (set by Vref)");
  } else {
    VF("Ihold="); V(settings.currentHold); VF("mA, ");
    VF("Irun="); V(settings.currentRun); VF("mA, ");
    VF("Igoto="); V(settings.currentGoto); VL("mA");
  }

  // get TMC UART driver ready
  pinModeEx(Pins->m0, OUTPUT);
  pinModeEx(Pins->m1, OUTPUT);

  // initialize the serial port
  VF("MSG: StepDirDriver"); V(axisNumber); VF(", TMC ");
  #if defined(SERIAL_TMC_HARDWARE_UART)
    // help user hard code the device addresses 0,1,2,3
    digitalWriteEx(Pins->m0, HIGH);
    digitalWriteEx(Pins->m1, HIGH);
    #define SerialTMC SERIAL_TMC
    static bool initialized = false;
    if (!initialized) {
      #if defined(SERIAL_TMC_RX) && defined(SERIAL_TMC_TX) && !defined(SERIAL_TMC_RXTX_SET)
        VF("HW UART driver pins rx="); V(SERIAL_TMC_RX); VF(", tx="); V(SERIAL_TMC_TX); VF(", baud="); V(SERIAL_TMC_BAUD); VLF("bps");
        SerialTMC.begin(SERIAL_TMC_BAUD, SERIAL_8N1, SERIAL_TMC_RX, SERIAL_TMC_TX);
      #else
        VF("HW UART driver pins on port default"); VF(", baud="); V(SERIAL_TMC_BAUD); VLF("bps");
        SerialTMC.begin(SERIAL_TMC_BAUD);
      #endif
      initialized = true;
    }
  #else
    // pull MS1 and MS2 low for device address 0
    digitalWriteEx(Pins->m0, LOW);
    digitalWriteEx(Pins->m1, LOW);
    VF("SW UART driver pins rx="); V(Pins->rx); VF(", tx="); V(Pins->tx); VF(", baud="); V(SERIAL_TMC_BAUD); VLF("bps");
    SerialTMC = new SoftwareSerial(Pins->rx, Pins->tx);
    SerialTMC.begin(SERIAL_TMC_BAUD);
  #endif

  // initialize the stepper driver
  if (settings.model == TMC2208) {
    driver = new TMC2208Stepper(&SerialTMC, 0.11F);
    ((TMC2208Stepper*)driver)->begin();
    ((TMC2208Stepper*)driver)->pwm_autoscale(true);
    ((TMC2208Stepper*)driver)->intpol(true);
  } else
  if (settings.model == TMC2209) { // also handles TMC2226
    driver = new TMC2209Stepper(&SerialTMC, 0.11F, SERIAL_TMC_ADDRESS_MAP(axisNumber - 1));
    ((TMC2209Stepper*)driver)->begin();
    ((TMC2209Stepper*)driver)->pwm_autoscale(true);
    ((TMC2209Stepper*)driver)->intpol(true);
  }

  // calibrate stealthChop
  modeMicrostepTracking();
  if (settings.decay == STEALTHCHOP || settings.decaySlewing == STEALTHCHOP) {
    driver->rms_current(settings.currentRun*0.707F);
    driver->hold_multiplier(1.0F);
    setDecayMode(STEALTHCHOP);
    VF("MSG: StepDirDriver"); V(axisNumber); VL(", TMC standstill automatic current calibration");
    delay(100);
  }
  driver->rms_current(settings.currentRun*0.707F);
  driver->hold_multiplier(settings.currentHold/settings.currentRun);
  setDecayMode(settings.decay);

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
bool StepDirTmcUART::validateParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  StepDirDriver::validateParameters(param1, param2, param3, param4, param5, param6);

  int maxCurrent;
  if (settings.model == TMC2208) maxCurrent = 1700; else
  if (settings.model == TMC2209) maxCurrent = 2000; else
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

void StepDirTmcUART::modeMicrostepTracking() {
  driver->microsteps(settings.microsteps);
}

int StepDirTmcUART::modeMicrostepSlewing() {
  if (microstepRatio > 1) {
    driver->microsteps(settings.microstepsSlewing);
  }
  return microstepRatio;
}

void StepDirTmcUART::modeDecayTracking() {
  setDecayMode(settings.decay);
  driver->rms_current(settings.currentRun*0.707F);
}

void StepDirTmcUART::modeDecaySlewing() {
  setDecayMode(settings.decaySlewing);
  int IGOTO = settings.currentGoto;
  if (IGOTO == OFF) IGOTO = settings.currentRun;
  driver->rms_current(IGOTO*0.707F);
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

void StepDirTmcUART::updateStatus() {
  if (settings.status == ON) {
    if ((long)(millis() - timeLastStatusUpdate) > 200) {

      uint32_t status_word;
      TMC2208_n::DRV_STATUS_t status_result;
      if (settings.model == TMC2208) {
        status_result.sr = ((TMC2208Stepper*)driver)->DRV_STATUS();
      } else
      if (settings.model == TMC2209) {
        status_result.sr = ((TMC2209Stepper*)driver)->DRV_STATUS();
      }
      status.outputA.shortToGround = status_result.s2ga;
      status.outputA.openLoad      = status_result.ola;
      status.outputB.shortToGround = status_result.s2gb;
      status.outputB.openLoad      = status_result.olb;
      status.overTemperatureWarning = status_result.otpw;
      status.overTemperature       = status_result.ot;
      status.standstill            = status_result.stst;

      // open load indication is not reliable in standstill
      if (status.outputA.shortToGround ||
          status.outputB.shortToGround ||
          status.overTemperatureWarning ||
          status.overTemperature) status.fault = true; else status.fault = false;

      timeLastStatusUpdate = millis();
    }
  } else
  if (settings.status == LOW || settings.status == HIGH) {
    status.fault = digitalReadEx(Pins->fault) == settings.status;
  }

  StepDirDriver::updateStatus();
}

// secondary way to power down not using the enable pin
void StepDirTmcUART::enable(bool state) {
  VF("MSG: StepDirDriver"); V(axisNumber);
  VF(", powered "); if (state) { VF("up"); } else { VF("down"); } VLF(" using SPI or UART");
  int I_run = 0, I_hold = 0;
  if (state) { I_run = settings.currentRun; I_hold = settings.currentHold; }
  driver->rms_current(I_run*0.707F);
}

#endif
