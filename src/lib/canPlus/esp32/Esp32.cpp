// -----------------------------------------------------------------------------------
// CAN library

#include "Esp32.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN_ESP32

#include "../../tasks/OnTask.h"

#include <ESP32CAN.h> // https://github.com/miwagner/ESP32-Arduino-CAN
#include <CAN_config.h>

void canEsp32Monitor() { canPlus.poll(); }

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
  CAN_cfg.rx_queue = xQueueCreate(32, sizeof(CAN_frame_t));
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

int CanPlusESP32::writePacket(int id, const uint8_t *buffer, size_t size) {
  if (!ready) return 0;
  if (size > 8) return 0;
  if (size > 0 && buffer == nullptr) return 0;

  CAN_frame_t tx_frame {0};
  tx_frame.FIR.B.FF = CAN_frame_std;
  tx_frame.MsgID = id & 0x7FF;
  tx_frame.FIR.B.DLC = (uint8_t)size;
  if (size && buffer) {
    memcpy(tx_frame.data.u8, buffer, size);
  }
  bool ok = ESP32Can.CANWriteFrame(&tx_frame);

  if (ok) tx_ok++; else tx_fail++;
  return ok ? 1 : 0;
}

int CanPlusESP32::writePacketRtr(int id, size_t dlc) {
  if (!ready) return 0;
  if (dlc > 8) return 0;

  CAN_frame_t tx_frame {0};
  tx_frame.FIR.B.FF  = CAN_frame_std;
  tx_frame.FIR.B.RTR = CAN_RTR;
  tx_frame.MsgID     = id & 0x7FF;
  tx_frame.FIR.B.DLC = (uint8_t)dlc;

  bool ok = ESP32Can.CANWriteFrame(&tx_frame);
  if (ok) tx_ok++; else tx_fail++;
  return ok ? 1 : 0;
}

void CanPlusESP32::poll() {
  if (!ready) return;
  CAN_frame_t rx_frame = {0};

  for (int n = 0; n < 8; n++) {
    if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 0) != pdTRUE) break;

    // extend Id not supported
    if (rx_frame.FIR.B.FF != CAN_frame_std) { rx_drop_ext++; continue; }
    // remote request not supported
    if (rx_frame.FIR.B.RTR == CAN_RTR) { rx_drop_rtr++; continue; }
    // mesage lengths greater than 8 not supported
    if (rx_frame.FIR.B.DLC < 1 || rx_frame.FIR.B.DLC > 8) { rx_drop_len++; continue; }

    rx_ok++;
    callbackProcess(rx_frame.MsgID & 0x7FF, rx_frame.data.u8, rx_frame.FIR.B.DLC);
  }
}

CanPlusESP32 canPlus;

#endif
