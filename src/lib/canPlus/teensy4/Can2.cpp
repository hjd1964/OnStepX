// -----------------------------------------------------------------------------------
// CAN library

#include "Can2.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN2_TEENSY4

#include "../../tasks/OnTask.h"

void canT4Recv2(const CAN_message_t &msg) { canPlus.poll(msg); }
void canT4Poll2() { canPlus.events(); }

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;

CanPlus2Teesny4::CanPlus2Teesny4() {
}

void CanPlus2Teesny4::init() {
  VF("MSG: CanPlus, CAN_TEENSY4 CAN2 Start...");
  can2.begin();
  can2.setBaudRate(CAN_BAUD);
  can2.setMaxMB(16);
  can2.enableFIFO();
  can2.enableFIFOInterrupt();
  can2.onReceive(canT4Recv2);
  ready = true;

  if (ready) {
    VLF("success");
    
    VF("MSG: CanPlus, start callback monitor task (rate "); V(CAN_RECV_RATE_MS); VF("ms priority 3)... ");
    if (tasks.add(CAN_RECV_RATE_MS, 0, true, 3, canT4Poll2, "SysCan2")) { VLF("success"); } else { VLF("FAILED!"); }
  } else {
    VLF("FAILED!");
  }
}

int CanPlus2Teesny4::writePacket(int id, uint8_t *buffer, size_t size) {
  if (!ready) return 0;
  if (size < 1 || size > 8) return 0;

  CAN_message_t msg;

  msg.id = id;
  msg.len = 8;
  for (int i = 0; i < msg.len; i++) msg.buf[i] = buffer[i];
  can2.write(msg);

  return 1;
}

void CanPlus2Teesny4::poll(const CAN_message_t &msg) {
  if (!ready) return;

  if (msg.flags.overrun) return;
  // extend Id not supported
  if (msg.flags.extended) return;
  // mesage lengths other than 8 not supported
  if (msg.len != 8) return;

  uint8_t buffer[8];
  for (int i = 0; i < msg.len; i++) buffer[i] = msg.buf[i];

  callbackProcess(msg.id, buffer, msg.len);
}

void CanPlus2Teesny4::events() {
  can2.events();
}

#if CAN_PLUS_DEFAULT == CAN2_TEENSY4
  CanPlus2Teesny4 canPlus;
#endif

#endif
