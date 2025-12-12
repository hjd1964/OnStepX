// -----------------------------------------------------------------------------------
// CAN library (MCP2515 via autowp/arduino-mcp2515)

#include "Mcp2515.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN_MCP2515

#include "../../tasks/OnTask.h"

#include <SPI.h>
#include <mcp2515.h>   // https://github.com/autowp/arduino-mcp2515

// Pick the right oscillator value for your module:
// - Most shields/modules are 16MHz
// - Some are 8MHz
#ifndef CAN_MCP2515_CLOCK
  #define CAN_MCP2515_CLOCK MCP_16MHZ
#endif

IRAM_ATTR void canMcp2515Monitor() { canPlus.poll(); }

static MCP2515 mcp2515(CAN_CS_PIN);

CanPlusMCP2515::CanPlusMCP2515() {}

static inline can_bitrate_t toBitrate() {
  #if CAN_BAUD == 125000
    return CAN_125KBPS;
  #elif CAN_BAUD == 250000
    return CAN_250KBPS;
  #elif CAN_BAUD == 500000
    return CAN_500KBPS;
  #elif CAN_BAUD == 1000000
    return CAN_1000KBPS;
  #else
    return CAN_500KBPS;
  #endif
}

void CanPlusMCP2515::init() {
  VF("MSG: CanPlus, CAN_MCP2515 (autowp) Start... ");

  #if CAN_INT_PIN != OFF
    pinMode(CAN_INT_PIN, INPUT_PULLUP);
  #endif

  SPI.begin();
  mcp2515.reset();

  ready = (mcp2515.setBitrate(toBitrate(), CAN_MCP2515_CLOCK) == MCP2515::ERROR_OK);
  if (ready) ready = (mcp2515.setNormalMode() == MCP2515::ERROR_OK);

  if (ready) {
    VLF("success");
    VF("MSG: CanPlus, start callback monitor task (rate "); V(CAN_RECV_RATE_MS);
    VF("ms priority 3)... ");
    if (tasks.add(CAN_RECV_RATE_MS, 0, true, 3, canMcp2515Monitor, "SysCanM")) VLF("success");
    else VLF("FAILED!");
  } else {
    VLF("FAILED!");
  }
}

int CanPlusMCP2515::writePacket(int id, const uint8_t *buffer, size_t size) {
  if (!ready) return 0;
  if (size > 8) return 0;
  if (size > 0 && buffer == nullptr) return 0;

  struct can_frame frame{};
  frame.can_id  = (uint32_t)(id & CAN_SFF_MASK);  // standard 11-bit
  frame.can_dlc = (uint8_t)size;
  if (size && buffer) {
    memcpy(frame.data, buffer, size);
  }
  auto err = mcp2515.sendMessage(&frame);
  if (err == MCP2515::ERROR_OK) tx_ok++; else tx_fail++;
  return (err == MCP2515::ERROR_OK) ? 1 : 0;
}

int CanPlusMCP2515::writePacketRtr(int id, size_t dlc) {
  if (!ready) return 0;
  if (dlc > 8) return 0;

  struct can_frame frame{};
  frame.can_id  = (uint32_t)(id & CAN_SFF_MASK) | CAN_RTR_FLAG;
  frame.can_dlc = (uint8_t)dlc;

  auto err = mcp2515.sendMessage(&frame);
  if (err == MCP2515::ERROR_OK) tx_ok++; else tx_fail++;
  return (err == MCP2515::ERROR_OK) ? 1 : 0;
}

void CanPlusMCP2515::poll() {
  if (!ready) return;

  for (int n = 0; n < 8; n++) {
    #if CAN_INT_PIN != OFF
      if (digitalRead(CAN_INT_PIN)) break;
    #endif

    struct can_frame frame{};
    auto err = mcp2515.readMessage(&frame);

    // If you're NOT using INT, this is your "no more frames" exit
    if (err != MCP2515::ERROR_OK) break;

    if (frame.can_id & CAN_EFF_FLAG) { rx_drop_ext++; continue; }
    if (frame.can_id & CAN_RTR_FLAG) { rx_drop_rtr++; continue; }

    const uint8_t dlc = frame.can_dlc;
    if (dlc < 1 || dlc > 8) { rx_drop_len++; continue; }

    rx_ok++;
    callbackProcess((int)(frame.can_id & CAN_SFF_MASK), frame.data, dlc);
  }
}


CanPlusMCP2515 canPlus;

#endif
