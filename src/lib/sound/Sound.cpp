// -----------------------------------------------------------------------------------------------------------------------------
// Sound

#include "Sound.h"

#ifdef STATUS_BUZZER

#include "../tasks/OnTask.h"

void Sound::init() {
  #if STATUS_BUZZER == ON
    pinModeEx(STATUS_BUZZER_PIN, OUTPUT);
    digitalWriteEx(STATUS_BUZZER_PIN, !STATUS_BUZZER_ON_STATE);
  #endif
  ready = true;
}

#if STATUS_BUZZER == ON
  uint8_t _buzzerHandle = 0;
  void buzzerOff() {
    digitalWriteEx(STATUS_BUZZER_PIN, !STATUS_BUZZER_ON_STATE);
    tasks.setDurationComplete(_buzzerHandle);
    _buzzerHandle = 0;
  }
#endif

void Sound::alert() {
  if (ready && enabled) {
    #if STATUS_BUZZER == ON
      digitalWriteEx(STATUS_BUZZER_PIN, STATUS_BUZZER_ON_STATE);
      if (_buzzerHandle) tasks.remove(_buzzerHandle);
      _buzzerHandle = tasks.add(1000, 0, false, 6, buzzerOff);
    #endif
    #if STATUS_BUZZER >= 0
      tone(STATUS_BUZZER_PIN, STATUS_BUZZER, 1000);
    #endif
  }
}

void Sound::beep() {
  if (ready && enabled) {
    #if STATUS_BUZZER == ON
      digitalWriteEx(STATUS_BUZZER_PIN, STATUS_BUZZER_ON_STATE);
      if (_buzzerHandle) tasks.remove(_buzzerHandle);
      _buzzerHandle = tasks.add(250, 0, false, 6, buzzerOff);
    #endif
    #if STATUS_BUZZER >= 0
      tone(STATUS_BUZZER_PIN, STATUS_BUZZER, 250);
    #endif
  }
}

void Sound::click() {
  if (ready && enabled) {
    #if STATUS_BUZZER == ON
      digitalWriteEx(STATUS_BUZZER_PIN, STATUS_BUZZER_ON_STATE);
      if (_buzzerHandle) tasks.remove(_buzzerHandle);
      _buzzerHandle = tasks.add(50, 0, false, 6, buzzerOff);
    #endif
    #if STATUS_BUZZER >= 0
      tone(STATUS_BUZZER_PIN, STATUS_BUZZER, 50);
    #endif
  }
}

#endif
