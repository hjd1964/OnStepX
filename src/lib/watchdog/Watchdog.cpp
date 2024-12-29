// -----------------------------------------------------------------------------------------------------------------------------
// General purpose "virtual" watchdog
#include "Watchdog.h"

#if defined(WATCHDOG) && WATCHDOG != OFF || true

#ifndef __AVR_ATmega2560__
  #include "../tasks/OnTask.h"
  void watchdogWrapper() { watchdog.poll(); }
#else
  void (*mega2560restart) (void) = 0;
  #include <avr/wdt.h>
#endif

void Watchdog::enable(int seconds) {
  #ifndef __AVR_ATmega2560__
    if (this->seconds == -1) {
      this->seconds = seconds;

      VF("MSG: Watchdog, start monitor task (100ms rate priority 0)... ");
      uint8_t taskHandle = tasks.add(100, 0, true, 0, watchdogWrapper, "wdog");

      if (taskHandle) {
        VLF("success"); 
        if (!tasks.requestHardwareTimer(taskHandle, 0)) { DLF("WRN: Watchdog::enable(), didn't get h/w timer (using s/w timer!!!)"); }
      } else {
        VLF("FAILED!");
        return;
      }
    }
  #else
    if (seconds == 1) wdt_enable(WDTO_1S); else
    if (seconds == 2) wdt_enable(WDTO_2S); else
    if (seconds == 4) wdt_enable(WDTO_4S); else
    if (seconds == 8) wdt_enable(WDTO_8S); else return;
  #endif
  enabled = true;
}

void Watchdog::disable() {
  #ifndef __AVR_ATmega2560__
    enabled = false;
  #else
    wdt_disable();
  #endif
}

void Watchdog::reset() {
  #ifndef __AVR_ATmega2560__
    count = 0;
  #else
    wdt_reset();
  #endif
}

void Watchdog::poll() {
  if (!enabled) return;
  if (count++ > 10 * seconds) { HAL_RESET(); }
}

Watchdog watchdog;

#endif
