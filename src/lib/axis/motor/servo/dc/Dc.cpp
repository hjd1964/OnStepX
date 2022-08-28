// -----------------------------------------------------------------------------------
// axis servo DC motor driver

#include "Dc.h"

#ifdef SERVO_DC_PRESENT

ServoDc::ServoDc(uint8_t axisNumber, const ServoDcPins *Pins, const ServoDcSettings *Settings) {
  this->axisNumber = axisNumber;

  this->Pins = Pins;
  enablePin = Pins->enable;
  enabledState = Pins->enabledState;
  faultPin = Pins->fault;

  this->Settings = Settings;
  model = Settings->model;
  statusMode = Settings->status;
}

void ServoDc::init() {
  ServoDriver::init();

  #if DEBUG == VERBOSE
    VF("MSG: ServoDriver"); V(axisNumber);
    if (model == SERVO_EE) {
      V(", pins pwm1="); if (Pins->in1 == OFF) V("OFF"); else V(Pins->in1);
      V(", pwm2="); if (Pins->in2 == OFF) VF("OFF"); else V(Pins->in2);
    } else
    if (model == SERVO_PE) {
      V(", pins dir="); if (Pins->in1 == OFF) V("OFF"); else V(Pins->in1);
      V(", pwm="); if (Pins->in2 == OFF) VF("OFF"); else V(Pins->in2);
    }
  #endif

  // init default driver control pins
  pinModeEx(Pins->in1, OUTPUT);
  digitalWriteF(Pins->in1, Pins->inState1); // either in1 or direction, state should default to inactive
  pinModeEx(Pins->in2, OUTPUT);
  digitalWriteF(Pins->in2, Pins->inState2); // either in2 or phase (PWM,) state should default to inactive

  // set fastest PWM speed for Teensy processors
  #ifdef ANALOG_WRITE_PWM_FREQUENCY
    VF("MSG: Servo"); V(axisNumber); VF(", setting control pins analog frequency "); VL(ANALOG_WRITE_PWM_FREQUENCY);
    analogWriteFrequency(Pins->in1, ANALOG_WRITE_PWM_FREQUENCY);
    analogWriteFrequency(Pins->in2, ANALOG_WRITE_PWM_FREQUENCY);
  #endif

  // set fault pin mode
  if (statusMode == ON) statusMode = LOW;
  if (statusMode == LOW) pinModeEx(faultPin, INPUT_PULLUP);
  #ifdef PULLDOWN
    if (statusMode == HIGH) pinModeEx(faultPin, INPUT_PULLDOWN);
  #else
    if (statusMode == HIGH) pinModeEx(faultPin, INPUT);
  #endif
}

// power level to the motor
void ServoDc::setMotorPower(float power) {
  if (!enabled) motorPwr = 0.0F; else motorPwr = fabs(power);
  if (motorPwr > motorPwrMax) motorPwr = motorPwrMax;
  if (power >= 0) setMotorDirection(DIR_FORWARD); else setMotorDirection(DIR_REVERSE);
  pwmUpdate();
 }

// motor direction (DIR_FORMWARD or DIR_REVERSE)
void ServoDc::setMotorDirection(Direction dir) {
  motorDir = dir;
  pwmUpdate();
}

// motor control update
void ServoDc::pwmUpdate() {
  if (model == SERVO_EE) {
    if (motorDir == DIR_FORWARD) {
      if (Pins->inState1 == HIGH) analogWrite(Pins->in1, motorPwrMax); else analogWrite(Pins->in1, 0);
      if (Pins->inState2 == HIGH) motorPwr = motorPwrMax - motorPwr;
      analogWrite(Pins->in2, motorPwr);
    } else
    if (motorDir == DIR_REVERSE) {
      if (Pins->inState1 == HIGH) motorPwr = motorPwrMax - motorPwr;
      analogWrite(Pins->in1, motorPwr);
      if (Pins->inState2 == HIGH) analogWrite(Pins->in2, motorPwrMax); else analogWrite(Pins->in2, 0);
    } else {
      if (Pins->inState1 == HIGH) analogWrite(Pins->in1, motorPwrMax); else analogWrite(Pins->in1, 0);
      if (Pins->inState2 == HIGH) analogWrite(Pins->in2, motorPwrMax); else analogWrite(Pins->in2, 0);
    }
  } else
  if (model == SERVO_PE) {
    if (motorDir == DIR_FORWARD) {
      digitalWriteF(Pins->in1, Pins->inState1);
      if (Pins->inState2 == HIGH) motorPwr = motorPwrMax - motorPwr;
      analogWrite(Pins->in2, motorPwr);
    } else
    if (motorDir == DIR_REVERSE) {
      digitalWriteF(Pins->in1, !Pins->inState1);
      if (Pins->inState2 == HIGH) motorPwr = motorPwrMax - motorPwr;
      analogWrite(Pins->in2, motorPwr);
    } else {
      digitalWriteF(Pins->in1, Pins->inState1);
      if (Pins->inState2 == HIGH) analogWrite(Pins->in2, motorPwrMax); else analogWrite(Pins->in2, 0);
    }
  }
}

// update status info. for driver
void ServoDc::updateStatus() {
  if (statusMode == LOW || statusMode == HIGH) {
    status.fault = digitalReadEx(faultPin) == statusMode;
  }
  ServoDriver::updateStatus();
}

#endif