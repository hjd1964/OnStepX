// -----------------------------------------------------------------------------------
// axis servo TMC2209 stepper motor driver

#include "Tmc2209.h"

#ifdef SERVO_TMC2209_PRESENT

#include "../../../../../tasks/OnTask.h"
#include "../../../../../gpioEx/GpioEx.h"

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

ServoTmc2209::ServoTmc2209(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings,
                           float countsToStepsRatio, int16_t microsteps, int16_t current, int8_t decay, int8_t decaySlewing)
                           :TmcServoDriver(axisNumber, Pins, Settings, microsteps, current, decay, decaySlewing) {
  if (axisNumber < 1 || axisNumber > 9) return;

  strcpy(axisPrefix, " Axis_ServoTmc2209, ");
  axisPrefix[5] = '0' + axisNumber;

  this->velocityThrs = velocityThrs;
  this->countsToStepsRatio.valueDefault = countsToStepsRatio;
}

bool ServoTmc2209::init(bool reverse) {
  if (!TmcServoDriver::init(reverse)) return false;

  // get TMC UART driver ready
  pinModeEx(Pins->m0, OUTPUT);
  pinModeEx(Pins->m1, OUTPUT);
  pinModeEx(Pins->ph1, OUTPUT); // step
  pinModeEx(Pins->ph2, OUTPUT); // dir

  // set S/D motion pins to known, fixed state
  digitalWriteEx(Pins->ph1, LOW); // step
  digitalWriteEx(Pins->ph2, LOW); // dir

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

  #if defined(SERIAL_TMC_HARDWARE_UART)
    driver = new TMC2209Stepper(&SerialTMC, rSense, SERIAL_TMC_ADDRESS_MAP(axisNumber - 1));
  #else
    driver = new TMC2209Stepper(SerialTMC, rSense, SERIAL_TMC_ADDRESS_MAP(axisNumber - 1));
  #endif
  driver->begin();
  driver->intpol(true);

  if (decay.value == STEALTHCHOP && decaySlewing.value == SPREADCYCLE && velocityThrs > 0) {
    VF("MSG:"); V(axisPrefix); VF("TMC decay mode velocity threshold "); V(velocityThrs); VLF(" sps");
    driver->TPWMTHRS(velocityThrs/0.715F);
  }

  driver->microsteps(256);
  
  driver->hold_multiplier(iHoldRatio);

  VF("MSG:"); V(axisPrefix); VF("Irun="); V(lround(iRun)); VLF("mA");

  driver->rms_current(lround(iRun*0.7071));

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

// enable or disable the driver using the enable pin or other method
void ServoTmc2209::enable(bool state) {
  enabled = state;

  if (enablePin == SHARED) {
    VF("MSG:"); V(axisPrefix); VF("powered "); if (state) { VF("up"); } else { VF("down"); } VLF(" using UART");
    if (state) {
      driver->en_spreadCycle(!stealthChop());
      driver->rms_current(iRun*0.7071);
    } else {
      driver->en_spreadCycle(false);
      driver->ihold(0);
    }
  } else {
    VF("MSG:"); V(axisPrefix); VF("powered "); if (state) { VF("up"); } else { VF("down"); } VLF(" using enable pin");
    if (!enabled) { digitalWriteF(enablePin, !enabledState); } else { digitalWriteF(enablePin, enabledState); }
  }

  velocityRamp = 0.0F;

  ServoDriver::updateStatus();

  return;
}

float ServoTmc2209::setMotorVelocity(float velocity) {
  velocity = ServoDriver::setMotorVelocity(velocity);

  driver->VACTUAL(reversed ? -(velocity/0.715F)*countsToStepsRatio.value : (velocity/0.715F)*countsToStepsRatio.value);

  return velocity;
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
    driver->rms_current(iRun*0.7071);
    driver->pwm_autograd(DRIVER_TMC_STEPPER_AUTOGRAD);
    driver->pwm_autoscale(true);
    driver->en_spreadCycle(false);
    delay(1000);
  }
}

#endif
