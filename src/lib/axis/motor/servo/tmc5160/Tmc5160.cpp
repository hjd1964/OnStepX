// -----------------------------------------------------------------------------------
// axis servo TMC5160 stepper motor driver

#include "Tmc5160.h"

#ifdef SERVO_TMC5160_PRESENT

#include "../../../../tasks/OnTask.h"

// help with pin names
#define mosi m0
#define sck  m1
#define cs   m2
#define miso m3

ServoTmc5160::ServoTmc5160(uint8_t axisNumber, const ServoTmcSpiPins *Pins, const ServoTmcSettings *TmcSettings) {
  this->axisNumber = axisNumber;

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

void ServoTmc5160::init() {
  ServoDriver::init();

  // automatically set fault status for known drivers
  status.active = statusMode != OFF;

  // get TMC SPI driver ready
  pinModeEx(Pins->step, OUTPUT);
  pinModeEx(Pins->dir, OUTPUT);

  // set S/D motion pins to known, fixed state
  digitalWriteEx(Pins->step, LOW);
  digitalWriteEx(Pins->dir, LOW);

  // show velocity control settings
  VF("MSG: ServoDriver"); V(axisNumber); VF(", Vmax="); V(Settings->velocityMax); VF(" steps/s, Acceleration="); V(Settings->acceleration); VLF(" %/s/s");
  VF("MSG: ServoDriver"); V(axisNumber); VF(", AccelerationFS="); V(accelerationFs); VLF(" steps/s/fs");

  driver = new TMC5160Stepper(Pins->cs, Pins->mosi, Pins->miso, Pins->sck);
  driver->begin();
  driver->intpol(true);

  if (decay == STEALTHCHOP && decaySlewing == SPREADCYCLE && velocityThrs > 0) {
    VF("MSG: ServoDriver"); V(axisNumber); VF(", TMC decay mode velocity threshold "); V(velocityThrs); VLF(" sps");
    driver->TPWMTHRS(velocityThrs/0.715F);
  }

  VF("MSG: ServoDriver"); V(axisNumber); VF(", TMC u-step mode ");
  if (Settings->microsteps == OFF) {
    VLF("OFF (assuming 1X)");
    driver->microsteps(1);
  } else {
    V(Settings->microsteps); VLF("X");
    driver->microsteps(Settings->microsteps);
  }

  currentRms = Settings->current*0.7071F;
  VF("MSG: ServoDriver"); V(axisNumber); VF(", TMC ");
  if (Settings->current == OFF) {
    VLF("current control OFF (600mA)");
    currentRms = 600*0.7071F;
  }
  driver->hold_multiplier(1.0F);

  VF("Irun="); V(currentRms/0.7071F); VLF("mA");
  driver->rms_current(currentRms);

  unsigned long mode = driver->IOIN();
  if (mode && 0b01000000 > 0) {
    VF("WRN: ServoDriver"); V(axisNumber); VLF(", TMC driver is in Step/Dir mode and WILL NOT WORK for TMC5160_SERVO!");
  }

  driver->en_pwm_mode(false);
  driver->AMAX(65535);
  driver->RAMPMODE(1);

  // automatically set fault status for known drivers
  status.active = statusMode != OFF;

  // set fault pin mode
  if (statusMode == LOW) pinModeEx(faultPin, INPUT_PULLUP);
  #ifdef PULLDOWN
    if (statusMode == HIGH) pinModeEx(faultPin, INPUT_PULLDOWN);
  #else
    if (statusMode == HIGH) pinModeEx(faultPin, INPUT);
  #endif
}

// enable or disable the driver using the enable pin or other method
void ServoTmc5160::enable(bool state) {
  enabled = state;
  if (enablePin == SHARED) {
    VF("MSG: ServoDriver"); V(axisNumber);
    VF(", powered "); if (state) { VF("up"); } else { VF("down"); } VLF(" using SPI");
    if (state) {
      driver->en_pwm_mode(stealthChop());
      driver->rms_current(currentRms);
    } else {
      driver->en_pwm_mode(true);
      driver->ihold(0);
    }
  } else {
    VF("MSG: ServoDriver"); V(axisNumber);
    VF(", powered "); if (state) { VF("up"); } else { VF("down"); } VLF(" using enable pin");
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

// update status info. for driver
void ServoTmc5160::updateStatus() {
  if (statusMode == ON) {
    if ((long)(millis() - timeLastStatusUpdate) > 200) {

      TMC2130_n::DRV_STATUS_t status_result;
      status_result.sr = ((TMC5160Stepper*)driver)->DRV_STATUS();
      status.outputA.shortToGround = status_result.s2ga;
      status.outputA.openLoad      = status_result.ola;
      status.outputB.shortToGround = status_result.s2gb;
      status.outputB.openLoad      = status_result.olb;
      status.overTemperatureWarning= status_result.otpw;
      status.overTemperature       = status_result.ot;
      status.standstill            = status_result.stst;

      // open load indication is not reliable in standstill
      if (status.outputA.shortToGround || status.outputB.shortToGround ||
          status.overTemperatureWarning || status.overTemperature) status.fault = true; else status.fault = false;

      timeLastStatusUpdate = millis();
    }
  } else
  if (statusMode == LOW || statusMode == HIGH) {
    status.fault = digitalReadEx(Pins->fault) == statusMode;
  }

  ServoDriver::updateStatus();
}

// calibrate the motor driver if required
void ServoTmc5160::calibrateDriver() {
  if (stealthChop()) {
    VF("MSG: ServoTmc5160 Axis"); V(axisNumber); VL(", TMC standstill automatic current calibration");
    driver->rms_current(currentRms);
    driver->pwm_autograd(DRIVER_TMC_STEPPER_AUTOGRAD);
    driver->pwm_autoscale(true);
    driver->en_pwm_mode(true);
    delay(1000);
  }
}

#endif
