// Flashes an esp8266 based SWS on the SERIAL_B interface by passing data to/from SERIAL_A

#include "AddonFlasher.h"
#include "../../lib/tasks/OnTask.h"

// ADDON_TRIGR_PIN  HIGH for run and LOW for trigger serial passthrough mode
// ADDON_RESET_PIN  HIGH for run and LOW for reset
// ADDON_GPIO0_PIN  HIGH for run and LOW for firmware upload

#if SERIAL_B_ESP_FLASHING == ON

  #if ADDON_TRIGR_PIN != OFF
    void addonFlasherWrapper() { addonFlasher.poll(); }
  #endif

  void AddonFlasher::init() {
    addonGPIOPin = ADDON_GPIO0_PIN;
    addonResetPin = ADDON_RESET_PIN;
    VF("MSG: AddonFlasher, init gpio0="); V(addonGPIOPin); VF(", reset="); VL(addonResetPin);
    pinModeEx(addonGPIOPin, OUTPUT);
    pinModeEx(addonResetPin, OUTPUT);

    run();

    #if ADDON_TRIGR_PIN != OFF
      pinModeEx(ADDON_TRIGR_PIN, INPUT);
      VF("MSG: AddonFlasher, trigger pin monitor (rate 100ms priority 5)... ");
      if (tasks.add(100, 0, true, 5, addonFlasherWrapper, "adonTgr")) { VLF("success"); } else { VLF("FAILED!"); }
    #endif
  }

  void AddonFlasher::go(bool timeout) {
    // program mode and 115200 serial comms
    flash();

    VLF("MSG: AddonFlasher, activating serial passthrough...");

    // so we have a total of 1.5 minutes to start the upload
    unsigned long lastRead = millis() + 85000;
    while (true) {
      // read from port 1, send to port 0:
      if (SERIAL_B.available()) {
        int inByte = SERIAL_B.read();
        delayMicroseconds(5);
        SERIAL_A.write(inByte);
        delayMicroseconds(5);
      }

      // read from port 0, send to port 1:
      if (SERIAL_A.available()) {
        int inByte = SERIAL_A.read();
        delayMicroseconds(5);
        SERIAL_B.write(inByte);
        delayMicroseconds(5);
        if (millis() > lastRead) lastRead = millis();
      }
      tasks.yield();

      // wait 5 seconds w/no traffic before resuming normal operation
      if (timeout && (long)(millis() - lastRead) > 5000) break;
    }
    VLF("MSG: AddonFlasher, serial passthrough deactivated");

    // back to run mode and normal serial comms
    run(true);
  }

  void AddonFlasher::run(bool setSerial) {
    VLF("MSG: AddonFlasher, setting addon run mode");

    // enter run mode
    digitalWriteEx(addonGPIOPin, HIGH);
    reset();

    if (setSerial) {
      #if defined(SERIAL_B_RX) && defined(SERIAL_B_TX) && !defined(SERIAL_B_RXTX_SET)
        SERIAL_B.begin(SERIAL_B_BAUD_DEFAULT, SERIAL_8N1, SERIAL_B_RX, SERIAL_B_TX);
      #else
        SERIAL_B.begin(SERIAL_B_BAUD_DEFAULT);
      #endif

      #if defined(SERIAL_A_RX) && defined(SERIAL_A_TX) && !defined(SERIAL_A_RXTX_SET)
        SERIAL_A.begin(SERIAL_A_BAUD_DEFAULT, SERIAL_8N1, SERIAL_A_RX, SERIAL_A_TX);
      #else
        SERIAL_A.begin(SERIAL_A_BAUD_DEFAULT);
      #endif
      tasks.yield(1000);
    }
  }

  void AddonFlasher::flash() {
    VLF("MSG: AddonFlasher, setting addon program mode");

    #if defined(SERIAL_A_RX) && defined(SERIAL_A_TX) && !defined(SERIAL_A_RXTX_SET)
      SERIAL_A.begin(115200, SERIAL_8N1, SERIAL_A_RX, SERIAL_A_TX);
    #else
      SERIAL_A.begin(115200);
    #endif

    #if defined(SERIAL_B_RX) && defined(SERIAL_B_TX) && !defined(SERIAL_B_RXTX_SET)
      SERIAL_B.begin(115200, SERIAL_8N1, SERIAL_B_RX, SERIAL_B_TX);
    #else
      SERIAL_B.begin(115200);
    #endif
    tasks.yield(1000);

    // enter program mode
    digitalWriteEx(addonGPIOPin, LOW);
    reset();
  }

  void AddonFlasher::reset() {
    // reset LOW (active) HIGH (inactive)
    tasks.yield(20);
    digitalWriteEx(addonResetPin, LOW);
    tasks.yield(20);
    digitalWriteEx(addonResetPin, HIGH);
    tasks.yield(20);
  }

  void AddonFlasher::poll() {
    #if ADDON_TRIGR_PIN == ON
      if (ADDON_TRIGR_PIN != OFF && digitalRead(ADDON_TRIGR_PIN) == LOW) go(false);
    #endif
  }

  AddonFlasher addonFlasher;
#endif
