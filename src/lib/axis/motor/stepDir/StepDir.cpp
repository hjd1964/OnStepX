// -----------------------------------------------------------------------------------
// axis step/dir motor

#include "StepDir.h"

#ifdef STEP_DIR_MOTOR_PRESENT

#include "../../../tasks/OnTask.h"

StepDirMotor *stepDirMotorInstance[9];

// if the AXISn_STEP_PIN isn't present, use the passed step pin instead (slower)
#ifndef AXIS1_STEP_PIN
  #define AXIS1_STEP_PIN stepDirMotorInstance[0]->Pins->step
#endif
#ifndef AXIS2_STEP_PIN
  #define AXIS2_STEP_PIN stepDirMotorInstance[1]->Pins->step
#endif
#ifndef AXIS3_STEP_PIN
  #define AXIS3_STEP_PIN stepDirMotorInstance[2]->Pins->step
#endif
#ifndef AXIS4_STEP_PIN
  #define AXIS4_STEP_PIN stepDirMotorInstance[3]->Pins->step
#endif
#ifndef AXIS5_STEP_PIN
  #define AXIS5_STEP_PIN stepDirMotorInstance[4]->Pins->step
#endif
#ifndef AXIS6_STEP_PIN
  #define AXIS6_STEP_PIN stepDirMotorInstance[5]->Pins->step
#endif
#ifndef AXIS7_STEP_PIN
  #define AXIS7_STEP_PIN stepDirMotorInstance[6]->Pins->step
#endif
#ifndef AXIS8_STEP_PIN
  #define AXIS8_STEP_PIN stepDirMotorInstance[7]->Pins->step
#endif
#ifndef AXIS9_STEP_PIN
  #define AXIS9_STEP_PIN stepDirMotorInstance[8]->Pins->step
#endif

