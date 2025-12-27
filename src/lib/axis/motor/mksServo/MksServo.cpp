// -----------------------------------------------------------------------------------
// axis MKS SERVO42D/57D motor driver via CAN, using 0xFE absolute position updates
// EXPERIMENTAL!!!

#include "MksServo.h"

#ifdef MKS42D_MOTOR_PRESENT

#include "../../../tasks/OnTask.h"
#include "../../../canPlus/CanPlus.h"

static Mks42DMotor *mksMotorInstance[9] = {nullptr};

IRAM_ATTR void moveMksAxis1() { mksMotorInstance[0]->move(); }
IRAM_ATTR void moveMksAxis2() { mksMotorInstance[1]->move(); }
IRAM_ATTR void moveMksAxis3() { mksMotorInstance[2]->move(); }
IRAM_ATTR void moveMksAxis4() { mksMotorInstance[3]->move(); }
IRAM_ATTR void moveMksAxis5() { mksMotorInstance[4]->move(); }
IRAM_ATTR void moveMksAxis6() { mksMotorInstance[5]->move(); }
IRAM_ATTR void moveMksAxis7() { mksMotorInstance[6]->move(); }
IRAM_ATTR void moveMksAxis8() { mksMotorInstance[7]->move(); }
IRAM_ATTR void moveMksAxis9() { mksMotorInstance[8]->move(); }

// Status callbacks (0xF1 and 0x3E both funnel into requestStatusCallback)
static void statusMksAxis1(uint8_t data[8]) { if (mksMotorInstance[0]) mksMotorInstance[0]->requestStatusCallback(data); }
static void statusMksAxis2(uint8_t data[8]) { if (mksMotorInstance[1]) mksMotorInstance[1]->requestStatusCallback(data); }
static void statusMksAxis3(uint8_t data[8]) { if (mksMotorInstance[2]) mksMotorInstance[2]->requestStatusCallback(data); }
static void statusMksAxis4(uint8_t data[8]) { if (mksMotorInstance[3]) mksMotorInstance[3]->requestStatusCallback(data); }
static void statusMksAxis5(uint8_t data[8]) { if (mksMotorInstance[4]) mksMotorInstance[4]->requestStatusCallback(data); }
static void statusMksAxis6(uint8_t data[8]) { if (mksMotorInstance[5]) mksMotorInstance[5]->requestStatusCallback(data); }
static void statusMksAxis7(uint8_t data[8]) { if (mksMotorInstance[6]) mksMotorInstance[6]->requestStatusCallback(data); }
static void statusMksAxis8(uint8_t data[8]) { if (mksMotorInstance[7]) mksMotorInstance[7]->requestStatusCallback(data); }
static void statusMksAxis9(uint8_t data[8]) { if (mksMotorInstance[8]) mksMotorInstance[8]->requestStatusCallback(data); }

Mks42DMotor::Mks42DMotor(uint8_t axisNumber,
                         int8_t reverse,
                         const MksDriverSettings *Settings,
                         bool useFastHardwareTimers)
  : Motor(axisNumber, reverse) {

  if (axisNumber < 1 || axisNumber > 9) return;

  driverType = ODRIVER;

  strcpy(axisPrefix, " Axis_Mks42D, ");
  axisPrefix[5] = '0' + axisNumber;

  // Mapping policy: CAN ID == axis number (1..9)
  canID = axisNumber;

  // reserve settings for future protocol/status differences
  if (Settings) {
    model = Settings->model;
    statusMode = Settings->status;
  } else {
    model = 0;
    statusMode = OFF;
  }

  // AxisParameter defaults (must be assigned here)
  // stepsPerSecondToRpm is intentionally *not* used in the simple/robust mode below.
  stepsPerSecondToRpm.valueDefault = 0.01F;
  maxRpm.valueDefault              = 60.0F;

  if (axisNumber > 2) useFastHardwareTimers = false;
  this->useFastHardwareTimers = useFastHardwareTimers;

  mksMotorInstance[axisNumber - 1] = this;
  switch (axisNumber) {
    case 1: callback = moveMksAxis1; break;
    case 2: callback = moveMksAxis2; break;
    case 3: callback = moveMksAxis3; break;
    case 4: callback = moveMksAxis4; break;
    case 5: callback = moveMksAxis5; break;
    case 6: callback = moveMksAxis6; break;
    case 7: callback = moveMksAxis7; break;
    case 8: callback = moveMksAxis8; break;
    case 9: callback = moveMksAxis9; break;
  }
}

