// -----------------------------------------------------------------------------------
// CAN library

#include "Esp32.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN_ESP32

#include "../../tasks/OnTask.h"

#include <ESP32CAN.h> // https://github.com/miwagner/ESP32-Arduino-CAN
#include <CAN_config.h>

IRAM_ATTR void canEsp32Monitor() { canPlus.poll(); }

CAN_device_t CAN_cfg;

CanPlusESP32::CanPlusESP32() {
}

void CanPlusESP32::init() {
  #if CAN_BAUD == 125000
    CAN_cfg.speed = CAN_SPEED_125KBPS;
  #elif CAN_BAUD == 250000
    CAN_cfg.speed = CAN_SPEED_250KBPS;
  #elif CAN_BAUD == 500000
    CAN_cfg.speed = CAN_SPEED_500KBPS;
  #elif CAN_BAUD == 1000000
    CAN_cfg.speed = CAN_SPEED_1000KBPS;
  #else
    CAN_cfg.speed = CAN_SPEED_500KBPS;
  #endif
  CAN_cfg.tx_pin_id = (gpio_num_t)CAN_TX_PIN;
  CAN_cfg.rx_pin_id = (gpio_num_t)CAN_RX_PIN;

  VF("MSG: CanPlus, CAN_ESP32 Start... ");
  CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));
  ESP32Can.CANInit();
  ready = true;

  if (ready) {
    VLF("success");
    
    VF("MSG: CanPlus, start callback monitor task (rate "); V(CAN_RECV_RATE_MS); VF("ms priority 3)... ");
    if (tasks.add(CAN_RECV_RATE_MS, 0, true, 3, canEsp32Monitor, "SysCanE")) { VLF("success"); } else { VLF("FAILED!"); }
  } else {
    VLF("FAILED!");
  }
}

int CanPlusESP32::writePacket(int id, uint8_t *buffer, size_t size) {
  if (!ready) return 0;
  if (size < 1 || size > 8) return 0;
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_std;
  tx_frame.MsgID = id;
  tx_frame.FIR.B.DLC = size;
  memcpy(tx_frame.data.u8, buffer, size);
  ESP32Can.CANWriteFrame(&tx_frame);
  sendId = 0;
  sendCount = -1;
  return 1;
}

IRAM_ATTR void CanPlusESP32::poll() {
  if (!ready) return;
  CAN_frame_t rx_frame;

  // Receive next CAN frame from queue
  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 0) == pdTRUE) {
    // extend Id not supported
    if (rx_frame.FIR.B.FF != CAN_frame_std) return;
    // remote request not supported
    if (rx_frame.FIR.B.RTR == CAN_RTR) return;
    // mesage lengths other than 8 not supported
    if (rx_frame.FIR.B.DLC != 8) return;

    if (!callbackProcess(rx_frame.MsgID, rx_frame.data.u8, rx_frame.FIR.B.DLC)) {
      // wasn't a callback so quickly get the next message
      poll();
    }

  }
}

CanPlusESP32 canPlus;

#endif
