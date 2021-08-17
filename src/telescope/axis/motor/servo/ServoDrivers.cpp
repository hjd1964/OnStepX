// -----------------------------------------------------------------------------------
// servo driver control

#include "ServoDrivers.h"

#ifdef SERVO_DRIVER_PRESENT

#if DEBUG_MODE != OFF
  const char* SERVO_DRIVER_NAME[DRIVER_SERVO_MODEL_COUNT] = { "SERVO_DIR_PHASE", "SERVO_IN_IN" };
#endif

extern const int AnalogRange; // help for analogWrite() range conversions

#ifdef AXIS1_SERVO
  const ServoDriverPins PinsAxis1 = { AXIS1_SERVO_PH1_PIN, AXIS1_SERVO_PH1_STATE, AXIS1_SERVO_PH2_PIN, AXIS1_SERVO_PH2_STATE, AXIS1_ENABLE_PIN, AXIS1_ENABLE_STATE, AXIS1_FAULT_PIN };
  const ServoDriverSettings SettingsAxis1 = { AXIS1_DRIVER_MODEL, AXIS1_SERVO_P, AXIS1_SERVO_I, AXIS1_SERVO_D, AXIS1_DRIVER_STATUS };
  ServoDriver servoDriver1(1, &PinsAxis1, &SettingsAxis1);
#endif
#ifdef AXIS2_SERVO
  const ServoDriverPins PinsAxis2 = { AXIS2_SERVO_PH1_PIN, AXIS2_SERVO_PH1_STATE, AXIS2_SERVO_PH2_PIN, AXIS2_SERVO_PH2_STATE, AXIS2_ENABLE_PIN, AXIS2_ENABLE_STATE, AXIS2_FAULT_PIN };
  const ServoDriverSettings SettingsAxis2 = { AXIS2_DRIVER_MODEL, AXIS2_SERVO_P, AXIS2_SERVO_I, AXIS2_SERVO_D, AXIS2_DRIVER_STATUS };
  ServoDriver servoDriver2(2, &PinsAxis2, &SettingsAxis2);
#endif
#ifdef AXIS3_SERVO
  const ServoDriverPins PinsAxis3 = { AXIS3_SERVO_PH1_PIN, AXIS3_SERVO_PH1_STATE, AXIS3_SERVO_PH2_PIN, AXIS3_SERVO_PH2_STATE, AXIS3_ENABLE_PIN, AXIS3_ENABLE_STATE, AXIS3_FAULT_PIN };
  const ServoDriverSettings SettingsAxis3 = { AXIS3_DRIVER_MODEL, AXIS3_SERVO_P, AXIS3_SERVO_I, AXIS3_SERVO_D, AXIS3_DRIVER_STATUS };
  ServoDriver servoDriver3(3, &PinsAxis3, &SettingsAxis3);
#endif
#ifdef AXIS4_SERVO
  const ServoDriverPins PinsAxis4 = { AXIS4_SERVO_PH1_PIN, AXIS4_SERVO_PH1_STATE, AXIS4_SERVO_PH2_PIN, AXIS4_SERVO_PH2_STATE, AXIS4_ENABLE_PIN, AXIS4_ENABLE_STATE, AXIS4_FAULT_PIN };
  const ServoDriverSettings SettingsAxis4 = { AXIS4_DRIVER_MODEL, AXIS4_SERVO_P, AXIS4_SERVO_I, AXIS4_SERVO_D, AXIS4_DRIVER_STATUS };
  ServoDriver servoDriver4(4, &PinsAxis4, &SettingsAxis4);
#endif
#ifdef AXIS5_SERVO
  const ServoDriverPins PinsAxis5 = { AXIS5_SERVO_PH1_PIN, AXIS5_SERVO_PH1_STATE, AXIS5_SERVO_PH2_PIN, AXIS5_SERVO_PH2_STATE, AXIS5_ENABLE_PIN, AXIS5_ENABLE_STATE, AXIS5_FAULT_PIN };
  const ServoDriverSettings SettingsAxis5 = { AXIS5_DRIVER_MODEL, AXIS5_SERVO_P, AXIS5_SERVO_I, AXIS5_SERVO_D, AXIS5_DRIVER_STATUS };
  ServoDriver servoDriver5(5, &PinsAxis5, &SettingsAxis5);
#endif
#ifdef AXIS6_SERVO
  const ServoDriverPins PinsAxis6 = { AXIS6_SERVO_PH1_PIN, AXIS6_SERVO_PH1_STATE, AXIS6_SERVO_PH2_PIN, AXIS6_SERVO_PH2_STATE, AXIS6_ENABLE_PIN, AXIS6_ENABLE_STATE, AXIS6_FAULT_PIN };
  const ServoDriverSettings SettingsAxis6 = { AXIS6_DRIVER_MODEL, AXIS6_SERVO_P, AXIS6_SERVO_I, AXIS6_SERVO_D, AXIS6_DRIVER_STATUS };
  ServoDriver servoDriver6(6, &PinsAxis6, &SettingsAxis6);
