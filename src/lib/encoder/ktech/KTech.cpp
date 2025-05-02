// KTech in motor encoder

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

KTechIME::KTechIME(int16_t axis) {
  if (axis < 1 || axis > 9) return;
  
  this->axis = axis;

  // the motor CAN ID is the axis number!
  canID = 0x140 + this->axis;

  ktechEncoderInstance[this->axis - 1] = this;
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

  if (!canPlus.ready) {
    DLF("WRN: Encoder KTech_IME"); D(axis); DLF(", no CAN interface!");
    ready = false;
    return false;
  }

  switch (this->axis) {
    case 1: canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis1); break;
    case 2: canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis2); break;
    case 3: canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis3); break;
    case 4: canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis4); break;
    case 5: canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis5); break;
    case 6: canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis6); break;
    case 7: canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis7); break;
    case 8: canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis8); break;
    case 9: canPlus.callbackRegisterMessage(canID, 0x90, positionKTechEncoderAxis9); break;
  }

  VF("MSG: Encoder KTech_IME"); V(axis); VF(", start callback monitor task (rate "); V(CAN_SEND_RATE_MS); VF("ms priority 6)... ");
  char name[] = "SysEnK_";
  name[6] = axis + '0';
  if (tasks.add(CAN_SEND_RATE_MS, 0, true, 6, callback, name)) { VLF("success"); } else { VLF("FAILED!"); }

  ready = true;
  return true;
}

int32_t KTechIME::read() {
  if (!ready) return 0;
  return count + origin;
}

void KTechIME::write(int32_t count) {
  if (!ready) return;
  origin = count - this->count;
}

void KTechIME::requestPosition() {
  uint8_t cmd[] = "\x90\x00\x00\x00\x00\x00\x00\x00";
  canPlus.writePacket(canID, cmd, 8);
}

IRAM_ATTR void KTechIME::updatePositionCallback(uint8_t data[8]) {
  uint16_t countSingleTurn;
  memcpy(&countSingleTurn, &data[2], 2);

  // count wraps forward and reverse
  if (lastCountSingleTurn > KTECH_SINGLE_TURN*0.75 && countSingleTurn < KTECH_SINGLE_TURN*0.25) countTurns++;
  if (lastCountSingleTurn < KTECH_SINGLE_TURN*0.25 && countSingleTurn > KTECH_SINGLE_TURN*0.75) countTurns--;

  lastCountSingleTurn = countSingleTurn;
  count = countSingleTurn + countTurns*KTECH_SINGLE_TURN;
}

#endif