bool Mks42DMotor::init() {
  if (!Motor::init()) return false;

  if (!canPlus.ready) {
    V("ERR:"); V(axisPrefix); VLF("No CAN interface!");
    return false;
  }

  ready = true;
  enable(false);

  VF("MSG:"); V(axisPrefix); VF("start task to synthesize motion... ");
  char timerName[] = "Ax_Mks_";
  timerName[2] = axisNumber + '0';

  taskHandle = tasks.add(0, 0, true, 0, callback, timerName);
  if (taskHandle) {
    VF("success");
    if (useFastHardwareTimers) {
      if (!tasks.requestHardwareTimer(taskHandle, 0)) {
        VF(" (no hardware timer!)");
      } else {
        maxFrequency = (1000000.0F / HAL_MAXRATE_LOWER_LIMIT) / 2.0F;
      }
    }
    VLF("");
  } else {
    VLF("FAILED!");
    ready = false;
    return false;
  }

  stopSyntheticMotion();
  resetToTrackingBaseline();

  // Always-on heartbeat + protect polling (RX callbacks)
  if (statusMode == ON) {
    switch (axisNumber) {
      case 1:
        canPlus.callbackRegisterMessage(canID, 0xF1, statusMksAxis1);
        canPlus.callbackRegisterMessage(canID, 0x3E, statusMksAxis1);
        break;
      case 2:
        canPlus.callbackRegisterMessage(canID, 0xF1, statusMksAxis2);
        canPlus.callbackRegisterMessage(canID, 0x3E, statusMksAxis2);
        break;
      case 3:
        canPlus.callbackRegisterMessage(canID, 0xF1, statusMksAxis3);
        canPlus.callbackRegisterMessage(canID, 0x3E, statusMksAxis3);
        break;
      case 4:
        canPlus.callbackRegisterMessage(canID, 0xF1, statusMksAxis4);
        canPlus.callbackRegisterMessage(canID, 0x3E, statusMksAxis4);
        break;
      case 5:
        canPlus.callbackRegisterMessage(canID, 0xF1, statusMksAxis5);
        canPlus.callbackRegisterMessage(canID, 0x3E, statusMksAxis5);
        break;
      case 6:
        canPlus.callbackRegisterMessage(canID, 0xF1, statusMksAxis6);
        canPlus.callbackRegisterMessage(canID, 0x3E, statusMksAxis6);
        break;
      case 7:
        canPlus.callbackRegisterMessage(canID, 0xF1, statusMksAxis7);
        canPlus.callbackRegisterMessage(canID, 0x3E, statusMksAxis7);
        break;
      case 8:
        canPlus.callbackRegisterMessage(canID, 0xF1, statusMksAxis8);
        canPlus.callbackRegisterMessage(canID, 0x3E, statusMksAxis8);
        break;
      case 9:
        canPlus.callbackRegisterMessage(canID, 0xF1, statusMksAxis9);
        canPlus.callbackRegisterMessage(canID, 0x3E, statusMksAxis9);
        break;
    }

    statusValid = false;
    protectActive = false;
    status.fault = false;
    lastStatusUpdateTime = 0;
    lastStatusRequestTime = 0;
    lastProtectRequestTime = 0;
  }

  return true;
}

void Mks42DMotor::setReverse(int8_t state) {
  if (!ready) return;
  if (state == ON) {
    V("WRN:"); V(axisPrefix); VLF("axis reversal must be done in MKS setup or wiring!");
  }
}

