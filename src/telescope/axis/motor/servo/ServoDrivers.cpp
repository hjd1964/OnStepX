// -----------------------------------------------------------------------------------
// servo driver control

#include "ServoDrivers.h"

#ifdef SERVO_DRIVER_PRESENT

#if DEBUG_MODE != OFF
  const char* DRIVER_NAME[DRIVER_SERVO_MODEL_COUNT] = {
  "SERVO_DIR_PHASE",
  "SERVO_IN_IN",
  };
#endif

const DriverPins Pins[] = {
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

const DriverSettings ModeSettings[] = {
  #ifdef AXIS1_DRIVER_SERVO
    { AXIS1_DRIVER_MODEL, AXIS1_DRIVER_STATUS },
  #endif
  #ifdef AXIS2_DRIVER_SERVO
    { AXIS2_DRIVER_MODEL, AXIS1_DRIVER_SERVO_P, AXIS1_DRIVER_SERVO_I, AXIS1_DRIVER_SERVO_D, AXIS2_DRIVER_STATUS },
  #endif
  #ifdef AXIS3_DRIVER_SERVO
    { AXIS3_DRIVER_MODEL, AXIS1_DRIVER_SERVO_P, AXIS1_DRIVER_SERVO_I, AXIS1_DRIVER_SERVO_D, AXIS3_DRIVER_STATUS },
  #endif
  #ifdef AXIS4_DRIVER_SERVO
    { AXIS4_DRIVER_MODEL, AXIS1_DRIVER_SERVO_P, AXIS1_DRIVER_SERVO_I, AXIS1_DRIVER_SERVO_D, AXIS4_DRIVER_STATUS },
  #endif
  #ifdef AXIS5_DRIVER_SERVO
    { AXIS5_DRIVER_MODEL, AXIS1_DRIVER_SERVO_P, AXIS1_DRIVER_SERVO_I, AXIS1_DRIVER_SERVO_D, AXIS5_DRIVER_STATUS },
  #endif
  #ifdef AXIS6_DRIVER_SERVO
    { AXIS6_DRIVER_MODEL, AXIS6_DRIVER_MICROSTEPS, AXIS6_DRIVER_MICROSTEPS_GOTO, AXIS6_DRIVER_IHOLD, AXIS6_DRIVER_IRUN, AXIS6_DRIVER_IGOTO, AXIS6_DRIVER_DECAY, AXIS6_DRIVER_DECAY_GOTO, AXIS6_DRIVER_STATUS },
  #endif
  #ifdef AXIS7_DRIVER_SERVO
    { AXIS7_DRIVER_MODEL, AXIS7_DRIVER_MICROSTEPS, AXIS7_DRIVER_MICROSTEPS_GOTO, AXIS7_DRIVER_IHOLD, AXIS7_DRIVER_IRUN, AXIS7_DRIVER_IGOTO, AXIS7_DRIVER_DECAY, AXIS7_DRIVER_DECAY_GOTO, AXIS7_DRIVER_STATUS },
  #endif
  #ifdef AXIS8_DRIVER_SERVO
    { AXIS8_DRIVER_MODEL, AXIS8_DRIVER_MICROSTEPS, AXIS8_DRIVER_MICROSTEPS_GOTO, AXIS8_DRIVER_IHOLD, AXIS8_DRIVER_IRUN, AXIS8_DRIVER_IGOTO, AXIS8_DRIVER_DECAY, AXIS8_DRIVER_DECAY_GOTO, AXIS8_DRIVER_STATUS },
  #endif
  #ifdef AXIS9_DRIVER_SERVO
    { AXIS9_DRIVER_MODEL, AXIS9_DRIVER_MICROSTEPS, AXIS9_DRIVER_MICROSTEPS_GOTO, AXIS9_DRIVER_IHOLD, AXIS9_DRIVER_IRUN, AXIS9_DRIVER_IGOTO, AXIS9_DRIVER_DECAY, AXIS9_DRIVER_DECAY_GOTO, AXIS9_DRIVER_STATUS },
  #endif
};

void DcDriver::init(uint8_t axisNumber) {
  this->axisNumber = axisNumber;

  // load constants for this axis
  for (uint8_t i = 0; i < 10; i++) { if (Pins[i].axis == axisNumber) { index = i; pins = Pins[i]; settings = ModeSettings[i]; break; } if (i == 9) { VLF("ERR: Servo::init(); indexing failed!"); return; } }

  // init default driver control pins
  pinModeEx(pins.enable, OUTPUT);
  digitalWriteEx(pins.enable, !pins.enabledState);
  pinMode(pins.in1, OUTPUT);
  digitalWriteF(pins.in1, pins.inState1); // either in1 or direction, state should default to inactive
  pinMode(pins.in2, OUTPUT);
  digitalWriteF(pins.in2, pins.inState2); // either in2 or phase (PWM,) state should default to inactive

  // automatically set fault status for known drivers
  if (settings.status == ON) {
    settings.status = LOW;
  }
}

void DcDriver::updateStatus() {
  if (settings.status == LOW || settings.status == HIGH) {
    status.fault = digitalReadEx(pins.fault) == settings.status;
  }
}

DriverStatus DcDriver::getStatus() {
  return status;
}

// power down using the enable pin
void DcDriver::power(bool state) {
  powered = state;
  if (!powered) { digitalWriteF(pins.enable, !pins.enabledState); } else { digitalWriteF(pins.enable, pins.enabledState); }
 }

// power level to the motor (0.0 to 1.0 = 0..100%)
void DcDriver::motorPower(float power) {
  if (!powered) dcPower = 0.0F; else dcPower = power;
  update();
 }

// motor direction (DIR_FORMWARD or DIR_REVERSE)
void DcDriver::motorDirection(Direction dir) {
  dcDirection = dir;
  update();
}

void DcDriver::update() {
  long p = dcPower*255;

  if (settings.model == SERVO_II) {
    if (dcDirection == DIR_FORWARD) {
      digitalWriteF(pins.in1, pins.inState1);
      if (pins.inState2 == HIGH) p = 255 - p;
      analogWrite(pins.in2, p);
    } else
    if (dcDirection == DIR_REVERSE) {
      if (pins.inState1 == HIGH) p = 255 - p;
      analogWrite(pins.in1, p);
      digitalWriteF(pins.in2, pins.inState2);
    } else {
      digitalWriteF(pins.in1, pins.inState1);
      digitalWriteF(pins.in2, pins.inState2);
    }
  } else
  if (settings.model == SERVO_DP) {
    if (dcDirection == DIR_FORWARD) {
      digitalWriteF(pins.in1, pins.inState1);
      if (pins.inState2 == HIGH) p = 255 - p;
      analogWrite(pins.in2, p);
    } else
    if (dcDirection == DIR_REVERSE) {
      digitalWriteF(pins.in1, !pins.inState1);
      if (pins.inState2 == HIGH) p = 255 - p;
      analogWrite(pins.in2, p);
    } else {
      digitalWriteF(pins.in1, pins.inState1);
      digitalWriteF(pins.in2, pins.inState2);
    }
  }

#endif