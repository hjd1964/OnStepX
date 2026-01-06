// -----------------------------------------------------------------------------------
// CAN library

#include "CanPlusBase.h"

#if defined(CAN_PLUS) && CAN_PLUS != OFF

#include "../tasks/OnTask.h"

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

int CanPlus::write(const uint8_t *buffer, size_t size) {
  if (!ready) return 0;
  if (sendCount < 0 || size < 1 || sendCount + size > 8) return 0;

  for (unsigned int i = 0; i < size; i++) { if (!write(buffer[i])) return 0; }
  return size;
}

// add an id only callback
int CanPlus::callbackRegisterId(int id, void (*callback)(uint8_t data[8])) {
  if (!ready) return 0;
  if (id < 0 || id > 0x7FF || idCallbackCount >= CAN_MAX_CALLBACKS) return 0;
  idCallback[idCallbackCount] = callback;
  idCallBackId[idCallbackCount] = id;
  idCallbackEx[idCallbackCount] = nullptr;
  idCallbackCount++;
  return 1;
}

// add an id only callback (len-aware)
int CanPlus::callbackRegisterId(int id, void (*callback)(uint8_t data[8], uint8_t len)) {
  if (!ready) return 0;
  if (id < 0 || id > 0x7FF || idCallbackCount >= CAN_MAX_CALLBACKS) return 0;
  idCallBackId[idCallbackCount] = id;
  idCallback[idCallbackCount] = nullptr;        // old slot unused
  idCallbackEx[idCallbackCount] = callback;     // new slot used
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
  msgCallbackEx[msgCallbackCount] = nullptr;
  msgCallbackCount++;
  return 1;
}

// add an id + message callback (len-aware)
int CanPlus::callbackRegisterMessage(int id, uint8_t msg, void (*callback)(uint8_t data[8], uint8_t len)) {
  if (!ready) return 0;
  if (id < 0 || id > 0x7FF || msgCallbackCount >= CAN_MAX_CALLBACKS) return 0;
  msgCallBackId[msgCallbackCount] = id;
  msgCallBackMsg[msgCallbackCount] = msg;
  msgCallback[msgCallbackCount] = nullptr;       // old slot unused
  msgCallbackEx[msgCallbackCount] = callback;    // new slot used
  msgCallbackCount++;
  return 1;
}

// run id/msg based callbacks
int CanPlus::callbackProcess(int id, uint8_t *buffer, size_t size) {
  uint8_t paddedBuffer[8] = {0};

  uint8_t len = 0;
  if (buffer && size) {
    if (size > 8) size = 8;
    len = (uint8_t)size;
    memcpy(paddedBuffer, buffer, size);
  } else {
    len = 0;
  }

  int called = 0;

  for (int i = 0; i < idCallbackCount; i++) {
    if (id == idCallBackId[i]) {
      if (idCallbackEx[i]) idCallbackEx[i](paddedBuffer, len);
      else if (idCallback[i]) idCallback[i](paddedBuffer);
      called++;
    }
  }

  for (int i = 0; i < msgCallbackCount; i++) {
    if (len >= 1 && id == msgCallBackId[i] && paddedBuffer[0] == msgCallBackMsg[i]) {
      if (msgCallbackEx[i]) msgCallbackEx[i](paddedBuffer, len);
      else if (msgCallback[i]) msgCallback[i](paddedBuffer);
      called++;
    }
  }

  return called ? 1 : 0;
}

bool CanPlus::txTryLock(uint32_t minGapUs) {
  uint32_t now = micros();
  if ((uint32_t)(now - lastTxUs) < minGapUs) return false;
  lastTxUs = now;
  return true;
}

void CanPlus::txWait(uint32_t minGapUs) {
  while (!txTryLock(minGapUs)) {
    tasks.yield(minGapUs/1000UL);
  }
}

#endif
