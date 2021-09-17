// -----------------------------------------------------------------------------------
// axis servo driver motion

#include "Servo.h"

#ifdef SERVO_DRIVER_PRESENT

//#define DEBUG_SERVO

#include "../../../../tasks/OnTask.h"

#include "../../../Telescope.h"
#include "../Motor.h"

#ifdef AXIS1_SERVO
  #if AXIS1_SERVO_ENCODER == ENC_AB
    Encoder encAxis1(AXIS1_SERVO_ENC1_PIN, AXIS1_SERVO_ENC2_PIN);
  #else
    Encoder encAxis1(AXIS1_SERVO_ENC1_PIN, AXIS1_SERVO_ENC2_PIN, AXIS1_SERVO_ENCODER, AXIS1_SERVO_ENCODER_TRIGGER, &motor1.directionHint);
  #endif
  PidControl pca1;
  PID pidAxis1(&pca1.in, &pca1.out, &pca1.set, AXIS1_SERVO_P, AXIS1_SERVO_I, AXIS1_SERVO_D, DIRECT);
  IRAM_ATTR void moveAxis1() { motor1.move(); }
  ServoMotor motor1(1, &encAxis1, &pidAxis1, &pca1, &servoDriver1, moveAxis1);
#endif
#ifdef AXIS2_SERVO
  #if AXIS2_SERVO_ENCODER == ENC_AB
    Encoder encAxis2(AXIS2_SERVO_ENC1_PIN, AXIS2_SERVO_ENC2_PIN);
  #else
    Encoder encAxis2(AXIS2_SERVO_ENC1_PIN, AXIS2_SERVO_ENC2_PIN, AXIS2_SERVO_ENCODER, AXIS2_SERVO_ENCODER_TRIGGER, &motor2.directionHint);
  #endif
  PidControl pca2;
  PID pidAxis2(&pca2.in, &pca2.out, &pca2.set, AXIS2_SERVO_P, AXIS2_SERVO_I, AXIS2_SERVO_D, DIRECT);
  IRAM_ATTR void moveAxis2() { motor2.move(); }
  ServoMotor motor2(2, &encAxis2, &pidAxis2, &pca2, &servoDriver2, moveAxis2);
#endif
#ifdef AXIS3_SERVO
  #if AXIS3_SERVO_ENCODER == ENC_AB
    Encoder encAxis3(AXIS3_SERVO_ENC1_PIN, AXIS3_SERVO_ENC2_PIN);
  #else
    Encoder encAxis3(AXIS3_SERVO_ENC1_PIN, AXIS3_SERVO_ENC2_PIN, AXIS3_SERVO_ENCODER, AXIS3_SERVO_ENCODER_TRIGGER, &motor3.directionHint);
  #endif
  PidControl pca3;
  PID pidAxis3(&pca3.in, &pca3.out, &pca3.set, AXIS3_SERVO_P, AXIS3_SERVO_I, AXIS3_SERVO_D, DIRECT);
  void moveAxis3() { motor3.move(); }
  ServoMotor motor3(3, &encAxis3, &pidAxis3, &pca3, &servoDriver3, moveAxis3);
#endif
#ifdef AXIS4_SERVO
  #if AXIS4_SERVO_ENCODER == ENC_AB
    Encoder encAxis4(AXIS4_SERVO_ENC1_PIN, AXIS4_SERVO_ENC2_PIN);
  #else
    Encoder encAxis4(AXIS4_SERVO_ENC1_PIN, AXIS4_SERVO_ENC2_PIN, AXIS4_SERVO_ENCODER, AXIS4_SERVO_ENCODER_TRIGGER, &motor4.directionHint);
  #endif
  PidControl pca4;
  PID pidAxis4(&pca4.in, &pca4.out, &pca4.set, AXIS4_SERVO_P, AXIS4_SERVO_I, AXIS4_SERVO_D, DIRECT);
  void moveAxis4() { motor4.move(); }
  ServoMotor motor4(4, &encAxis4, &pidAxis4, &pca4, &servoDriver4, moveAxis4);
#endif
#ifdef AXIS5_SERVO
  #if AXIS5_SERVO_ENCODER == ENC_AB
    Encoder encAxis5(AXIS5_SERVO_ENC1_PIN, AXIS5_SERVO_ENC2_PIN);
  #else
    Encoder encAxis5(AXIS5_SERVO_ENC1_PIN, AXIS5_SERVO_ENC2_PIN, AXIS5_SERVO_ENCODER, AXIS5_SERVO_ENCODER_TRIGGER, &motor5.directionHint);
  #endif
  PidControl pca5;
  PID pidAxis5(&pca5.in, &pca5.out, &pca5.set, AXIS5_SERVO_P, AXIS5_SERVO_I, AXIS5_SERVO_D, DIRECT);
  void moveAxis5() { motor5.move(); }
  ServoMotor motor5(5, &encAxis5, &pidAxis5, pca5, &servoDriver5, moveAxis5);
