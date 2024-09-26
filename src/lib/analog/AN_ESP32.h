// -----------------------------------------------------------------------------------
// Arduino compatible analogWrite() and tone() functions for the ESP32
#pragma once

#include "Arduino.h"
#include "../../Common.h"

#if defined(ESP32)
  #if ESP_ARDUINO_VERSION < 0x20000 + 3
    // starts tone on the specified pin of frequency (Hz) for the duration (in ms)
    extern void tone(uint8_t pin, unsigned int frequency, unsigned long duration = 0);
    // stops tone on the specified pin
    extern void noTone(uint8_t pin);

    // analog pwm of value (default 8 bit) on the specified pin
    extern void analogWrite(uint8_t pin, int value);
    // analog pwm resolution (8 to 16 bit)
    extern void analogWriteResolution(int value);
    // analog pwm frequency (500 to 10000Hz)
    extern void analogWriteFrequency(int value);
  #endif
#endif
