// -----------------------------------------------------------------------------------
// Axis step/dir driver motion

#include "../Common.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;
#include "../telescope/Telescope.h"
#include "Axis.h"
#include "../lib/sense/Sense.h"

#if AXIS1_DRIVER_MODEL != OFF
  const AxisPins     Axis1Pins = {AXIS1_STEP_PIN, AXIS1_DIR_PIN, AXIS1_ENABLE_PIN, AXIS1_SENSE_LIMIT_MIN_PIN, AXIS1_SENSE_HOME_PIN, AXIS1_SENSE_LIMIT_MAX_PIN, false, false, true };
  const AxisSettings Axis1Settings = {AXIS1_STEPS_PER_DEGREE*RAD_DEG_RATIO, AXIS1_DRIVER_REVERSE, AXIS1_DRIVER_MICROSTEPS, AXIS1_DRIVER_IRUN , { degToRad(AXIS1_LIMIT_MIN), degToRad(AXIS1_LIMIT_MAX) }, { AXIS1_SENSE_HOME, AXIS1_SENSE_HOME_INIT, AXIS1_SENSE_LIMIT_MIN, AXIS1_SENSE_LIMIT_MAX, AXIS1_SENSE_LIMIT_INIT } };
  IRAM_ATTR void moveAxis1() { telescope.mount.axis1.move(AXIS1_STEP_PIN, AXIS1_DIR_PIN); }
  IRAM_ATTR void slewForwardAxis1() { telescope.mount.axis1.slewForward(AXIS1_STEP_PIN); }
  IRAM_ATTR void slewReverseAxis1() { telescope.mount.axis1.slewReverse(AXIS1_STEP_PIN); }
#endif
#if AXIS2_DRIVER_MODEL != OFF
  const AxisPins     Axis2Pins = {AXIS2_STEP_PIN, AXIS2_DIR_PIN, AXIS2_ENABLE_PIN, AXIS2_SENSE_LIMIT_MIN_PIN, AXIS2_SENSE_HOME_PIN, AXIS2_SENSE_LIMIT_MAX_PIN, false, false, true };
  const AxisSettings Axis2Settings = {AXIS2_STEPS_PER_DEGREE*RAD_DEG_RATIO, AXIS2_DRIVER_REVERSE, AXIS2_DRIVER_MICROSTEPS, AXIS2_DRIVER_IRUN, { degToRad(AXIS2_LIMIT_MIN), degToRad(AXIS2_LIMIT_MAX) }, { AXIS2_SENSE_HOME, AXIS2_SENSE_HOME_INIT, AXIS2_SENSE_LIMIT_MIN, AXIS2_SENSE_LIMIT_MAX, AXIS2_SENSE_LIMIT_INIT } };
  IRAM_ATTR void moveAxis2() { telescope.mount.axis2.move(AXIS2_STEP_PIN, AXIS2_DIR_PIN); }
  IRAM_ATTR void slewForwardAxis2() { telescope.mount.axis2.slewForward(AXIS2_STEP_PIN); }
  IRAM_ATTR void slewReverseAxis2() { telescope.mount.axis2.slewReverse(AXIS2_STEP_PIN); }
#endif
#if AXIS3_DRIVER_MODEL != OFF
  const AxisPins     Axis3Pins = {AXIS3_STEP_PIN, AXIS3_DIR_PIN, AXIS3_ENABLE_PIN, AXIS3_SENSE_LIMIT_MIN_PIN, AXIS3_SENSE_HOME_PIN, AXIS3_SENSE_LIMIT_MAX_PIN, false, false, true };
  const AxisSettings Axis3Settings = {AXIS3_STEPS_PER_DEGREE*RAD_DEG_RATIO, AXIS3_DRIVER_REVERSE, AXIS3_DRIVER_MICROSTEPS, AXIS3_DRIVER_IRUN, { degToRad(AXIS3_LIMIT_MIN), degToRad(AXIS3_LIMIT_MAX) }, { AXIS3_SENSE_HOME, AXIS3_SENSE_HOME_INIT, AXIS3_SENSE_LIMIT_MIN, AXIS3_SENSE_LIMIT_MAX, AXIS3_SENSE_LIMIT_INIT } };
  inline void moveAxis3() { telescope.rotator.axis.move(AXIS3_STEP_PIN, AXIS3_DIR_PIN); }
