// -----------------------------------------------------------------------------------
// axis step/dir motor

#include "StepDir.h"

#ifdef STEP_DIR_MOTOR_PRESENT

#include "../../../tasks/OnTask.h"

StepDirMotor::StepDirMotor(uint8_t axisNumber, const StepDirPins *Pins, StepDirDriver *driver) {
  axisPrefix[10] = '0' + axisNumber;
  driverType = STEP_DIR;
  this->axisNumber = axisNumber;
  this->Pins = Pins;
  this->driver = driver;
}

bool StepDirMotor::init(void (*volatile move)(), void (*volatile moveFF)(), void (*volatile moveFR)()) {
  this->_move = move;
  this->_moveFF = moveFF;
  this->_moveFR = moveFR;
  if (_move == NULL) { D(axisPrefix); DLF("nothing to do exiting!"); return false; }

  // get the axis monitor handle, by name
  char taskName[] = "Ax_Mtr";
  taskName[2] = axisNumber + '0';
  mtrHandle = tasks.getHandleByName(taskName);
  if (mtrHandle == 0) { D(axisPrefix); DLF("no axis monitor, exiting!"); return false; }

  #if DEBUG == VERBOSE
    V(axisPrefix); V("pins step="); if (Pins->step == OFF) V("OFF"); else V(Pins->step);
    V(", dir="); if (Pins->dir == OFF) VF("OFF"); else V(Pins->dir);
    V(", en="); if (Pins->enable == OFF) VLF("OFF"); else if (Pins->enable == SHARED) VLF("SHARED"); else VL(Pins->enable);
  #endif

  // this driver requires available pins to function
  if (Pins->dir == OFF || Pins->step == OFF) {
    D(axisPrefix); DLF("step or dir pin not present, exiting!");
    return false;
  }

  // init default driver direction pin for output
  pinMode(Pins->dir, OUTPUT);

  // init default driver step state (clear)
  #ifndef DRIVER_STEP_DEFAULTS
    stepSet = Pins->stepState;
    stepClr = !stepSet;
  #endif
  pinMode(Pins->step, OUTPUT);
  digitalWriteF(Pins->step, stepClr);

  // init default driver enable pin
  pinModeEx(Pins->enable, OUTPUT);
  // driver enabled for possible TMC current calibration
  digitalWriteEx(Pins->enable, Pins->enabledState)

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
  } else { VLF("FAILED!"); return false; }

  return true;
}

// set driver default reverse state
void StepDirMotor::setReverse(int8_t state) {
  if (state == OFF) { dirFwd = LOW; dirRev = HIGH; } else { dirFwd = HIGH; dirRev = LOW; }
  digitalWriteEx(Pins->dir, dirFwd);
  direction = dirFwd;
}

// set default driver microsteps and current
void StepDirMotor::setParam(float param1, float param2, float param3, float param4, float param5, float param6) {
  driver->setParam(param1, param2, param3, param4, param5, param6);
  homeSteps = driver->getMicrostepRatio();
  V(axisPrefix); VF("sequencer homes every "); V(homeSteps); VLF(" step(s)");
}

// validate driver parameters
bool StepDirMotor::validateParam(float param1, float param2, float param3, float param4, float param5, float param6) {
  return driver->validateParam(param1, param2, param3, param4, param5, param6);
}

// sets motor power on/off (if possible)
void StepDirMotor::power(bool state) {
  if (Pins->enable != OFF && Pins->enable != SHARED) {
    digitalWriteEx(Pins->enable, state?Pins->enabledState:!Pins->enabledState);
  } else {
    driver->power(state);
  }
}

// get the associated stepper drivers status
DriverStatus StepDirMotor::getDriverStatus() {
  driver->updateStatus();
  return driver->getStatus();
}

// set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
void StepDirMotor::setFrequencySteps(float frequency) {
  // negative frequency, convert to positive and reverse the direction
  int dir = 0;
  if (frequency > 0.0F) dir = 1; else if (frequency < 0.0F) { frequency = -frequency; dir = -1; }

  // if in backlash override the frequency OR change
  // microstep mode and/or swap in fast ISRs as required
  if (inBacklash) frequency = backlashFrequency; else modeSwitch();

  if (frequency != currentFrequency || microstepModeControl >= MMC_SLEWING_PAUSE) {
    lastFrequency = frequency;

    // if slewing has a larger step size divide the frequency to account for it
    if (microstepModeControl == MMC_SLEWING || microstepModeControl == MMC_SLEWING_READY) frequency /= slewStep;

    // frequency in steps per second to period in microsecond counts per step
    // also runs the timer twice as fast if using a square wave
    #if STEP_WAVE_FORM == SQUARE
      float period = 500000.0F/frequency;
    #else
      float period = 1000000.0F/frequency;
    #endif
   
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
    if (step != dir) step = 0;
    tasks.setPeriodSubMicros(taskHandle, lastPeriod);
    step = dir;

    if (microstepModeControl == MMC_TRACKING_READY) microstepModeControl = MMC_TRACKING;
    if (microstepModeControl == MMC_SLEWING_READY) microstepModeControl = MMC_SLEWING;

  } else {
    noInterrupts();
    step = dir;
    interrupts();
  }
}