#endif
#ifdef AXIS7_SERVO
  const ServoDriverPins PinsAxis7 = { AXIS7_SERVO_PH1_PIN, AXIS7_SERVO_PH1_STATE, AXIS7_SERVO_PH2_PIN, AXIS7_SERVO_PH2_STATE, AXIS7_ENABLE_PIN, AXIS7_ENABLE_STATE, AXIS7_FAULT_PIN };
  const ServoDriverSettings SettingsAxis7 = { AXIS7_DRIVER_MODEL, AXIS7_SERVO_P, AXIS7_SERVO_I, AXIS7_SERVO_D, AXIS7_DRIVER_STATUS };
  ServoDriver servoDriver7(7, &PinsAxis7, &SettingsAxis7);
#endif
#ifdef AXIS8_SERVO
  const ServoDriverPins PinsAxis8 = { AXIS8_SERVO_PH1_PIN, AXIS8_SERVO_PH1_STATE, AXIS8_SERVO_PH2_PIN, AXIS8_SERVO_PH2_STATE, AXIS8_ENABLE_PIN, AXIS8_ENABLE_STATE, AXIS8_FAULT_PIN };
  const ServoDriverSettings SettingsAxis8 = { AXIS8_DRIVER_MODEL, AXIS8_SERVO_P, AXIS8_SERVO_I, AXIS8_SERVO_D, AXIS8_DRIVER_STATUS };
  ServoDriver servoDriver8(8, &PinsAxis8, &SettingsAxis8);
#endif
#ifdef AXIS9_SERVO
  const ServoDriverPins PinsAxis9 = { AXIS9_SERVO_PH1_PIN, AXIS9_SERVO_PH1_STATE, AXIS9_SERVO_PH2_PIN, AXIS9_SERVO_PH2_STATE, AXIS9_ENABLE_PIN, AXIS9_ENABLE_STATE, AXIS9_FAULT_PIN };
  const ServoDriverSettings SettingsAxis9 = { AXIS9_DRIVER_MODEL, AXIS9_SERVO_P, AXIS9_SERVO_I, AXIS9_SERVO_D, AXIS9_DRIVER_STATUS };
  ServoDriver servoDriver9(9, &PinsAxis9, &SettingsAxis9);
#endif

ServoDriver::ServoDriver(uint8_t axisNumber, const ServoDriverPins *Pins, const ServoDriverSettings *Settings) {
  this->axisNumber = axisNumber;
  this->Pins = Pins;
  settings = *Settings;
}

void ServoDriver::init() {
  #if DEBUG == VERBOSE
    VF("MSG: Servo"); V(axisNumber); VF(", init model "); V(SERVO_DRIVER_NAME[settings.model - SERVO_DRIVER_FIRST]);
    VF(" p="); V(settings.p); VF(", i="); V(settings.i); VF(", d="); VL(settings.d);
  #endif

  // init default driver control pins
  pinModeEx(Pins->enable, OUTPUT);
  digitalWriteEx(Pins->enable, !Pins->enabledState);
  pinMode(Pins->in1, OUTPUT);
  digitalWriteF(Pins->in1, Pins->inState1); // either in1 or direction, state should default to inactive
  pinMode(Pins->in2, OUTPUT);
  digitalWriteF(Pins->in2, Pins->inState2); // either in2 or phase (PWM,) state should default to inactive

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

// power level to the motor (-AnalogRange to AnalogRange, negative for reverse)
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
      digitalWriteF(Pins->in1, Pins->inState1);
      if (Pins->inState2 == HIGH) motorPwr = AnalogRange - motorPwr;
      analogWrite(Pins->in2, motorPwr);
    } else
    if (motorDir == DIR_REVERSE) {
      if (Pins->inState1 == HIGH) motorPwr = AnalogRange - motorPwr;
      analogWrite(Pins->in1, motorPwr);
      digitalWriteF(Pins->in2, Pins->inState2);
    } else {
      digitalWriteF(Pins->in1, Pins->inState1);
      digitalWriteF(Pins->in2, Pins->inState2);
    }
  } else
  if (settings.model == SERVO_PD) {
    if (motorDir == DIR_FORWARD) {
      digitalWriteF(Pins->in2, Pins->inState2);
      if (Pins->inState1 == HIGH) motorPwr = AnalogRange - motorPwr;
      analogWrite(Pins->in1, motorPwr);
    } else
    if (motorDir == DIR_REVERSE) {
      digitalWriteF(Pins->in2, !Pins->inState2);
      if (Pins->inState1 == HIGH) motorPwr = AnalogRange - motorPwr;
      analogWrite(Pins->in1, motorPwr);
    } else {
      digitalWriteF(Pins->in1, Pins->inState1);
      digitalWriteF(Pins->in2, Pins->inState2);
    }
  }
}

#endif