// -----------------------------------------------------------------------------------
// CAN library

#include "Mcp2515.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN_MCP2515

#include "../../tasks/OnTask.h"

#include <mcp_can.h>
#include <SPI.h>

MCP_CAN mcp(CAN_CS_PIN);

IRAM_ATTR void canMcp2515Monitor() { canPlus.poll(); }

CanPlusMCP2515::CanPlusMCP2515() {
}

void CanPlusMCP2515::init() {
  uint8_t canSpeed;
  #if CAN_BAUD == 125000
    canSpeed = CAN_125KBPS;
  #elif CAN_BAUD == 250000
    canSpeed = CAN_250KBPS;
  #elif CAN_BAUD == 500000
    canSpeed = CAN_500KBPS;
  #elif CAN_BAUD == 1000000
    canSpeed = CAN_1000KBPS;
  #else
    canSpeed = CAN_500KBPS;
  #endif
 
  VF("MSG: CanPlus, CAN_MCP2515 Start... ");
  ready = mcp.begin(MCP_ANY, canSpeed, CAN_CLOCK) == CAN_OK;
  if (ready) {
    VLF("success");

    VF("MSG: CanPlus, start callback monitor task (rate "); V(CAN_RECV_RATE_MS); VF("ms priority 3)... ");
    if (tasks.add(CAN_RECV_RATE_MS, 0, true, 3, canMcp2515Monitor, "SysCanM")) { VLF("success"); } else { VLF("FAILED!"); }
  } else {
    VLF("FAILED!");
  }

  mcp.setMode(MCP_NORMAL);

  pinMode(CAN_INT_PIN, INPUT);
}

int CanPlusMCP2515::writePacket(int id, uint8_t *buffer, size_t size) {
  if (!ready) return 0;
  if (size < 1 || size > 8) return 0;

  // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
  int result = mcp.sendMsgBuf(id, 0, size, buffer);

  return result == CAN_OK;
}

IRAM_ATTR void CanPlusMCP2515::poll() {
  if (!ready) return;

  unsigned long packetId;
  uint8_t packetSize;
  uint8_t buffer[255];

  if (!digitalRead(CAN_INT_PIN)) {
    mcp.readMsgBuf(&packetId, &packetSize, buffer);
    
    // extend Id not supported
    if((packetId & 0x80000000) == 0x80000000) return;
    // remote request not supported
    if((packetId & 0x40000000) == 0x40000000) return;
    // mesage lengths other than 8 not supported
    if (packetSize != 8) return;

    if (!callbackProcess(packetId, buffer, packetSize)) {
      // wasn't a callback so quickly get the next message
      poll();
    }
  }
}

CanPlusMCP2515 canPlus;

#endif
