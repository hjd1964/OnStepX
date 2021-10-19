// ---------------------------------------------------------------------------------------------------
// Mount status LED and buzzer

#include "Status.h"

#ifdef MOUNT_PRESENT

#include "../../../lib/tasks/OnTask.h"

#if STATUS_MOUNT_LED != OFF && STATUS_MOUNT_LED_PIN != OFF
  bool ledOn = false;
  bool ledOff = false;
  void flash() {
    if (ledOff) { digitalWriteEx(STATUS_LED_PIN, !STATUS_LED_ON_STATE); return; }
    if (ledOn) { digitalWriteEx(STATUS_LED_PIN, STATUS_LED_ON_STATE); return; }
    static uint8_t cycle = 0;
    if ((cycle++)%2 == 0) {
      digitalWriteEx(STATUS_MOUNT_LED_PIN, !STATUS_MOUNT_LED_ON_STATE);
    } else {
      digitalWriteEx(STATUS_MOUNT_LED_PIN, STATUS_MOUNT_LED_ON_STATE);
    }
  }
#endif

void Status::init() {
  #if STATUS_LED == ON
    // if anything else is using the status LED, disable it
    if ((STATUS_MOUNT_LED != OFF && STATUS_MOUNT_LED_PIN == STATUS_LED_PIN) || 
        (STATUS_BUZZER != OFF && STATUS_BUZZER_PIN == STATUS_LED_PIN)) tasks.remove(tasks.getHandleByName("StaLed"));
  #endif

  #if STATUS_BUZZER_MEMORY == ON
    sound.enabled = misc.buzzer;
  #endif

  #if STATUS_MOUNT_LED != OFF && STATUS_MOUNT_LED_PIN != OFF
    if (!tasks.getHandleByName("mntLed")) {
      pinModeEx(STATUS_MOUNT_LED_PIN, OUTPUT);
      VF("MSG: Mount, status start LED task (variable rate priority 4)... ");
      statusTaskHandle = tasks.add(0, 0, true, 4, flash, "mntLed");
      if (statusTaskHandle) { VLF("success"); } else { VLF("FAILED!"); }
    }
  #endif
}

// mount status LED flash rate (in ms)
void Status::flashRate(int period) {
  #if STATUS_MOUNT_LED != OFF && STATUS_MOUNT_LED_PIN != OFF
    if (period == 0) { period = 500; ledOff = true; } else ledOff = false;
    if (period == 1) { period = 500; ledOn = true; } else ledOn = false;
    tasks.setPeriod(statusTaskHandle, period/2UL);
  #else
    period = period;
  #endif
}

Status status;

#endif
