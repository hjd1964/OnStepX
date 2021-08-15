// -----------------------------------------------------------------------------------
// servo driver control

#include "ServoDrivers.h"

#ifdef SERVO_DRIVER_PRESENT

#if DEBUG_MODE != OFF
  const char* SERVO_DRIVER_NAME[DRIVER_SERVO_MODEL_COUNT] = {
  "SERVO_DIR_PHASE",
  "SERVO_IN_IN",
  };
#endif

const ServoDriverPins ServoPins[] = {
  #ifdef AXIS1_DRIVER_SERVO
    { 1, AXIS1_DC_IN1_PIN, AXIS1_DC_IN1_STATE, AXIS1_DC_IN2_PIN, AXIS1_DC_IN2_STATE, AXIS1_ENABLE_PIN, AXIS1_ENABLE_STATE, AXIS1_FAULT_PIN },
  #endif
  #ifdef AXIS2_DRIVER_SERVO
    { 2, AXIS2_DC_IN1_PIN, AXIS2_DC_IN1_STATE, AXIS2_DC_IN2_PIN, AXIS2_DC_IN2_STATE, AXIS2_ENABLE_PIN, AXIS2_ENABLE_STATE, AXIS2_FAULT_PIN },
  #endif
  #ifdef AXIS3_DRIVER_SERVO
    { 3, AXIS3_DC_IN1_PIN, AXIS3_DC_IN1_STATE, AXIS3_DC_IN2_PIN, AXIS3_DC_IN2_STATE, AXIS3_ENABLE_PIN, AXIS3_ENABLE_STATE, AXIS3_FAULT_PIN },
  #endif
  #ifdef AXIS4_DRIVER_SERVO
    { 4, AXIS4_DC_IN1_PIN, AXIS4_DC_IN1_STATE, AXIS4_DC_IN2_PIN, AXIS4_DC_IN2_STATE, AXIS4_ENABLE_PIN, AXIS4_ENABLE_STATE, AXIS4_FAULT_PIN },
  #endif
  #ifdef AXIS5_DRIVER_SERVO
    { 5, AXIS5_DC_IN1_PIN, AXIS5_DC_IN1_STATE, AXIS5_DC_IN2_PIN, AXIS5_DC_IN2_STATE, AXIS5_ENABLE_PIN, AXIS5_ENABLE_STATE, AXIS5_FAULT_PIN },
  #endif
  #ifdef AXIS6_DRIVER_SERVO
    { 6, AXIS6_DC_IN1_PIN, AXIS6_DC_IN1_STATE, AXIS6_DC_IN2_PIN, AXIS6_DC_IN2_STATE, AXIS6_ENABLE_PIN, AXIS6_ENABLE_STATE, AXIS6_FAULT_PIN },
  #endif
  #ifdef AXIS7_DRIVER_SERVO
    { 7, AXIS7_DC_IN1_PIN, AXIS7_DC_IN1_STATE, AXIS7_DC_IN2_PIN, AXIS7_DC_IN2_STATE, AXIS7_ENABLE_PIN, AXIS7_ENABLE_STATE, AXIS7_FAULT_PIN },
  #endif
  #ifdef AXIS8_DRIVER_SERVO
    { 8, AXIS8_DC_IN1_PIN, AXIS8_DC_IN1_STATE, AXIS8_DC_IN2_PIN, AXIS8_DC_IN2_STATE, AXIS8_ENABLE_PIN, AXIS8_ENABLE_STATE, AXIS8_FAULT_PIN },
  #endif
  #ifdef AXIS9_DRIVER_SERVO
    { 9, AXIS9_DC_IN1_PIN, AXIS9_DC_IN1_STATE, AXIS9_DC_IN2_PIN, AXIS9_DC_IN2_STATE, AXIS9_ENABLE_PIN, AXIS9_ENABLE_STATE, AXIS9_FAULT_PIN },
  #endif
};

