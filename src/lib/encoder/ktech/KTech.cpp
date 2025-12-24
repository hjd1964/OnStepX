// KTech in motor encoder (designed for the MS4010v3 with 16bit encoder)

#include "KTech.h"

#if AXIS1_ENCODER == KTECH_IME || AXIS2_ENCODER == KTECH_IME || AXIS3_ENCODER == KTECH_IME || \
    AXIS4_ENCODER == KTECH_IME || AXIS5_ENCODER == KTECH_IME || AXIS6_ENCODER == KTECH_IME || \
    AXIS7_ENCODER == KTECH_IME || AXIS8_ENCODER == KTECH_IME || AXIS9_ENCODER == KTECH_IME

#if !defined(CAN_PLUS) || CAN_PLUS == OFF
  #error "No KTECH motor CAN interface!"
#endif

#include "../../tasks/OnTask.h"
#include "../../canPlus/CanPlus.h"

KTechIME *ktechEncoderInstance[9];
void requestKTechEncoderAxis1() { ktechEncoderInstance[0]->requestPosition(); }
void requestKTechEncoderAxis2() { ktechEncoderInstance[1]->requestPosition(); }
void requestKTechEncoderAxis3() { ktechEncoderInstance[2]->requestPosition(); }
void requestKTechEncoderAxis4() { ktechEncoderInstance[3]->requestPosition(); }
void requestKTechEncoderAxis5() { ktechEncoderInstance[4]->requestPosition(); }
void requestKTechEncoderAxis6() { ktechEncoderInstance[5]->requestPosition(); }
void requestKTechEncoderAxis7() { ktechEncoderInstance[6]->requestPosition(); }
void requestKTechEncoderAxis8() { ktechEncoderInstance[7]->requestPosition(); }
void requestKTechEncoderAxis9() { ktechEncoderInstance[8]->requestPosition(); }

void positionKTechEncoderAxis1(uint8_t data[8]) { ktechEncoderInstance[0]->updatePositionCallback(data); }
void positionKTechEncoderAxis2(uint8_t data[8]) { ktechEncoderInstance[1]->updatePositionCallback(data); }
void positionKTechEncoderAxis3(uint8_t data[8]) { ktechEncoderInstance[2]->updatePositionCallback(data); }
void positionKTechEncoderAxis4(uint8_t data[8]) { ktechEncoderInstance[3]->updatePositionCallback(data); }
void positionKTechEncoderAxis5(uint8_t data[8]) { ktechEncoderInstance[4]->updatePositionCallback(data); }
void positionKTechEncoderAxis6(uint8_t data[8]) { ktechEncoderInstance[5]->updatePositionCallback(data); }
void positionKTechEncoderAxis7(uint8_t data[8]) { ktechEncoderInstance[6]->updatePositionCallback(data); }
void positionKTechEncoderAxis8(uint8_t data[8]) { ktechEncoderInstance[7]->updatePositionCallback(data); }
void positionKTechEncoderAxis9(uint8_t data[8]) { ktechEncoderInstance[8]->updatePositionCallback(data); }

void velocityKTechEncoderAxis1(uint8_t data[8]) { ktechEncoderInstance[0]->updateVelocityCallback(data); }
void velocityKTechEncoderAxis2(uint8_t data[8]) { ktechEncoderInstance[1]->updateVelocityCallback(data); }
void velocityKTechEncoderAxis3(uint8_t data[8]) { ktechEncoderInstance[2]->updateVelocityCallback(data); }
void velocityKTechEncoderAxis4(uint8_t data[8]) { ktechEncoderInstance[3]->updateVelocityCallback(data); }
void velocityKTechEncoderAxis5(uint8_t data[8]) { ktechEncoderInstance[4]->updateVelocityCallback(data); }
void velocityKTechEncoderAxis6(uint8_t data[8]) { ktechEncoderInstance[5]->updateVelocityCallback(data); }
void velocityKTechEncoderAxis7(uint8_t data[8]) { ktechEncoderInstance[6]->updateVelocityCallback(data); }
void velocityKTechEncoderAxis8(uint8_t data[8]) { ktechEncoderInstance[7]->updateVelocityCallback(data); }
void velocityKTechEncoderAxis9(uint8_t data[8]) { ktechEncoderInstance[8]->updateVelocityCallback(data); }

KTechIME::KTechIME(int16_t axis) {
  if (axis < 1 || axis > 9) return;
  
  this->axis = axis;
  axis_index = axis - 1;

  // the motor CAN ID is the axis number!
  canID = 0x140 + axis;

  ktechEncoderInstance[axis_index] = this;

  switch (this->axis) {
    case 1: callback = requestKTechEncoderAxis1; break;
    case 2: callback = requestKTechEncoderAxis2; break;
    case 3: callback = requestKTechEncoderAxis3; break;
    case 4: callback = requestKTechEncoderAxis4; break;
    case 5: callback = requestKTechEncoderAxis5; break;
    case 6: callback = requestKTechEncoderAxis6; break;
    case 7: callback = requestKTechEncoderAxis7; break;
    case 8: callback = requestKTechEncoderAxis8; break;
    case 9: callback = requestKTechEncoderAxis9; break;
  }
}

