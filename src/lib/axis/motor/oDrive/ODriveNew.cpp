// -----------------------------------------------------------------------------------
// oDrive servo motor

#include "ODriveNew.h"

#ifdef ODRIVE_NEW_MOTOR_PRESENT
#ifdef ODRIVE_MOTOR_PRESENT

#include "../../../tasks/OnTask.h"
#include "../../../gpioEx/GpioEx.h"

#include "ODriveCanPlus.h"

// constructor
ODriveMotor::ODriveMotor(uint8_t axisNumber, int8_t reverse, const ODriveDriverSettings *Settings, float countsPerRad, bool useFastHardwareTimers)
                         :Motor(axisNumber, reverse) {
  if (axisNumber < 1 || axisNumber > 2) return;

  driverType = ODRIVER;

  this->axisNumber = axisNumber;
  this->useFastHardwareTimers = useFastHardwareTimers;

  #if ODRIVE_SWAP_AXES == ON
    oDriveNode = 2 - axisNumber;
  #else
    oDriveNode = axisNumber - 1;
  #endif

  strcpy(axisPrefix, " Axis_oDrive, ");
  axisPrefix[5] = '0' + this->axisNumber;

  // set parameter default values during object creation
  this->countsPerRad.valueDefault = countsPerRad;

  // attach the function pointers to the callbacks
  odriveMotorInstance[axisNumber - 1] = this;
  switch (this->axisNumber) {
    case 1: callback = moveODriveMotorAxis1; break;
    case 2: callback = moveODriveMotorAxis2; break;
  }
}

bool ODriveMotor::init() {
  if (!Motor::init()) return false;

  // reset the ODrive(s) via h/w pin
  if (axisNumber == 1) {
    pinModeEx(ODRIVE_RST_PIN, OUTPUT);
    digitalWriteEx(ODRIVE_RST_PIN, LOW);
    tasks.yield(10);
    digitalWriteEx(ODRIVE_RST_PIN, HIGH);
    // allow time for boot
    tasks.yield(1000);
  }

  if (!odriveCan.init(oDriveNode)) return false;
  odriveCan.requestVersion(oDriveNode);

  const uint32_t t0 = millis();
  while (!odriveCan.hasFreshVersion(oDriveNode, 1000) && (uint32_t)(millis() - t0) < 1000U) { tasks.yield(10); }

  if (odriveCan.hasFreshVersion(oDriveNode, 1000)) {
    uint8_t raw[8];
    uint16_t proto;
    uint8_t maj, min, rev, hwv, hww;

    if (odriveCan.getVersionSnapshot(oDriveNode, raw, proto, maj, min, rev, hwv, hww)) {
      VF("MSG:"); V(axisPrefix); VF("ODrive version: node "); V(oDriveNode);
      VF(" proto "); V(proto);
      VF(" fw "); V(maj); VF("."); V(min); VF("."); V(rev);
      VF(" hwVar "); V(hwv);
      VF(" hwVer "); V(hww);
      VLF("");

      VF("MSG:"); V(axisPrefix); VF("ODrive version raw: ");
      for (int i = 0; i < 8; i++) {
        if (raw[i] < 16) V("0");
        VH(raw[i]);
        if (i != 7) V(" ");
      }
      VLF("");
    }
  }

  odriveCan.setAxisState(oDriveNode, AXIS_STATE_IDLE);

  // request an encoder position update
  odriveCan.requestEncoder(oDriveNode);
  lastEncoderUpdateRequestTime = millis();
  // let CAN poll task (prio 3) run and deliver the reply
  tasks.yield(250);
  // require an encoder sample
  if (!odriveCan.hasFreshEncoder(oDriveNode, 250)) {
    VF("MSG:"); V(axisPrefix); VLF("no encoder reply at startup");
    ready = false;
    return false;
  }

  // local state: definitely disabled, no transition pending
  ready = true;
  enabled = false;
  enablePending = false;
  enableTarget = false;
  enableDeadlineMs = 0;

  // start the motion timer
  VF("MSG:"); V(axisPrefix); VF("start task to synthesize motion... ");
  char timerName[] = "Ax_ODr";
  timerName[2] = axisNumber + '0';
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
    ready = false;
    return false;
  }

  stopSyntheticMotion();

  return true;
}

