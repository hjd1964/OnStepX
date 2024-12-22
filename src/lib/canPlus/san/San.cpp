// -----------------------------------------------------------------------------------
// CAN library

#include "San.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN_SAN

#include <CAN.h> // https://github.com/sandeepmistry/arduino-CAN

IRAM_ATTR void onReceive(int packetSize) { canPlus.poll(packetSize); }

CanPlusSan::CanPlusSan() {
}

void CanPlusSan::init() {
  CAN.setPins(CAN_RX_PIN, CAN_TX_PIN);
  VF("MSG: CanPlus, CAN Start... ");
  ready = CAN.begin(CAN_BAUD);
  if (ready) { VLF("success"); } else { VLF("FAILED!"); }
  CAN.onReceive(onReceive);
}

int CanPlusSan::beginPacket(int id) {
  if (!ready) return 0;
  return CAN.beginPacket(id);
}

int CanPlusSan::endPacket() {
  if (!ready) return 0;
  return CAN.endPacket();
}

int CanPlusSan::write(uint8_t byte) {
  if (!ready) return 0;
  return CAN.write(byte);
}

int CanPlusSan::write(uint8_t *buffer, size_t size) {
  if (!ready) return 0;
  return CAN.write(buffer, size);
}

int CanPlusSan::writePacket(int id, uint8_t *buffer, size_t size) {
  if (!ready) return 0;
  if (CAN.beginPacket(id)) {
    int written = CAN.write(buffer, size);
    if (CAN.endPacket()) {
      return written;
    } else return 0;
  } else return 0;
}

int CanPlusSan::registerIdCallback(int id, void (*callback)(uint8_t data[8])) {
  if (id < 0 || id > 0x7FF || idCallbackCount >= CAN_MAX_CALLBACKS) return 0;
  idCallback[idCallbackCount] = callback;
  idCallBackId[idCallbackCount] = id;
  idCallbackCount++;
  return 1;
}

int CanPlusSan::callbackRegisterMessage(int id, uint8_t msg, void (*callback)(uint8_t data[8])) {
  if (id < 0 || id > 0x7FF || msgCallbackCount >= CAN_MAX_CALLBACKS) return 0;
  msgCallback[msgCallbackCount] = callback;
  msgCallBackId[msgCallbackCount] = id;
  msgCallBackMsg[msgCallbackCount] = msg;
  msgCallbackCount++;
  return 1;
}

IRAM_ATTR void CanPlusSan::poll(int packetSize) {
  if (!ready) return;

  // extend Id not supported
  if (CAN.packetExtended()) return;
  // remote request not supported
  if (CAN.packetRtr()) return;
  // mesage lengths other than 8 not supported
  if (packetSize != 8) return;

  int packetId = CAN.packetId();

  uint8_t buffer[8];
  for (int i = 0; i < packetSize; i++) { buffer[i] = CAN.read(); }

  callbackProcess(packetId, buffer, packetSize);
}

CanPlusSan canPlus;

#endif
