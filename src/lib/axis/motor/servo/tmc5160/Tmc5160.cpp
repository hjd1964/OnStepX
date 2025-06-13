// -----------------------------------------------------------------------------------
// axis servo TMC5160 stepper motor driver

#include "Tmc5160.h"

#ifdef SERVO_TMC5160_PRESENT

#include "../../../../tasks/OnTask.h"
#include "../../../../gpioEx/GpioEx.h"

// help with pin names
#define mosi m0
#define sck  m1
#define cs   m2
#define miso m3

ServoTmc5160::ServoTmc5160(uint8_t axisNumber, const ServoTmcSpiPins *Pins, const ServoTmcSettings *TmcSettings) {
  this->axisNumber = axisNumber;

  strcpy(axisPrefix, " Axis_ServoTmc5160, ");
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

bool ServoTmc5160::init() {
  ServoDriver::init();

  // override max current with user setting
  if (user_currentMax > 0) currentMax = user_currentMax; else currentMax = TMC5160_MAX_CURRENT;

  // automatically set fault status for known drivers
  status.active = statusMode != OFF;

  // get TMC SPI driver ready
  pinModeEx(Pins->step, OUTPUT);
  pinModeEx(Pins->dir, OUTPUT);

  // set S/D motion pins to known, fixed state
  digitalWriteEx(Pins->step, LOW);
  digitalWriteEx(Pins->dir, LOW);

  // show velocity control settings
  VF("MSG:"); V(axisPrefix); VF("Vmax="); V(Settings->velocityMax); VF(" steps/s, Acceleration="); V(Settings->acceleration); VLF(" %/s/s");
  VF("MSG:"); V(axisPrefix); VF("AccelerationFS="); V(accelerationFs); VLF(" steps/s/fs");

  if (user_rSense > 0.0F) rSense = user_rSense; else rSense = TMC5160_RSENSE;
  VF("MSG:"); V(axisPrefix); VF("Rsense="); V(rSense); VL("ohms");

  driver = new TMC5160Stepper(Pins->cs, rSense, Pins->mosi, Pins->miso, Pins->sck);
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
    VF("MSG:"); V(axisPrefix); VLF("TMC current control OFF (600mA)");
    currentRms = 600*0.7071F;
  }
  driver->hold_multiplier(1.0F);

  if (currentRms < 0 || currentRms > currentMax*0.7071F) {
    DF("ERR:"); D(axisPrefix); DF("bad current setting="); DL(currentRms/0.7071F);
    return false;
  }

  VF("MSG:"); V(axisPrefix); VF("Irun="); V(currentRms/0.7071F); VLF("mA");
  driver->rms_current(currentRms);

  unsigned long mode = driver->IOIN();
  if (mode && 0b01000000 > 0) {
    DF("WRN:"); D(axisPrefix); DLF("TMC driver is in Step/Dir mode and WILL NOT WORK for TMC5160_SERVO!");
    return false;
  }

  driver->en_pwm_mode(false);
  driver->AMAX(65535);
  driver->RAMPMODE(1);

  // automatically set fault status for known drivers
  status.active = statusMode == ON;

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

// enable or disable the driver using the enable pin or other method
void ServoTmc5160::enable(bool state) {
  enabled = state;
  if (enablePin == SHARED) {
    VF("MSG:"); V(axisPrefix); VF(", powered "); if (state) { VF("up"); } else { VF("down"); } VLF(" using SPI");
    if (state) {
      driver->en_pwm_mode(stealthChop());
      driver->rms_current(currentRms);
    } else {
      driver->en_pwm_mode(true);
      driver->ihold(0);
    }
  } else {
    VF("MSG:"); V(axisPrefix); VF(", powered "); if (state) { VF("up"); } else { VF("down"); } VLF(" using enable pin");
    if (!enabled) { digitalWriteF(enablePin, !enabledState); } else { digitalWriteF(enablePin, enabledState); }
  }

  currentVelocity = 0.0F;

  ServoDriver::updateStatus();
}

// set motor velocity (in microsteps/s)
float ServoTmc5160::setMotorVelocity(float velocity) {
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

  if (currentVelocity >= 0.0F) {
    driver->shaft(false);
    motorDirection = DIR_FORWARD;
  } else {
    driver->shaft(true);
    motorDirection = DIR_REVERSE;
  }

  driver->VMAX(abs(currentVelocity/0.715F));

  return currentVelocity;
}

// read status info. from driver
void ServoTmc5160::readStatus() {
  TMC2130_n::DRV_STATUS_t status_result;
  status_result.sr = ((TMC5160Stepper*)driver)->DRV_STATUS();

  status.outputA.shortToGround  = status_result.s2ga;
  status.outputA.openLoad       = status_result.ola;
  status.outputB.shortToGround  = status_result.s2gb;
  status.outputB.openLoad       = status_result.olb;
  status.overTemperatureWarning = status_result.otpw;
  status.overTemperature        = status_result.ot;
  status.standstill             = status_result.stst;
}

// calibrate the motor driver if required
void ServoTmc5160::calibrateDriver() {
  if (stealthChop()) {
    VF("MSG:"); V(axisPrefix); VL("TMC standstill automatic current calibration");
    driver->rms_current(currentRms);
    driver->pwm_autograd(DRIVER_TMC_STEPPER_AUTOGRAD);
    driver->pwm_autoscale(true);
    driver->en_pwm_mode(true);
    delay(1000);
  }
}

#endif