// set motor reverse state
void ODriveMotor::setReverse(bool state) {
  if (!ready) return;

  if (state) {
    VF("MSG:"); V(axisPrefix); VLF("axis reversal must be accomplished with hardware or ODrive setup!");
  }
}

// sets motor enable on/off (if possible)
void ODriveMotor::enable(bool state) {
  if (!ready) return;

  // If we're already in the requested steady-state AND no transition is pending, do nothing.
  if (!enablePending && (state == enabled)) return;

  // Always reset local motion first
  if (!state) enabled = false;
  stopSyntheticMotion();
  resetToTrackingBaseline();

  const uint32_t requestedState = state ? AXIS_STATE_CLOSED_LOOP_CONTROL
                                        : AXIS_STATE_IDLE;

  if (!odriveCan.setAxisState(oDriveNode, requestedState)) {
    enabled = false;
    enablePending = false;
    return;
  }

  if (!state) {
    // disable: treat as immediate locally; no need to wait for heartbeats/encoder
    enabled = false;
    enablePending = false;
    enableTarget = false;
    enableDeadlineMs = 0;
    return;
  }

  // Enable: wait for confirmation
  enablePending = true;
  enableTarget = true;
  enableDeadlineMs = millis() + 500;
}

void ODriveMotor::setInstrumentCoordinateSteps(long value) {
  if (!ready) return;

  #if ODRIVE_ABSOLUTE == ON && ODRIVE_SYNC_LIMIT != OFF
    long index = value - ATOMIC_LOAD(motorSteps);
    float indexRad = index/countsPerRad.value;
    if (indexRad >= -degToRadF(ODRIVE_SYNC_LIMIT/3600.0F) && indexRad <= degToRadF(ODRIVE_SYNC_LIMIT/3600.0F))
  #endif

  Motor::setInstrumentCoordinateSteps(value);
}

// resets motor and target angular position in steps, also zeros backlash and index
void ODriveMotor::resetPositionSteps(long value) {
  if (!ready) return;

  // make sure we have a recent encoder reading
  if (!odriveCan.hasFreshEncoder(oDriveNode, 250)) {
    status.fault = true;
    odriveCan.requestEncoder(oDriveNode);
    return;
  } else {
    status.fault = false;
  }

  // Convert turns -> steps (countsPerRad is counts-per-radian)
  const float posTurns = odriveCan.getPosTurns(oDriveNode);
  const long  oPositionSteps = lroundf(posTurns * TWO_PI * countsPerRad.value);

  noInterrupts();
  motorSteps  = oPositionSteps;
  targetSteps = motorSteps;

  #if ODRIVE_ABSOLUTE == OFF
    // For incremental encoders, keep an index offset so OnStepX can still align to "value".
    indexSteps = value - motorSteps;
  #else
    UNUSED(value);
    indexSteps = 0;
  #endif

  backlashSteps = 0;
  interrupts();
}

// set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
void ODriveMotor::setFrequencySteps(float frequency) {
  if (!ready) return;

  if (!enabled) { stopSyntheticMotion(); return; }

  // negative frequency, convert to positive and reverse the direction
  int dir = 0;
  if (frequency > 0.0F) dir = 1; else if (frequency < 0.0F) { frequency = -frequency; dir = -1; }

  // if in backlash override the frequency
  if (inBacklash) frequency = backlashFrequency;

  if (frequency != currentFrequency) {
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

    // change the motor rate/direction
    noInterrupts();
    step = 0;
    interrupts();
    tasks.setPeriodSubMicros(taskHandle, lastPeriod);
  }

  noInterrupts();
  step = dir * stepSize;
  absStep = (int)labs(step);
  interrupts();

  if (dir == 0 || lastPeriod == 0 || frequency == 0.0F) {
    feedForwardVelocity = 0.0F;
  } else {
    feedForwardVelocity = (dir > 0) ? frequency : -frequency;
  }
}

