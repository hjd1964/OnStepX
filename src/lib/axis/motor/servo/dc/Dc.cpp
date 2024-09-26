// -----------------------------------------------------------------------------------
// axis servo DC motor driver

#include "Dc.h"

#ifdef SERVO_DC_PRESENT

#if defined(ARDUINO_TEENSY41) && defined(AXIS1_STEP_PIN) && AXIS1_STEP_PIN == 38 && defined(SERVO_ANALOG_WRITE_FREQUENCY)
  // this is only for pin 38 of a Teensy4.1
  IntervalTimer itimer4;
  uint16_t _pwm38_period = 0;
  uint8_t _pwm38_toggle = 0;
  float _base_freq_divider = SERVO_ANALOG_WRITE_FREQUENCY/(1.0F/(ANALOG_WRITE_RANGE/1000000.0F));

  void PWM38_HWTIMER() {
    if (_pwm38_period == 0 || _pwm38_period == ANALOG_WRITE_RANGE) {
      itimer4.update(ANALOG_WRITE_RANGE/_base_freq_divider);
      digitalWriteF(38, _pwm38_period == ANALOG_WRITE_RANGE);
    } else {
      if (!_pwm38_toggle) {
        itimer4.update(_pwm38_period/_base_freq_divider);
        digitalWriteF(38, LOW);
      } else {
        itimer4.update((ANALOG_WRITE_RANGE - _pwm38_period)/_base_freq_divider);
        digitalWriteF(38, HIGH);
      }
    }
    _pwm38_toggle = !_pwm38_toggle;
  }
  #define analogWritePin38(x) _pwm38_period = x
#endif

ServoDc::ServoDc(uint8_t axisNumber, const ServoDcPins *Pins, const ServoDcSettings *Settings) {
  this->axisNumber = axisNumber;

  this->Pins = Pins;
  enablePin = Pins->enable;
  enabledState = Pins->enabledState;
  faultPin = Pins->fault;

  this->Settings = Settings;
  model = Settings->model;
  statusMode = Settings->status;
  velocityMax = (Settings->velocityMax/100.0F)*SERVO_ANALOG_WRITE_RANGE;
  acceleration = (Settings->acceleration/100.0F)*velocityMax;
  accelerationFs = acceleration/FRACTIONAL_SEC;
}

