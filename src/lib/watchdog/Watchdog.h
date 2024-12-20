// -----------------------------------------------------------------------------------------------------------------------------
// General purpose "virtual" watchdog
#pragma once

#include "../../Common.h"

#if defined(WATCHDOG) && WATCHDOG != OFF || true

class Watchdog {
  public:
    // initialize and start the watchdog with timeout (to reset MCU) of seconds
    void enable(int seconds);

    // call at least once per timeout period or the MCU will be reset if the WD is enabled
    void reset();

    // disable the watchdog
    void disable();

    // for internal use
    void poll();

  private:
    volatile int16_t count = 0;
    volatile int16_t seconds = -1;
    volatile bool enabled = false;
};

extern Watchdog watchdog;

#endif
