// -----------------------------------------------------------------------------------
// CAN library

#include "CanPlusBase.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

int CanPlus::beginPacket(int id) {
  if (!ready) return 0;
  sendId = id;
  sendCount = 0;
  return  1;
}

int CanPlus::endPacket() {
  if (!ready) return 0;
  if (sendCount < 1) return 0;
  if (writePacket(sendId, sendData, sendCount)) return 1; else return 0;
}

int CanPlus::write(uint8_t byte) {
  if (!ready) return 0;
  if (sendCount < 0 || sendCount > 7) return 0;
  sendData[sendCount++] = byte;
  return 1;
}

int CanPlus::write(uint8_t *buffer, size_t size) {
  if (!ready) return 0;
  if (sendCount < 0 || size < 1 || size > 8) return 0;
  for (unsigned int i = 0; i < size; i++) { if (!write(buffer[i])) return 0; }
  return size;
}

// add an id only callback
int CanPlus::callbackRegisterId(int id, void (*callback)(uint8_t data[8])) {
  if (!ready) return 0;
  if (id < 0 || id > 0x7FF || idCallbackCount >= CAN_MAX_CALLBACKS) return 0;
  idCallback[idCallbackCount] = callback;
  idCallBackId[idCallbackCount] = id;
  idCallbackCount++;
  return 1;
}

// add an id + message callback
int CanPlus::callbackRegisterMessage(int id, uint8_t msg, void (*callback)(uint8_t data[8])) {
  if (!ready) return 0;
  if (id < 0 || id > 0x7FF || msgCallbackCount >= CAN_MAX_CALLBACKS) return 0;
  msgCallback[msgCallbackCount] = callback;
  msgCallBackId[msgCallbackCount] = id;
  msgCallBackMsg[msgCallbackCount] = msg;
  msgCallbackCount++;
  return 1;
}

// run id/msg based callbacks
int CanPlus::callbackProcess(int id, uint8_t *buffer, size_t size) {
  for (int i = 0; i < idCallbackCount; i++) {
    if (id == idCallBackId[i]) {
      idCallback[i](buffer);
      return 1;
    }
  }
  for (int i = 0; i < msgCallbackCount; i++) {
    if (id == msgCallBackId[i] && buffer[0] == msgCallBackMsg[i]) {
      msgCallback[i](buffer);
      return 1;
    }
  }
  return 0;
}

#endif
