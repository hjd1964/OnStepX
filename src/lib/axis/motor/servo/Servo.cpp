// -----------------------------------------------------------------------------------
// axis servo motor

#include "Servo.h"

#ifdef SERVO_MOTOR_PRESENT

#include "../../../tasks/OnTask.h"
#include "../Motor.h"

ServoMotor *servoMotorInstance[9];
IRAM_ATTR void moveServoMotorAxis1() { servoMotorInstance[0]->move(); }
IRAM_ATTR void moveServoMotorAxis2() { servoMotorInstance[1]->move(); }
IRAM_ATTR void moveServoMotorAxis3() { servoMotorInstance[2]->move(); }
IRAM_ATTR void moveServoMotorAxis4() { servoMotorInstance[3]->move(); }
IRAM_ATTR void moveServoMotorAxis5() { servoMotorInstance[4]->move(); }
IRAM_ATTR void moveServoMotorAxis6() { servoMotorInstance[5]->move(); }
IRAM_ATTR void moveServoMotorAxis7() { servoMotorInstance[6]->move(); }
IRAM_ATTR void moveServoMotorAxis8() { servoMotorInstance[7]->move(); }
IRAM_ATTR void moveServoMotorAxis9() { servoMotorInstance[8]->move(); }

// constructor
ServoMotor::ServoMotor(uint8_t axisNumber, ServoDriver *Driver, Encoder *encoder, Feedback *feedback, ServoControl *control, int16_t syncThreshold, bool useFastHardwareTimers) {
  if (axisNumber < 1 || axisNumber > 9) return;

  driverType = SERVO;
  strcpy(axisPrefix, "MSG: Servo_, ");
  axisPrefix[10] = '0' + axisNumber;
  this->axisNumber = axisNumber;
  this->encoder = encoder;
  this->feedback = feedback;
  this->control = control;
  this->syncThreshold = syncThreshold;

  if (axisNumber > 2) useFastHardwareTimers = false;
  this->useFastHardwareTimers = useFastHardwareTimers;

  this->driver = Driver;

  encoder->init();

  feedback->getDefaultParameters(&default_param1, &default_param2, &default_param3, &default_param4, &default_param5, &default_param6);

  // attach the function pointers to the callbacks
  servoMotorInstance[axisNumber - 1] = this;
  switch (axisNumber) {
    case 1: callback = moveServoMotorAxis1; break;
    case 2: callback = moveServoMotorAxis2; break;
    case 3: callback = moveServoMotorAxis3; break;
    case 4: callback = moveServoMotorAxis4; break;
    case 5: callback = moveServoMotorAxis5; break;
    case 6: callback = moveServoMotorAxis6; break;
    case 7: callback = moveServoMotorAxis7; break;
    case 8: callback = moveServoMotorAxis8; break;
    case 9: callback = moveServoMotorAxis9; break;
  }

  // get the feedback control loop ready
  feedback->init(axisNumber, control, driver->getMotorControlRange());
}

bool ServoMotor::init() {
  if (axisNumber < 1 || axisNumber > 9) return false;

  driver->init();
  enable(false);

  // start the motion timer
  V(axisPrefix);
  VF("start task to track motion... ");
  char timerName[] = "Target_";
  timerName[6] = '0' + axisNumber;
  taskHandle = tasks.add(0, 0, true, 0, callback, timerName);
  if (taskHandle) {
    V("success");
    if (useFastHardwareTimers && !tasks.requestHardwareTimer(taskHandle, 0)) { VLF(" (no hardware timer!)"); } else { VLF(""); }
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

// set driver parameters
void ServoMotor::setParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  feedback->setParameters(param1, param2, param3, param4, param5, param6);
}

// validate driver parameters
bool ServoMotor::validateParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  return feedback->validateParameters(param1, param2, param3, param4, param5, param6);
}

// sets motor enable on/off (if possible)
void ServoMotor::enable(bool state) {
  V(axisPrefix); VF("driver powered ");
  if (state) { VLF("up"); } else { VLF("down"); } 

  if (!state) feedback->reset();
  driver->enable(state);
  enabled = state;
}

// get the associated driver status
DriverStatus ServoMotor::getDriverStatus() {
  driver->updateStatus();
  return driver->getStatus();
}

// resets motor and target angular position in steps, also zeros backlash and index
void ServoMotor::resetPositionSteps(long value) {
  if (syncThreshold == OFF) {
    Motor::resetPositionSteps(value);
    encoder->write(value);
  } else {
    // disregard any home position, the absolute encoders are the final authority
    Motor::resetPositionSteps(encoder->read());
    D(axisPrefix);
    DL("absolute encoder ignored reset position");
  }
}

// get instrument coordinate, in steps
long ServoMotor::getInstrumentCoordinateSteps() {
  return encoder->read() + indexSteps;
}

// set instrument coordinate, in steps
void ServoMotor::setInstrumentCoordinateSteps(long value) {
  noInterrupts();
  long i = value - motorSteps;
  interrupts();
  if (syncThreshold == OFF || syncThreshold >= i) {
    indexSteps = i;
  } else {
    D(axisPrefix);
    DL("absolute encoder ignored sync exceeds threshold");
  }
}

// distance to target in steps (+/-)
long ServoMotor::getTargetDistanceSteps() {
  int32_t position = encoder->read();

  noInterrupts();
  long distance = targetSteps - position;
  interrupts();

  return distance;
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
  if (lastPeriod == 0) return 0;
  return (16000000.0F / lastPeriod) * absStep;
}

// set slewing state (hint that we are about to slew or are done slewing)
void ServoMotor::setSlewing(bool state) {
  slewing = state;
}

// updates PID and sets servo motor power/direction
void ServoMotor::poll() {
  int32_t position = encoder->read();

  noInterrupts();
  long target = motorSteps + backlashSteps;
  interrupts();

  control->set = target;
  control->in = position;
  feedback->poll();

  float velocity = control->out;
  if (!enabled) velocity = 0.0F;
  float velocityPercent = (velocity/driver->getMotorControlRange()) * 100.0F;

  // for range 0% tracking params to >= 25% for slewing params
  feedback->variableParameters(fabs(velocityPercent*4.0F));

  // if we're not moving "fast" and the motor is above 70% power something is seriously wrong, so shut it down
  if (millis() - lastCheckTime > 1000) {
    if (labs(position - lastPosition) < 10 && abs(velocityPercent) >= 70) {
      D(axisPrefix);
      D("stall detected!");
      D(" control->in = "); D(control->in);
      D(", control->set = "); D(control->set);
      D(", control->out = "); D(control->out);
      D(", velocity % = "); DL(velocityPercent);
      enable(false);
    }
    lastPosition = position;
    lastCheckTime = millis();
  }

  #if DEBUG != OFF && defined(DEBUG_SERVO) && DEBUG_SERVO != OFF
    if (axisNumber == DEBUG_SERVO) {
      static uint16_t count = 0;
      count++;
      if (count % 25 == 0) {
        char s[80];
        sprintf(s, "Servo%d_Delta: %6ld, Servo%d_Power: %6.3f%%\r\n", (int)axisNumber, (target - position), (int)axisNumber, velocityPercent * 10.0F);
        D(s);
      }
    }
  #endif

  driver->setMotorVelocity(velocity);
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
