// -----------------------------------------------------------------------------------
// CAN library

#include "Can1.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN1_TEENSY4

#include "../../tasks/OnTask.h"

void canT4Recv1(const CAN_message_t &msg) { canPlus.poll(msg); }
void canT4Poll1() { canPlus.events(); }

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

CanPlus1Teesny4::CanPlus1Teesny4() {
}

void CanPlus1Teesny4::init() {
  VF("MSG: CanPlus, CAN_TEENSY4 CAN1 Start...");
  can1.begin();
  can1.setBaudRate(CAN_BAUD);
  can1.setMaxMB(16);
  can1.enableFIFO();
  can1.enableFIFOInterrupt();
  can1.onReceive(canT4Recv1);
  ready = true;

  if (ready) {
    VLF("success");

    VF("MSG: CanPlus, start callback monitor task (rate "); V(CAN_RECV_RATE_MS); VF("ms priority 3)... ");
    if (tasks.add(CAN_RECV_RATE_MS, 0, true, 3, canT4Poll1, "SysCan1")) { VLF("success"); } else { VLF("FAILED!"); }
  } else {
    VLF("FAILED!");
  }
}

int CanPlus1Teesny4::writePacket(int id, const uint8_t *buffer, size_t size) {
  if (!ready) return 0;
  if (size > 8) return 0;
  if (size > 0 && buffer == nullptr) return 0;

  CAN_message_t msg{};
  msg.id = id & 0x7FF;
  msg.len = (uint8_t)size;
  msg.flags.remote = 0;
  msg.flags.extended = 0;

  for (uint8_t i = 0; i < msg.len; i++) msg.buf[i] = buffer[i];

  bool ok = can1.write(msg);
  if (ok) tx_ok++; else tx_fail++;
  return ok ? 1 : 0;
}

int CanPlus1Teesny4::writePacketRtr(int id, size_t dlc) {
  if (!ready) return 0;
  if (dlc > 8) return 0;             // DLC may be 0..8

  CAN_message_t msg{};
  msg.id = id & 0x7FF;
  msg.len = (uint8_t)dlc;
  msg.flags.remote = 1;
  msg.flags.extended = 0;

  bool ok = can1.write(msg);
  if (ok) tx_ok++; else tx_fail++;
  return ok ? 1 : 0;
}

void CanPlus1Teesny4::poll(const CAN_message_t &msg) {
  if (!ready) return;

  if (msg.flags.overrun) { rx_drop_len++; return; }
  if (msg.flags.extended) { rx_drop_ext++; return; }
  if (msg.flags.remote) { rx_drop_rtr++; return; }

  if (msg.len < 1 || msg.len > 8) { rx_drop_len++; return; }

  rx_ok++;
  callbackProcess(msg.id & 0x7FF, (uint8_t*)msg.buf, msg.len);
}

void CanPlus1Teesny4::events() {
  can1.events();
}

#if CAN_PLUS_DEFAULT == CAN1_TEENSY4
  CanPlus1Teesny4 canPlus;
#endif

#endif
