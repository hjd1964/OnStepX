// -----------------------------------------------------------------------------------------------------------------------------
// General purpose "virtual" watchdog
#pragma once

#include "../../Common.h"

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
