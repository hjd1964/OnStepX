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
ServoMotor::ServoMotor(uint8_t axisNumber, int8_t reverse,
                       ServoDriver *Driver, Filter *filter,
                       Encoder *encoder, uint32_t encoderOrigin, bool encoderReverse,
                       Feedback *feedback, ServoControl *control,
                       long syncThreshold, bool useFastHardwareTimers)
                       :Motor(axisNumber, reverse) {
  if (axisNumber < 1 || axisNumber > 9) return;

  driverType = SERVO;

  strcpy(axisPrefix, " Axis_Servo, ");
  axisPrefix[5] = '0' + axisNumber;

  this->filter = filter;
  this->encoder = encoder;
  this->feedback = feedback;
  this->control = control;
  this->syncThreshold = syncThreshold;

  if (axisNumber > 2) useFastHardwareTimers = false;
  this->useFastHardwareTimers = useFastHardwareTimers;

  this->driver = Driver;

  this->encoderOrigin = encoderOrigin;
  this->encoderReverse = encoderReverse;
  this->encoderReverseDefault = encoderReverse;

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
}

bool ServoMotor::init() {
  if (!Motor::init()) return false;

  #ifdef CALIBRATE_SERVO_DC
    calibrateVelocity = new ServoCalibrateTrackingVelocity(axisNumber);
  #endif

  if (!encoder->init()) { DF("ERR:"); D(axisPrefix); DLF("no encoder!"); return false; }

  encoder->setOrigin(encoderOrigin);

  if (!driver->init(normalizedReverse)) { DF("ERR:"); D(axisPrefix); DLF("no motor driver!"); return false; }

  driver->enable(false);
  
  // get the feedback control loop ready
  feedback->init(axisNumber, control, driver->getMotorControlRange());
  feedback->reset();

  trackingFrequency = (AXIS1_STEPS_PER_DEGREE/240.0F)*SIDEREAL_RATIO_F;

  // start the motion timer
  VF("MSG:"); V(axisPrefix); VF("start task to synthesize motion... ");
  char timerName[] = "Ax_Svo";
  timerName[2] = '0' + axisNumber;
  taskHandle = tasks.add(0, 0, true, 0, callback, timerName);
  if (taskHandle) {
    VF("success");
    if (useFastHardwareTimers) {
      if (!tasks.requestHardwareTimer(taskHandle, 0)) {
        VF(" (no hardware timer!)");
      } else {
        maxFrequency = (1000000.0F/HAL_MAXRATE_LOWER_LIMIT)/2.0F;
      };
    }
    VLF("");
  } else {
    VLF("FAILED!");
    return false;
  }

  ready = true;
  return true;
}

void ServoMotor::setReverse(int8_t state) {
  if (!ready) return;

  feedback->setControlDirection(state);
  if (state == ON) encoderReverse = encoderReverseDefault; else encoderReverse = !encoderReverseDefault; 
}

void ServoMotor::enable(bool state) {
  if (!ready) return;

  driver->enable(state);
  if (state == false) feedback->reset(); else safetyShutdown = false;
  enabled = state;

  #ifdef CALIBRATE_SERVO_DC
    if (enabled && !encoder->isVirtual) calibrateVelocity->start(trackingFrequency, getInstrumentCoordinateSteps());
  #endif
}

DriverStatus ServoMotor::getDriverStatus() {
  if (!ready) return errorStatus;

  DriverStatus driverStatus;
  if (ready) { driver->updateStatus(); driverStatus = driver->getStatus(); } else driverStatus.fault = true;
  if (encoder->errorThresholdExceeded()) driverStatus.fault = true;
  if (safetyShutdown) driverStatus.fault = true;
  return driverStatus;
}

// resets motor and target angular position in steps, also zeros backlash and index
void ServoMotor::resetPositionSteps(long value) {
  if (!ready) return;

  Motor::resetPositionSteps(value);
  if (syncThreshold == OFF) {
    encoder->write(value);
  } else {
    VF("MSG:"); V(axisPrefix); VL("absolute encoder ignored reset position");
  }
}

// get instrument coordinate, in steps
long ServoMotor::getInstrumentCoordinateSteps() {
  if (!ready) return 0;

  return encoderRead() + indexSteps;
}

