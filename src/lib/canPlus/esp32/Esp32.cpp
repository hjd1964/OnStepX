// -----------------------------------------------------------------------------------
// CAN library (ESP32 TWAI backend)

#include "Esp32.h"

#if defined(CAN_PLUS) && CAN_PLUS == CAN_ESP32

#include "../../tasks/OnTask.h"

#include "driver/twai.h"   // ESP-IDF TWAI (CAN) driver
#include "esp_err.h"

void canEsp32Monitor() { canPlus.poll(); }

static bool twaiReady = false;

CanPlusESP32::CanPlusESP32() {}

static twai_timing_config_t timingFromBaud() {
  #if CAN_BAUD == 125000
    return TWAI_TIMING_CONFIG_125KBITS();
  #elif CAN_BAUD == 250000
    return TWAI_TIMING_CONFIG_250KBITS();
  #elif CAN_BAUD == 500000
    return TWAI_TIMING_CONFIG_500KBITS();
  #elif CAN_BAUD == 1000000
    return TWAI_TIMING_CONFIG_1MBITS();
  #else
    return TWAI_TIMING_CONFIG_500KBITS();
  #endif
}

void CanPlusESP32::init() {
  VF("MSG: CanPlus, CAN_ESP32 (TWAI) Start... ");

  // General config: NORMAL mode means we transmit and ACK others.
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
      (gpio_num_t)CAN_TX_PIN,
      (gpio_num_t)CAN_RX_PIN,
      TWAI_MODE_NORMAL
  );

  // You can tune these if you want more buffering:
  g_config.tx_queue_len = 32;
  g_config.rx_queue_len = 128;

  // Optional: reduce ISR workload by not receiving every single frame if you use acceptance filters.
  // Here we accept everything (no filtering at hardware level).
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  twai_timing_config_t t_config = timingFromBaud();

  esp_err_t err = twai_driver_install(&g_config, &t_config, &f_config);
  if (err != ESP_OK) {
    V("FAILED! twai_driver_install err="); VL((int)err);
    ready = false;
    return;
  }

  err = twai_start();
  if (err != ESP_OK) {
    V("FAILED! twai_start err="); VL((int)err);
    twai_driver_uninstall();
    ready = false;
    return;
  }

  twaiReady = true;
  ready = true;

  VLF("success");

  VF("MSG: CanPlus, start callback monitor task (rate "); V(CAN_RECV_RATE_MS);
  VF("ms priority 3)... ");
  if (tasks.add(CAN_RECV_RATE_MS, 0, true, 3, canEsp32Monitor, "CanESP")) {
    VLF("success");

    // setup the burst task disabled (period 0) and so it only runs once
    burst = tasks.add(0, 0, true, 3, canEsp32Monitor, "CanBrst");

  } else {
    VLF("FAILED!");
  }
}

int CanPlusESP32::writePacket(int id, const uint8_t *buffer, size_t size) {
  if (!ready || !twaiReady) return 0;
  if (size > 8) return 0;
  if (size > 0 && buffer == nullptr) return 0;

  twai_message_t msg = {};
  msg.identifier = (uint32_t)(id & 0x7FF);
  msg.extd = 0;              // standard 11-bit
  msg.rtr  = 0;
  msg.data_length_code = (uint8_t)size;
  if (size) memcpy(msg.data, buffer, size);

  // Zero timeout: do not block the caller; return failure if TX queue is full or bus is off.
  esp_err_t err = twai_transmit(&msg, 0);

  if (err == ESP_OK) {
    tx_ok++;
    return 1;
  } else {
    tx_fail++;
    // Optional 1 Hz debug:
    // DL1("TWAI tx fail err="); DL1((int)err);
    return 0;
  }
}

int CanPlusESP32::writePacketRtr(int id, size_t dlc) {
  if (!ready || !twaiReady) return 0;
  if (dlc > 8) return 0;

  twai_message_t msg = {};
  msg.identifier = (uint32_t)(id & 0x7FF);
  msg.extd = 0;
  msg.rtr  = 1;
  msg.data_length_code = (uint8_t)dlc;

  esp_err_t err = twai_transmit(&msg, 0);

  if (err == ESP_OK) { tx_ok++; return 1; }
  tx_fail++;
  return 0;
}

void CanPlusESP32::rxBrust(uint32_t periodUs) {
  const uint32_t now = micros();
  const uint32_t until = now + periodUs;

  if (!burstEnabled) {
    burstEnabled = true;
    burstUntilUs = until;
    tasks.setPeriod(burst, 1);
  } else {
    if ((int32_t)(until - burstUntilUs) > 0) burstUntilUs = until;
  }
}

void CanPlusESP32::poll() {
  if (!ready || !twaiReady) return;

  if (burstEnabled && (int32_t)(micros() - burstUntilUs) >= 0) {
    tasks.setPeriod(burst, 0);
    burstEnabled = false;
  }

  const uint32_t start = micros();

  // shorten workload during busts
  const int frames = burstEnabled ? (maxFrames >> 1) : maxFrames;

  // drain up to N frames per poll so we don't monopolize the CPU
  for (int n = 0; n < frames; n++) {
    twai_message_t msg;
    esp_err_t err = twai_receive(&msg, 0);  // non-blocking
    if (err != ESP_OK) break;

    // drop extended IDs (unless you want to support them)
    if (msg.extd) { rx_drop_ext++; continue; }
    // drop RTR frames (unless you want them)
    if (msg.rtr)  { rx_drop_rtr++; continue; }
    // DLC sanity
    if (msg.data_length_code < 1 || msg.data_length_code > 8) { rx_drop_len++; continue; }

    rx_ok++;

    callbackProcess((int)(msg.identifier & 0x7FF), (uint8_t*)msg.data, msg.data_length_code);

    debugStatus();

    if ((uint32_t)(micros() - start) > budgetUs) break;
  }
}

void CanPlusESP32::debugStatus() {
  #if DEBUG != OFF && DEBUG_CAN != OFF
    if (!ready || !twaiReady) return;
    twai_status_info_t s;
    if (twai_get_status_info(&s) != ESP_OK) return;

    // 1 Hz debug suggested:
    D1("MSG: TWAI, state="); D1((int)s.state);
    D1(" tx_err="); D1((int)s.tx_error_counter);
    D1(" rx_err="); D1((int)s.rx_error_counter);
    D1(" bus_err="); DL1((int)s.bus_error_count);
  #endif
}

CanPlusESP32 canPlus;

#endif
