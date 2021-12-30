// -----------------------------------------------------------------------------------
// axis servo motor

#include "Servo.h"

#ifdef SERVO_MOTOR_PRESENT

#include "../../../tasks/OnTask.h"
#include "../Motor.h"

// constructor
ServoMotor::ServoMotor(uint8_t axisNumber, Encoder *enc, Feedback *feedback, ServoDriver *driver, ServoControl *control) {
  axisPrefix[10] = '0' + axisNumber;
  this->axisNumber = axisNumber;
  this->enc = enc;
  this->feedback = feedback;
  this->control = control;
  this->driver = driver;
  driverType = SERVO;

  // get the feedback control loop ready
  feedback->init(axisNumber, control);
}

bool ServoMotor::init(void (*volatile move)(), void (*volatile moveFF)(), void (*volatile moveFR)()) {
  this->_move = move;
  UNUSED(moveFF);
  UNUSED(moveFR);

  // make sure there is something to do
  if (_move == NULL) {
    V(axisPrefix);
    VF("nothing to do exiting!");
    return false;
  }

  // get the driver ready
  driver->init();

  // now disable the driver
  power(false);

  // start the motor timer
  V(axisPrefix);
  VF("start task to move motor... ");
  char timerName[] = "Target_";
  timerName[6] = '0' + axisNumber;
  taskHandle = tasks.add(0, 0, true, 0, _move, timerName);
  if (taskHandle) {
    V("success");
    if (axisNumber <= 2) {
      if (!tasks.requestHardwareTimer(taskHandle, axisNumber, 0)) {
        VF(" (no hardware timer!)");
      }
    }
    VL("");
  } else {
    VLF("FAILED!");
    return false;
  }

  return true;
}

// set driver reverse state
void ServoMotor::setReverse(int8_t state) {
  feedback->setControlDirection(state);
}

// set default driver parameters
void ServoMotor::setParam(float param1, float param2, float param3, float param4, float param5, float param6) {
  feedback->setParam(param1, param2, param3, param4, param5, param6);
  setSlewing(isSlewing);
}

// validate driver parameters
bool ServoMotor::validateParam(float param1, float param2, float param3, float param4, float param5, float param6) {
  return feedback->validateParam(param1, param2, param3, param4, param5, param6);
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
  if (inBacklash)
    frequency = backlashFrequency;

  if (frequency != currentFrequency) {
    lastFrequency = frequency;

    // if slewing has a larger step size divide the frequency to account for it
    if (lastFrequency <= backlashFrequency * 2.0F) stepSize = 1; else { if (!inBacklash) stepSize = 64; }
    frequency /= stepSize;

    // timer period in microseconds
    float period = 1000000.0F / frequency;

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
  if (lastPeriod == 0)
    return 0;
  return (16000000.0F / lastPeriod) * absStep;
}

// set slewing state (hint that we are about to slew or are done slewing)
void ServoMotor::setSlewing(bool state) {
  isSlewing = state;
  feedback->selectAlternateParam(isSlewing);
}

// updates PID and sets servo motor power/direction
void ServoMotor::poll() {
  long position = enc->read();
  noInterrupts();
  long target = motorSteps + backlashSteps;
  interrupts();

  control->set = target;
  control->in = position;
  feedback->poll();

  #if DEBUG != OFF && defined(DEBUG_SERVO) && DEBUG_SERVO != OFF
    if (axisNumber == DEBUG_SERVO) {
      static uint16_t count = 0;
      count++;
      if (count % 100 == 0) {
        char s[80];
        sprintf(s, "Servo%d_Delta %6ld, Servo%d_Power %6.3f%%\r\n", (int)axisNumber, target - position, (int)axisNumber, (control->out / ANALOG_WRITE_PWM_RANGE) * 100.0F);
        D(s);
      }
    }
  #endif

  driver->setMotorPower(round(control->out));
  if (driver->getMotorDirection() == DIR_FORWARD) control->directionHint = 1; else control->directionHint = -1;
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