// set instrument coordinate, in steps
void ServoMotor::setInstrumentCoordinateSteps(long value) {
  noInterrupts();
  long i = value - motorSteps;
  interrupts();

  bool atHome = indexSteps == 0 && motorSteps == 0 && targetSteps == 0 && backlashSteps == 0;

  if (syncThreshold == OFF || atHome) {
    indexSteps = i;
    originIndexSteps = i;
    if (atHome) homeSet = true;
  } else {
    if (abs(originIndexSteps - i) < syncThreshold) {
      indexSteps = i;
    } else {
      VF("MSG:"); V(axisPrefix); VL("absolute encoder ignored sync exceeds threshold");
    }
  }
}

// distance to target in steps (+/-)
long ServoMotor::getTargetDistanceSteps() {
  if (!ready) return 0;

  long encoderCounts = encoderRead();
  noInterrupts();
  long dist = targetSteps - encoderCounts;
  interrupts();
  return dist;
}

// set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
void ServoMotor::setFrequencySteps(float frequency) {
  if (!ready) return;

  // negative frequency, convert to positive and reverse the direction
  int dir = 0;
  if (frequency > 0.0F) dir = 1; else if (frequency < 0.0F) { frequency = -frequency; dir = -1; }

  // if in backlash override the frequency
  if (inBacklash) frequency = backlashFrequency;

  if (frequency != currentFrequency) {
    // compensate for performace limitations by taking larger steps as needed
    if (frequency < maxFrequency) stepSize = 1; else
    if (frequency < maxFrequency*2) stepSize = 2; else
    if (frequency < maxFrequency*4) stepSize = 4; else
    if (frequency < maxFrequency*8) stepSize = 8; else
    if (frequency < maxFrequency*16) stepSize = 16; else
    if (frequency < maxFrequency*32) stepSize = 32; else
    if (frequency < maxFrequency*64) stepSize = 64; else
    if (frequency < maxFrequency*128) stepSize = 128; else stepSize = 256;

    // timer period in microseconds
    float period = 1000000.0F / (frequency/stepSize);

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
    currentDirection = dir;

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
  if (!ready) return 0;

  if (lastPeriod == 0) return 0;
  return (16000000.0F / lastPeriod) * absStep;
}

// set slewing state (hint that we are about to slew or are done slewing)
void ServoMotor::setSlewing(bool state) {
  if (!ready) return;

  slewing = state;
}

// set zero/origin of absolute encoders
uint32_t ServoMotor::encoderZero() {
  if (!ready) return 0;

  encoder->origin = 0;
  encoder->index = 0;

  uint32_t zero = (uint32_t)(-encoder->read());
  encoder->origin = zero;

  return zero;
}

int32_t ServoMotor::encoderRead() {
  int32_t encoderCounts = encoder->read();
  if (encoderReverse) encoderCounts = -encoderCounts;
  return encoderCounts;
}

// updates PID and sets servo motor power/direction
void ServoMotor::poll() {
  #ifdef CALIBRATE_SERVO_DC
    calibrateVelocity->updateState(getInstrumentCoordinateSteps());
  #endif

  long encoderCounts = encoderRead();

  // for absolute encoders initialize the motor position at startup
  if (syncThreshold != OFF) {
    if (!motorStepsInitDone && homeSet) {
      noInterrupts();
      motorSteps = encoderCounts;
      targetSteps = encoderCounts;
      backlashSteps = 0;
      interrupts();
      motorStepsInitDone = true;
    }
  }

  long motorCounts = 0;

  noInterrupts();
  motorCounts = motorSteps;
  interrupts();

  long unfilteredEncoderCounts = encoderCounts;
  UNUSED(unfilteredEncoderCounts);
  bool isTracking = (abs(currentFrequency - trackingFrequency) < trackingFrequency/10.0F);

  encoderCounts = filter->update(encoderCounts, motorCounts, isTracking);

  control->set = motorCounts;
  control->in = encoderCounts;
  float velocity;
  if (enabled) {
    feedback->poll();

    // directly use fixed PWM value during calibration
    #ifdef CALIBRATE_SERVO_DC
      velocity = calibrateVelocity->experimentMode ? calibrateVelocity->experimentPwm * driver->getMotorControlRange() / 100.0F : control->out;
    #else
      velocity = control->out + currentDirection*currentFrequency;
    #endif

  } else velocity = 0.0F;

  // for virtual encoders set the velocity and direction
  if (encoder->isVirtual) {
    encoder->setVelocity(abs(velocity));
    volatile int8_t dir = velocity < 0.0F ? 1 : -1;
    encoder->setDirection(&dir);
  }

  delta = motorCounts - encoderCounts;
  velocityPercent = (driver->setMotorVelocity(velocity)/driver->getMotorControlRange()) * 100.0F;
  if (driver->getMotorDirection() == DIR_FORWARD) control->directionHint = 1; else control->directionHint = -1;

  if (feedback->manuallySwitchParameters) {
    if (!slewing && enabled) {
      if ((long)(millis() - lastSlewingTime) > SERVO_SLEWING_TO_TRACKING_DELAY) feedback->selectTrackingParameters(); else feedback->selectSlewingParameters();
    } else {
      lastSlewingTime = millis();
      feedback->selectSlewingParameters();
    }
  } else {
    feedback->variableParameters(fabs(velocityPercent));
  }

  if (velocityPercent < -33) wasBelow33 = true;
  if (velocityPercent > 33) wasAbove33 = true;

  if (millis() - lastCheckTime > 1000) {

    #ifndef SERVO_SAFETY_DISABLE
      // if above SERVO_SAFETY_STALL_POWER (33% default) and we're not moving something is seriously wrong, so shut it down
      if (labs(encoderCounts - lastEncoderCounts) < 10 && abs(velocityPercent) >= SERVO_SAFETY_STALL_POWER) {
        DF("WRN:"); D(axisPrefix); DF("stall detected!");
        DF(" control->in = "); D(control->in); DF(", control->set = "); D(control->set);
        DF(", control->out = "); D(control->out); DF(", velocity % = "); DL(velocityPercent);
        enable(false);
        safetyShutdown = true;
      }

      // if above 90% power for > three seconds and the distance to the target is increasing
      // something is seriously wrong so shut it down
      if (labs(delta - lastDelta) > lastTargetDistance && abs(velocityPercent) >= 90) {
        movingAwaySeconds++;
        if (movingAwaySeconds >= 3) {
          DF("WRN:"); D(axisPrefix); DF("runaway detected!");
          DLF(" > 90% power while moving away from the target!");
          enable(false);
          safetyShutdown = true;
        }
      } else movingAwaySeconds = 0;
      lastTargetDistance = labs(delta - lastDelta);

      // if we were below -33% and above 33% power in a one second period something is seriously wrong, so shut it down
      if (wasBelow33 && wasAbove33) {
        DF("WRN:"); D(axisPrefix); DF("oscillation detected!");
        DLF(" below -33% and above 33% power in a 2 second period!");
        enable(false);
        safetyShutdown = true;
      }
    #endif

    wasAbove33 = false;
    wasBelow33 = false;
    lastEncoderCounts = encoderCounts;
    lastDelta = delta;
    lastCheckTime = millis();
  }

  #if DEBUG != OFF && defined(DEBUG_SERVO) && DEBUG_SERVO != OFF
    if (axisNumber == DEBUG_SERVO) {
      static uint16_t count = 0;
      count++;
      if (count % 10 == 0) {
        char s[800];

        float spas = 0;
        if (axisNumber == 1) spas = AXIS1_STEPS_PER_DEGREE/3600.0F; else if (axisNumber == 2) spas = AXIS2_STEPS_PER_DEGREE/3600.0F;

//      sprintf(s, "Ax%dSvo: Delta %6ld, Motor %6ld, Encoder %6ld, Ax%dSvo_Power: %6.3f%%\r\n", (int)axisNumber, (motorCounts - encoderCounts), motorCounts, (long)encoderCounts, (int)axisNumber, velocityPercent);
//      sprintf(s, "Ax%dSvo: Motor %6ld, Encoder %6ld\r\n", (int)axisNumber, motorCounts, (long)encoderCounts);
//      sprintf(s, "Ax%dSvo: Delta %0.2f\r\n", (int)axisNumber, (motorCounts - (long)encoderCounts)/12.9425);
      sprintf(s, "Ax%dSvo: DeltaASf: %0.2f, DeltaAS: %0.2f, Ax%dSvo_Power: %6.3f%%\r\n", (int)axisNumber, (motorCounts - encoderCounts)/spas, (motorCounts - unfilteredEncoderCounts)/spas, (int)axisNumber, velocityPercent);

        D(s);
        UNUSED(spas);
      }
    }
  #endif
}

// sets dir as required and moves coord toward target at setFrequencySteps() rate
IRAM_ATTR void ServoMotor::move() {

  #if SERVO_SLEW_DIRECT == ON
    if (sync && !inBacklash) targetSteps += step;

    if (motorSteps > targetSteps) {
      motorSteps = targetSteps;
    } else

    if (motorSteps < targetSteps + backlashAmountSteps) {
      motorSteps = targetSteps + backlashAmountSteps;
    }

  #else
    if (sync && !inBacklash) targetSteps += step;

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

  #endif
}

#endif
