// -----------------------------------------------------------------------------------
// axis servo TMC2209 stepper motor driver

#include "Tmc2209.h"

#ifdef SERVO_TMC2209_PRESENT

#include "../../../../tasks/OnTask.h"
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

ServoTmc2209::ServoTmc2209(uint8_t axisNumber, const ServoTmcPins *Pins, const ServoTmcSettings *TmcSettings) {
  this->axisNumber = axisNumber;

  strcpy(axisPrefix, " Axis_ServoTmc2209, ");
  axisPrefix[5] = '0' + axisNumber;

  this->Pins = Pins;
  enablePin = Pins->enable;
  enabledState = Pins->enabledState;
  faultPin = Pins->fault;

  this->Settings = TmcSettings;
  model = TmcSettings->model;
  statusMode = TmcSettings->status;
  velocityMax = TmcSettings->velocityMax;
  acceleration = (TmcSettings->acceleration/100.0F)*velocityMax;
  accelerationFs = acceleration/FRACTIONAL_SEC;
  decay = TmcSettings->decay;
  if (decay == OFF) decay = STEALTHCHOP;
  decaySlewing = TmcSettings->decaySlewing;
  if (decaySlewing == OFF) decaySlewing = SPREADCYCLE;
  velocityThrs = TmcSettings->velocityThrs;
}

bool ServoTmc2209::init() {
  ServoDriver::init();

  // override max current with user setting
  if (user_currentMax > 0) currentMax = user_currentMax; else currentMax = TMC2209_MAX_CURRENT;

  // automatically set fault status for known drivers
  status.active = statusMode != OFF;

  // get TMC UART driver ready
  pinModeEx(Pins->m0, OUTPUT);
  pinModeEx(Pins->m1, OUTPUT);
  pinModeEx(Pins->step, OUTPUT);
  pinModeEx(Pins->dir, OUTPUT);

  // set S/D motion pins to known, fixed state
  digitalWriteEx(Pins->step, LOW);
  digitalWriteEx(Pins->dir, LOW);

  // show velocity control settings
  VF("MSG:"); V(axisPrefix); VF("Vmax="); V(Settings->velocityMax); VF(" steps/s, Acceleration="); V(Settings->acceleration); VLF(" %/s/s");
  VF("MSG:"); V(axisPrefix); VF("AccelerationFS="); V(accelerationFs); VLF(" steps/s/fs");

  // initialize the serial port
  VF("MSG:"); V(axisPrefix); VF("TMC ");
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

  if (user_rSense > 0.0F) rSense = user_rSense; else rSense = TMC2209_RSENSE;
  VF("MSG:"); V(axisPrefix); VF("Rsense="); V(rSense); VL("ohms");

  #if defined(SERIAL_TMC_HARDWARE_UART)
    driver = new TMC2209Stepper(&SerialTMC, rSense, SERIAL_TMC_ADDRESS_MAP(axisNumber - 1));
  #else
    driver = new TMC2209Stepper(SerialTMC, rSense, SERIAL_TMC_ADDRESS_MAP(axisNumber - 1));
  #endif
  driver->begin();
  driver->intpol(true);

  if (decay == STEALTHCHOP && decaySlewing == SPREADCYCLE && velocityThrs > 0) {
    VF("MSG:"); V(axisPrefix); VF("TMC decay mode velocity threshold "); V(velocityThrs); VLF(" sps");
    driver->TPWMTHRS(velocityThrs/0.715F);
  }

  VF("MSG:"); V(axisPrefix); VF("TMC u-step mode ");
  if (Settings->microsteps == OFF) {
    VLF("OFF (assuming 1X)");
    driver->microsteps(1);
  } else {
    V(Settings->microsteps); VLF("X");
    driver->microsteps(Settings->microsteps);
  }

  currentRms = Settings->current*0.7071F;
  if (Settings->current == OFF) {
    VF("MSG:"); V(axisPrefix); VLF("TMC current control OFF setting 300mA");
    currentRms = 300*0.7071F;
  }

  if (currentRms < 0 || currentRms > currentMax*0.7071F) {
    DF("ERR:"); D(axisPrefix); DF("bad current setting="); DL(currentRms);
    return false;
  }

  driver->hold_multiplier(1.0F);

  VF("MSG:"); V(axisPrefix); VF("Irun="); V(currentRms/0.7071F); VLF("mA");
  driver->rms_current(currentRms);

  driver->en_spreadCycle(true);

  // automatically set fault status for known drivers
  status.active = statusMode == ON;

  // check to see if the driver is there and ok
  #ifdef MOTOR_DRIVER_DETECT
    readStatus();
    if (!status.standstill || status.overTemperature) {
      DF("ERR:"); D(axisPrefix); DLF("no driver detected!");
      return false;
    } else { VF("MSG:"); V(axisPrefix); VLF("motor driver device detected"); }
  #endif

  return true;
}

// move using step/dir signals
void ServoTmc2209::alternateMode(bool state) {
  sdMode = state;
  if (sdMode) driver->VACTUAL(0);
}

// enable or disable the driver using the enable pin or other method
void ServoTmc2209::enable(bool state) {
  enabled = state;
  if (enablePin == SHARED) {
    VF("MSG:"); V(axisPrefix); VF("powered "); if (state) { VF("up"); } else { VF("down"); } VLF(" using UART");
    if (state) {
      driver->en_spreadCycle(!stealthChop());
      driver->rms_current(currentRms);
    } else {
      driver->en_spreadCycle(false);
      driver->ihold(0);
    }
  } else {
    VF("MSG:"); V(axisPrefix); VF("powered "); if (state) { VF("up"); } else { VF("down"); } VLF(" using enable pin");
    if (!enabled) { digitalWriteF(enablePin, !enabledState); } else { digitalWriteF(enablePin, enabledState); }
  }

  currentVelocity = 0.0F;

  ServoDriver::updateStatus();

  return;
}

// set motor velocity (in microsteps/s)
float ServoTmc2209::setMotorVelocity(float velocity) {
  if (sdMode) return velocity;

  if (!enabled) velocity = 0.0F;
  if (velocity > velocityMax) velocity = velocityMax; else
  if (velocity < -velocityMax) velocity = -velocityMax;

  if (velocity > currentVelocity) {
    currentVelocity += accelerationFs;
    if (currentVelocity > velocity) currentVelocity = velocity;
  } else
  if (velocity < currentVelocity) {
    currentVelocity -= accelerationFs;
    if (currentVelocity < velocity) currentVelocity = velocity;
  }

  if (currentVelocity >= 0.0F) motorDirection = DIR_FORWARD; else motorDirection = DIR_REVERSE;

  driver->VACTUAL(currentVelocity/0.715F);

  return currentVelocity;
}

void ServoTmc2209::readStatus() {
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

// calibrate the motor driver if required
void ServoTmc2209::calibrateDriver() {
  if (stealthChop()) {
    VF("MSG:"); V(axisPrefix); VL("TMC standstill automatic current calibration");
    driver->rms_current(currentRms);
    ((TMC2209Stepper*)driver)->pwm_autograd(DRIVER_TMC_STEPPER_AUTOGRAD);
    ((TMC2209Stepper*)driver)->pwm_autoscale(true);
    ((TMC2209Stepper*)driver)->en_spreadCycle(false);
    delay(1000);
  }
}

#endif
