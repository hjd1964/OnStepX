// ---------------------------------------------------------------------------------------------------
// Mount status LED and buzzer

#include "Status.h"

#ifdef MOUNT_PRESENT

#include "../../../lib/tasks/OnTask.h"
#include "../park/Park.h"

#if STATUS_MOUNT_LED != OFF && MOUNT_LED_PIN != OFF
  bool ledOn = false;
  bool ledOff = false;
  void flash() {
    if (ledOff) { digitalWriteEx(MOUNT_LED_PIN, !MOUNT_LED_ON_STATE); return; }
    if (ledOn) { digitalWriteEx(MOUNT_LED_PIN, MOUNT_LED_ON_STATE); return; }
    static uint8_t cycle = 0;
    if ((cycle++)%2 == 0) {
      digitalWriteEx(MOUNT_LED_PIN, !MOUNT_LED_ON_STATE);
    } else {
      digitalWriteEx(MOUNT_LED_PIN, MOUNT_LED_ON_STATE);
    }
  }
#endif

void generalWrapper() { mountStatus.general(); }

// get mount status ready
void Status::init() {
  if (!nv.hasValidKey()) {
    VLF("MSG: Mount, status writing defaults to NV");
    nv.write(NV_MOUNT_STATUS_BASE, (uint8_t)sound.enabled);
  }

  #if STATUS_BUZZER_MEMORY == ON
    sound.enabled = nv.read(NV_MOUNT_STATUS_BASE);
  #endif

  #if PARK_STATUS != OFF && PARK_STATUS_PIN != OFF
    pinModeEx(PARK_STATUS_PIN, OUTPUT);
  #endif
}

// mount status wake on demand
void Status::wake() {
  static bool ready = false;

  if (!ready) {
    #if STATUS_LED == ON
      // if Mount buzzer or status LED are using the Telescope status LED pin, disable it and use here now
      if (STATUS_MOUNT_LED != OFF && MOUNT_LED_PIN == STATUS_LED_PIN) tasks.remove(tasks.getHandleByName("StaLed"));
      if (STATUS_BUZZER != OFF && STATUS_BUZZER_PIN == STATUS_LED_PIN) tasks.remove(tasks.getHandleByName("StaLed"));
    #endif

    #if STATUS_MOUNT_LED != OFF && MOUNT_LED_PIN != OFF
      if (!tasks.getHandleByName("mntLed")) {
        pinModeEx(MOUNT_LED_PIN, OUTPUT);
        VF("MSG: Mount, status start LED task (variable rate priority 4)... ");
        statusTaskHandle = tasks.add(0, 0, true, 4, flash, "mntLed");
        if (statusTaskHandle) { VLF("success"); } else { VLF("FAILED!"); }
      }
    #endif

    #if STATUS_BUZZER != OFF
      VLF("MSG: Mount, status start buzzer");
      sound.init();
    #endif

    VF("MSG: Mount, status start general status task (1s rate priority 4)... ");
    if (tasks.add(1000, 0, true, 4, generalWrapper, "genSta")) { VLF("success"); } else { VLF("FAILED!"); }

    ready = true;
  }
}

// mount status LED flash rate (in ms)
void Status::flashRate(int period) {
  #if STATUS_MOUNT_LED != OFF && MOUNT_LED_PIN != OFF
    if (period == 0) { period = 500; ledOff = true; } else ledOff = false;
    if (period == 1) { period = 500; ledOn = true; } else ledOn = false;
    tasks.setPeriod(statusTaskHandle, period/2UL);
  #else
    period = period;
  #endif
}

// mount misc. general status indicators
void Status::general() {
  #if PARK_STATUS != OFF && PARK_STATUS_PIN != OFF
    digitalWriteEx(PARK_STATUS_PIN, (park.state == PS_PARKED) ? PARK_STATUS : !PARK_STATUS)
  #endif
}

Status mountStatus;

#endif
