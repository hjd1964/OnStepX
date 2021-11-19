// -----------------------------------------------------------------------------------
// combined digital and analog threshold read, 10 bit analog read resolution only
// analog mode reads uses software schmitt trigger with threshold/hysteresis-band
// digital mode reads have basic hf EMI/RFI noise filtering

#include "Sense.h"

SenseInput::SenseInput(int pin, int initState, int32_t trigger) {
  this->pin = pin;

  activeState = (trigger & 0b0000000000000000000001);
  threshold   = (trigger & 0b0000000000011111111110) >> 1;
  hysteresis  = (trigger & 0b0111111111100000000000) >> 11;
  isAnalog    = threshold != 0;

  if (isAnalog) {
  	if (threshold + hysteresis > 1023) {
      hysteresis = 0;
      VF("WRN: Sense::Sense(); Threshold + hysteresis for pin "); V(pin); VLF(" above Analog range, hysteresis set to 0.");
    }
	  if (threshold - hysteresis < 0) {
      hysteresis = 0;
      VF("WRN: Sense::Sense(); Threshold - hysteresis for pin "); V(pin); VLF(" below Analog range, hysteresis set to 0.");
    }
  }

  VF("MSG: Sense, ");
  VF("pin="); V(pin); V(", ");
  VF("active="); V(activeState ? "HIGH" : "LOW"); V(", ");
  VF("ths="); V(threshold); V(", ");
  VF("hys="); VL(hysteresis);

  pinModeEx(pin, initState);

  reset();
}

int SenseInput::isOn() {
  int value = lastValue;
  if (isAnalog) {
    int sample = analogRead(pin);
    if (sample >= threshold + hysteresis) value = HIGH;
    if (sample < threshold - hysteresis) value = LOW;
  } else {
    int sample = digitalReadEx(pin); delayMicroseconds(10); int sample1 = digitalReadEx(pin);
    if (stableSample != sample || sample1 != sample) { stableStartMs = millis(); stableSample = sample; }
    long stableMs = (long)(millis() - stableStartMs);
    if (stableMs >= hysteresis) value = stableSample;
  }
  lastValue = value;
  return value == activeState;
}

int SenseInput::changed() {
  int value = lastValue;
  if (isAnalog) {
    int sample = analogRead(pin);
    if (sample >= threshold + hysteresis) value = HIGH;
    if (sample < threshold - hysteresis) value = LOW;
  } else {
    int sample = digitalReadEx(pin); delayMicroseconds(10); int sample1 = digitalReadEx(pin);
    if (stableSample != sample || sample1 != sample) { stableStartMs = millis(); stableSample = sample; }
    long stableMs = (long)(millis() - stableStartMs);
    if (stableMs >= hysteresis) value = stableSample;
  }
  return lastValue != value;
}

void SenseInput::poll() {
  int value = lastValue;
  if (!isAnalog) {
    int sample = digitalReadEx(pin); delayMicroseconds(10); int sample1 = digitalReadEx(pin);
    if (stableSample != sample || sample1 != sample) { stableStartMs = millis(); stableSample = sample; }
    long stableMs = (long)(millis() - stableStartMs);
    if (stableMs > hysteresis) value = stableSample;
  }
  lastValue = value;
}

void SenseInput::reset() {
  if (isAnalog) { if ((int)analogRead(pin) > threshold) lastValue = HIGH; else lastValue = LOW; } else lastValue = digitalReadEx(pin);
  stableSample = lastValue;
}

// Manage sense pins

uint8_t Sense::add(int pin, int initState, int32_t trigger, bool force) {
  if ((pin == OFF || trigger == OFF) && !force) return 0;
  if (senseCount >= SENSE_MAX) { VF("WRN: Senses::add(); senseCount exceeded ignoring pin "); VL(pin); return 0; }
  if (trigger < 0 || trigger >= SENSE_MAX_TRIGGER) { VF("WRN: Senses::add(); trigger value invalid ignoring pin "); VL(pin); return 0; }
  if (initState != INPUT && initState != INPUT_PULLUP && initState != INPUT_PULLDOWN) {
    VF("WRN: Senses::add(); initState value invalid ignoring pin "); VL(pin);
    return 0;
  }
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

void Sense::poll() {
  for (int i = 0; i < senseCount; i++) senseInput[i]->poll();
}

Sense sense;
