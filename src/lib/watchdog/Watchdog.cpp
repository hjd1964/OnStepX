// -----------------------------------------------------------------------------------------------------------------------------
// General purpose "virtual" watchdog
#include "Watchdog.h"

#include "../tasks/OnTask.h"

#if !defined(__AVR_ATmega2560__) && defined(WATCHDOG) && WATCHDOG != OFF

void watchdogWrapper() { watchdog.poll(); }

void Watchdog::enable(int seconds) {
  if (this->seconds == -1) {
    this->seconds = seconds;

    // poll at 100ms
    VF("MSG: Watchdog, start monitor task (100ms rate priority 0)... ");
    uint8_t taskHandle = tasks.add(100, 0, true, 0, watchdogWrapper, "wdog");

    if (taskHandle) {
      VLF("success"); 
      if (!tasks.requestHardwareTimer(taskHandle, 0)) { DLF("WRN: Watchdog::enable(), didn't get h/w timer (using s/w timer!!!)"); }
    } else { VLF("FAILED!"); return; }
  }
  enabled = true;
}

void Watchdog::poll() {
  if (!enabled) return;
  if (count++ > 10 * seconds) { HAL_RESET(); }
}

Watchdog watchdog;

#endif