#endif
#if AXIS4_DRIVER_MODEL != OFF
  const AxisPins     Axis4Pins = {AXIS4_STEP_PIN, AXIS4_DIR_PIN, AXIS4_ENABLE_PIN, AXIS4_SENSE_LIMIT_MIN_PIN, AXIS4_SENSE_HOME_PIN, AXIS4_SENSE_LIMIT_MAX_PIN, false, false, true };
  const AxisSettings Axis4Settings = {AXIS4_STEPS_PER_MICRON, AXIS4_DRIVER_REVERSE, AXIS4_DRIVER_MICROSTEPS, AXIS4_DRIVER_IRUN, { degToRad(AXIS4_LIMIT_MIN), degToRad(AXIS4_LIMIT_MAX) }, { AXIS4_SENSE_HOME, AXIS4_SENSE_HOME_INIT, AXIS4_SENSE_LIMIT_MIN, AXIS4_SENSE_LIMIT_MAX, AXIS4_SENSE_LIMIT_INIT } };
  inline void moveAxis4() { telescope.focuser1.axis.move(AXIS4_STEP_PIN, AXIS4_DIR_PIN); }
#endif
#if AXIS5_DRIVER_MODEL != OFF
  const AxisPins     Axis5Pins = {AXIS5_STEP_PIN, AXIS5_DIR_PIN, AXIS5_ENABLE_PIN, AXIS5_SENSE_LIMIT_MIN_PIN, AXIS5_SENSE_HOME_PIN, AXIS5_SENSE_LIMIT_MAX_PIN, false, false, true };
  const AxisSettings Axis5Settings = {AXIS5_STEPS_PER_MICRON, AXIS5_DRIVER_REVERSE, AXIS5_DRIVER_MICROSTEPS, AXIS5_DRIVER_IRUN, { degToRad(AXIS5_LIMIT_MIN), degToRad(AXIS5_LIMIT_MAX) }, { AXIS5_SENSE_HOME, AXIS5_SENSE_HOME_INIT, AXIS5_SENSE_LIMIT_MIN, AXIS5_SENSE_LIMIT_MAX, AXIS5_SENSE_LIMIT_INIT } };
  inline void moveAxis5() { telescope.focuser2.axis.move(AXIS2_STEP_PIN, AXIS5_DIR_PIN); }
#endif
#if AXIS6_DRIVER_MODEL != OFF
  const AxisPins     Axis6Pins = {AXIS6_STEP_PIN, AXIS6_DIR_PIN, AXIS6_ENABLE_PIN, AXIS6_SENSE_LIMIT_MIN_PIN, AXIS6_SENSE_HOME_PIN, AXIS6_SENSE_LIMIT_MAX_PIN, false, false, true };
  const AxisSettings Axis6Settings = {AXIS6_STEPS_PER_MICRON, AXIS6_DRIVER_REVERSE, AXIS6_DRIVER_MICROSTEPS, AXIS6_DRIVER_IRUN, { degToRad(AXIS6_LIMIT_MIN), degToRad(AXIS6_LIMIT_MAX) }, { AXIS6_SENSE_HOME, AXIS6_SENSE_HOME_INIT, AXIS6_SENSE_LIMIT_MIN, AXIS6_SENSE_LIMIT_MAX, AXIS6_SENSE_LIMIT_INIT } };
  inline void moveAxis6() { telescope.focuser3.axis.move(AXIS6_STEP_PIN, AXIS6_DIR_PIN); }
#endif
#if AXIS7_DRIVER_MODEL != OFF
  const AxisPins     Axis7Pins = {AXIS7_STEP_PIN, AXIS7_DIR_PIN, AXIS7_ENABLE_PIN, AXIS7_SENSE_LIMIT_MIN_PIN, AXIS7_SENSE_HOME_PIN, AXIS7_SENSE_LIMIT_MAX_PIN, false, false, true };
  const AxisSettings Axis7Settings = {AXIS7_STEPS_PER_MICRON, AXIS7_DRIVER_REVERSE, AXIS7_DRIVER_MICROSTEPS, AXIS7_DRIVER_IRUN, { degToRad(AXIS7_LIMIT_MIN), degToRad(AXIS7_LIMIT_MAX) }, { AXIS7_SENSE_HOME, AXIS7_SENSE_HOME_INIT, AXIS7_SENSE_LIMIT_MIN, AXIS7_SENSE_LIMIT_MAX, AXIS7_SENSE_LIMIT_INIT } };
  inline void moveAxis7() { telescope.focuser4.axis.move(AXIS7_STEP_PIN, AXIS7_DIR_PIN); }
