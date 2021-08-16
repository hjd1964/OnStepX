// -----------------------------------------------------------------------------------
// axis servo driver motion

#include "Servo.h"

#ifdef SERVO_DRIVER_PRESENT

#include "../../../../tasks/OnTask.h"
extern Tasks tasks;
#include "../../../Telescope.h"
#include "../Motor.h"

#ifdef MOUNT_PRESENT
  extern unsigned long periodSubMicros;
#else
  #define periodSubMicros 1.0
#endif

#ifdef AXIS1_DRIVER_SERVO
  PidControl pca1;
  PID pidAxis1(&pca1.in, &pca1.out, &pca1.set, AXIS1_DRIVER_SERVO_P, AXIS1_DRIVER_SERVO_I, AXIS1_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis1(AXIS1_ENC_A_PIN, AXIS1_ENC_B_PIN);
  IRAM_ATTR void moveAxis1() { motor1.move(); }
  ServoMotor motor1(1, &pidAxis1, pca1, &encAxis1, &servoDriver1, moveAxis1);
#endif
#ifdef AXIS2_DRIVER_SERVO
  PidControl pca2;
  PID pidAxis2(&pca2.in, &pca2.out, &pca2.set, AXIS2_DRIVER_SERVO_P, AXIS2_DRIVER_SERVO_I, AXIS2_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis2(AXIS2_ENC_A_PIN, AXIS2_ENC_B_PIN);
  IRAM_ATTR void moveAxis2() { motor2.move(); }
  ServoMotor motor2(2, &pidAxis2, pca2, &encAxis2, &servoDriver2, moveAxis2);
#endif
#ifdef AXIS3_DRIVER_SERVO
  PidControl pca3;
  PID pidAxis3(&pca3.in, &pca3.out, &pca3.set, AXIS3_DRIVER_SERVO_P, AXIS3_DRIVER_SERVO_I, AXIS3_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis3(AXIS3_ENC_A_PIN, AXIS3_ENC_B_PIN);
  void moveAxis3() { motor3.move(); }
  ServoMotor motor3(3, &pidAxis3, pca3, &encAxis3, &servoDriver3, moveAxis3);
#endif
#ifdef AXIS4_DRIVER_SERVO
  PidControl pca4;
  PID pidAxis4(&pca4.in, &pca4.out, &pca4.set, AXIS4_DRIVER_SERVO_P, AXIS4_DRIVER_SERVO_I, AXIS4_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis4(AXIS4_ENC_A_PIN, AXIS4_ENC_B_PIN);
  void moveAxis4() { motor4.move(); }
  ServoMotor motor4(4, &pidAxis4, pca4, &encAxis4, &servoDriver4, moveAxis4);
#endif
#ifdef AXIS5_DRIVER_SERVO
  PidControl pca5;
  PID pidAxis5(&pca5.in, &pca5.out, &pca5.set, AXIS5_DRIVER_SERVO_P, AXIS5_DRIVER_SERVO_I, AXIS5_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis5(AXIS5_ENC_A_PIN, AXIS5_ENC_B_PIN);
  void moveAxis5() { motor5.move(); }
  ServoMotor motor5(5, &pidAxis5, pca5, &encAxis5, &servoDriver5, moveAxis5);
#endif
#ifdef AXIS6_DRIVER_SERVO
  PidControl pca6;
  PID pidAxis6(&pca6.in, &pca6.out, &pca6.set, AXIS6_DRIVER_SERVO_P, AXIS6_DRIVER_SERVO_I, AXIS6_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis6(AXIS6_ENC_A_PIN, AXIS6_ENC_B_PIN);
  void moveAxis6() { motor6.move(); }
  ServoMotor motor6(6, &pidAxis6, pca6, &encAxis6, &servoDriver6, moveAxis6);
#endif
#ifdef AXIS7_DRIVER_SERVO
  PidControl pca7;
  PID pidAxis7(&pca7.in, &pca7.out, &pca7.set, AXIS7_DRIVER_SERVO_P, AXIS7_DRIVER_SERVO_I, AXIS7_DRIVER_SERVO_D, DIRECT);
  Encoder encAaxis7(AXIS7_ENC_A_PIN, AXIS7_ENC_B_PIN);
  void moveAxis7() { motor7.move(); }
  ServoMotor motor7(7, &pidAxis7, pca7, &encAxis7, &servoDriver7, moveAxis7);
#endif
#ifdef AXIS8_DRIVER_SERVO
  PidControl pca8;
  PID pidAxis8(&pca8.in, &pca8.out, &pca8.set, AXIS8_DRIVER_SERVO_P, AXIS8_DRIVER_SERVO_I, AXIS8_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis8(AXIS8_ENC_A_PIN, AXIS8_ENC_B_PIN);
  void moveAxis8() { motor8.move(); }
  ServoMotor motor8(8, &pidAxis8, pca8, &encAxis8, &servoDriver8, moveAxis8);
#endif
#ifdef AXIS9_DRIVER_SERVO
  PidControl pca9;
  PID pidAxis9(&pca9.in, &pca9.out, &pca9.set, AXIS9_DRIVER_SERVO_P, AXIS9_DRIVER_SERVO_I, AXIS9_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis9(AXIS9_ENC_A_PIN, AXIS9_ENC_B_PIN);
  void moveAxis9() { motor9.move(); }
  ServoMotor motor9(9, &pidAxis9, pca9, &encAxis9, &servoDriver9, moveAxis9);
#endif

inline void pollServos() {
  #ifdef AXIS1_DRIVER_SERVO
    motor1.poll(); Y;
  #endif
  #ifdef AXIS2_DRIVER_SERVO
    motor2.poll(); Y;
  #endif
  #ifdef AXIS3_DRIVER_SERVO
    motor3.poll(); Y;
  #endif
  #ifdef AXIS4_DRIVER_SERVO
    motor4.poll(); Y;
  #endif
  #ifdef AXIS5_DRIVER_SERVO
    motor5.poll(); Y;
  #endif
  #ifdef AXIS6_DRIVER_SERVO
    motor6.poll(); Y;
  #endif
  #ifdef AXIS7_DRIVER_SERVO
    motor7.poll(); Y;
  #endif
  #ifdef AXIS8_DRIVER_SERVO
    motor8.poll(); Y;
  #endif
  #ifdef AXIS9_DRIVER_SERVO
    motor9.poll(); Y;
  #endif
}

// constructor
ServoMotor::ServoMotor(uint8_t axisNumber, PID *pid, PidControl pidControl, Encoder *enc, ServoDriver *driver, void (*volatile move)()) {
  axisPrefix[10] = '0' + axisNumber;
  this->axisNumber = axisNumber;
  this->pid = pid;
  this->pidControl = pidControl;
  this->enc = enc;
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
  pid->SetMode(AUTOMATIC);
  pid->SetSampleTime(10);
  pid->SetOutputLimits(-AnalogRange, AnalogRange);
  if (reverse == ON) pid->SetControllerDirection(REVERSE);

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
      // convert microsecond counts to sub-microsecond counts
      period *= 16.0F;

      // adjust period for MCU clock inaccuracy (signed 32bit numeric range covers about +/- 3% here)
      period *= (SIDEREAL_PERIOD/periodSubMicros);

      // remember the last active period
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

  pidControl.set = target;
  pidControl.in = position;
  pid->Compute();

  driver->setMotorPower(round(pidControl.out));
}

// sets dir as required and moves coord toward target at setFrequencySteps() rate
IRAM_ATTR void ServoMotor::move() {
  if (synchronized && !inBacklash) targetSteps += step;
  if (motorSteps > targetSteps) {
    if (backlashSteps > 0) { inBacklash = true; backlashSteps -= absStep; } else { inBacklash = false; motorSteps -= absStep; }
  } else 
  if (motorSteps < targetSteps || inBacklash) {
    if (backlashSteps < backlashAmountSteps) { inBacklash = true; backlashSteps += absStep; } else { inBacklash = false; motorSteps += absStep; }
  }
}

#endif