void Mks42DMotor::setReverse(bool state) {
  if (state) {
    V("WRN:"); V(axisPrefix); VLF("software reverse requested; prefer MKS setup or wiring!");
  }
  Motor::setReverse(state);
}

void Mks42DMotor::enable(bool state) {
  if (!ready || state == enabled) return;

  if (!state) enabled = false;

  stopSyntheticMotion();
  resetToTrackingBaseline();

  VF("MSG:"); V(axisPrefix); VF("driver powered "); VLF(state ? "up" : "down");

  canPlus.txWait();
  sendF3(state);

  if (state) {
    enabled = true;
    lastEnableTime = millis();

    // clear latched faults on enable attempt (simple policy for v1)
    status.fault = false;
    protectActive = false;
    // statusValid remains as-is; always-on polling will refresh it soon
  }

  // force next poll() to send target
  lastTarget = LONG_MIN;
}

void Mks42DMotor::resetPositionSteps(long value) {
  if (!ready) return;
  Motor::resetPositionSteps(value);

  // force next poll() to send target
  lastTarget = LONG_MIN;
}

float Mks42DMotor::getFrequencySteps() {
  if (!ready) return 0.0F;
  if (lastPeriod == 0) return 0.0F;
  return (16000000.0F / lastPeriod) * absStep;
}