void ServoDc::init() {
  ServoDriver::init();

  // show velocity control settings
  VF("MSG: ServoDriver"); V(axisNumber); VF(", Vmax="); V(Settings->velocityMax); VF("% power, Acceleration="); V(Settings->acceleration); VLF("%/s/s");
  VF("MSG: ServoDriver"); V(axisNumber); VF(", AccelerationFS="); V(accelerationFs); VLF("%/s/fs");

  #if DEBUG == VERBOSE
    VF("MSG: ServoDriver"); V(axisNumber);
    if (model == SERVO_EE) {
      V(", pins pwm1="); if (Pins->in1 == OFF) VF("OFF"); else V(Pins->in1);
      V(", pwm2="); if (Pins->in2 == OFF) VLF("OFF"); else VL(Pins->in2);
    } else
    if (model == SERVO_PE) {
      V(", pins dir="); if (Pins->in1 == OFF) VF("OFF"); else V(Pins->in1);
      V(", pwm="); if (Pins->in2 == OFF) VLF("OFF"); else VL(Pins->in2);
    }
  #endif

  // init default driver control pins
  pinModeEx(Pins->in1, OUTPUT);
  digitalWriteF(Pins->in1, Pins->inState1); // either in1 or direction, state should default to inactive
  pinModeEx(Pins->in2, OUTPUT);
  digitalWriteF(Pins->in2, Pins->inState2); // either in2 or phase (PWM,) state should default to inactive

  // if this is a T4.1 and we're using a PE driver and in2 == 38, assume its a MaxPCB4 and make our own PWM on that pin
  #ifdef analogWritePin38
    if (model == SERVO_PE && Pins->in2 == 38) {
      itimer4.priority(0);
      itimer4.begin(PWM38_HWTIMER, 100);
      VF("MSG: ServoDriver"); V(axisNumber); VLF(", emulating PWM on pin 38");
    }
  #endif

  // set PWM frequency
  #ifdef SERVO_ANALOG_WRITE_FREQUENCY
    VF("MSG: Servo"); V(axisNumber); VF(", setting control pins analog frequency "); VL(SERVO_ANALOG_WRITE_FREQUENCY);
    #ifndef analogWritePin38
      analogWriteFrequency(Pins->in1, SERVO_ANALOG_WRITE_FREQUENCY);
    #endif
    analogWriteFrequency(Pins->in2, SERVO_ANALOG_WRITE_FREQUENCY);
  #endif

  // set PWM bits
  #ifdef SERVO_ANALOG_WRITE_RESOLUTION
    VF("MSG: Servo"); V(axisNumber); VF(", setting control pins analog bits "); VL(SERVO_ANALOG_WRITE_RESOLUTION);
    analogWriteResolution(Pins->in2, SERVO_ANALOG_WRITE_RESOLUTION);
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

// enable or disable the driver using the enable pin or other method
void ServoDc::enable(bool state) {
  int32_t power = 0;

  enabled = state;
  if (enablePin == SHARED) {
    VF("MSG: ServoDriver"); V(axisNumber);
    VF(", powered "); if (state) { VF("up"); } else { VF("down"); } VLF(" using PE or EE signals");

    if (!enabled) {
      if (model == SERVO_EE) {
        if (Pins->inState1 == HIGH) analogWrite(Pins->in1, SERVO_ANALOG_WRITE_RANGE); else analogWrite(Pins->in1, 0);
        if (Pins->inState2 == HIGH) analogWrite(Pins->in2, SERVO_ANALOG_WRITE_RANGE); else analogWrite(Pins->in2, 0);
      } else
      if (model == SERVO_PE) {
        digitalWriteF(Pins->in1, Pins->inState1);
        if (Pins->inState2 == HIGH) power = SERVO_ANALOG_WRITE_RANGE; else power = 0; 
        #ifdef analogWritePin38
          if (Pins->in2 == 38) analogWritePin38(round(power)); else
        #endif
        analogWrite(Pins->in2, round(power));
      }
    }
  } else {
    VF("MSG: ServoDriver"); V(axisNumber);
    VF(", powered "); if (state) { VF("up"); } else { VF("down"); } VLF(" using enable pin");
    if (!enabled) { digitalWriteF(enablePin, !enabledState); } else { digitalWriteF(enablePin, enabledState); }
  }

  ServoDriver::updateStatus();
}

// set motor velocity by adjusting power (0 to SERVO_ANALOG_WRITE_RANGE for 0 to 100% power)
float ServoDc::setMotorVelocity(float velocity) {
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
  if (currentVelocity >= 0) motorDirection = DIR_FORWARD; else motorDirection = DIR_REVERSE;

  pwmUpdate(fabs(currentVelocity));
  return currentVelocity;
}

// motor control update
void ServoDc::pwmUpdate(float power) {
  if (!enabled) return;

  if (model == SERVO_EE) {
    if (motorDirection == DIR_FORWARD) {
      if (Pins->inState1 == HIGH) analogWrite(Pins->in1, SERVO_ANALOG_WRITE_RANGE); else analogWrite(Pins->in1, 0);
      if (Pins->inState2 == HIGH) power = SERVO_ANALOG_WRITE_RANGE - power;
      analogWrite(Pins->in2, round(power));
    } else
    if (motorDirection == DIR_REVERSE) {
      if (Pins->inState1 == HIGH) power = SERVO_ANALOG_WRITE_RANGE - power;
      analogWrite(Pins->in1, round(power));
      if (Pins->inState2 == HIGH) analogWrite(Pins->in2, SERVO_ANALOG_WRITE_RANGE); else analogWrite(Pins->in2, 0);
    } else {
      if (Pins->inState1 == HIGH) analogWrite(Pins->in1, SERVO_ANALOG_WRITE_RANGE); else analogWrite(Pins->in1, 0);
      if (Pins->inState2 == HIGH) analogWrite(Pins->in2, SERVO_ANALOG_WRITE_RANGE); else analogWrite(Pins->in2, 0);
    }
  } else
  if (model == SERVO_PE) {
    if (motorDirection == DIR_FORWARD) {
      digitalWriteF(Pins->in1, Pins->inState1);
      if (Pins->inState2 == HIGH) power = SERVO_ANALOG_WRITE_RANGE - power;
    } else
    if (motorDirection == DIR_REVERSE) {
      digitalWriteF(Pins->in1, !Pins->inState1);
      if (Pins->inState2 == HIGH) power = SERVO_ANALOG_WRITE_RANGE - power;
    } else {
      digitalWriteF(Pins->in1, Pins->inState1);
      if (Pins->inState2 == HIGH) power = SERVO_ANALOG_WRITE_RANGE; else power = 0;
    }
    #ifdef analogWritePin38
      if (Pins->in2 == 38) analogWritePin38(round(power)); else
    #endif
    analogWrite(Pins->in2, round(power));
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