#endif
#ifdef AXIS6_SERVO
  #if AXIS6_SERVO_ENCODER == ENC_AB
    Encoder encAxis6(AXIS6_SERVO_ENC1_PIN, AXIS6_SERVO_ENC2_PIN);
  #else
    Encoder encAxis6(AXIS6_SERVO_ENC1_PIN, AXIS6_SERVO_ENC2_PIN, AXIS6_SERVO_ENCODER, AXIS6_SERVO_ENCODER_TRIGGER, &motor6.directionHint);
  #endif
  PidControl pca6;
  PID pidAxis6(&pca6.in, &pca6.out, &pca6.set, AXIS6_SERVO_P, AXIS6_SERVO_I, AXIS6_SERVO_D, DIRECT);
  void moveAxis6() { motor6.move(); }
  ServoMotor motor6(6, &encAxis6, &pidAxis6, &pca6, &servoDriver6, moveAxis6);
#endif
#ifdef AXIS7_SERVO
  #if AXIS7_SERVO_ENCODER == ENC_AB
    Encoder encAxis7(AXIS7_SERVO_ENC1_PIN, AXIS7_SERVO_ENC2_PIN);
  #else
    Encoder encAxis7(AXIS7_SERVO_ENC1_PIN, AXIS7_SERVO_ENC2_PIN, AXIS7_SERVO_ENCODER, AXIS7_SERVO_ENCODER_TRIGGER, &motor7.directionHint);
  #endif
  PidControl pca7;
  PID pidAxis7(&pca7.in, &pca7.out, &pca7.set, AXIS7_SERVO_P, AXIS7_SERVO_I, AXIS7_SERVO_D, DIRECT);
  void moveAxis7() { motor7.move(); }
  ServoMotor motor7(7, &encAxis7, &pidAxis7, &pca7, &servoDriver7, moveAxis7);
#endif
#ifdef AXIS8_SERVO
  #if AXIS8_SERVO_ENCODER == ENC_AB
    Encoder encAxis8(AXIS8_SERVO_ENC1_PIN, AXIS8_SERVO_ENC2_PIN);
  #else
    Encoder encAxis8(AXIS8_SERVO_ENC1_PIN, AXIS8_SERVO_ENC2_PIN, AXIS8_SERVO_ENCODER, AXIS8_SERVO_ENCODER_TRIGGER, &motor8.directionHint);
  #endif
  PidControl pca8;
  PID pidAxis8(&pca8.in, &pca8.out, &pca8.set, AXIS8_SERVO_P, AXIS8_SERVO_I, AXIS8_SERVO_D, DIRECT);
  void moveAxis8() { motor8.move(); }
  ServoMotor motor8(8, &encAxis8, &pidAxis8, &pca8, &servoDriver8, moveAxis8);
#endif
#ifdef AXIS9_SERVO
  #if AXIS9_SERVO_ENCODER == ENC_AB
    Encoder encAxis9(AXIS9_SERVO_ENC1_PIN, AXIS9_SERVO_ENC2_PIN);
  #else
    Encoder encAxis9(AXIS9_SERVO_ENC1_PIN, AXIS9_SERVO_ENC2_PIN, AXIS9_SERVO_ENCODER, AXIS9_SERVO_ENCODER_TRIGGER, &motor9.directionHint);
  #endif
  PidControl pca9;
  PID pidAxis9(&pca9.in, &pca9.out, &pca9.set, AXIS9_SERVO_P, AXIS9_SERVO_I, AXIS9_SERVO_D, DIRECT);
  void moveAxis9() { motor9.move(); }
  ServoMotor motor9(9, &encAxis9, &pidAxis9, &pca9, &servoDriver9, moveAxis9);
#endif

inline void pollServos() {
  #ifdef AXIS1_SERVO
    motor1.poll(); Y;
  #endif
  #ifdef AXIS2_SERVO
    motor2.poll(); Y;
  #endif
  #ifdef AXIS3_SERVO
    motor3.poll(); Y;
  #endif
  #ifdef AXIS4_SERVO
    motor4.poll(); Y;
  #endif
  #ifdef AXIS5_SERVO
    motor5.poll(); Y;
  #endif
  #ifdef AXIS6_SERVO
    motor6.poll(); Y;
  #endif
  #ifdef AXIS7_SERVO
    motor7.poll(); Y;
  #endif
  #ifdef AXIS8_SERVO
    motor8.poll(); Y;
  #endif
  #ifdef AXIS9_SERVO
    motor9.poll(); Y;
  #endif
}

// constructor
ServoMotor::ServoMotor(uint8_t axisNumber, Encoder *enc, PID *pid, PidControl *pidControl, ServoDriver *driver, void (*volatile move)()) {
  axisPrefix[10] = '0' + axisNumber;
  this->axisNumber = axisNumber;
  this->enc = enc;
  this->pid = pid;
  this->pidControl = pidControl;
  this->_move = move;
  this->driver = driver;
  driverType = SERVO;
}

