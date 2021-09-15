// -----------------------------------------------------------------------------------
// axis step/dir driver motion

#include "StepDir.h"

#ifdef SD_DRIVER_PRESENT

#include "../../../../tasks/OnTask.h"
extern Tasks tasks;
#include "../../../Telescope.h"
#include "StepDir.defaults.h"

#ifdef MOUNT_PRESENT
  extern unsigned long periodSubMicros;
#else
  #define periodSubMicros 1.0
#endif

#ifdef AXIS1_DRIVER_SD
  const StepDirPins PinsAxis1 = { AXIS1_STEP_PIN, AXIS1_STEP_STATE, AXIS1_DIR_PIN, AXIS1_ENABLE_PIN, AXIS1_ENABLE_STATE };
  IRAM_ATTR void moveAxis1() { motor1.move(AXIS1_STEP_PIN, AXIS1_DIR_PIN); }
  IRAM_ATTR void moveFFAxis1() { motor1.moveFF(AXIS1_STEP_PIN); }
  IRAM_ATTR void moveFRAxis1() { motor1.moveFR(AXIS1_STEP_PIN); }
  StepDirMotor motor1(1, &PinsAxis1, &stepDirDriver1, &moveAxis1, &moveFFAxis1, &moveFRAxis1);
#endif
#ifdef AXIS2_DRIVER_SD
  const StepDirPins PinsAxis2 = { AXIS2_STEP_PIN, AXIS2_STEP_STATE, AXIS2_DIR_PIN, AXIS2_ENABLE_PIN, AXIS2_ENABLE_STATE };
  IRAM_ATTR void moveAxis2() { motor2.move(AXIS2_STEP_PIN, AXIS2_DIR_PIN); }
  IRAM_ATTR void moveFFAxis2() { motor2.moveFF(AXIS2_STEP_PIN); }
  IRAM_ATTR void moveFRAxis2() { motor2.moveFR(AXIS2_STEP_PIN); }
  StepDirMotor motor2(2, &PinsAxis2, &stepDirDriver2, &moveAxis2, &moveFFAxis2, &moveFRAxis2);
#endif
#ifdef AXIS3_DRIVER_SD
  const StepDirPins PinsAxis3 = { AXIS3_STEP_PIN, AXIS3_STEP_STATE, AXIS3_DIR_PIN, AXIS3_ENABLE_PIN, AXIS3_ENABLE_STATE };
  void moveAxis3() { motor3.move(AXIS3_STEP_PIN, AXIS3_DIR_PIN); }
  StepDirMotor motor3(3, &PinsAxis3, &stepDirDriver3, &moveAxis3);
#endif
#ifdef AXIS4_DRIVER_SD
  const StepDirPins PinsAxis4 = { AXIS4_STEP_PIN, AXIS4_STEP_STATE, AXIS4_DIR_PIN, AXIS4_ENABLE_PIN, AXIS4_ENABLE_STATE };
  void moveAxis4() { motor4.move(AXIS4_STEP_PIN, AXIS4_DIR_PIN); }
  StepDirMotor motor4(4, &PinsAxis4, &stepDirDriver4, &moveAxis4);
#endif
#ifdef AXIS5_DRIVER_SD
  const StepDirPins PinsAxis5 = { AXIS5_STEP_PIN, AXIS5_STEP_STATE, AXIS5_DIR_PIN, AXIS5_ENABLE_PIN, AXIS5_ENABLE_STATE };
  void moveAxis5() { motor5.move(AXIS5_STEP_PIN, AXIS5_DIR_PIN); }
  StepDirMotor motor5(5, &PinsAxis5, &stepDirDriver5, &moveAxis5);
#endif
#ifdef AXIS6_DRIVER_SD
  const StepDirPins PinsAxis6 = { AXIS6_STEP_PIN, AXIS6_STEP_STATE, AXIS6_DIR_PIN, AXIS6_ENABLE_PIN, AXIS6_ENABLE_STATE };
  void moveAxis6() { motor6.move(AXIS6_STEP_PIN, AXIS6_DIR_PIN); }
  StepDirMotor motor6(6, &PinsAxis6, &stepDirDriver6, &moveAxis6);
#endif
#ifdef AXIS7_DRIVER_SD
  const StepDirPins PinsAxis7 = { AXIS7_STEP_PIN, AXIS7_STEP_STATE, AXIS7_DIR_PIN, AXIS7_ENABLE_PIN, AXIS7_ENABLE_STATE };
  void moveAxis7() { motor7.move(AXIS7_STEP_PIN, AXIS7_DIR_PIN); }
  StepDirMotor motor7(7, &PinsAxis7, &stepDirDriver7, &moveAxis7);
