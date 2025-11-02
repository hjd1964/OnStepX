// -----------------------------------------------------------------------------------
// axis servo TMC5160 stepper motor driver

#include "Tmc5160.h"

#ifdef SERVO_TMC5160_PRESENT

#include "../../../../../tasks/OnTask.h"
#include "../../../../../gpioEx/GpioEx.h"

// help with pin names
#define mosi m0
#define sck  m1
#define cs   m2
#define miso m3

ServoTmc5160::ServoTmc5160(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *TmcSettings,
                           float countsToStepsRatio, int16_t microsteps, int16_t current, int8_t decay, int8_t decaySlewing)
                           :TmcServoDriver(axisNumber, Pins, Settings, microsteps, current, decay, decaySlewing) {
  if (axisNumber < 1 || axisNumber > 9) return;

  strcpy(axisPrefix, " Axis_ServoTmc5160, ");
  axisPrefix[5] = '0' + axisNumber;

  this->velocityThrs = velocityThrs;
  this->countsToStepsRatio.valueDefault = countsToStepsRatio;
}

bool ServoTmc5160::init(bool reverse) {
  if (!TmcServoDriver::init(reverse)) return false;

  // get TMC SPI driver ready
  pinModeEx(Pins->ph1, OUTPUT); // step
  pinModeEx(Pins->ph2, OUTPUT); // dir

  // set S/D motion pins to known, fixed state
  digitalWriteEx(Pins->ph1, LOW); // step
  digitalWriteEx(Pins->ph2, LOW); // dir

  driver = new TMC5160Stepper(Pins->cs, rSense, Pins->mosi, Pins->miso, Pins->sck);
  driver->begin();
  driver->intpol(true);

  if (decay.value == STEALTHCHOP && decaySlewing.value == SPREADCYCLE && velocityThrs > 0) {
    VF("MSG:"); V(axisPrefix); VF("TMC decay mode velocity threshold "); V(velocityThrs); VLF(" sps");
    driver->TPWMTHRS(velocityThrs/0.715F);
  }

  driver->microsteps(256);

  driver->hold_multiplier(iHoldRatio);
  driver->rms_current(iRun*0.7071);

  unsigned long mode = driver->IOIN();
  if (mode && 0b01000000 > 0) {
    DF("WRN:"); D(axisPrefix); DLF("TMC driver is in Step/Dir mode and WILL NOT WORK for TMC5160_SERVO!");
    return false;
  }

  driver->en_pwm_mode(false);
  driver->AMAX(65535);
  driver->RAMPMODE(1);

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
      driver->rms_current(iRun*0.7071);
    } else {
      driver->en_pwm_mode(true);
      driver->ihold(0);
    }
  } else {
    VF("MSG:"); V(axisPrefix); VF(", powered "); if (state) { VF("up"); } else { VF("down"); } VLF(" using enable pin");
    if (!enabled) { digitalWriteF(enablePin, !enabledState); } else { digitalWriteF(enablePin, enabledState); }
  }

  velocityRamp = 0.0F;

  ServoDriver::updateStatus();
}

float ServoTmc5160::setMotorVelocity(float velocity) {
  velocity = ServoDriver::setMotorVelocity(velocity);

  if (motorDirection = DIR_FORWARD) driver->shaft(reversed ? true : false); else driver->shaft(reversed ? false : true);
  driver->VMAX(fabs((velocity/0.715F)*countsToStepsRatio.value));

  return velocity;
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
    driver->rms_current(iRun*0.7071);
    driver->pwm_autograd(DRIVER_TMC_STEPPER_AUTOGRAD);
    driver->pwm_autoscale(true);
    driver->en_pwm_mode(true);
    delay(1000);
  }
}

#endif