void Mks42DMotor::setFrequencySteps(float frequency) {
  if (!ready) return;
  if (!enabled) { stopSyntheticMotion(); return; }

  int dir = 0;
  if (frequency > 0.0F) dir = 1;
  else if (frequency < 0.0F) { frequency = -frequency; dir = -1; }

  if (inBacklash) frequency = backlashFrequency;

  if (frequency != currentFrequency) {
    // compensate for performance limitations by taking larger steps as needed
    if (frequency < maxFrequency) stepSize = 1; else
    if (frequency < maxFrequency*2) stepSize = 2; else
    if (frequency < maxFrequency*4) stepSize = 4; else
    if (frequency < maxFrequency*8) stepSize = 8; else
    if (frequency < maxFrequency*16) stepSize = 16; else
    if (frequency < maxFrequency*32) stepSize = 32; else
    if (frequency < maxFrequency*64) stepSize = 64; else
    if (frequency < maxFrequency*128) stepSize = 128; else stepSize = 256;

    // timer period in microseconds
    float period = (1000000.0F * stepSize) / frequency;

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
}

void Mks42DMotor::setSlewing(bool state) {
  if (!ready) return;
  isSlewing = state;
}

// ---- CAN helpers ----

inline void Mks42DMotor::sendWithCrc(const uint8_t *payload, size_t n) {
  if (!payload || n < 1 || n > 7) return;

  uint8_t buf[8] = {0};
  memcpy(buf, payload, n);
  buf[n] = crc8((uint16_t)canID, buf, n);

  canPlus.writePacket(canID, buf, n + 1);
}

inline void Mks42DMotor::sendF3(bool en) {
  const uint8_t p[2] = { 0xF3, (uint8_t)(en ? 1 : 0) };
  sendWithCrc(p, 2);
}

inline void Mks42DMotor::sendFE(int32_t pos, uint16_t speed) {
  const int32_t p = clamp24(pos);
  const uint32_t u = (uint32_t)p & 0x00FFFFFFUL; // signed 24-bit in low 24 bits

  const uint8_t payload[7] = {
    0xFE,
    (uint8_t)(speed >> 8),
    (uint8_t)(speed & 0xFF),
    (uint8_t)MKS_FE_ACCEL_CONST,
    (uint8_t)((u >> 16) & 0xFF),
    (uint8_t)((u >>  8) & 0xFF),
    (uint8_t)((u >>  0) & 0xFF),
  };

  sendWithCrc(payload, 7);
}

// Always-on polling commands
inline void Mks42DMotor::sendF1() { const uint8_t p[1] = { 0xF1 }; sendWithCrc(p, 1); }
inline void Mks42DMotor::send3E() { const uint8_t p[1] = { 0x3E }; sendWithCrc(p, 1); }

// RX: treat 0xF1 and 0x3E as heartbeat sources; latch fault on protect active.
void Mks42DMotor::requestStatusCallback(uint8_t data[8]) {
  if (statusMode == OFF) return;

  const uint8_t code = data[0];
  if (code != 0xF1 && code != 0x3E) return;

  lastStatusUpdateTime = millis();
  statusValid = true;

  if (code == 0x3E) {
    protectActive = (data[1] != 0);
    if (protectActive) status.fault = true; // latch
  }

  // Optional extension hook: decode 0xF1 state from data[1] later if desired.
}

bool Mks42DMotor::hasHeartbeat(uint32_t maxAgeMs) const {
  if (!ready || statusMode == OFF) return false;
  if (!statusValid) return false;

  const unsigned long now = millis();
  return (now - lastStatusUpdateTime) < maxAgeMs;
}

// Absolute position streaming (simple/robust):
// - Stream 0xFE target position
// - Use a fixed speed cap (maxRpm) rather than a dynamic feed-forward mapping
void Mks42DMotor::poll() {
  if (!ready) return;

  const unsigned long now = millis();

  // Always-on heartbeat + protect polling
  if (statusMode == ON) {
    if ((now - lastStatusRequestTime) >= MKS_STATUS_MS) {
      lastStatusRequestTime = now;
      if (canPlus.txTryLock()) sendF1(); // best-effort, non-blocking
    }

    if ((now - lastProtectRequestTime) >= MKS_PROTECT_MS) {
      lastProtectRequestTime = now;
      if (canPlus.txTryLock()) send3E(); // best-effort, non-blocking
    }

    // Hard faults only when enabled, with grace window after enabling
    if (enabled) {
      const bool inGrace = (now - lastEnableTime) < (unsigned long)MKS_HEARTBEAT_GRACE_MS;

      const bool ok = statusValid ? hasHeartbeat(MKS_STATUS_MS * 4U) : inGrace;

      if (!ok || status.fault) {
        enabled = false;
        stopSyntheticMotion();
        resetToTrackingBaseline();
        return;
      }
    }
  }

  if (!enabled) return;

  noInterrupts();
  #if MKS_SLEW_DIRECT == ON
    long target = targetSteps + backlashSteps;
  #else
    long target = motorSteps + backlashSteps;
  #endif
  interrupts();

  // signed 24-bit domain enforced here
  const int32_t pos = clamp24((int32_t)target);

  // simple: speed is a cap (RPM), not a feed-forward term
  uint16_t spd = (uint16_t)lroundf(pval(maxRpm));
  if (spd > 3000) spd = 3000;

  // send only on position change (avoid any "speed jitter" surprises)
  if (lastTarget == (long)pos) return;

  if (!canPlus.txTryLock()) return;

  sendFE(pos, spd);

  lastTarget = (long)pos;
}

void Mks42DMotor::stopSyntheticMotion() {
  if (lastPeriod == 0 && step == 0 && absStep == 0) return;
  currentFrequency = 0.0F;
  lastPeriod = 0;
  noInterrupts();
  step = 0;
  absStep = 0;
  interrupts();
  if (taskHandle) tasks.setPeriodSubMicros(taskHandle, 0);
}

void Mks42DMotor::resetToTrackingBaseline() {
  noInterrupts();
  step = 0;
  absStep = 0;
  inBacklash = false;
  backlashSteps = 0;
  interrupts();

  currentFrequency = 0.0F;
  lastPeriod = 0;
  stepSize = 1;

  // force next poll() to send target
  lastTarget = LONG_MIN;
}

// sets dir as required and moves coord toward target at setFrequencySteps() rate
IRAM_ATTR void Mks42DMotor::move() {
  if (sync && !inBacklash) targetSteps += step;

  if (motorSteps > targetSteps) {
    if (backlashSteps > 0) {
      backlashSteps -= absStep;
      inBacklash = backlashSteps > 0;
    } else {
      motorSteps -= absStep;
      inBacklash = false;
    }
  } else if (motorSteps < targetSteps || inBacklash) {
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