float ODriveMotor::getFrequencySteps() {
  if (!ready) return 0.0F;

  if (lastPeriod == 0) return 0.0F;
  return (16000000.0F / lastPeriod) * absStep;
}

// set slewing state (hint that we are about to slew or are done slewing)
void ODriveMotor::setSlewing(bool state) {
  if (!ready) return;

  isSlewing = state;
}

// updates PID and sets odrive position
void ODriveMotor::poll() {
  const uint32_t now = millis();

  // grab a new encoder position update at 10Hz
  if ((uint32_t)(now - lastEncoderUpdateRequestTime) >= 100U) {
    odriveCan.requestEncoder(oDriveNode);
    lastEncoderUpdateRequestTime = now;
  }

  // -------------------------
  // handle enable/disable handshake (async confirmation)
  // -------------------------
  if (enablePending) {
    // snapshot ODrive status
    const uint8_t  st  = odriveCan.axisState(oDriveNode);
    const uint32_t err = odriveCan.lastError(oDriveNode);

    // hard faults during transition: lose heartbeat or error latched
    if (!odriveCan.isAlive(oDriveNode, 250) || (err != 0)) {
      enabled = false;
      enablePending = false;
      stopSyntheticMotion();
      resetToTrackingBaseline();
      return;
    }

    // commit when we observe the desired steady state
    if (enableTarget) {
      if (st == AXIS_STATE_CLOSED_LOOP_CONTROL) {
        enabled = true;
        enablePending = false;
      }
    } else {
      if (st == AXIS_STATE_IDLE) {
        enabled = false;
        enablePending = false;
      }
    }

    // deadline: if still pending after the window, fail closed
    if (enablePending && (int32_t)(now - enableDeadlineMs) >= 0) {
      enabled = false;
      enablePending = false;
      stopSyntheticMotion();
      resetToTrackingBaseline();
      return;
    }
  }

  // -------------------------
  // normal operation (only when truly enabled)
  // -------------------------
  if (!ready || !enabled) return;

  // fail closed if we lose comms or an error appears while running
  // (no encoder freshness requirement in this variant)
  if (!odriveCan.isAlive(oDriveNode, 250) || (odriveCan.lastError(oDriveNode) != 0)) {
    enabled = false;
    enablePending = false;
    stopSyntheticMotion();
    resetToTrackingBaseline();
    return;
  }

  // rate-limit position commands
  if (now - lastSetPositionTime < ODRIVE_UPDATE_MS) return;
  lastSetPositionTime = now;

  long target;
  noInterrupts();
  #if ODRIVE_SLEW_DIRECT == ON
    target = targetSteps + backlashSteps;
  #else
    target = motorSteps + backlashSteps;
  #endif
  interrupts();

  float vel_ff_turns_s = feedForwardVelocity/(TWO_PI*countsPerRad.value);

  odriveCan.setInputPos(oDriveNode, target/(TWO_PI*countsPerRad.value), vel_ff_turns_s);
}

void ODriveMotor::stopSyntheticMotion() {
  feedForwardVelocity = 0.0F;

  if (lastPeriod == 0 && step == 0 && absStep == 0) return;
  currentFrequency = 0.0F;
  lastPeriod = 0;
  feedForwardVelocity = 0.0F;
  noInterrupts();
  step = 0;
  absStep = 0;
  interrupts();
  if (taskHandle) tasks.setPeriodSubMicros(taskHandle, 0);
}

void ODriveMotor::resetToTrackingBaseline() {
  noInterrupts();
  step = 0;
  absStep = 0;
  inBacklash = false;
  backlashSteps = 0;
  interrupts();

  currentFrequency = 0.0F;
  lastPeriod = 0;
  stepSize = 1;
  feedForwardVelocity = 0.0F;
}

// sets dir as required and moves coord toward target at setFrequencySteps() rate
IRAM_ATTR void ODriveMotor::move() {
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
}

#endif
#endif