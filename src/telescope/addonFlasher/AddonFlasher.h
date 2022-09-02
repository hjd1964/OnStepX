// Flashes an esp8266 based SWS on the SERIAL_B interface by passing data to/from SERIAL_A
#pragma once
#include "../../Common.h"

#if defined(SERIAL_PASSTHROUGH)
  class AddonFlasher {
    public:
      // startup the addon flasher, monitors the ADDON_TRIGR_PIN if present
      void init();

      // flashes the addon
      void go(bool timeout = true);

      // set the addon for run mode
      void run(bool setSerial = false);

      // set the addon for flash mode
      void flash();

      // reset the addon
      void reset();

      // monitors for flash button press
      void poll();
    private:
  };

  extern AddonFlasher addonFlasher;
#endif