// switch microstep modes as needed
void StepDirMotor::modeSwitch() {
  Y;
  if (lastFrequency <= backlashFrequency*2.0F) {

    if (microstepModeControl >= MMC_SLEWING) {

      if (enableMoveFast(false)) {
        V(axisPrefix); VF("high speed ISR swapped out at "); V(lastFrequency); VL(" steps/sec.");
      }

      if (driver->modeSwitchAllowed()) {
        V(axisPrefix); VLF("mode switch tracking set");
        driver->modeMicrostepTracking();
      }

      microstepModeControl = MMC_TRACKING_READY;
    }
  } else {

    if (microstepModeControl == MMC_TRACKING) {
      microstepModeControl = MMC_SLEWING_REQUEST;
    } else
    if (microstepModeControl == MMC_SLEWING_PAUSE) {

      if (driver->modeSwitchAllowed()) {
        V(axisPrefix); VLF("mode switch slewing set");
        slewStep = driver->modeMicrostepSlewing();
      }

      if (enableMoveFast(true)) {
        V(axisPrefix); VF("high speed ISR swapped in at "); V(lastFrequency); VLF(" steps/sec.");
      }

      microstepModeControl = MMC_SLEWING_READY;
    }
  }
}

float StepDirMotor::getFrequencySteps() {
  if (lastPeriod == 0) return 0;
  #if STEP_WAVE_FORM == SQUARE
    return 8000000.0F/lastPeriod;
  #else
    return 16000000.0F/lastPeriod;
  #endif
}

// set slewing state (hint that we are about to slew or are done slewing)
void StepDirMotor::setSlewing(bool state) {
  if (state == true) driver->modeDecaySlewing(); else driver->modeDecayTracking();
}

// swaps in/out fast unidirectional ISR for slewing 
bool StepDirMotor::enableMoveFast(const bool fast) {
  if (axisNumber <= 2) {
    if (fast) {
      if (direction == dirRev) tasks.setCallback(taskHandle, _moveFR); else tasks.setCallback(taskHandle, _moveFF);
    } else tasks.setCallback(taskHandle, _move);
    return true;
  } else return false;
}

// monitor and respond to motor state as required
void StepDirMotor::poll() {
  #if defined(GPIO_DIRECTION_PINS) || defined(SHARED_DIRECTION_PINS)
    updateMotorDirection();
  #endif
}

IRAM_ATTR void StepDirMotor::updateMotorDirection() {
  if (direction == DirSetRev) {
    digitalWriteEx(Pins->dir, dirRev);
    #ifdef SHARED_DIRECTION_PINS
      if (axisNumber > 2) delayMicroseconds(1);
    #endif
    direction = dirRev;
  } else
  if (direction == DirSetFwd) {
    digitalWriteEx(Pins->dir, dirFwd);
    #ifdef SHARED_DIRECTION_PINS
      if (axisNumber > 2) delayMicroseconds(1);
    #endif
    direction = dirFwd;
  }
}

