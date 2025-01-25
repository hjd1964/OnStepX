//--------------------------------------------------------------------------------------------------
// telescope mount control
#pragma once

#include "../../../Common.h"

#ifdef MOUNT_PRESENT

class St4 {
  public:
    void init();

    // monitor ST4 port for guiding, basic hand controller, and smart hand controller activation
    void poll();

    #if ST4_HAND_CONTROL == ON
    // transmit and recieve the smart hand controller data
    void pollSerial();
    #endif

  private:
    uint8_t handleSerialST4 = 0;
};

extern St4 st4;

#endif