bool KTechIME::init() {
  if (ready) return true;
  if (!Encoder::init()) return false;

  if (!canPlus.ready) {
    DLF("WRN: Encoder KTech_IME"); D(axis); DLF(", no CAN interface!");
    ready = false;
    return false;
  }

  switch (this->axis) {
    case 1:
      canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis1);
      canPlus.callbackRegisterMessage(canID, 0xA2, velocityKTechEncoderAxis1);
    break;
    case 2:
      canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis2);
      canPlus.callbackRegisterMessage(canID, 0xA2, velocityKTechEncoderAxis2);
    break;
    case 3:
      canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis3);
      canPlus.callbackRegisterMessage(canID, 0xA2, velocityKTechEncoderAxis3);
    break;
    case 4:
      canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis4);
      canPlus.callbackRegisterMessage(canID, 0xA2, velocityKTechEncoderAxis4);
    break;
    case 5:
      canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis5);
      canPlus.callbackRegisterMessage(canID, 0xA2, velocityKTechEncoderAxis5);
    break;
    case 6:
      canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis6);
      canPlus.callbackRegisterMessage(canID, 0xA2, velocityKTechEncoderAxis6);
    break;
    case 7:
      canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis7);
      canPlus.callbackRegisterMessage(canID, 0xA2, velocityKTechEncoderAxis7);
    break;
    case 8:
      canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis8);
      canPlus.callbackRegisterMessage(canID, 0xA2, velocityKTechEncoderAxis8);
    break;
    case 9:
      canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis9);
      canPlus.callbackRegisterMessage(canID, 0xA2, velocityKTechEncoderAxis9);
    break;
  }

  VF("MSG: Encoder KTech_IME"); V(axis); VF(", start request position task (rate 500ms priority 6)... ");
  char name[] = "SysEnK_";
  name[6] = axis + '0';
  if (tasks.add(500, 0, true, 6, callback, name)) { VLF("success"); } else { VLF("FAILED!"); }

  ready = true;
  return true;
}

int32_t KTechIME::read() {
  if (!ready) return 0;

  int32_t result = ATOMIC_LOAD(count) + index;
  motorStepsAtLastReadValue = motorStepsAtLastUpdate;

  #if ENCODER_VELOCITY == ON
    velNoteSampledCount(ATOMIC_LOAD(count));
  #endif

  return result;
}

void KTechIME::write(int32_t position) {
  if (!ready) return;

  index = position - ATOMIC_LOAD(count);
}

void KTechIME::requestPosition() {
  uint32_t now = millis();

  if ((uint32_t)(now - lastUpdateByPositionCommand) > 1000UL &&
      (uint32_t)(now - lastUpdateByVelocityCommand) > 1000UL) error++;

  if ((uint32_t)(now - lastUpdateByVelocityCommand) < 500UL) return;

  static const uint8_t cmd[] = "\x90\x00\x00\x00\x00\x00\x00\x00";
  if (canPlus.txTryLock()) canPlus.writePacket(canID, cmd, 8);
}

IRAM_ATTR void KTechIME::updatePositionCallback(uint8_t data[8]) {
  lastUpdateByPositionCommand = millis();

  if (motorStepsPtr) {
    motorStepsAtLastUpdate = *motorStepsPtr;
    hasMotorStepsAtLastReadValue = true;
  }

  uint16_t countSingleTurn = (uint16_t)data[2] | ((uint16_t)data[3] << 8);

  static constexpr uint16_t high = (KTECH_SINGLE_TURN*3)/4;
  static constexpr uint16_t low = (KTECH_SINGLE_TURN*1)/4;

  // count wraps forward and reverse
  if (!firstCall) {
    if (lastCountSingleTurn > high && countSingleTurn < low) countTurns++;
    if (lastCountSingleTurn < low && countSingleTurn > high) countTurns--;
  }
  firstCall = false;

  lastCountSingleTurn = countSingleTurn;
  count = countSingleTurn + countTurns*KTECH_SINGLE_TURN;
}

IRAM_ATTR void KTechIME::updateVelocityCallback(uint8_t data[8]) {
  lastUpdateByVelocityCommand = millis();

  if (motorStepsPtr) {
    motorStepsAtLastUpdate = *motorStepsPtr;
    hasMotorStepsAtLastReadValue = true;
  }

  uint16_t countSingleTurn = (uint16_t)data[6] | ((uint16_t)data[7] << 8);

  static constexpr uint16_t high = (KTECH_SINGLE_TURN*3)/4;
  static constexpr uint16_t low  = (KTECH_SINGLE_TURN*1)/4;

  if (!firstCall) {
    if (lastCountSingleTurn > high && countSingleTurn < low) countTurns++;
    if (lastCountSingleTurn < low  && countSingleTurn > high) countTurns--;
  }
  firstCall = false;

  lastCountSingleTurn = countSingleTurn;
  count = countSingleTurn + countTurns*KTECH_SINGLE_TURN;
}

#endif
