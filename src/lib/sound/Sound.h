// -----------------------------------------------------------------------------------------------------------------------------
// Sound

#include "../../Common.h"

class Sound {
  public:
    Sound();
    void alert(); // sound/buzzer
    void click(); // sound/beep
    void beep();  // sound/click

    bool enabled = BUZZER_DEFAULT == ON; 
  private:
};
