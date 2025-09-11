// -----------------------------------------------------------------------------------
// axis servo KTech motor driver

#include "KTech.h"

#ifdef SERVO_KTECH_PRESENT

#if !defined(CAN_PLUS) || CAN_PLUS == OFF
  #error "No KTECH motor CAN interface!"
#endif

#include "../../../../tasks/OnTask.h"
#include "../../../../canPlus/CanPlus.h"

ServoKTech *ktechServoInstance[9];
void requestKTechServoAxis1() { ktechServoInstance[0]->requestStatus(); }
void requestKTechServoAxis2() { ktechServoInstance[1]->requestStatus(); }
void requestKTechServoAxis3() { ktechServoInstance[2]->requestStatus(); }
void requestKTechServoAxis4() { ktechServoInstance[3]->requestStatus(); }
void requestKTechServoAxis5() { ktechServoInstance[4]->requestStatus(); }
void requestKTechServoAxis6() { ktechServoInstance[5]->requestStatus(); }
void requestKTechServoAxis7() { ktechServoInstance[6]->requestStatus(); }
void requestKTechServoAxis8() { ktechServoInstance[7]->requestStatus(); }
void requestKTechServoAxis9() { ktechServoInstance[8]->requestStatus(); }

void statusKTechServoAxis1(uint8_t data[8]) { ktechServoInstance[0]->requestStatusCallback(data); }
void statusKTechServoAxis2(uint8_t data[8]) { ktechServoInstance[1]->requestStatusCallback(data); }
void statusKTechServoAxis3(uint8_t data[8]) { ktechServoInstance[2]->requestStatusCallback(data); }
void statusKTechServoAxis4(uint8_t data[8]) { ktechServoInstance[3]->requestStatusCallback(data); }
void statusKTechServoAxis5(uint8_t data[8]) { ktechServoInstance[4]->requestStatusCallback(data); }
void statusKTechServoAxis6(uint8_t data[8]) { ktechServoInstance[5]->requestStatusCallback(data); }
void statusKTechServoAxis7(uint8_t data[8]) { ktechServoInstance[6]->requestStatusCallback(data); }
void statusKTechServoAxis8(uint8_t data[8]) { ktechServoInstance[7]->requestStatusCallback(data); }
void statusKTechServoAxis9(uint8_t data[8]) { ktechServoInstance[8]->requestStatusCallback(data); }

ServoKTech::ServoKTech(uint8_t axisNumber, const ServoSettings *Settings, float countsToStepsRatio)
                       :ServoDriver(axisNumber, NULL, Settings) {
  if (axisNumber < 1 || axisNumber > 9) return;

  strcpy(axisPrefix, " Axis_ServoKTech, ");
  axisPrefix[5] = '0' + axisNumber;

  // the motor CAN ID is the axis number!
  canID = 0x140 + axisNumber;

  this->countsToStepsRatio.valueDefault = countsToStepsRatio;

  ktechServoInstance[this->axisNumber - 1] = this;
  switch (this->axisNumber) {
    case 1: callback = requestKTechServoAxis1; break;
    case 2: callback = requestKTechServoAxis2; break;
    case 3: callback = requestKTechServoAxis3; break;
    case 4: callback = requestKTechServoAxis4; break;
    case 5: callback = requestKTechServoAxis5; break;
    case 6: callback = requestKTechServoAxis6; break;
    case 7: callback = requestKTechServoAxis7; break;
    case 8: callback = requestKTechServoAxis8; break;
    case 9: callback = requestKTechServoAxis9; break;
  }
}

bool ServoKTech::init(bool reverse) {
  if (!ServoDriver::init(reverse)) return false;

  // automatically set fault status for known drivers
  status.active = statusMode != OFF;

  // get ready for status messages
  if (statusMode == ON) {

    switch (this->axisNumber) {
      case 1: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechServoAxis1); break;
      case 2: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechServoAxis2); break;
      case 3: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechServoAxis3); break;
      case 4: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechServoAxis4); break;
      case 5: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechServoAxis5); break;
      case 6: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechServoAxis6); break;
      case 7: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechServoAxis7); break;
      case 8: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechServoAxis8); break;
      case 9: canPlus.callbackRegisterMessage(canID, 0x9a, statusKTechServoAxis9); break;
    }

    VF("MSG:"); V(axisPrefix); VF("start callback status request task (rate "); V(KTECH_STATUS_MS); VF("ms priority 7)... ");
    char name[] = "Ax_StaK";
    name[2] = axisNumber + '0';
    if (tasks.add(KTECH_STATUS_MS, 0, true, 7, callback, name)) { VLF("success"); } else { VLF("FAILED!"); }
  } else {
    VF("MSG:"); V(axisPrefix); VLF("no driver status");
  }

  return true;
}

// enable or disable the driver using the enable pin or other method
void ServoKTech::enable(bool state) {
  enabled = state;

  VF("MSG:"); V(axisPrefix); VF("powered ");

  uint8_t cmd[] = "\xa2\x00\x00\x00\x00\x00\x00\x00";
  canPlus.writePacket(canID, cmd, 8);

  if (state) {
    uint8_t cmd[] = "\x88\x00\x00\x00\x00\x00\x00\x00";
    canPlus.writePacket(canID, cmd, 8);
    VLF("up");
  } else {
    uint8_t cmd[] = "\x80\x00\x00\x00\x00\x00\x00\x00";
    canPlus.writePacket(canID, cmd, 8);
    VLF("down");
  } 
  
  velocityRamp = 0.0F;
}

float ServoKTech::setMotorVelocity(float velocity) {
  velocity = ServoDriver::setMotorVelocity(velocity);

  if (velocityLast != lround(velocity) && ((long)(millis() - lastVelocityUpdateTime) > CAN_SEND_RATE_MS)) {
    uint8_t cmd[] = "\xa2\x00\x00\x00";
    canPlus.beginPacket(canID);
    canPlus.write(cmd, 4);
    velocityLast = lround(velocity*countsToStepsRatio.value);
    if (reversed) velocityLast = -velocityLast;
    canPlus.write((uint8_t*)&velocityLast, 4);
    canPlus.endPacket();
    lastVelocityUpdateTime = millis();
  }

  return velocity;
}

// request driver status from CAN
void ServoKTech::requestStatus() {
  uint8_t cmd[] = "\x9a\x00\x00\x00\x00\x00\x00\x00";
  canPlus.writePacket(canID, cmd, 8);
}

void ServoKTech::readStatus() {
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
void ServoKTech::requestStatusCallback(uint8_t data[8]) {
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