#endif
#if AXIS8_DRIVER_MODEL != OFF
  const AxisPins     Axis8Pins = {AXIS8_STEP_PIN, AXIS8_DIR_PIN, AXIS8_ENABLE_PIN, AXIS8_SENSE_LIMIT_MIN_PIN, AXIS8_SENSE_HOME_PIN, AXIS8_SENSE_LIMIT_MAX_PIN, false, false, true };
  const AxisSettings Axis8Settings = {AXIS8_STEPS_PER_MICRON, AXIS8_DRIVER_REVERSE, AXIS8_DRIVER_MICROSTEPS, AXIS8_DRIVER_IRUN, { degToRad(AXIS8_LIMIT_MIN), degToRad(AXIS8_LIMIT_MAX) }, { AXIS8_SENSE_HOME, AXIS8_SENSE_HOME_INIT, AXIS8_SENSE_LIMIT_MIN, AXIS8_SENSE_LIMIT_MAX, AXIS8_SENSE_LIMIT_INIT } };
  inline void moveAxis8() { telescope.focuser5.axis.move(AXIS8_STEP_PIN, AXIS8_DIR_PIN); }
#endif
#if AXIS9_DRIVER_MODEL != OFF
  const AxisPins     Axis9Pins = {AXIS9_STEP_PIN, AXIS9_DIR_PIN, AXIS9_ENABLE_PIN, AXIS9_SENSE_LIMIT_MIN_PIN, AXIS9_SENSE_HOME_PIN, AXIS9_SENSE_LIMIT_MAX_PIN, false, false, true };
  const AxisSettings Axis9Settings = {AXIS9_STEPS_PER_MICRON, AXIS9_DRIVER_REVERSE, AXIS9_DRIVER_MICROSTEPS, AXIS9_DRIVER_IRUN, { degToRad(AXIS9_LIMIT_MIN), degToRad(AXIS9_LIMIT_MAX) }, { AXIS9_SENSE_HOME, AXIS9_SENSE_HOME_INIT, AXIS9_SENSE_LIMIT_MIN, AXIS9_SENSE_LIMIT_MAX, AXIS9_SENSE_LIMIT_INIT } };
  inline void moveAxis8() { telescope.focuser5.axis.move(AXIS8_STEP_PIN, AXIS8_DIR_PIN); }
#endif

static uint8_t pollingTaskHandle  = 0;
inline void pollAxes() {
  #if AXIS1_DRIVER_MODEL != OFF
    telescope.mount.axis1.poll();
  #endif
  #if AXIS2_DRIVER_MODEL != OFF
    telescope.mount.axis2.poll();
  #endif
  #if AXIS3_DRIVER_MODEL != OFF
    telescope.mount.axis3.poll();
  #endif
  #if AXIS4_DRIVER_MODEL != OFF
    telescope.mount.axis4.poll();
  #endif
  #if AXIS5_DRIVER_MODEL != OFF
    telescope.mount.axis5.poll();
  #endif
  #if AXIS6_DRIVER_MODEL != OFF
    telescope.mount.axis6.poll();
  #endif
  #if AXIS7_DRIVER_MODEL != OFF
    telescope.mount.axis7.poll();
  #endif
  #if AXIS8_DRIVER_MODEL != OFF
    telescope.mount.axis8.poll();
  #endif
  #if AXIS9_DRIVER_MODEL != OFF
    telescope.mount.axis9.poll();
  #endif
}

