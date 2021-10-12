// -----------------------------------------------------------------------------------------------------------------------------
// Sound
#pragma once

#include "../../Common.h"

#ifdef STATUS_BUZZER

class Sound {
  public:
    Sound();
    void alert(); // sound/buzzer
    void click(); // sound/beep
    void beep();  // sound/click

    bool enabled = STATUS_BUZZER_DEFAULT == ON; 
  private:
};

#endif
