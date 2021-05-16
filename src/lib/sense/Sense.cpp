// -----------------------------------------------------------------------------------
// combined digital and analog threshold read, 10 bit analog read resolution only
// analog mode reads uses software schmitt trigger with threshold/hysteresis-band
// digital mode reads have basic hf EMI/RFI noise filtering

#include "Sense.h"

Sense::Sense(int8_t pin, int8_t inputMode, bool isAnalog, int threshold, int hysteresis) {
  if (inputMode == OUTPUT) { this->inputMode = OFF; return; }
  this->inputMode = inputMode;
  this->isAnalog = isAnalog;
  this->pin = pin;
  this->threshold = threshold;
  this->hysteresis = hysteresis;
  this->triggerMode = triggerMode;
  if (isAnalog) {
  	if (threshold + hysteresis > 1023) { threshold = (int)(1023*0.95); hysteresis = 0; }
	  if (threshold - hysteresis < 0) { threshold = (int)(1023*0.05); hysteresis = 0; }
  }

  invert = false;
  int mode = INPUT;
  switch (inputMode) {
    case ON: case HIGH: mode = INPUT_PULLUP; break;
    case LOW: 
      invert = true;
      #ifdef INPUT_PULLDOWN
        mode = INPUT_PULLDOWN;
      #endif
    break;
  }
  pinMode(pin, mode);

  reset();
}

int Sense::read() {
  int value = lastValue;
  if (isAnalog) {
    int sample = analogRead(pin);
    if (sample > threshold + hysteresis ) value = HIGH;
    if (sample <= threshold - hysteresis ) value = LOW;
  } else {
    int sample = digitalRead(pin); delayMicroseconds(10); int sample1 = digitalRead(pin);
    if (stableSample != sample || sample1 != sample) { stableStartMs = millis(); stableSample = sample; }
    long stableMs = (long)(millis() - stableStartMs);
    if (stableMs > hysteresis) value = stableSample;
  }
  lastValue = value;
  if (invert) { if (value == LOW) return HIGH; else return LOW; } else return value;
}

void Sense::poll() {
  int value = lastValue;
  if (!isAnalog) {
    int sample = digitalRead(pin); delayMicroseconds(10); int sample1 = digitalRead(pin);
    if (stableSample != sample || sample1 != sample) { stableStartMs = millis(); stableSample = sample; }
    long stableMs = (long)(millis() - stableStartMs);
    if (stableMs > hysteresis) value = stableSample;
  }
  lastValue = value;
}

void Sense::reset() {
  if (inputMode == OFF) return;
  if (isAnalog) { if (analogRead(pin) > threshold) lastValue = HIGH; else lastValue = LOW; } else lastValue = digitalRead(pin);
  stableSample = lastValue;
}

uint8_t Senses::add(int8_t pin, int8_t inputMode, bool isAnalog, int threshold, int hysteresis) {
  if (senseCount >= SENSE_MAX) return 0;
  sense[senseCount] = new Sense(pin, inputMode, isAnalog, threshold, hysteresis);
  senseCount++;
  return senseCount;
}

int Senses::read(uint8_t handle) {
  if (handle == 0) return -1;
  return (*sense)[handle - 1].read();
}

void Senses::poll() {
  for (int i = 0; i < senseCount; i++) { (*sense)[i].poll(); }
}

Senses senses;