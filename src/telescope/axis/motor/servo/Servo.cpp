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
  ServoMotor motor1;
  Encoder axis1Enc(AXIS1_ENC_A_PIN, AXIS1_ENC_B_PIN);
  IRAM_ATTR void moveAxis1() { motor1.move(); }
#endif
#ifdef AXIS2_DRIVER_SERVO
  ServoMotor motor2;
  Encoder axis2Enc(AXIS2_ENC_A_PIN, AXIS2_ENC_B_PIN);
  IRAM_ATTR void moveAxis2() { motor2.move(); }
#endif
#ifdef AXIS3_DRIVER_SERVO
  ServoMotor motor3;
  Encoder axis3Enc(AXIS3_ENC_A_PIN, AXIS3_ENC_B_PIN);
  IRAM_ATTR void moveAxis3() { motor3.move(); }
#endif
#ifdef AXIS4_DRIVER_SERVO
  ServoMotor motor4;
  Encoder axis4Enc(AXIS4_ENC_A_PIN, AXIS4_ENC_B_PIN);
  IRAM_ATTR void moveAxis4() { motor4.move(); }
#endif
#ifdef AXIS5_DRIVER_SERVO
  ServoMotor motor5;
  Encoder axis5Enc(AXIS5_ENC_A_PIN, AXIS5_ENC_B_PIN);
  IRAM_ATTR void moveAxis5() { motor5.move(); }
#endif
#ifdef AXIS6_DRIVER_SERVO
  ServoMotor motor6;
  Encoder axis6Enc(AXIS6_ENC_A_PIN, AXIS6_ENC_B_PIN);
  IRAM_ATTR void moveAxis6() { motor6.move(); }
#endif
#ifdef AXIS7_DRIVER_SERVO
  ServoMotor motor7;
  Encoder axis7Enc(AXIS7_ENC_A_PIN, AXIS7_ENC_B_PIN);
  IRAM_ATTR void moveAxis7() { motor7.move(); }
#endif
#ifdef AXIS8_DRIVER_SERVO
  ServoMotor motor8;
  Encoder axis8Enc(AXIS8_ENC_A_PIN, AXIS8_ENC_B_PIN);
  IRAM_ATTR void moveAxis8() { motor8.move(); }
#endif
#ifdef AXIS9_DRIVER_SERVO
  ServoMotor motor9;
  Encoder axis9Enc(AXIS9_ENC_A_PIN, AXIS9_ENC_B_PIN);
  IRAM_ATTR void moveAxis9() { motor9.move(); }
#endif

bool ServoMotor::init(uint8_t axisNumber, int8_t reverse, int16_t integral, int16_t porportional) {
  axisPrefix[10] = '0' + axisNumber;
  this->axisNumber = axisNumber;
  driverType = SERVO;

  taskHandle = 0;
  #ifdef AXIS1_DRIVER_DC
    if (axisNumber == 1) { enc = &axis1Enc; _move = moveAxis1; }
  #endif
  #ifdef AXIS2_DRIVER_SERVO
    if (axisNumber == 2) { enc = &axis2Enc; _move = moveAxis2; }
  #endif
  #ifdef AXIS3_DRIVER_SERVO
    if (axisNumber == 3) { enc = &axis3Enc; _move = moveAxis3; }
  #endif
  #ifdef AXIS4_DRIVER_SERVO
    if (axisNumber == 4) { enc = &axis4Enc; _move = moveAxis4; }
  #endif
  #ifdef AXIS5_DRIVER_SERVO
    if (axisNumber == 5) { enc = &axis5Enc; _move = moveAxis5; }
  #endif
  #ifdef AXIS6_DRIVER_SERVO
    if (axisNumber == 6) { enc = &axis6Enc; _move = moveAxis6; }
  #endif
  #ifdef AXIS7_DRIVER_SERVO
    if (axisNumber == 7) { enc = &axis7Enc; _move = moveAxis7; }
  #endif
  #ifdef AXIS8_DRIVER_SERVO
    if (axisNumber == 8) { enc = &axis8Enc; _move = moveAxis8; }
  #endif
  #ifdef AXIS9_DRIVER_SERVO
    if (axisNumber == 9) { enc = &axis9Enc; _move = moveAxis9; }
  #endif

  // make sure there is something to do
  if (_move == NULL) { V(axisPrefix); VF("nothing to do exiting!"); return false; }

  // init driver advanced modes, etc.
  driver.init(axisNumber);

  // now disable the driver
  power(false);

  // start the motor timer
  V(axisPrefix); VF("start task to move motor... ");
  char timerName[] = "Motor_";
  timerName[5] = '0' + axisNumber;
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

// set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
void ServoMotor::setFrequencySteps(float frequency) {
  // negative frequency, convert to positive and reverse the direction
  int dir = 0;
  if (frequency > 0.0F) dir = 1; else if (frequency < 0.0F) { frequency = -frequency; dir = -1; }

  // if in backlash override the frequency
  if (inBacklash) frequency = backlashFrequency;

  if (frequency != currentFrequency) {
    lastFrequency = frequency;

    // frequency in steps per second to period in microsecond counts per step
    // also runs the timer twice as fast if using a square wave
    #if STEP_WAVE_FORM == SQUARE
      float period = 500000.0F/frequency;
    #else
      float period = 1000000.0F/frequency;
    #endif
   
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
    if (step != dir) step = 0;
    tasks.setPeriodSubMicros(taskHandle, lastPeriod);
  }

  noInterrupts();
  step = dir;
  interrupts();
}

float ServoMotor::getFrequencySteps() {
  if (lastPeriod == 0) return 0;
  return 16000000.0F/lastPeriod;
}

IRAM_ATTR void ServoMotor::move() {
  if (synchronized && !inBacklash) targetSteps += step;
  if (motorSteps > targetSteps) {
    direction = DIR_REVERSE;
    if (backlashSteps > 0) { inBacklash = true; backlashSteps--; } else { inBacklash = false; motorSteps--; }
  } else 
  if (motorSteps < targetSteps || inBacklash) {
    direction = DIR_FORWARD;
    if (backlashSteps < backlashAmountSteps) { inBacklash = true; backlashSteps++; } else { inBacklash = false; motorSteps++; }
  } else { direction = DIR_NONE; return; }
}

#endif