const ServoDriverSettings ServoSettings[] = {
  #ifdef AXIS1_DRIVER_SERVO
    { AXIS1_DRIVER_MODEL, AXIS1_DRIVER_SERVO_P, AXIS1_DRIVER_SERVO_I, AXIS1_DRIVER_SERVO_D, AXIS1_DRIVER_STATUS },
  #endif
  #ifdef AXIS2_DRIVER_SERVO
    { AXIS2_DRIVER_MODEL, AXIS2_DRIVER_SERVO_P, AXIS2_DRIVER_SERVO_I, AXIS2_DRIVER_SERVO_D, AXIS2_DRIVER_STATUS },
  #endif
  #ifdef AXIS3_DRIVER_SERVO
    { AXIS3_DRIVER_MODEL, AXIS3_DRIVER_SERVO_P, AXIS3_DRIVER_SERVO_I, AXIS3_DRIVER_SERVO_D, AXIS3_DRIVER_STATUS },
  #endif
  #ifdef AXIS4_DRIVER_SERVO
    { AXIS4_DRIVER_MODEL, AXIS4_DRIVER_SERVO_P, AXIS4_DRIVER_SERVO_I, AXIS4_DRIVER_SERVO_D, AXIS4_DRIVER_STATUS },
  #endif
  #ifdef AXIS5_DRIVER_SERVO
    { AXIS5_DRIVER_MODEL, AXIS5_DRIVER_SERVO_P, AXIS5_DRIVER_SERVO_I, AXIS5_DRIVER_SERVO_D, AXIS5_DRIVER_STATUS },
  #endif
  #ifdef AXIS6_DRIVER_SERVO
    { AXIS6_DRIVER_MODEL, AXIS6_DRIVER_SERVO_P, AXIS6_DRIVER_SERVO_I, AXIS6_DRIVER_SERVO_D, AXIS6_DRIVER_STATUS },
  #endif
  #ifdef AXIS7_DRIVER_SERVO
    { AXIS7_DRIVER_MODEL, AXIS7_DRIVER_SERVO_P, AXIS7_DRIVER_SERVO_I, AXIS7_DRIVER_SERVO_D, AXIS7_DRIVER_STATUS },
  #endif
  #ifdef AXIS8_DRIVER_SERVO
    { AXIS8_DRIVER_MODEL, AXIS8_DRIVER_SERVO_P, AXIS8_DRIVER_SERVO_I, AXIS8_DRIVER_SERVO_D, AXIS8_DRIVER_STATUS },
  #endif
  #ifdef AXIS9_DRIVER_SERVO
    { AXIS9_DRIVER_MODEL, AXIS9_DRIVER_SERVO_P, AXIS9_DRIVER_SERVO_I, AXIS9_DRIVER_SERVO_D, AXIS9_DRIVER_STATUS },
  #endif
};

void ServoDriver::init(uint8_t axisNumber) {
  this->axisNumber = axisNumber;

  // load constants for this axis
  for (uint8_t i = 0; i < 10; i++) {
    if (ServoPins[i].axis == axisNumber) {
      pins = &ServoPins[i]; settings = ServoSettings[i]; break;
    } if (i == 9) { VLF("ERR: Servo::init(); indexing failed!"); return; }
  }

  #if DEBUG == VERBOSE
    VF("MSG: ServoDriver, init model "); V(SERVO_DRIVER_NAME[settings.model]);
    VF(" p = "); V(settings.p); VF(", i = "); V(settings.i); VF(", d = "); VL(settings.d);
  #endif

  // init default driver control pins
  pinModeEx(pins->enable, OUTPUT);
  digitalWriteEx(pins->enable, !pins->enabledState);
  pinMode(pins->in1, OUTPUT);
  digitalWriteF(pins->in1, pins->inState1); // either in1 or direction, state should default to inactive
  pinMode(pins->in2, OUTPUT);
  digitalWriteF(pins->in2, pins->inState2); // either in2 or phase (PWM,) state should default to inactive

  // automatically set fault status for known drivers
  if (settings.status == ON) {
    settings.status = LOW;
  }

  // set fault pin mode
  if (settings.status == LOW) pinModeEx(pins->fault, INPUT_PULLUP);
  #ifdef PULLDOWN
    if (settings.status == HIGH) pinModeEx(pins->fault, INPUT_PULLDOWN);
  #else
    if (settings.status == HIGH) pinModeEx(pins->fault, INPUT);
  #endif
}

void ServoDriver::updateStatus() {
  if (settings.status == LOW || settings.status == HIGH) {
    status.fault = digitalReadEx(pins->fault) == settings.status;
  }
}

DriverStatus ServoDriver::getStatus() {
  return status;
}

// power down using the enable pin
void ServoDriver::power(bool state) {
  powered = state;
  if (!powered) { digitalWriteF(pins->enable, !pins->enabledState); } else { digitalWriteF(pins->enable, pins->enabledState); }
 }

// power level to the motor (-255 to 255, negative for reverse)
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
      digitalWriteF(pins->in1, pins->inState1);
      if (pins->inState2 == HIGH) motorPwr = 255 - motorPwr;
      analogWrite(pins->in2, motorPwr);
    } else
    if (motorDir == DIR_REVERSE) {
      if (pins->inState1 == HIGH) motorPwr = 255 - motorPwr;
      analogWrite(pins->in1, motorPwr);
      digitalWriteF(pins->in2, pins->inState2);
    } else {
      digitalWriteF(pins->in1, pins->inState1);
      digitalWriteF(pins->in2, pins->inState2);
    }
  } else
  if (settings.model == SERVO_DP) {
    if (motorDir == DIR_FORWARD) {
      digitalWriteF(pins->in1, pins->inState1);
      if (pins->inState2 == HIGH) motorPwr = 255 - motorPwr;
      analogWrite(pins->in2, motorPwr);
    } else
    if (motorDir == DIR_REVERSE) {
      digitalWriteF(pins->in1, !pins->inState1);
      if (pins->inState2 == HIGH) motorPwr = 255 - motorPwr;
      analogWrite(pins->in2, motorPwr);
    } else {
      digitalWriteF(pins->in1, pins->inState1);
      digitalWriteF(pins->in2, pins->inState2);
    }
  }
}

#endif