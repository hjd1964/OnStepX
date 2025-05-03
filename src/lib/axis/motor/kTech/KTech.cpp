// -----------------------------------------------------------------------------------
// axis ktech servo motor

#include "KTech.h"

#ifdef KTECH_MOTOR_PRESENT

#include "../../../tasks/OnTask.h"
#include "../../../canPlus/CanPlus.h"

KTechMotor *ktechMotorInstance[9];
IRAM_ATTR void moveKTechMotorAxis1() { ktechMotorInstance[0]->move(); }
IRAM_ATTR void moveKTechMotorAxis2() { ktechMotorInstance[1]->move(); }
IRAM_ATTR void moveKTechMotorAxis3() { ktechMotorInstance[2]->move(); }
IRAM_ATTR void moveKTechMotorAxis4() { ktechMotorInstance[3]->move(); }
IRAM_ATTR void moveKTechMotorAxis5() { ktechMotorInstance[4]->move(); }
IRAM_ATTR void moveKTechMotorAxis6() { ktechMotorInstance[5]->move(); }
IRAM_ATTR void moveKTechMotorAxis7() { ktechMotorInstance[6]->move(); }
IRAM_ATTR void moveKTechMotorAxis8() { ktechMotorInstance[7]->move(); }
IRAM_ATTR void moveKTechMotorAxis9() { ktechMotorInstance[8]->move(); }

void statusKTechMotorAxis1(uint8_t data[8]) { ktechMotorInstance[0]->requestStatusCallback(data); }
void statusKTechMotorAxis2(uint8_t data[8]) { ktechMotorInstance[1]->requestStatusCallback(data); }
void statusKTechMotorAxis3(uint8_t data[8]) { ktechMotorInstance[2]->requestStatusCallback(data); }
void statusKTechMotorAxis4(uint8_t data[8]) { ktechMotorInstance[3]->requestStatusCallback(data); }
void statusKTechMotorAxis5(uint8_t data[8]) { ktechMotorInstance[4]->requestStatusCallback(data); }
void statusKTechMotorAxis6(uint8_t data[8]) { ktechMotorInstance[5]->requestStatusCallback(data); }
void statusKTechMotorAxis7(uint8_t data[8]) { ktechMotorInstance[6]->requestStatusCallback(data); }
void statusKTechMotorAxis8(uint8_t data[8]) { ktechMotorInstance[7]->requestStatusCallback(data); }
void statusKTechMotorAxis9(uint8_t data[8]) { ktechMotorInstance[8]->requestStatusCallback(data); }

// constructor
KTechMotor::KTechMotor(uint8_t axisNumber, const KTechDriverSettings *Settings, bool useFastHardwareTimers) {
  if (axisNumber < 1 || axisNumber > 9) return;
  this->axisNumber = axisNumber;

  driverType = ODRIVER;

  strcpy(axisPrefix, " Axis_KTech, ");
  axisPrefix[5] = '0' + axisNumber;

  // the motor CAN ID is the axis number!
  canID = 0x140 + axisNumber;

  statusMode = Settings->status;

  if (axisNumber > 2) useFastHardwareTimers = false;
  this->useFastHardwareTimers = useFastHardwareTimers;

  // attach the function pointers to the callbacks
  ktechMotorInstance[this->axisNumber - 1] = this;
  switch (this->axisNumber) {
    case 1: callback = moveKTechMotorAxis1; break;
    case 2: callback = moveKTechMotorAxis2; break;
    case 3: callback = moveKTechMotorAxis3; break;
    case 4: callback = moveKTechMotorAxis4; break;
    case 5: callback = moveKTechMotorAxis5; break;
    case 6: callback = moveKTechMotorAxis6; break;
    case 7: callback = moveKTechMotorAxis7; break;
    case 8: callback = moveKTechMotorAxis8; break;
    case 9: callback = moveKTechMotorAxis9; break;
  }
}

bool KTechMotor::init() {
  if (axisNumber < 1 || axisNumber > 9) return false;

  if (!canPlus.ready) {
    V("ERR:"); V(axisPrefix); VLF("No CAN interface!");
    return false;
  }

  // automatically set fault status for known drivers
  status.active = statusMode != OFF;

  enable(false);

  // start the motion timer
  VF("MSG:"); V(axisPrefix); VF("start task to track motion... ");
  char timerName[] = "Ax_KTec";
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
    return false;
  }

  // get ready for status messages
  if (statusMode == ON) {
    switch (this->axisNumber) {
      case 1: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechMotorAxis1); break;
      case 2: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechMotorAxis2); break;
      case 3: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechMotorAxis3); break;
      case 4: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechMotorAxis4); break;
      case 5: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechMotorAxis5); break;
      case 6: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechMotorAxis6); break;
      case 7: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechMotorAxis7); break;
      case 8: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechMotorAxis8); break;
      case 9: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechMotorAxis9); break;
    }
  }

  ready = true;
  return true;
}

