// -----------------------------------------------------------------------------------
// CAN library

#include "Can3.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN3_TEENSY4

#include "../../tasks/OnTask.h"

void canT4Recv3(const CAN_message_t &msg) { canPlus.poll(msg); }
void canT4Poll3() { canPlus.events(); }

FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can3;

CanPlus3Teesny4::CanPlus3Teesny4() {
}

void CanPlus3Teesny4::init() {
  VF("MSG: CanPlus, CAN_TEENSY4 CAN3 Start...");
  can3.begin();
  can3.setBaudRate(CAN_BAUD);
  can3.setMaxMB(16);
  can3.enableFIFO();
  can3.enableFIFOInterrupt();
  can3.onReceive(canT4Recv3);
  ready = true;

  if (ready) {
    VLF("success");

    VF("MSG: CanPlus, start callback monitor task (rate "); V(CAN_RECV_RATE_MS); VF("ms priority 3)... ");
    if (tasks.add(CAN_RECV_RATE_MS, 0, true, 3, canT4Poll3, "SysCan3")) { VLF("success"); } else { VLF("FAILED!"); }
  } else {
    VLF("FAILED!");
  }
}

int CanPlus3Teesny4::writePacket(int id, const uint8_t *buffer, size_t size) {
  if (!ready) return 0;
  if (size > 8) return 0;
  if (size > 0 && buffer == nullptr) return 0;

  CAN_message_t msg{};
  msg.id = id & 0x7FF;
  msg.len = (uint8_t)size;
  msg.flags.remote = 0;
  msg.flags.extended = 0;

  for (uint8_t i = 0; i < msg.len; i++) msg.buf[i] = buffer[i];

  bool ok = can3.write(msg);
  if (ok) tx_ok++; else tx_fail++;
  return ok ? 1 : 0;
}

int CanPlus3Teesny4::writePacketRtr(int id, size_t dlc) {
  if (!ready) return 0;
  if (dlc > 8) return 0;             // DLC may be 0..8

  CAN_message_t msg{};
  msg.id = id & 0x7FF;
  msg.len = (uint8_t)dlc;
  msg.flags.remote = 1;
  msg.flags.extended = 0;

  bool ok = can3.write(msg);
  if (ok) tx_ok++; else tx_fail++;
  return ok ? 1 : 0;
}

void CanPlus3Teesny4::poll(const CAN_message_t &msg) {
  if (!ready) return;

  if (msg.flags.overrun) { rx_drop_len++; return; }
  if (msg.flags.extended) { rx_drop_ext++; return; }
  if (msg.flags.remote) { rx_drop_rtr++; return; }

  if (msg.len < 1 || msg.len > 8) { rx_drop_len++; return; }

  rx_ok++;
  callbackProcess(msg.id & 0x7FF, (uint8_t*)msg.buf, msg.len);
}

void CanPlus3Teesny4::events() {
  can3.events();
}

#if CAN_PLUS_DEFAULT == CAN3_TEENSY4
  CanPlus3Teesny4 canPlus;
#endif

#endif