#if STEP_WAVE_FORM == SQUARE
  IRAM_ATTR void StepDirMotor::move(const int8_t stepPin) {
    if (direction > DirNone) return;

    if (microstepModeControl == MMC_SLEWING_REQUEST && (motorSteps + backlashSteps) % homeSteps == 0) {
      microstepModeControl = MMC_SLEWING_PAUSE;
      tasks.immediate(mtrHandle);
    }
    if (microstepModeControl >= MMC_SLEWING_PAUSE) return;

    if (takeStep) {
      if (direction == dirRev) {
        if (backlashSteps > 0) {
          backlashSteps--;
          inBacklash = backlashSteps > 0;
        } else {
          motorSteps--;
          inBacklash = false;
        }
        digitalWriteF(stepPin, stepSet);
      } else
      if (direction == dirFwd) {
        if (backlashSteps < backlashAmountSteps) {
          backlashSteps++;
          inBacklash = backlashSteps < backlashAmountSteps;
        } else {
          motorSteps++;
          inBacklash = false;
        }
        digitalWriteF(stepPin, stepSet);
      }
    } else {
      if (synchronized && !inBacklash) targetSteps += step;
      if (motorSteps > targetSteps) {
        if (direction != dirRev) {
          direction = DirSetRev;
          #if !defined(GPIO_DIRECTION_PINS) && !defined(SHARED_DIRECTION_PINS)
            updateMotorDirection();
          #endif
        }
      } else
      if (motorSteps < targetSteps) {
        if (direction != dirFwd) {
          direction = DirSetFwd;
          #if !defined(GPIO_DIRECTION_PINS) && !defined(SHARED_DIRECTION_PINS)
            updateMotorDirection();
          #endif
        }
      } else {
        if (!inBacklash) direction = DirNone;
      }

      digitalWriteF(stepPin, stepClr);
    }

    takeStep = !takeStep;
  }

  IRAM_ATTR void StepDirMotor::moveFF(const int8_t stepPin) {
    if (microstepModeControl >= MMC_SLEWING_PAUSE) return;

    if (takeStep) {
      if (synchronized) targetSteps += slewStep;
      if (motorSteps < targetSteps) {
        motorSteps += slewStep;
        digitalWriteF(stepPin, stepSet);
      }
    } else digitalWriteF(stepPin, stepClr);
    takeStep = !takeStep;
  }

  IRAM_ATTR void StepDirMotor::moveFR(const int8_t stepPin) {
    if (microstepModeControl >= MMC_SLEWING_PAUSE) return;

    if (takeStep) {
      if (synchronized) targetSteps -= slewStep;
      if (motorSteps > targetSteps) {
        motorSteps -= slewStep;
        digitalWriteF(stepPin, stepSet);
      }
    } else digitalWriteF(stepPin, stepClr);
    takeStep = !takeStep;
  }
#else
  IRAM_ATTR void StepDirMotor::move(const int8_t stepPin) {
    digitalWriteF(stepPin, stepClr);

    if (direction > DirNone) return;

    if (microstepModeControl == MMC_SLEWING_REQUEST && (motorSteps + backlashSteps) % homeSteps == 0) {
      microstepModeControl = MMC_SLEWING_PAUSE;
      tasks.immediate(mtrHandle);
    }
    if (microstepModeControl >= MMC_SLEWING_PAUSE) return;

    if (synchronized && !inBacklash) targetSteps += step;

    if (motorSteps > targetSteps) {
      if (direction != dirRev) {
        direction = DirSetRev;
        #if !defined(GPIO_DIRECTION_PINS) && !defined(SHARED_DIRECTION_PINS)
          updateMotorDirection();
        #endif
      }
    } else
    if (motorSteps < targetSteps) {
      if (direction != dirFwd) {
        direction = DirSetFwd;
        #if !defined(GPIO_DIRECTION_PINS) && !defined(SHARED_DIRECTION_PINS)
          updateMotorDirection();
        #endif
      }
    } else {
      if (!inBacklash) direction = DirNone;
    }

    if (direction == dirRev) {
      if (backlashSteps > 0) {
        backlashSteps--;
        inBacklash = backlashSteps > 0;
      } else {
        motorSteps--;
        inBacklash = false;
      }
      if (axisNumber > 2) delayMicroseconds(1);
      digitalWriteF(stepPin, stepSet);
    } else
    if (direction == dirFwd) {
      if (backlashSteps < backlashAmountSteps) {
        backlashSteps++;
        inBacklash = backlashSteps < backlashAmountSteps;
      } else {
        motorSteps++;
        inBacklash = false;
      }
      if (axisNumber > 2) delayMicroseconds(1);
      digitalWriteF(stepPin, stepSet);
    }
  }

  IRAM_ATTR void StepDirMotor::moveFF(const int8_t stepPin) {
    digitalWriteF(stepPin, stepClr);

    if (microstepModeControl >= MMC_SLEWING_PAUSE) return;

    if (synchronized) targetSteps += slewStep;
    if (motorSteps < targetSteps) {
      motorSteps += slewStep;
      digitalWriteF(stepPin, stepSet);
    }
  }

  IRAM_ATTR void StepDirMotor::moveFR(const int8_t stepPin) {
    digitalWriteF(stepPin, stepClr);

    if (microstepModeControl >= MMC_SLEWING_PAUSE) return;

    if (synchronized) targetSteps -= slewStep;
    if (motorSteps > targetSteps) {
      motorSteps -= slewStep;
      digitalWriteF(stepPin, stepSet);
    }
  }
#endif

#endif
