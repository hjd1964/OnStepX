// -----------------------------------------------------------------------------------------------------------------------------
// Sound

#include "../../common.h"

class Sound {
  public:
    void alert();
    void click();
    void beep();

    bool enabled = BUZZER_DEFAULT; 
  private:
};
