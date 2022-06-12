// -----------------------------------------------------------------------------------------------------------------------------
// Sound
#pragma once

#include "../../Common.h"

#ifdef STATUS_BUZZER

class Sound {
  public:
    void init();  // get sound pins ready
    void alert(); // sound/buzzer
    void click(); // sound/beep
    void beep();  // sound/click

    bool enabled = STATUS_BUZZER_DEFAULT == ON;
  private:
    bool ready = false;
};

#endif
