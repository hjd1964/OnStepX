// -----------------------------------------------------------------------------------------------------------------------------
// Sound

#include "Sound.h"

#ifdef STATUS_BUZZER

#include "../tasks/OnTask.h"

#if STATUS_BUZZER == ON
  void buzzerOff() {
    digitalWriteEx(STATUS_BUZZER_PIN, !STATUS_BUZZER_ON_STATE);
  }
#endif

Sound::Sound() {
  #if STATUS_BUZZER == ON
    pinModeEx(STATUS_BUZZER_PIN, OUTPUT);
    digitalWriteEx(STATUS_BUZZER_PIN, !STATUS_BUZZER_ON_STATE);
  #endif
}

void Sound::alert() {
  if (enabled) {
    #if STATUS_BUZZER == ON
      digitalWriteEx(STATUS_BUZZER_PIN, STATUS_BUZZER_ON_STATE);
      tasks.add(100, 0, false, 6, buzzerOff);
    #endif
    #if STATUS_BUZZER >= 0
      tone(STATUS_BUZZER_PIN, STATUS_BUZZER, 1000);
    #endif
  }
}

void Sound::beep() {
  if (enabled) {
    #if STATUS_BUZZER == ON
      digitalWriteEx(STATUS_BUZZER_PIN, STATUS_BUZZER_ON_STATE);
      tasks.add(25, 0, false, 6, buzzerOff);
    #endif
    #if STATUS_BUZZER >= 0
      tone(STATUS_BUZZER_PIN, STATUS_BUZZER, 250);
    #endif
  }
}

void Sound::click() {
  if (enabled) {
    #if STATUS_BUZZER == ON
      digitalWriteEx(STATUS_BUZZER_PIN, STATUS_BUZZER_ON_STATE);
      tasks.add(5, 0, false, 6, buzzerOff);
    #endif
    #if STATUS_BUZZER >= 0
      tone(STATUS_BUZZER_PIN, STATUS_BUZZER, 50);
    #endif
  }
}

#endif
