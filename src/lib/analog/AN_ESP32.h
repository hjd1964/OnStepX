// -----------------------------------------------------------------------------------
// Arduino compatible analogWrite() and tone() functions for the ESP32
#pragma once

#include "Arduino.h"
#include "../../Common.h"

#if defined(ESP32)
  #if !defined(ESP_ARDUINO_VERSION) || ESP_ARDUINO_VERSION < 131072 + 3 // version 2.0.3
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