// set motor parameters
bool KTechMotor::setParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  UNUSED(param1); // general purpose settings defined in Extended.config.h and stored in NV, they can be modified at runtime
  UNUSED(param2);
  UNUSED(param3);
  UNUSED(param4);
  UNUSED(param5);
  stepsPerMeasure = param6;
  return true;
}

// validate motor parameters
bool KTechMotor::validateParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  UNUSED(param1);
  UNUSED(param2);
  UNUSED(param3);
  UNUSED(param4);
  UNUSED(param5);
  UNUSED(param6);
  return true;
}

// low level reversal of axis directions
// \param state: OFF normal or ON to reverse
void KTechMotor::setReverse(int8_t state) {
  if (!ready) return;

  if (state == ON) {
    V("WRN:"); V(axisPrefix); VLF("axis reversal must be accomplished with hardware or KTech setup!");
  }
}

// sets motor enable on/off (if possible)
void KTechMotor::enable(bool state) {
  if (!ready) return;

  VF("MSG:"); V(axisPrefix);
  if (state) {
    uint8_t cmd[] = "\x88\x00\x00\x00\x00\x00\x00\x00";
    canPlus.writePacket(canID, cmd, 8);
    VLF("powered up");
  } else {
    uint8_t cmd[] = "\x80\x00\x00\x00\x00\x00\x00\x00";
    canPlus.writePacket(canID, cmd, 8);
    VLF("powered down");
  } 
  
  enabled = state;
}

void KTechMotor::setInstrumentCoordinateSteps(long value) {
  Motor::setInstrumentCoordinateSteps(value);
}

// resets motor and target angular position in steps, also zeros backlash and index
void KTechMotor::resetPositionSteps(long value) {
  if (!ready) return;

  uint8_t cmd[] = "\x95\x00\x00\x00";
  canPlus.beginPacket(canID);
  canPlus.write(cmd, 4);
  canPlus.write((uint8_t*)&value, 4);
  canPlus.endPacket();

  noInterrupts();
  motorSteps    = value;
  targetSteps   = value;
  backlashSteps = 0;
  interrupts();
}

// set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
void KTechMotor::setFrequencySteps(float frequency) {
  if (!ready) return;

  // negative frequency, convert to positive and reverse the direction
  int dir = 0;
  if (frequency > 0.0F) dir = 1; else if (frequency < 0.0F) { frequency = -frequency; dir = -1; }

  // if in backlash override the frequency
  if (inBacklash)
    frequency = backlashFrequency;

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

float KTechMotor::getFrequencySteps() {
  if (!ready) return 0.0F;

  if (lastPeriod == 0) return 0;
  return (16000000.0F / lastPeriod) * absStep;
}

// set slewing state (hint that we are about to slew or are done slewing)
void KTechMotor::setSlewing(bool state) {
  if (!ready) return;

  isSlewing = state;
}

// updates PID and sets ktech position
void KTechMotor::poll() {
  if (statusMode == ON && (long)(millis() - lastStatusRequestTime) > KTECH_STATUS_MS) {
    lastStatusRequestTime = millis();
    uint8_t cmd[] = "\x9a\x00\x00\x00\x00\x00\x00\x00";
    canPlus.writePacket(canID, cmd, 8);
    return;
  }

  if ((long)(millis() - lastSetPositionTime) < CAN_SEND_RATE_MS) return;
  lastSetPositionTime = millis();

  noInterrupts();
  #if KTECH_SLEW_DIRECT == ON
    long target = targetSteps + backlashSteps;
  #else
    long target = motorSteps + backlashSteps;
  #endif
  interrupts();

  if (lastTarget != target) {
    uint8_t cmd[] = "\xa3\x00\x00\x00\x00\x00\x00\x00";
    canPlus.beginPacket(canID);
    canPlus.write(cmd, 4);
    canPlus.write((uint8_t*)&target, 4);
    canPlus.endPacket();
    lastTarget = target;
  }
}

// sets dir as required and moves coord toward target at setFrequencySteps() rate
IRAM_ATTR void KTechMotor::move() {
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

void KTechMotor::updateStatus() {
  if (statusMode == OFF) return;
  if ((long)(millis() - lastStatusUpdateTime) > (KTECH_STATUS_MS*4)) {
    status.outputA.shortToGround  = true;
    status.outputA.openLoad       = true;
    status.outputB.shortToGround  = true;
    status.outputB.openLoad       = true;
    status.overTemperatureWarning = true;
    status.overTemperature        = true;
    status.standstill             = true;
    status.fault                  = false;
  }
}

// update the associated driver status from CAN
void KTechMotor::requestStatusCallback(uint8_t data[8]) {
  if (statusMode == OFF) return;

  uint8_t errorState = data[7];
  status.outputA.shortToGround  = false;
  status.outputA.openLoad       = false;
  status.outputB.shortToGround  = false;
  status.outputB.openLoad       = false;
  status.overTemperatureWarning = false;
  status.overTemperature        = bitRead(errorState, 3);
  status.standstill             = false;
  status.fault                  = bitRead(errorState, 0) || bitRead(errorState, 3);
  lastStatusUpdateTime          = millis();
}

#endif