bool ServoMotor::init(int8_t reverse, int16_t integral, int16_t porportional) {
  // make sure there is something to do
  if (_move == NULL) { V(axisPrefix); VF("nothing to do exiting!"); return false; }

  if (servoMonitorHandle == 0) {
    // start task to poll servos
    VF("MSG: Motors, start servo monitor task (rate 5ms priority 1)... ");
    char timerName[] = "Servos_";
    timerName[6] = '0' + axisNumber;
    servoMonitorHandle = tasks.add(5, 0, true, 1, pollServos, timerName);
    if (servoMonitorHandle) { VL("success"); } else { VL("FAILED!"); return false; }
  }

  // setup the PID
  V(axisPrefix); VF("setting PID range +/-"); VL(AnalogRange);

  pidControl->in = 0;
  pidControl->set = 0;
  pid->SetSampleTime(10);
  pid->SetOutputLimits(-AnalogRange, AnalogRange);
  if (reverse == ON) pid->SetControllerDirection(REVERSE);
  pid->SetMode(AUTOMATIC);

  // init driver advanced modes, etc.
  driver->init();

  // now disable the driver
  power(false);

  // start the motor timer
  V(axisPrefix); VF("start task to move motor... ");
  char timerName[] = "Target_";
  timerName[6] = '0' + axisNumber;
  taskHandle = tasks.add(0, 0, true, 0, _move, timerName);
  if (taskHandle) {
    V("success");
    if (axisNumber <= 2) { if (!tasks.requestHardwareTimer(taskHandle, axisNumber, 0)) { VF(" (no hardware timer!)"); } }
    VL("");
  } else { VL("FAILED!"); return false; }

  return true;
}

// sets motor power on/off (if possible)
void ServoMotor::power(bool state) {
  driver->power(state);
}

// get the associated stepper drivers status
DriverStatus ServoMotor::getDriverStatus() {
  driver->updateStatus();
  return driver->getStatus();
}

// resets motor and target angular position in steps, also zeros backlash and index 
void ServoMotor::resetPositionSteps(long value) {
  Motor::resetPositionSteps(value);
  enc->write(motorSteps);
}

// set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
void ServoMotor::setFrequencySteps(float frequency) {
  // negative frequency, convert to positive and reverse the direction
  int dir = 0;
  if (frequency > 0.0F) dir = 1; else if (frequency < 0.0F) { frequency = -frequency; dir = -1; }

  // if in backlash override the frequency
  if (inBacklash) frequency = backlashFrequency;

  if (frequency != currentFrequency) {
    lastFrequency = frequency;

    // if slewing has a larger step size divide the frequency to account for it
    if (lastFrequency <= backlashFrequency*2.0F) stepSize = 1; else { if (!inBacklash) stepSize = 64; }
    frequency /= stepSize;

    // timer period in microseconds
    float period = 1000000.0F/frequency;

    // range is 0 to 134 seconds/step
    if (!isnan(period) && period <= 130000000.0F) {
      period *= 16.0F;
      lastPeriod = (unsigned long)lroundf(period);
    } else {
      lastPeriod = 0;
      frequency = 0.0F;
      dir = 0;
    }

    currentFrequency = frequency;

    // change the motor rate/direction
    noInterrupts();
    step = 0;
    interrupts();
    tasks.setPeriodSubMicros(taskHandle, lastPeriod);
  }

  noInterrupts();
  step = dir * stepSize;
  absStep = abs(step);
  interrupts();
}

float ServoMotor::getFrequencySteps() {
  if (lastPeriod == 0) return 0;
  return (16000000.0F/lastPeriod)*absStep;
}

// set slewing state (hint that we are about to slew or are done slewing)
void ServoMotor::setSlewing(bool state) {
  return;
}

// updates PID and sets servo motor power/direction
void ServoMotor::poll() {
  long position = enc->read();
  noInterrupts();
  long target = motorSteps + backlashSteps;
  interrupts();

  pidControl->set = target;
  pidControl->in = position;
  pid->Compute();

  #if DEBUG == VERBOSE && defined(DEBUG_SERVO)
    if (axisNumber == 1) {
      static uint16_t count = 0;
      count++;
      if (count%100 == 0) {
        char s[80];
        sprintf(s, "%sdelta = %6ld, power = %8.6f\r\n", axisPrefix, target - position, pidControl->out); V(s);
      }
    }
  #endif
  
  driver->setMotorPower(round(pidControl->out));

  #if AXIS1_SERVO_ENCODER == ENCODER_PULSE
    if (driver->getMotorDirection() == DIR_FORWARD) directionHint = 1; else directionHint = -1;
  #endif
}

// sets dir as required and moves coord toward target at setFrequencySteps() rate
IRAM_ATTR void ServoMotor::move() {
  if (synchronized && !inBacklash) targetSteps += step;
  if (motorSteps > targetSteps) {
    if (backlashSteps > 0) {
      backlashSteps -= absStep;
      inBacklash = backlashSteps > 0;
    } else {
      motorSteps -= absStep;
      inBacklash = false;
    }
  } else 
  if (motorSteps < targetSteps || inBacklash) {
    if (backlashSteps < backlashAmountSteps) {
      backlashSteps += absStep;
      inBacklash = backlashSteps < backlashAmountSteps;
    } else {
      motorSteps += absStep;
      inBacklash = false;
    }
  }
}

#endif
