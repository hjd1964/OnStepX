// -----------------------------------------------------------------------------------
// axis step/dir motor driver, TMC2209Stepper

#include "Tmc2209.h"

#if defined(DRIVER_TMC_STEPPER) && defined(TMC2209_PRESENT)

#include "../../../../../../gpioEx/GpioEx.h"

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
StepDirTmc2209::
StepDirTmc2209(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings,
               int16_t currentHold, int16_t currentRun, int16_t currentSlewing, int8_t intpol)
               :TmcStepDirDriver(axisNumber, Pins, Settings, currentHold, currentRun, currentSlewing, intpol) {
  strcpy(axisPrefix, " Axis_Tmc2209StepDir, ");
  axisPrefix[5] = '0' + axisNumber;
}

// setup driver
bool StepDirTmc2209::init() {
  if (!TmcStepDirDriver::init()) return false;

  // get TMC UART driver address select pins ready
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
      // pull MS1 and MS2 high for device address 3
      // the user can then hard code the device address 0,1,2,3 by cutting off pins as needed
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

  // initialize the stepper driver
  #if defined(SERIAL_TMC_HARDWARE_UART)
    driver = new TMC2209Stepper(&SerialTMC, rSense, SERIAL_TMC_ADDRESS_MAP(axisNumber - 1));
  #else
    driver = new TMC2209Stepper(SerialTMC, rSense, SERIAL_TMC_ADDRESS_MAP(axisNumber - 1));
  #endif
  driver->begin();
  driver->intpol(intpol.value == ON);
  modeMicrostepTracking();
  current(iRun, iHoldRatio);
  driver->en_spreadCycle(true);

  // check to see if the driver is there and ok
  #ifdef MOTOR_DRIVER_DETECT
    readStatus();
    if (!status.standstill || status.overTemperature) {
      DF("ERR:"); D(axisPrefix); DLF("no motor driver device detected!");
      return false;
    } else { VF("MSG:"); V(axisPrefix); VLF("motor driver device detected"); }
  #endif

  return true;
}

void StepDirTmc2209::modeMicrostepTracking() {
  if (normalizedMicrosteps == 1) driver->microsteps(0); else driver->microsteps(normalizedMicrosteps);
}

int StepDirTmc2209::modeMicrostepSlewing() {
  if (microstepRatio > 1) {
    if (normalizedMicrostepsSlewing == 1) driver->microsteps(0); else driver->microsteps(normalizedMicrostepsSlewing);
  }
  return microstepRatio;
}

void StepDirTmc2209::modeDecayTracking() {
  setDecayMode(decay.value);
  current(iRun, iHoldRatio);
}

void StepDirTmc2209::modeDecaySlewing() {
  setDecayMode(decaySlewing.value);
  current(iGoto, 1.0F);
}

// set the decay mode STEALTHCHOP or SPREADCYCLE
void StepDirTmc2209::setDecayMode(int decayMode) {
  driver->en_spreadCycle(decayMode == SPREADCYCLE);
}

void StepDirTmc2209::readStatus() {
  TMC2208_n::DRV_STATUS_t status_result;
  status_result.sr = driver->DRV_STATUS();
  if (driver->CRCerror) status_result.sr = 0xFFFFFFFF;

  status.outputA.shortToGround  = status_result.s2ga;
  status.outputA.openLoad       = status_result.ola;
  status.outputB.shortToGround  = status_result.s2gb;
  status.outputB.openLoad       = status_result.olb;
  status.overTemperatureWarning = status_result.otpw;
  status.overTemperature        = status_result.ot;
  status.standstill             = status_result.stst;
}

// secondary way to power down not using the enable pin
bool StepDirTmc2209::enable(bool state) {
  if (state) {
    modeDecayTracking();
  } else {
    setDecayMode(STEALTHCHOP);
    driver->ihold(0);
  }
  return true;
}

// calibrate the motor driver if required
void StepDirTmc2209::calibrateDriver() {
  if (decay.value == STEALTHCHOP || decaySlewing.value == STEALTHCHOP) {
    VF("MSG:"); V(axisPrefix); VL("standstill automatic current calibration");

    current(iRun, 1.0F);
    driver->pwm_autograd(DRIVER_TMC_STEPPER_AUTOGRAD);
    driver->pwm_autoscale(true);
    driver->en_spreadCycle(false);
    delay(1000);
    modeDecayTracking();
  }
}

#endif
