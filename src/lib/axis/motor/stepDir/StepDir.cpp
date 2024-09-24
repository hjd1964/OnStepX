// -----------------------------------------------------------------------------------
// axis step/dir motor

#include "StepDir.h"

#ifdef STEP_DIR_MOTOR_PRESENT

#include "../../../tasks/OnTask.h"

StepDirMotor *stepDirMotorInstance[9];

#ifndef AXIS1_STEP_PIN
  #define AXIS1_STEP_PIN stepDirMotorInstance[0]->Pins->step
#endif
IRAM_ATTR void moveStepDirMotorAxis1() { stepDirMotorInstance[0]->move(AXIS1_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorFFAxis1() { stepDirMotorInstance[0]->moveFF(AXIS1_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorFRAxis1() { stepDirMotorInstance[0]->moveFR(AXIS1_STEP_PIN); }

#ifndef AXIS2_STEP_PIN
  #define AXIS2_STEP_PIN stepDirMotorInstance[1]->Pins->step
#endif
IRAM_ATTR void moveStepDirMotorAxis2() { stepDirMotorInstance[1]->move(AXIS2_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorFFAxis2() { stepDirMotorInstance[1]->moveFF(AXIS2_STEP_PIN); }
IRAM_ATTR void moveStepDirMotorFRAxis2() { stepDirMotorInstance[1]->moveFR(AXIS2_STEP_PIN); }

#ifndef AXIS3_STEP_PIN
  #define AXIS3_STEP_PIN stepDirMotorInstance[2]->Pins->step
#endif
void moveStepDirMotorAxis3() { stepDirMotorInstance[2]->move(AXIS3_STEP_PIN); }
void moveStepDirMotorFFAxis3() { stepDirMotorInstance[2]->moveFF(AXIS3_STEP_PIN); }
void moveStepDirMotorFRAxis3() { stepDirMotorInstance[2]->moveFR(AXIS3_STEP_PIN); }

#ifndef AXIS4_STEP_PIN
  #define AXIS4_STEP_PIN stepDirMotorInstance[3]->Pins->step
#endif
void moveStepDirMotorAxis4() { stepDirMotorInstance[3]->move(AXIS4_STEP_PIN); }
void moveStepDirMotorFFAxis4() { stepDirMotorInstance[3]->moveFF(AXIS4_STEP_PIN); }
void moveStepDirMotorFRAxis4() { stepDirMotorInstance[3]->moveFR(AXIS4_STEP_PIN); }

#ifndef AXIS5_STEP_PIN
  #define AXIS5_STEP_PIN stepDirMotorInstance[4]->Pins->step
#endif
void moveStepDirMotorAxis5() { stepDirMotorInstance[4]->move(AXIS5_STEP_PIN); }
void moveStepDirMotorFFAxis5() { stepDirMotorInstance[4]->moveFF(AXIS5_STEP_PIN); }
void moveStepDirMotorFRAxis5() { stepDirMotorInstance[4]->moveFR(AXIS5_STEP_PIN); }

#ifndef AXIS6_STEP_PIN
  #define AXIS6_STEP_PIN stepDirMotorInstance[5]->Pins->step
#endif
void moveStepDirMotorAxis6() { stepDirMotorInstance[5]->move(AXIS6_STEP_PIN); }
void moveStepDirMotorFFAxis6() { stepDirMotorInstance[5]->moveFF(AXIS6_STEP_PIN); }
void moveStepDirMotorFRAxis6() { stepDirMotorInstance[5]->moveFR(AXIS6_STEP_PIN); }

#ifndef AXIS7_STEP_PIN
  #define AXIS7_STEP_PIN stepDirMotorInstance[6]->Pins->step
#endif
void moveStepDirMotorAxis7() { stepDirMotorInstance[6]->move(AXIS7_STEP_PIN); }
void moveStepDirMotorFFAxis7() { stepDirMotorInstance[6]->moveFF(AXIS7_STEP_PIN); }
void moveStepDirMotorFRAxis7() { stepDirMotorInstance[6]->moveFR(AXIS7_STEP_PIN); }

#ifndef AXIS8_STEP_PIN
  #define AXIS8_STEP_PIN stepDirMotorInstance[7]->Pins->step
#endif
void moveStepDirMotorAxis8() { stepDirMotorInstance[7]->move(AXIS8_STEP_PIN); }
void moveStepDirMotorFFAxis8() { stepDirMotorInstance[7]->moveFF(AXIS8_STEP_PIN); }
void moveStepDirMotorFRAxis8() { stepDirMotorInstance[7]->moveFR(AXIS8_STEP_PIN); }

#ifndef AXIS9_STEP_PIN
  #define AXIS9_STEP_PIN stepDirMotorInstance[8]->Pins->step
#endif
void moveStepDirMotorAxis9() { stepDirMotorInstance[8]->move(AXIS9_STEP_PIN); }
void moveStepDirMotorFFAxis9() { stepDirMotorInstance[8]->moveFF(AXIS9_STEP_PIN); }
void moveStepDirMotorFRAxis9() { stepDirMotorInstance[8]->moveFR(AXIS9_STEP_PIN); }

StepDirMotor::StepDirMotor(const uint8_t axisNumber, const StepDirPins *Pins, StepDirDriver *Driver, bool useFastHardwareTimers) {
  if (axisNumber < 1 || axisNumber > 9) return;

  driverType = STEP_DIR;
  strcpy(axisPrefix, "MSG: StepDir_, ");
  axisPrefix[12] = '0' + axisNumber;
  this->axisNumber = axisNumber;
  this->Pins = Pins;

  if (axisNumber > 2) useFastHardwareTimers = false;
  this->useFastHardwareTimers = useFastHardwareTimers;

  driver = Driver;
  pulseWidth = driver->getPulseWidth();
  setDefaultParameters(driver->settings.microsteps, driver->settings.microstepsSlewing, driver->settings.currentHold, driver->settings.currentRun, driver->settings.currentGoto, 0);

  // attach the function pointers to the callbacks
  stepDirMotorInstance[axisNumber - 1] = this;
  switch (axisNumber) {
    case 1: callback = moveStepDirMotorAxis1; callbackFF = moveStepDirMotorFFAxis1; callbackFR = moveStepDirMotorFRAxis1; break;
    case 2: callback = moveStepDirMotorAxis2; callbackFF = moveStepDirMotorFFAxis2; callbackFR = moveStepDirMotorFRAxis2; break;
    case 3: callback = moveStepDirMotorAxis3; callbackFF = moveStepDirMotorFFAxis3; callbackFR = moveStepDirMotorFRAxis3; break;
    case 4: callback = moveStepDirMotorAxis4; callbackFF = moveStepDirMotorFFAxis4; callbackFR = moveStepDirMotorFRAxis4; break;
    case 5: callback = moveStepDirMotorAxis5; callbackFF = moveStepDirMotorFFAxis5; callbackFR = moveStepDirMotorFRAxis5; break;
    case 6: callback = moveStepDirMotorAxis6; callbackFF = moveStepDirMotorFFAxis6; callbackFR = moveStepDirMotorFRAxis6; break;
    case 7: callback = moveStepDirMotorAxis7; callbackFF = moveStepDirMotorFFAxis7; callbackFR = moveStepDirMotorFRAxis7; break;
    case 8: callback = moveStepDirMotorAxis8; callbackFF = moveStepDirMotorFFAxis8; callbackFR = moveStepDirMotorFRAxis8; break;
    case 9: callback = moveStepDirMotorAxis9; callbackFF = moveStepDirMotorFFAxis9; callbackFR = moveStepDirMotorFRAxis9; break;
  }
}

bool StepDirMotor::init() {
  if (axisNumber < 1 || axisNumber > 9) return false;
 
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
  digitalWriteEx(Pins->enable, !Pins->enabledState)

  // start the motor timer
  V(axisPrefix); VF("start task to move motor... ");
  char timerName[] = "Motor_";
  timerName[5] = '0' + axisNumber;
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

// set driver default reverse state
void StepDirMotor::setReverse(int8_t state) {
  if (state == OFF) { dirFwd = LOW; dirRev = HIGH; } else { dirFwd = HIGH; dirRev = LOW; }
  digitalWriteEx(Pins->dir, dirFwd);
  direction = dirFwd;
}

// sets driver parameters: microsteps, microsteps goto, hold current, run current, goto current, unused
void StepDirMotor::setParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  driver->init(param1, param2, param3, param4, param5, param6);
  homeSteps = driver->getMicrostepRatio();
  V(axisPrefix); VF("sequencer homes every "); V(homeSteps); VLF(" step(s)");
}

// validate driver parameters
bool StepDirMotor::validateParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  return driver->validateParameters(param1, param2, param3, param4, param5, param6);
}

// sets motor enable on/off (if possible)
void StepDirMotor::enable(bool state) {
  V(axisPrefix); VF("driver powered ");
  if (state) { VF("up"); } else { VF("down"); }

  if (Pins->enable != OFF && Pins->enable != SHARED) {
    VF(" using pin "); VL(Pins->enable);
    digitalWriteEx(Pins->enable, state ? Pins->enabledState : !Pins->enabledState);
  } else {
    if (driver->enable(state)) { VLF(" using secondary method"); } else { VLF(" skipped no control available"); }
  }
  enabled = state;
}

// get the associated stepper drivers status
DriverStatus StepDirMotor::getDriverStatus() {
  driver->updateStatus();
  return driver->getStatus();
}

// set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
void StepDirMotor::setFrequencySteps(float frequency) {

  // chart acceleration
  #if DEBUG != OFF && defined(DEBUG_STEPDIR_ACCEL)
    if (axisNumber == DEBUG_STEPDIR_ACCEL) {
      static unsigned long t = 0;
      if ((long)(millis() - t) > 100) {
        DF("Axis"); D(axisNumber);
        D(" step frequency "); D(frequency); DL("Hz");
        t = millis();
      }
    }
  #endif

  // chart motor position
  #if DEBUG != OFF && defined(DEBUG_STEPDIR_POSITION)
    if (axisNumber == DEBUG_STEPDIR_POSITION) {
      static unsigned long t = 0;
      if ((long)(millis() - t) > 100) {
        DF("Axis"); D(axisNumber);
        D(" motor step position "); D(motorSteps);
        D(" motor target position "); DL(targetSteps);
        t = millis();
      }
    }
  #endif

  if (!inBacklash) modeSwitch();

  Y;
  // negative frequency, convert to positive and reverse the direction
  int dir = 0;
  if (frequency > 0.0F) dir = 1; else if (frequency < 0.0F) { frequency = -frequency; dir = -1; }

  // if in backlash override the frequency OR change
  // microstep mode and/or swap in fast ISRs as required
  if (inBacklash) frequency = backlashFrequency;

  if (frequency != currentFrequency || microstepModeControl >= MMC_SLEWING_PAUSE) {
    lastFrequency = frequency;

    // if slewing has a larger step size divide the frequency to account for it
    if (microstepModeControl == MMC_SLEWING || microstepModeControl == MMC_SLEWING_READY) frequency /= stepSize;

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
        V(axisPrefix); VF("high speed swap in took "); V(millis() - switchStartTimeMs); VLF(" ms");
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
  if (lastFrequency <= backlashFrequency*2.0F) {
    if (microstepModeControl >= MMC_SLEWING) {
      microstepModeControl = MMC_TRACKING_READY;

      enableMoveFast(false);

      if (driver->modeSwitchAllowed || driver->modeSwitchFastAllowed) {
        V(axisPrefix); VLF("mode switch tracking set");
        driver->modeMicrostepTracking();
      }
    }
  } else {
    if (microstepModeControl == MMC_TRACKING) {
      noInterrupts();
      if (!sync || (step == -1 && direction == dirRev) || (step == 1 && direction == dirFwd)) {
        microstepModeControl = MMC_SLEWING_REQUEST;
      }
      interrupts();
      switchStartTimeMs = millis();
    } else
    if (microstepModeControl == MMC_SLEWING_PAUSE) {
      if (driver->modeSwitchAllowed || driver->modeSwitchFastAllowed) {
        V(axisPrefix); VLF("mode switch slewing set");
        stepSize = driver->modeMicrostepSlewing();
      }
      enableMoveFast(true);
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
  if (fast) {
    if (direction == dirRev) {
      tasks.setCallback(taskHandle, callbackFR);
      V(axisPrefix); VF("high speed Rev ISR swapped in at "); V(lastFrequency); VLF(" steps/sec.");
    } else {
      tasks.setCallback(taskHandle, callbackFF);
      V(axisPrefix); VF("high speed Fwd ISR swapped in at "); V(lastFrequency); VLF(" steps/sec.");
    }
  } else {
    tasks.setCallback(taskHandle, callback);
    V(axisPrefix); VF("high speed ISR swapped out at "); V(lastFrequency); VL(" steps/sec.");
  }
  return true;
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

IRAM_ATTR void StepDirMotor::move(const int16_t stepPin) {
  #if STEP_WAVE_FORM == PULSE
    digitalWriteF(stepPin, stepClr);
  #endif

  #ifdef GPIO_DIRECTION_PINS
    if (direction > DirNone) return;
  #endif

  if (microstepModeControl == MMC_SLEWING_REQUEST && (motorSteps + backlashSteps) % homeSteps == 0 && direction < DirNone) {
    microstepModeControl = MMC_SLEWING_PAUSE;
    tasks.immediate(monitorHandle);
  }
  if (microstepModeControl >= MMC_SLEWING_PAUSE) return;

  #if STEP_WAVE_FORM == SQUARE
    if (takeStep) {
  #endif

  long lastTargetSteps = targetSteps;
  if (sync && !inBacklash) targetSteps += step;

  if (motorSteps > targetSteps || (inBacklash && direction == dirRev)) {
    if (direction != dirRev) {
      targetSteps = lastTargetSteps;
      #ifdef GPIO_DIRECTION_PINS
        direction = DirSetRev;
      #else
        direction = dirRev;
        digitalWriteF(Pins->dir, dirRev);
      #endif
      return;
    }

    if (backlashSteps > 0) {
      backlashSteps--;
      inBacklash = backlashSteps > 0;
    } else {
      motorSteps--;
      inBacklash = false;
    }

    #ifdef SHARED_DIRECTION_PINS
      if (axisNumber > 2) { digitalWriteF(Pins->dir, direction); delayNanoseconds(pulseWidth); }
    #endif
    digitalWriteF(stepPin, stepSet);
  } else

  if (motorSteps < targetSteps || (inBacklash && direction == dirFwd)) {
    if (direction != dirFwd) {
      targetSteps = lastTargetSteps;
      #ifdef GPIO_DIRECTION_PINS
        direction = DirSetFwd;
      #else
        direction = dirFwd;
        digitalWriteF(Pins->dir, dirFwd);
      #endif
      return;
    }

    if (backlashSteps < backlashAmountSteps) {
      backlashSteps++;
      inBacklash = backlashSteps < backlashAmountSteps;
    } else {
      motorSteps++;
      inBacklash = false;
    }

    #ifdef SHARED_DIRECTION_PINS
      if (axisNumber > 2) { digitalWriteF(Pins->dir, direction); delayNanoseconds(pulseWidth); }
    #endif
    digitalWriteF(stepPin, stepSet);

  } else if (!inBacklash) direction = DirNone;

  #if STEP_WAVE_FORM == SQUARE
    } else digitalWriteF(stepPin, stepClr);
    takeStep = !takeStep;
  #endif
}

IRAM_ATTR void StepDirMotor::moveFF(const int16_t stepPin) {
  #if STEP_WAVE_FORM == PULSE
    digitalWriteF(stepPin, stepClr);
  #endif

  if (microstepModeControl >= MMC_SLEWING_PAUSE) return;

  #if STEP_WAVE_FORM == SQUARE
    if (takeStep) {
  #endif

  if (sync) targetSteps += stepSize;

  if (motorSteps < targetSteps) {
    motorSteps += stepSize;

    #ifdef SHARED_DIRECTION_PINS
      if (axisNumber > 2) { digitalWriteF(Pins->dir, direction); delayNanoseconds(pulseWidth); }
    #endif
    digitalWriteF(stepPin, stepSet);
  }

  #if STEP_WAVE_FORM == SQUARE
    } else digitalWriteF(stepPin, stepClr);
    takeStep = !takeStep;
  #endif
}

IRAM_ATTR void StepDirMotor::moveFR(const int16_t stepPin) {
  #if STEP_WAVE_FORM == PULSE
    digitalWriteF(stepPin, stepClr);
  #endif

  if (microstepModeControl >= MMC_SLEWING_PAUSE) return;

  #if STEP_WAVE_FORM == SQUARE
    if (takeStep) {
  #endif

  if (sync) targetSteps -= stepSize;

  if (motorSteps > targetSteps) {
    motorSteps -= stepSize;

    #ifdef SHARED_DIRECTION_PINS
      if (axisNumber > 2) { digitalWriteF(Pins->dir, direction); delayNanoseconds(pulseWidth); }
    #endif
    digitalWriteF(stepPin, stepSet);
  }

  #if STEP_WAVE_FORM == SQUARE
    } else digitalWriteF(stepPin, stepClr);
    takeStep = !takeStep;
  #endif
}

#endif