void Axis::init(uint8_t axisNumber, bool validKey) {
  axisPrefix[9] = '0' + axisNumber;

  if (pollingTaskHandle == 0) {
    VF("MSG: All Axes, start polling task (rate 20ms priority 0)... ");
    pollingTaskHandle = tasks.add(20, 0, true, 0, pollAxes, "AxsPoll");
    if (pollingTaskHandle) { VL("success"); } else { VL("FAILED!"); }
  }

  this->axisNumber = axisNumber;
  void (*move)();

  taskHandle = 0;
  #if AXIS1_DRIVER_MODEL != OFF
    if (axisNumber == 1) { pins = Axis1Pins; settings = Axis1Settings; move = moveAxis1; }
  #endif
  #if AXIS1_DRIVER_MODEL != OFF
    if (axisNumber == 2) { pins = Axis2Pins; settings = Axis2Settings; move = moveAxis2; }
  #endif
  #if AXIS3_DRIVER_MODEL != OFF
    if (axisNumber == 3) { pins = Axis3Pins; settings = Axis3Settings; move = moveAxis3; }
  #endif
  #if AXIS4_DRIVER_MODEL != OFF
    if (axisNumber == 4) { pins = Axis4Pins; settings = Axis4Settings; move = moveAxis4; }
  #endif
  #if AXIS5_DRIVER_MODEL != OFF
    if (axisNumber == 5) { pins = Axis5Pins; settings = Axis5Settings; move = moveAxis5; }
  #endif
  #if AXIS6_DRIVER_MODEL != OFF
    if (axisNumber == 6) { pins = Axis6Pins; settings = Axis6Settings; move = moveAxis6; }
  #endif
  #if AXIS7_DRIVER_MODEL != OFF
    if (axisNumber == 7) { pins = Axis7Pins; settings = Axis7Settings; move = moveAxis7; }
  #endif
  #if AXIS8_DRIVER_MODEL != OFF
    if (axisNumber == 8) { pins = Axis8Pins; settings = Axis8Settings; move = moveAxis8; }
  #endif
  #if AXIS9_DRIVER_MODEL != OFF
    if (axisNumber == 9) { pins = Axis9Pins; settings = Axis9Settings; move = moveAxis9; }
  #endif

  V(axisPrefix); VF("start timer task... ");
  char timerName[] = "Axis0"; timerName[4] = '0' + axisNumber;
  taskHandle = tasks.add(0, 0, true, 0, move, timerName);
  if (taskHandle) {
    V("success");
    if (axisNumber <= 2) { if (!tasks.requestHardwareTimer(taskHandle, axisNumber, 0)) { VF(" (no hardware timer!)"); } }
    VL("");
  } else { VL("FAILED!"); }

  // init. axis settings
  if (!validKey) {
    V(axisPrefix); VLF("writing default settings to NV");
    uint16_t axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);
    bitSet(axesToRevert, axisNumber);
    nv.write(NV_AXIS_SETTINGS_REVERT, axesToRevert);
  }

  // bit 0 = settings at compile (0) or run time (1), bits 1 to 9 = reset axis n on next boot
  uint16_t axesToRevert = nv.readUI(NV_AXIS_SETTINGS_REVERT);
  if (!(axesToRevert & 1)) bitSet(axesToRevert, axisNumber);
  if (bitRead(axesToRevert, axisNumber)) nv.updateBytes(NV_AXIS_SETTINGS_BASE + (axisNumber - 1)*AxisSettingsSize, &settings, sizeof(AxisSettings));
  bitClear(axesToRevert, axisNumber);
  nv.write(NV_AXIS_SETTINGS_REVERT, axesToRevert);

  // get axis settings
  nv.readBytes(NV_AXIS_SETTINGS_BASE + (axisNumber - 1)*AxisSettingsSize, &settings, sizeof(AxisSettings));
  if (settings.currentRun != AXIS1_DRIVER_IRUN) {
    driver.settings.currentRun  = settings.currentRun;
    driver.settings.currentGoto = settings.currentRun;
    driver.settings.currentHold = settings.currentRun/2;
  }
  if (!validateAxisSettings(1, MOUNT_TYPE == ALTAZM, settings)) initError.value = true;

  V(axisPrefix); VLF("adding home and limit senses");
  hHomeSense = senses.add(pins.home, settings.sense.homeInit, settings.sense.home);
  hMinSense = senses.add(pins.min, settings.sense.minMaxInit, settings.sense.min);
  hMaxSense = senses.add(pins.max, settings.sense.minMaxInit, settings.sense.max);

  V(axisPrefix); VF("start move task... ");
  if (taskHandle) { VL("success"); } else { VL("FAILED!"); }

  driver.init(axisNumber);

  if (settings.reverse) invertDir = !invertDir;

  pinModeInitEx(pins.step, OUTPUT, !invertStep?LOW:HIGH);
  pinModeInitEx(pins.dir, OUTPUT, !invertDir?LOW:HIGH);
  pinModeEx(pins.enable, OUTPUT); enable(false);

  axisPrefix[10] = ':';
  axisPrefix[11] = ':';
}

