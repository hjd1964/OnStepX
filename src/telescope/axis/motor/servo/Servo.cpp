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

PidControl pidControl[9];

#ifdef AXIS1_DRIVER_SERVO
  ServoMotor motor1;
  PID pidAxis1(&pidControl[0].in, &pidControl[0].out, &pidControl[0].set, AXIS1_DRIVER_SERVO_P, AXIS1_DRIVER_SERVO_I, AXIS1_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis1(AXIS1_ENC_A_PIN, AXIS1_ENC_B_PIN);
  IRAM_ATTR void moveAxis1() { motor1.move(); }
#endif
#ifdef AXIS2_DRIVER_SERVO
  ServoMotor motor2;
  PID pidAxis2(&pidControl[1].in, &pidControl[1].out, &pidControl[1].set, AXIS2_DRIVER_SERVO_P, AXIS2_DRIVER_SERVO_I, AXIS2_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis2(AXIS2_ENC_A_PIN, AXIS2_ENC_B_PIN);
  IRAM_ATTR void moveAxis2() { motor2.move(); }
#endif
#ifdef AXIS3_DRIVER_SERVO
  ServoMotor motor3;
  PID pidAxis3(&pidControl[2].in, &pidControl[2].out, &pidControl[2].set, AXIS3_DRIVER_SERVO_P, AXIS3_DRIVER_SERVO_I, AXIS3_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis3(AXIS3_ENC_A_PIN, AXIS3_ENC_B_PIN);
  IRAM_ATTR void moveAxis3() { motor3.move(); }
#endif
#ifdef AXIS4_DRIVER_SERVO
  ServoMotor motor4;
  PID pidAxis4(&pidControl[3].in, &pidControl[3].out, &pidControl[3].set, AXIS4_DRIVER_SERVO_P, AXIS4_DRIVER_SERVO_I, AXIS4_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis4(AXIS4_ENC_A_PIN, AXIS4_ENC_B_PIN);
  IRAM_ATTR void moveAxis4() { motor4.move(); }
#endif
#ifdef AXIS5_DRIVER_SERVO
  ServoMotor motor5;
  PID pidAxis5(&pidControl[4].in, &pidControl[4].out, &pidControl[4].set, AXIS5_DRIVER_SERVO_P, AXIS5_DRIVER_SERVO_I, AXIS5_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis5(AXIS5_ENC_A_PIN, AXIS5_ENC_B_PIN);
  IRAM_ATTR void moveAxis5() { motor5.move(); }
#endif
#ifdef AXIS6_DRIVER_SERVO
  ServoMotor motor6;
  PID pidAxis6(&pidControl[5].in, &pidControl[5].out, &pidControl[5].set, AXIS6_DRIVER_SERVO_P, AXIS6_DRIVER_SERVO_I, AXIS6_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis6(AXIS6_ENC_A_PIN, AXIS6_ENC_B_PIN);
  IRAM_ATTR void moveAxis6() { motor6.move(); }
#endif
#ifdef AXIS7_DRIVER_SERVO
  ServoMotor motor7;
  PID pidAxis7(&pidControl[6].in, &pidControl[6].out, &pidControl[6].set, AXIS7_DRIVER_SERVO_P, AXIS7_DRIVER_SERVO_I, AXIS7_DRIVER_SERVO_D, DIRECT);
  Encoder encAaxis7(AXIS7_ENC_A_PIN, AXIS7_ENC_B_PIN);
  IRAM_ATTR void moveAxis7() { motor7.move(); }
#endif
#ifdef AXIS8_DRIVER_SERVO
  ServoMotor motor8;
  PID pidAxis8(&pidControl[7].in, &pidControl[7].out, &pidControl[7].set, AXIS8_DRIVER_SERVO_P, AXIS8_DRIVER_SERVO_I, AXIS8_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis8(AXIS8_ENC_A_PIN, AXIS8_ENC_B_PIN);
  IRAM_ATTR void moveAxis8() { motor8.move(); }
#endif
#ifdef AXIS9_DRIVER_SERVO
  ServoMotor motor9;
  PID pidAxis9(&pidControl[8].in, &pidControl[8].out, &pidControl[8].set, AXIS9_DRIVER_SERVO_P, AXIS9_DRIVER_SERVO_I, AXIS9_DRIVER_SERVO_D, DIRECT);
  Encoder encAxis9(AXIS9_ENC_A_PIN, AXIS9_ENC_B_PIN);
  IRAM_ATTR void moveAxis9() { motor9.move(); }
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

bool ServoMotor::init(uint8_t axisNumber, int8_t reverse, int16_t integral, int16_t porportional) {
  axisPrefix[10] = '0' + axisNumber;
  this->axisNumber = axisNumber;
  driverType = SERVO;

  if (servoMonitorHandle == 0) {
    // start task to poll servos
    VF("MSG: ServoMotor, start monitor task (rate 5ms priority 1)... ");
    char timerName[] = "Servos_";
    timerName[6] = '0' + axisNumber;
    servoMonitorHandle = tasks.add(5, 0, true, 1, pollServos, timerName);
    if (servoMonitorHandle) { VL("success"); } else { VL("FAILED!"); return false; }
  }

  taskHandle = 0;
  #ifdef AXIS1_DRIVER_SERVO
    if (axisNumber == 1) { pid = &pidAxis1; enc = &encAxis1; _move = moveAxis1; }
  #endif
  #ifdef AXIS2_DRIVER_SERVO
    if (axisNumber == 2) { pid = &pidAxis2; enc = &encAxis2; _move = moveAxis2; }
  #endif
  #ifdef AXIS3_DRIVER_SERVO
    if (axisNumber == 3) { pid = &pidAxis3; enc = &encAxis3; _move = moveAxis3; }
  #endif
  #ifdef AXIS4_DRIVER_SERVO
    if (axisNumber == 4) { pid = &pidAxis4; enc = &encAxis4; _move = moveAxis4; }
  #endif
  #ifdef AXIS5_DRIVER_SERVO
    if (axisNumber == 5) { pid = &pidAxis5; enc = &encAxis5; _move = moveAxis5; }
  #endif
  #ifdef AXIS6_DRIVER_SERVO
    if (axisNumber == 6) { pid = &pidAxis6; enc = &encAxis6; _move = moveAxis6; }
  #endif
  #ifdef AXIS7_DRIVER_SERVO
    if (axisNumber == 7) { pid = &pidAxis7; enc = &encAxis7; _move = moveAxis7; }
  #endif
  #ifdef AXIS8_DRIVER_SERVO
    if (axisNumber == 8) { pid = &pidAxis8; enc = &encAxis8; _move = moveAxis8; }
  #endif
  #ifdef AXIS9_DRIVER_SERVO
    if (axisNumber == 9) { pid = &pidAxis9; enc = &encAxis9; _move = moveAxis9; }
  #endif

  // make sure there is something to do
  if (_move == NULL) { V(axisPrefix); VF("nothing to do exiting!"); return false; }

  // setup the PID
  pid->SetMode(AUTOMATIC);
  pid->SetSampleTime(10);
  pid->SetOutputLimits(-255, 255);
  if (reverse == ON) pid->SetControllerDirection(REVERSE);

  // init driver advanced modes, etc.
  driver.init(axisNumber);

  // now disable the driver
  power(false);

  // start the motor timer
  V(axisPrefix); VF("start task to move motor targets... ");
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
  driver.power(state);
}

// get the associated stepper drivers status
DriverStatus ServoMotor::getDriverStatus() {
  driver.updateStatus();
  return driver.getStatus();
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
  interrupts();
}

float ServoMotor::getFrequencySteps() {
  if (lastPeriod == 0) return 0;
  return 16000000.0F/lastPeriod;
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

  driver.setMotorPower(pidControl->out);
}

// sets dir as required and moves coord toward target at setFrequencySteps() rate
IRAM_ATTR void ServoMotor::move() {
  if (synchronized && !inBacklash) targetSteps += step;
  if (motorSteps > targetSteps) {
    if (backlashSteps > 0) { inBacklash = true; backlashSteps--; } else { inBacklash = false; motorSteps--; }
  } else 
  if (motorSteps < targetSteps || inBacklash) {
    if (backlashSteps < backlashAmountSteps) { inBacklash = true; backlashSteps++; } else { inBacklash = false; motorSteps++; }
  }
}

#endif
