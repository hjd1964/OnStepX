// -----------------------------------------------------------------------------------
// CAN library

#include "San.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN_SAN

#include <CAN.h> // https://github.com/sandeepmistry/arduino-CAN

// Pick the right oscillator value for your module:
// - Most shields/modules are 16MHz
// - Some are 8MHz
#ifndef CAN_SAN_CLOCK
  #define CAN_SAN_CLOCK 16000000UL   // or 8000000UL for 8MHz modules
#endif

IRAM_ATTR void onReceive(int packetSize) { canPlus.poll(packetSize); }

CanPlusSan::CanPlusSan() {
}

void CanPlusSan::init() {
  CAN.setClockFrequency(CAN_SAN_CLOCK); 
  CAN.setPins(CAN_RX_PIN, CAN_TX_PIN);
  VF("MSG: CanPlus, CAN Start... ");
  ready = CAN.begin(CAN_BAUD);
  if (ready) { VLF("success"); } else { VLF("FAILED!"); }
  CAN.onReceive(onReceive);
}

int CanPlusSan::beginPacket(int id) {
  if (!ready) return 0;
  return CAN.beginPacket(id & 0x7FF);
}

int CanPlusSan::endPacket() {
  if (!ready) return 0;
  return CAN.endPacket();
}

int CanPlusSan::write(uint8_t byte) {
  if (!ready) return 0;
  return CAN.write(byte);
}

int CanPlusSan::write(const uint8_t *buffer, size_t size) {
  if (!ready) return 0;
  return CAN.write(buffer, size);
}

int CanPlusSan::writePacket(int id, const uint8_t *buffer, size_t size) {
  if (!ready) return 0;
  if (size > 8) return 0;
  if (size > 0 && buffer == nullptr) return 0;

  const uint16_t sid = (uint16_t)id & 0x7FF;
  if (!CAN.beginPacket(sid)) { tx_fail++; return 0; }

  int written = 0;

  if (size > 0) {
    if (!buffer) { (void)CAN.endPacket(); tx_fail++; return 0; }

    written = CAN.write((uint8_t*)buffer, size);  // only if API requires non-const
    if (written != (int)size) {
      (void)CAN.endPacket();
      tx_fail++;
      return 0;
    }
  }

  bool ok = CAN.endPacket();
  if (ok) tx_ok++; else tx_fail++;
  return ok ? 1 : 0;
}

int CanPlusSan::writePacketRtr(int id, size_t dlc) {
  if (!ready) return 0;
  if (dlc > 8) return 0;

  // arduino-CAN supports beginPacket(id, dlc, rtr) in most versions
  if (!CAN.beginPacket(id & 0x7FF, (int)dlc, true)) { tx_fail++; return 0; }

  bool ok = CAN.endPacket();
  if (ok) tx_ok++; else tx_fail++;
  return ok ? 1 : 0;
}

IRAM_ATTR void CanPlusSan::poll(int packetSize) {
  if (!ready) return;

  bool ext = CAN.packetExtended();
  bool rtr = CAN.packetRtr();
  int  packetId = CAN.packetId() & 0x7FF;

  if (ext) { rx_drop_ext++; while (CAN.available()) (void)CAN.read(); return; }

  // Handle RTR early; DLC may legitimately be 0
  if (rtr) {
    rx_drop_rtr++;
    while (CAN.available()) (void)CAN.read();
    return;
  }

  if (packetSize < 1 || packetSize > 8) {
    rx_drop_len++;
    while (CAN.available()) (void)CAN.read();
    return;
  }

  uint8_t buffer[8] = {0};
  for (int i = 0; i < packetSize; i++) {
    int b = CAN.read();
    buffer[i] = (b < 0) ? 0 : (uint8_t)b;
  }
  while (CAN.available()) (void)CAN.read();

  rx_ok++;
  callbackProcess(packetId, buffer, (size_t)packetSize);
}

CanPlusSan canPlus;

#endif
