// -----------------------------------------------------------------------------------------------------------------------------
// Sound

#include "../../Common.h"
#include "../../tasks/OnTask.h"
extern Tasks tasks;
#include "Sound.h"

#if BUZZER == ON
  void buzzerOff() {
    digitalWriteEx(BUZZER_PIN, !BUZZER_ON_STATE);
  }
#endif

Sound::Sound() {
  #if BUZZER == ON
    pinModeEx(BUZZER_PIN, OUTPUT);
    digitalWriteEx(BUZZER_PIN, !BUZZER_ON_STATE);
  #endif
}

void Sound::alert() {
  if (enabled) {
    #if BUZZER == ON
      digitalWriteEx(BUZZER_PIN, BUZZER_ON_STATE);
      tasks.add(100, 0, false, 7, buzzerOff);
    #endif
    #if BUZZER >= 0
      tone(BUZZER_PIN, BUZZER, 1000);
    #endif
  }
}

void Sound::beep() {
  if (enabled) {
    #if BUZZER == ON
      digitalWriteEx(BUZZER_PIN, BUZZER_ON_STATE);
      tasks.add(25, 0, false, 7, buzzerOff);
    #endif
    #if BUZZER >= 0
      tone(BUZZER_PIN, BUZZER, 250);
    #endif
  }
}

void Sound::click() {
  if (enabled) {
    #if BUZZER == ON
      digitalWriteEx(BUZZER_PIN, BUZZER_ON_STATE);
      tasks.add(5, 0, false, 7, buzzerOff);
    #endif
    #if BUZZER >= 0
      tone(BUZZER_PIN, BUZZER, 50);
    #endif
  }
}