#endif
#ifdef AXIS8_DRIVER_SD
  const StepDirPins PinsAxis8 = { AXIS8_STEP_PIN, AXIS8_STEP_STATE, AXIS8_DIR_PIN, AXIS8_ENABLE_PIN, AXIS8_ENABLE_STATE };
  void moveAxis8() { motor8.move(AXIS8_STEP_PIN, AXIS8_DIR_PIN); }
  StepDirMotor motor8(8, &PinsAxis8, &stepDirDriver8, &moveAxis8);
#endif
#ifdef AXIS9_DRIVER_SD
  const StepDirPins PinsAxis9 = { AXIS9_STEP_PIN, AXIS9_STEP_STATE, AXIS9_DIR_PIN, AXIS9_ENABLE_PIN, AXIS9_ENABLE_STATE };
  void moveAxis9() { motor9.move(AXIS9_STEP_PIN, AXIS9_DIR_PIN); }
  StepDirMotor motor9(9, &PinsAxis9, &stepDirDriver9, &moveAxis9);
#endif

StepDirMotor::StepDirMotor(uint8_t axisNumber, const StepDirPins *Pins, StepDirDriver *driver,
                           void (*volatile move)(), void (*volatile moveFF)(), void (*volatile moveFR)()) {
  axisPrefix[10] = '0' + axisNumber;
  driverType = STEP_DIR;
  this->axisNumber = axisNumber;
  this->Pins = Pins;
  this->driver = driver;
  this->_move = move;
  this->_moveFF = moveFF;
  this->_moveFR = moveFR;
}

bool StepDirMotor::init(int8_t reverse, int16_t microsteps, int16_t current) {
  if (_move == NULL) { D(axisPrefix); DLF("nothing to do exiting!"); return false; }

  // get the axis monitor handle, by name
  char monitorName[] = "mtrAx_";
  monitorName[5] = axisNumber + '0';
  mtrHandle = tasks.getHandleByName(monitorName);
  if (mtrHandle == 0) { D(axisPrefix); DLF("no axis monitor, exiting!"); return false; }

  #if DEBUG == VERBOSE
    V(axisPrefix); V("init step="); if (Pins->step == OFF) V("OFF"); else V(Pins->step);
    V(", dir="); if (Pins->dir == OFF) V("OFF"); else V(Pins->dir);
    V(", en="); if (Pins->enable == OFF) VL("OFF"); else if (Pins->enable == SHARED) VL("SHARED"); else VL(Pins->enable);
  #endif

  // this driver requires available pins to function
  if (Pins->dir == OFF || Pins->step == OFF) {
    D(axisPrefix); DLF("step or dir pin not present, exiting!");
    return false;
  }

  // init default driver direction state (forward)
  if (reverse == OFF) { dirFwd = LOW; dirRev = HIGH; } else { dirFwd = HIGH; dirRev = LOW; }
  pinMode(Pins->dir, OUTPUT);
  digitalWriteF(Pins->dir, dirFwd);

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

  // init driver advanced modes, etc.
  driver->init(microsteps, current);
  homeSteps = driver->getMicrostepRatio();
  V(axisPrefix); V("sequencer homes every "); V(homeSteps); VL(" step(s)");

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
      // adjust period for MCU clock inaccuracy (signed 32bit numeric range covers about +/- 3% here)
      period *= (SIDEREAL_PERIOD/periodSubMicros);
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
        V(axisPrefix); VF("high speed ISR swapped in at "); V(lastFrequency); VL(" steps/sec.");
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

#if STEP_WAVE_FORM == SQUARE
  IRAM_ATTR void StepDirMotor::move(const int8_t stepPin, const int8_t dirPin) {
    #ifdef SHARED_DIRECTION_PINS
      if (axisNumber > 2 && takeStep) {
        digitalWriteF(dirPin, direction);
        delayMicroseconds(1);
      }
    #endif

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
      if (direction == dirFwd || inBacklash) {
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
      if (motorSteps > targetSteps) direction = dirRev; else
      if (motorSteps < targetSteps) direction = dirFwd; else direction = 255;
      #ifdef SHARED_DIRECTION_PINS
        if (axisNumber <= 2) digitalWriteF(dirPin, direction);
      #else
        digitalWriteF(dirPin, direction);
      #endif
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
  IRAM_ATTR void StepDirMotor::move(const int8_t stepPin, const int8_t dirPin) {
    digitalWriteF(stepPin, stepClr);

    if (microstepModeControl == MMC_SLEWING_REQUEST && (motorSteps + backlashSteps) % homeSteps == 0) {
      microstepModeControl = MMC_SLEWING_PAUSE;
      tasks.immediate(mtrHandle);
    }
    if (microstepModeControl >= MMC_SLEWING_PAUSE) return;

    if (synchronized && !inBacklash) targetSteps += step;

    if (motorSteps > targetSteps) direction = dirRev; else
    if (motorSteps < targetSteps) direction = dirFwd; else direction = 255;
    digitalWriteF(dirPin, direction);

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
    if (direction == dirFwd || inBacklash) {
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