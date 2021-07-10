// ---------------------------------------------------------------------------------------------------
// Mount status LED

#include "Mount.h"

#ifdef MOUNT_PRESENT

#include "../../Common.h"
#include "../../tasks/OnTask.h"
extern Tasks tasks;


  #if LED_MOUNT_STATUS != OFF && LED_MOUNT_STATUS_PIN != OFF
    bool ledOn = false;
    bool ledOff = false;
    void mountStatusFlash() {
      if (ledOff) { digitalWriteF(LED_STATUS_PIN, !LED_STATUS_ON_STATE); return; }
      if (ledOn) { digitalWriteF(LED_STATUS_PIN, LED_STATUS_ON_STATE); return; }
      static uint8_t cycle = 0;
      if ((cycle++)%2 == 0) {
        digitalWriteF(LED_MOUNT_STATUS_PIN, !LED_MOUNT_STATUS_ON_STATE);
      } else {
        digitalWriteF(LED_MOUNT_STATUS_PIN, LED_MOUNT_STATUS_ON_STATE);
      }
    }
  #endif

  // Prepare status LED feature for use
  void Mount::statusInit() {
    #if LED_MOUNT_STATUS != OFF && LED_MOUNT_STATUS_PIN != OFF
      if (!tasks.getHandleByName("mntLed")) {
        #if LED_STATUS == ON
          if (LED_MOUNT_STATUS_PIN == LED_STATUS_PIN) tasks.remove(tasks.getHandleByName("staLed"));
        #endif
        pinMode(LED_MOUNT_STATUS_PIN, OUTPUT);
        VF("MSG: Mount, start status LED task (variable rate priority 6)... ");
        statusTaskHandle = tasks.add(0, 0, true, 6, mountStatusFlash, "mntLed");
        if (statusTaskHandle) { VL("success"); } else { VL("FAILED!"); }
      }
    #endif
  }

  // Sets status LED flash period in ms, or use 0 to turn LED off, or 1 to turn LED on
  void Mount::statusSetPeriodMillis(int period) {
    #if LED_MOUNT_STATUS != OFF && LED_MOUNT_STATUS_PIN != OFF
      if (period == 0) { period = 500; ledOff = true; } else ledOff = false;
      if (period == 1) { period = 500; ledOn = true; } else ledOn = false;
      tasks.setPeriod(statusTaskHandle, period/2UL);
    #endif
  }

#endif
