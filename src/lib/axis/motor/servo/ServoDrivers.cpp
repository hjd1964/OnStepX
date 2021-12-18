// -----------------------------------------------------------------------------------
// axis servo motor driver

#include "ServoDrivers.h"

#ifdef SERVO_DRIVER_PRESENT
#include "../Motor.h"

#if DEBUG != OFF
  const char* SERVO_DRIVER_NAME[DRIVER_SERVO_MODEL_COUNT] = { "SERVO_PD", "SERVO_II" };
#endif

ServoDriver::ServoDriver(uint8_t axisNumber, const ServoDriverPins *Pins, const ServoDriverSettings *Settings) {
  this->axisNumber = axisNumber;
  this->Pins = Pins;
  settings = *Settings;
}

void ServoDriver::init() {
  #if DEBUG == VERBOSE
    VF("MSG: Servo"); V(axisNumber); VF(", init model "); V(SERVO_DRIVER_NAME[settings.model - SERVO_DRIVER_FIRST]);
  #endif

  // init default driver control pins
  pinModeEx(Pins->enable, OUTPUT);
  digitalWriteEx(Pins->enable, !Pins->enabledState);
  pinMode(Pins->in1, OUTPUT);
  digitalWriteF(Pins->in1, Pins->inState1); // either in1 or direction, state should default to inactive
  pinMode(Pins->in2, OUTPUT);
  digitalWriteF(Pins->in2, Pins->inState2); // either in2 or phase (PWM,) state should default to inactive

  // set fastest PWM speed for Teensy processors
  #ifdef ANALOG_WRITE_PWM_FREQUENCY
    VF("MSG: Servo"); V(axisNumber); VF(", setting control pins analog frequency "); VL(ANALOG_WRITE_PWM_FREQUENCY);
    analogWriteFrequency(Pins->in1, ANALOG_WRITE_PWM_FREQUENCY);
    analogWriteFrequency(Pins->in2, ANALOG_WRITE_PWM_FREQUENCY);
  #endif

  // automatically set fault status for known drivers
  if (settings.status == ON) {
    settings.status = LOW;
  }

  // set fault pin mode
  if (settings.status == LOW) pinModeEx(Pins->fault, INPUT_PULLUP);
  #ifdef PULLDOWN
    if (settings.status == HIGH) pinModeEx(Pins->fault, INPUT_PULLDOWN);
  #else
    if (settings.status == HIGH) pinModeEx(Pins->fault, INPUT);
  #endif
}

void ServoDriver::updateStatus() {
  if (settings.status == LOW || settings.status == HIGH) {
    status.fault = digitalReadEx(Pins->fault) == settings.status;
  }
}

DriverStatus ServoDriver::getStatus() {
  return status;
}

// power down using the enable pin
void ServoDriver::power(bool state) {
  powered = state;
  if (!powered) { digitalWriteF(Pins->enable, !Pins->enabledState); } else { digitalWriteF(Pins->enable, Pins->enabledState); }
 }

// power level to the motor (-ANALOG_WRITE_PWM_RANGE to ANALOG_WRITE_PWM_RANGE, negative for reverse)
void ServoDriver::setMotorPower(int power) {
  if (!powered) motorPwr = 0; else motorPwr = abs(power);
  if (power >= 0) setMotorDirection(DIR_FORWARD); else setMotorDirection(DIR_REVERSE);
  update();
 }

// motor direction (DIR_FORMWARD or DIR_REVERSE)
void ServoDriver::setMotorDirection(Direction dir) {
  motorDir = dir;
  update();
}

void ServoDriver::update() {
  if (settings.model == SERVO_II) {
    if (motorDir == DIR_FORWARD) {
      if (Pins->inState1 == HIGH) analogWrite(Pins->in1, ANALOG_WRITE_PWM_RANGE); else analogWrite(Pins->in1, 0);
      if (Pins->inState2 == HIGH) motorPwr = ANALOG_WRITE_PWM_RANGE - motorPwr;
      analogWrite(Pins->in2, motorPwr);
    } else
    if (motorDir == DIR_REVERSE) {
      if (Pins->inState1 == HIGH) motorPwr = ANALOG_WRITE_PWM_RANGE - motorPwr;
      analogWrite(Pins->in1, motorPwr);
      if (Pins->inState2 == HIGH) analogWrite(Pins->in2, ANALOG_WRITE_PWM_RANGE); else analogWrite(Pins->in2, 0);
    } else {
      if (Pins->inState1 == HIGH) analogWrite(Pins->in1, ANALOG_WRITE_PWM_RANGE); else analogWrite(Pins->in1, 0);
      if (Pins->inState2 == HIGH) analogWrite(Pins->in2, ANALOG_WRITE_PWM_RANGE); else analogWrite(Pins->in2, 0);
    }
  } else
  if (settings.model == SERVO_PD) {
    if (motorDir == DIR_FORWARD) {
      if (Pins->inState2 == HIGH) analogWrite(Pins->in2, ANALOG_WRITE_PWM_RANGE); else analogWrite(Pins->in2, 0);
      if (Pins->inState1 == HIGH) motorPwr = ANALOG_WRITE_PWM_RANGE - motorPwr;
      analogWrite(Pins->in1, motorPwr);
    } else
    if (motorDir == DIR_REVERSE) {
      digitalWriteF(Pins->in2, !Pins->inState2);
      if (Pins->inState1 == HIGH) motorPwr = ANALOG_WRITE_PWM_RANGE - motorPwr;
      analogWrite(Pins->in1, motorPwr);
    } else {
      digitalWriteF(Pins->in1, Pins->inState1);
      digitalWriteF(Pins->in2, Pins->inState2);
    }
  }
}

#endif