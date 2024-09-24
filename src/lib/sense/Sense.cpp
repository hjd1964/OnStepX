// -----------------------------------------------------------------------------------
// combined digital and analog threshold read, 10 bit analog read resolution only
// analog mode reads uses software schmitt trigger with threshold/hysteresis-band
// digital mode reads have basic hf EMI/RFI noise filtering

#include "Sense.h"
#include "../tasks/OnTask.h"

#ifndef ANALOG_READ_RANGE
  #define ANALOG_READ_RANGE 1023
#endif

SenseInput::SenseInput(int pin, int initState, int32_t trigger) {
  this->pin = pin;

  activeState = (trigger & 0b0000000000000000000001);
  threshold   = (trigger & 0b0000000000011111111110) >> 1;
  hysteresis  = (trigger & 0b0111111111100000000000) >> 11;
  isAnalog    = threshold != 0;

  VF("pin="); V(pin); V(", ");
  VF("active="); V(activeState ? "HIGH" : "LOW"); V(", ");
  VF("ths="); V(threshold); V(", ");
  VF("hys="); VL(hysteresis);

  if (isAnalog) {
    if (threshold < 0) {
      threshold = 0;
      VF("WRN: SenseInput::SenseInput(), Threshold for pin "); V(pin); VLF(" is below Analog range setting to "); VL(threshold);
    } else
    if (threshold > ANALOG_READ_RANGE) {
      threshold = ANALOG_READ_RANGE;
      VF("WRN: SenseInput::SenseInput(), Threshold for pin "); V(pin); VF(" is above Analog range setting to "); VL(threshold);
    }
	  if (threshold - hysteresis < 0) {
      hysteresis = threshold;
      VF("WRN: SenseInput::SenseInput(), Threshold - Hysteresis for pin "); V(pin); VF(" is below Analog range setting Hysteresis to "); VL(hysteresis);
    } else
  	if (threshold + hysteresis > ANALOG_READ_RANGE) {
      hysteresis = ANALOG_READ_RANGE - threshold;
      VF("WRN: SenseInput::SenseInput(), Threshold + Hysteresis for pin "); V(pin); VF(" is above Analog range setting Hysteresis to "); VL(hysteresis);
    }
  }

  pinModeEx(pin, initState);

  reset();
}

int SenseInput::isOn() {
  int value = lastValue;
  if (isAnalog) {
    #ifdef ESP32
      int sample = round((analogReadMilliVolts(pin)/3300.0F)*(float)ANALOG_READ_RANGE);
    #else
      int sample = analogRead(pin);
    #endif

    if (sample >= threshold + hysteresis) value = HIGH;
    if (sample <= threshold - hysteresis) value = LOW;
  } else {
    int sample = digitalReadEx(pin);
    if (stableSample != sample) { stableStartMs = millis(); stableSample = sample; }
    long stableMs = (long)(millis() - stableStartMs);
    if (stableMs >= hysteresis) value = stableSample;
  }
  if (reverseState) { if (value == LOW) value = HIGH; else value = LOW; }
  lastValue = value;
  return value == activeState;
}

int SenseInput::changed() {
  int value = lastChangedValue;
  if (isAnalog) {
    #ifdef ESP32
      int sample = round((analogReadMilliVolts(pin)/3300.0F)*(float)ANALOG_READ_RANGE);
    #else
      int sample = analogRead(pin);
    #endif

    if (sample >= threshold + hysteresis) value = HIGH;
    if (sample < threshold - hysteresis) value = LOW;
  } else {
    int sample = digitalReadEx(pin);
    if (stableSample != sample) { stableStartMs = millis(); stableSample = sample; }
    long stableMs = (long)(millis() - stableStartMs);
    if (stableMs >= hysteresis) value = stableSample;
  }
  if (reverseState) { if (value == LOW) value = HIGH; else value = LOW; }
  bool result = lastChangedValue != value;
  lastChangedValue = value;
  return result;
}

void SenseInput::poll() {
  int value = lastValue;
  if (!isAnalog) {
    int sample = digitalReadEx(pin);
    if (stableSample != sample) { stableStartMs = millis(); stableSample = sample; }
    long stableMs = (long)(millis() - stableStartMs);
    if (stableMs > hysteresis) value = stableSample;
    if (reverseState) { if (value == LOW) value = HIGH; else value = LOW; }
  }
  lastValue = value;
}

void SenseInput::reset() {
  if (isAnalog) {
    #ifdef ESP32
      int sample = round((analogReadMilliVolts(pin)/3300.0F)*(float)ANALOG_READ_RANGE);
    #else
      int sample = analogRead(pin);
    #endif
    if (sample > threshold) lastValue = HIGH; else lastValue = LOW;
  } else {
    lastValue = digitalReadEx(pin);
  }

  stableSample = lastValue;
}

// Manage sense pins

uint8_t Sense::add(int pin, int initState, int32_t trigger, bool force) {
  if ((pin == OFF || trigger == OFF) && !force) return 0;
  if (senseCount >= SENSE_MAX) { VF("WRN: Sense::add(), senseCount exceeded ignoring pin "); VL(pin); return 0; }
  if (trigger < 0 || trigger >= SENSE_MAX_TRIGGER) { VF("WRN: Sense::add(), trigger value invalid ignoring pin "); VL(pin); return 0; }
  if (initState != INPUT && initState != INPUT_PULLUP && initState != INPUT_PULLDOWN) {
    VF("WRN: Sense::add(), initState value invalid ignoring pin "); VL(pin);
    return 0;
  }
  VF("MSG: Sense"); V(senseCount); V(", init ");
  senseInput[senseCount] = new SenseInput(pin, initState, trigger);
  senseCount++;
  return senseCount;
}

int Sense::isOn(uint8_t handle) {
  if (handle == 0) return false;
  return senseInput[handle - 1]->isOn();
}

int Sense::changed(uint8_t handle) {
  if (handle == 0) return false;
  return senseInput[handle - 1]->changed();
}

void Sense::reverse(uint8_t handle, bool state) {
  if (handle == 0) return;
  senseInput[handle - 1]->reverse(state);
}

void Sense::poll() {
  for (int i = 0; i < senseCount; i++) { senseInput[i]->poll(); Y; }
}

Sense sense;