IRAM_ATTR void moveStepDirMotorAxis1() { stepDirMotorInstance[0]->move(AXIS1_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorAxis2() { stepDirMotorInstance[1]->move(AXIS2_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorAxis3() { stepDirMotorInstance[2]->move(AXIS3_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorAxis4() { stepDirMotorInstance[3]->move(AXIS4_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorAxis5() { stepDirMotorInstance[4]->move(AXIS5_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorAxis6() { stepDirMotorInstance[5]->move(AXIS6_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorAxis7() { stepDirMotorInstance[6]->move(AXIS7_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorAxis8() { stepDirMotorInstance[7]->move(AXIS8_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorAxis9() { stepDirMotorInstance[8]->move(AXIS9_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorFFAxis1() { stepDirMotorInstance[0]->moveFF(AXIS1_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorFFAxis2() { stepDirMotorInstance[1]->moveFF(AXIS2_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorFRAxis1() { stepDirMotorInstance[0]->moveFR(AXIS1_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorFRAxis2() { stepDirMotorInstance[1]->moveFR(AXIS2_STEP_PIN); }

StepDirMotor::StepDirMotor(const uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings, bool useFastHardwareTimers) {
  if (axisNumber < 1 || axisNumber > 9) return;

  strcpy(axisPrefix, "MSG: StepDir_, ");
  axisPrefix[12] = '0' + axisNumber;
  driverType = STEP_DIR;
  this->axisNumber = axisNumber;
  this->Pins = Pins;
  this->useFastHardwareTimers = useFastHardwareTimers;

  driver = new StepDirDriver(axisNumber, Pins, Settings);
  pulseWidth = driver->getPulseWidth();

  setDefaultParameters(Settings->microsteps, Settings->microstepsGoto, Settings->currentHold, Settings->currentRun, Settings->currentGoto, 0);

  // attach the function pointers to the callbacks
  stepDirMotorInstance[axisNumber - 1] = this;
  callbackFF = NULL;
  callbackFR = NULL;
  switch (axisNumber) {
    case 1: callback = moveStepDirMotorAxis1; callbackFF = moveStepDirMotorFFAxis1; callbackFR = moveStepDirMotorFRAxis1; break;
    case 2: callback = moveStepDirMotorAxis2; callbackFF = moveStepDirMotorFFAxis2; callbackFR = moveStepDirMotorFRAxis2; break;
    case 3: callback = moveStepDirMotorAxis3; break;
    case 4: callback = moveStepDirMotorAxis4; break;
    case 5: callback = moveStepDirMotorAxis5; break;
    case 6: callback = moveStepDirMotorAxis6; break;
    case 7: callback = moveStepDirMotorAxis7; break;
    case 8: callback = moveStepDirMotorAxis8; break;
    case 9: callback = moveStepDirMotorAxis9; break;
  }
}

bool StepDirMotor::init() {
  if (axisNumber < 1 || axisNumber > 9) return false;

  if (callbackFF == NULL || callbackFR == NULL) useFastCalls = false; else useFastCalls = true;
 
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
  pinModeEx(Pins->dir, OUTPUT);

  // init default driver step state (clear)
  #ifndef DRIVER_STEP_DEFAULTS
    stepSet = Pins->stepState;
    stepClr = !stepSet;
  #endif
  pinModeEx(Pins->step, OUTPUT);
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
  taskHandle = tasks.add(0, 0, true, 0, callback, timerName);
  if (taskHandle) {
    V("success");
    if (axisNumber <= 2 && useFastHardwareTimers) { if (!tasks.requestHardwareTimer(taskHandle, axisNumber, 0)) { VF(" (no hardware timer!)"); } }
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

// sets driver parameters: microsteps, microsteps goto, hold current, run current, goto current, unused
void StepDirMotor::setParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  driver->setParameters(param1, param2, param3, param4, param5, param6);
  homeSteps = driver->getMicrostepRatio();
  V(axisPrefix); VF("sequencer homes every "); V(homeSteps); VLF(" step(s)");
}

// validate driver parameters
bool StepDirMotor::validateParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  return driver->validateParameters(param1, param2, param3, param4, param5, param6);
}

// sets motor power on/off (if possible)
void StepDirMotor::power(bool state) {
  if (Pins->enable != OFF && Pins->enable != SHARED) {
    digitalWriteEx(Pins->enable, state ? Pins->enabledState : !Pins->enabledState);
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

  // chart acceleration
  #if DEBUG != OFF && defined(DEBUG_STEPDIR_ACCEL) && DEBUG_STEPDIR_ACCEL != OFF
    if (axisNumber == DEBUG_STEPDIR) {
      static unsigned long t = 0;
      if ((long)(millis() - t) > 100) {
        DL(frequency);
        t = millis();
      }
    }
  #endif

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
    if (lastPeriodSet != lastPeriod) {
      tasks.setPeriodSubMicros(taskHandle, lastPeriod);
      lastPeriodSet = lastPeriod;
    }
    step = dir;

    if (microstepModeControl == MMC_TRACKING_READY) microstepModeControl = MMC_TRACKING;
    if (microstepModeControl == MMC_SLEWING_READY) {
      #if DEBUG == VERBOSE
        if (useFastCalls) { V(axisPrefix); VF("high speed swap in took "); V(millis() - switchStartTimeMs); VLF(" ms"); }
      #endif
      microstepModeControl = MMC_SLEWING;
    }

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
      switchStartTimeMs = millis();
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
  if (useFastCalls) {
    if (fast) {
      if (direction == dirRev) tasks.setCallback(taskHandle, callbackFR); else tasks.setCallback(taskHandle, callbackFF);
    } else tasks.setCallback(taskHandle, callback);
    return true;
  } else return false;
}

#if defined(GPIO_DIRECTION_PINS)
  // change motor direction on request by polling
  IRAM_ATTR void StepDirMotor::updateMotorDirection() {
    if (direction == DirSetRev) {
      digitalWriteEx(Pins->dir, dirRev);
      direction = dirRev;
    } else
    if (direction == DirSetFwd) {
      digitalWriteEx(Pins->dir, dirFwd);
      direction = dirFwd;
    }
  }
#endif

#if STEP_WAVE_FORM == SQUARE
  IRAM_ATTR void StepDirMotor::move(const int16_t stepPin) {
    #ifdef GPIO_DIRECTION_PINS
      if (direction > DirNone) return;
    #endif

    if (microstepModeControl == MMC_SLEWING_REQUEST && (motorSteps + backlashSteps) % homeSteps == 0) {
      microstepModeControl = MMC_SLEWING_PAUSE;
      tasks.immediate(monitorHandle);
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

        #ifdef SHARED_DIRECTION_PINS
          if (axisNumber > 2) {
            digitalWriteF(Pins->dir, direction);
            delayNanoseconds(pulseWidth);
          }
        #endif

        digitalWriteF(stepPin, stepSet);
      }
    } else {
      if (synchronized && !inBacklash) targetSteps += step;
      if (motorSteps > targetSteps) {
        if (direction != dirRev) {
          #ifdef GPIO_DIRECTION_PINS
            direction = DirSetRev;
          #else
            direction = dirRev;
            digitalWriteF(Pins->dir, dirRev);
          #endif
        }
      } else
      if (motorSteps < targetSteps) {
        if (direction != dirFwd) {
          #ifdef GPIO_DIRECTION_PINS
            direction = DirSetFwd;
          #else
            direction = dirFwd;
            digitalWriteF(Pins->dir, dirFwd);
          #endif
        }
      } else {
        if (!inBacklash) direction = DirNone;
      }

      digitalWriteF(stepPin, stepClr);
    }

    takeStep = !takeStep;
  }

  IRAM_ATTR void StepDirMotor::moveFF(const int16_t stepPin) {
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

  IRAM_ATTR void StepDirMotor::moveFR(const int16_t stepPin) {
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

  IRAM_ATTR void StepDirMotor::move(const int16_t stepPin) {
    digitalWriteF(stepPin, stepClr);

    #ifdef GPIO_DIRECTION_PINS
      if (direction > DirNone) return;
    #endif

    if (microstepModeControl == MMC_SLEWING_REQUEST && (motorSteps + backlashSteps) % homeSteps == 0) {
      microstepModeControl = MMC_SLEWING_PAUSE;
      tasks.immediate(monitorHandle);
    }
    if (microstepModeControl >= MMC_SLEWING_PAUSE) return;

    if (synchronized && !inBacklash) targetSteps += step;

    if (motorSteps > targetSteps) {
      if (direction != dirRev) {
        #ifdef GPIO_DIRECTION_PINS
          direction = DirSetRev;
        #else
          direction = dirRev;
          digitalWriteF(Pins->dir, dirRev);
          #ifdef SHARED_DIRECTION_PINS
            if (axisNumber < 3) return;
          #else
            return;
          #endif
        #endif
      }
    } else
    if (motorSteps < targetSteps) {
      if (direction != dirFwd) {
        #ifdef GPIO_DIRECTION_PINS
          direction = DirSetFwd;
        #else
          direction = dirFwd;
          digitalWriteF(Pins->dir, dirFwd);
          #ifdef SHARED_DIRECTION_PINS
            if (axisNumber < 3) return;
          #else
            return;
          #endif
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

      #ifdef SHARED_DIRECTION_PINS
        if (axisNumber > 2) {
          delayNanoseconds(pulseWidth);
          direction = DirNone;
        }
      #endif

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

      #ifdef SHARED_DIRECTION_PINS
        if (axisNumber > 2) {
          delayNanoseconds(pulseWidth);
          direction = DirNone;
        }
      #endif

      digitalWriteF(stepPin, stepSet);
    }
  }

  IRAM_ATTR void StepDirMotor::moveFF(const int16_t stepPin) {
    digitalWriteF(stepPin, stepClr);

    if (microstepModeControl >= MMC_SLEWING_PAUSE) return;

    if (synchronized) targetSteps += slewStep;
    if (motorSteps < targetSteps) {
      motorSteps += slewStep;
      digitalWriteF(stepPin, stepSet);
    }
  }

  IRAM_ATTR void StepDirMotor::moveFR(const int16_t stepPin) {
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
