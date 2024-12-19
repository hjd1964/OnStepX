// -----------------------------------------------------------------------------------------------------------------------------
// General purpose "virtual" watchdog
#pragma once

#include "../../Common.h"

// Watchdog control macros
#ifdef __AVR_ATmega2560__
  #if defined(WATCHDOG) && WATCHDOG != OFF
    #include <avr/wdt.h>
    #define WDT_ENABLE wdt_enable(WDTO_8S)
    #define WDT_RESET wdt_reset()
    #define WDT_DISABLE wdt_disable()
  #else
    #define WDT_ENABLE
    #define WDT_RESET
    #define WDT_DISABLE
  #endif
#else
  #if defined(WATCHDOG) && WATCHDOG != OFF
    #define WDT_ENABLE watchdog.enable(8);
    #define WDT_RESET watchdog.reset();
    #define WDT_DISABLE watchdog.disable();
  #else
    #define WDT_ENABLE
    #define WDT_RESET
    #define WDT_DISABLE
  #endif
#endif

#if !defined(__AVR_ATmega2560__) && defined(WATCHDOG) && WATCHDOG != OFF

class Watchdog {
  public:
    // initialize and start the watchdog with timeout (to reset MCU) of seconds
    void enable(int seconds);

    // call atleast once every (seconds) to reset the count or the MCU will be reset if WD is enabled
    inline void reset() { count = 0; }

    // disable the watchdog
    inline void disable() { enabled = false; }

    // for internal use
    void poll();

  private:
    volatile int16_t count = 0;
    volatile int16_t seconds = -1;
    volatile bool enabled = false;
};

extern Watchdog watchdog;

#endif