void Axis::enableMoveFast(const bool fast) {
  #if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF
    if (fast) {
      disableBacklash();
      if (axisNumber == 1) {
        if (direction == DIR_FORWARD) tasks.setCallback(taskHandle, slewForwardAxis1); else tasks.setCallback(taskHandle, slewReverseAxis1);
      }
      if (axisNumber == 2) {
        if (direction == DIR_FORWARD) tasks.setCallback(taskHandle, slewForwardAxis2); else tasks.setCallback(taskHandle, slewReverseAxis2);
      }
    } else {
      if (axisNumber == 1) tasks.setCallback(taskHandle, moveAxis1);
      if (axisNumber == 2) tasks.setCallback(taskHandle, moveAxis2);
      enableBacklash();
    }
  #endif
}

#if STEP_WAVE_FORM == SQUARE
  IRAM_ATTR void Axis::move(const int8_t stepPin, const int8_t dirPin) {
    if (microstepModeControl == MMC_SLEWING_READY) return;
    if (takeStep) {
      if (direction == DIR_FORWARD) {
        if (backlashSteps < backlashAmountSteps) backlashSteps += step; else motorSteps += step;
        digitalWriteF(stepPin, HIGH);
      } else
      if (direction == DIR_REVERSE) {
        if (backlashSteps > 0) backlashSteps -= step; else motorSteps -= step;
        digitalWriteF(stepPin, HIGH);
      }
    } else {
      if (tracking) targetSteps += trackingStep;
      if (motorSteps + backlashSteps < targetSteps) {
        direction = DIR_FORWARD;
        digitalWriteF(dirPin, invertDir?HIGH:LOW);
      } else if (motorSteps + backlashSteps > targetSteps) {
        direction = DIR_REVERSE;
        digitalWriteF(dirPin, invertDir?LOW:HIGH);
      } else direction = DIR_NONE;
      digitalWriteF(stepPin, LOW);
      if (microstepModeControl == MMC_SLEWING_REQUEST && (motorSteps + backlashSteps)%stepsPerStepSlewing == 0) microstepModeControl = MMC_SLEWING_READY;
    }
    takeStep = !takeStep;
  }
  IRAM_ATTR void Axis::slewForward(const int8_t stepPin) {
    if (takeStep) {
      if (tracking) targetSteps += slewStep;
      if (motorSteps < targetSteps) { motorSteps += slewStep; digitalWriteF(stepPin, HIGH); }
    } else digitalWriteF(stepPin, LOW);
    takeStep = !takeStep;
  }
  IRAM_ATTR void Axis::slewReverse(const int8_t stepPin) {
    if (takeStep) {
      if (tracking) targetSteps -= slewStep;
      if (motorSteps > targetSteps) { motorSteps -= slewStep; digitalWriteF(stepPin, HIGH); }
    } else digitalWriteF(stepPin, LOW);
    takeStep = !takeStep;
  }
#endif
#if STEP_WAVE_FORM == PULSE
  IRAM_ATTR void Axis::move(const int8_t stepPin, const int8_t dirPin) {
    if (microstepModeControl == MMC_SLEWING_READY) return;
    digitalWriteF(stepPin, LOW);
    if (tracking) targetSteps += trackingStep;
    if (motorSteps + backlashSteps < targetSteps) {
      digitalWriteF(dirPin, invertDir?HIGH:LOW);
      direction = DIR_FORWARD;
      if (backlashSteps < backlashAmountSteps) backlashSteps += step; else motorSteps += step;
    } else
    if (motorSteps + backlashSteps > targetSteps) {
      digitalWriteF(dirPin, invertDir?LOW:HIGH);
      direction = DIR_REVERSE;
      if (backlashSteps > 0) backlashSteps -= step; else motorSteps -= step;
    } else { direction = DIR_NONE; return; }
    digitalWriteF(stepPin, HIGH);
    if (microstepModeControl == MMC_SLEWING_REQUEST && (motorSteps + backlashSteps)%stepsPerStepSlewing == 0) microstepModeControl = MMC_SLEWING_READY;
  }
  IRAM_ATTR void Axis::slewForward(const int8_t stepPin) {
    digitalWriteF(stepPin, LOW);
    if (tracking) targetSteps += slewStep;
    if (motorSteps < targetSteps) { motorSteps += slewStep; digitalWriteF(stepPin, HIGH); }
  }
  IRAM_ATTR void Axis::slewReverse(const int8_t stepPin) {
    digitalWriteF(stepPin, LOW);
    if (tracking) targetSteps -= slewStep;
    if (motorSteps > targetSteps) { motorSteps -= slewStep; digitalWriteF(stepPin, HIGH); }
  }
#endif
