//--------------------------------------------------------------------------------------------------
// telescope mount control
#pragma once

#include "../../../Common.h"

#ifdef MOUNT_PRESENT

class St4 {
  public:
    void init();

    // monitor ST4 port for guiding, basic hand controller, and smart hand controller
    void poll();

  private:
 
};

extern St4 st4;

#endif
