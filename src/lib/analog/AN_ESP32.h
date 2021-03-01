// -----------------------------------------------------------------------------------
// Arduino compatible analogWrite() and tone() functions for the ESP32

#pragma once

#ifdef ESP32

// starts tone on the specified pin of frequency (Hz) for the duration (in ms)
extern void tone(uint8_t pin, unsigned int frequency, unsigned long duration = 0);
// stops tone on the specified pin
extern void noTone(uint8_t pin);
// analog pwm of value (10 bit) on the specified pin
extern void analogWrite(uint8_t pin, int value);

#endif
