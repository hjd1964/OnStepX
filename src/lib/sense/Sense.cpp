// -----------------------------------------------------------------------------------
// combined digital and analog threshold read, 10 bit analog read resolution only
// analog mode reads uses software schmitt trigger with threshold/hysteresis-band
// digital mode reads have basic hf EMI/RFI noise filtering

#include "Sense.h"
#include "../../Common.h"

Sense::Sense(int pin, int initState, int32_t trigger) {
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

  pinModeEx(pin, initState);

  reset();
}

int Sense::read() {
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

void Sense::poll() {
  int value = lastValue;
  if (!isAnalog) {
    int sample = digitalReadEx(pin); delayMicroseconds(10); int sample1 = digitalReadEx(pin);
    if (stableSample != sample || sample1 != sample) { stableStartMs = millis(); stableSample = sample; }
    long stableMs = (long)(millis() - stableStartMs);
    if (stableMs > hysteresis) value = stableSample;
  }
  lastValue = value;
}

void Sense::reset() {
  if (isAnalog) { if (analogRead(pin) > threshold) lastValue = HIGH; else lastValue = LOW; } else lastValue = digitalReadEx(pin);
  stableSample = lastValue;
}

// Manage sense pins

uint8_t Senses::add(int pin, int initState, int32_t trigger) {
  if (pin == OFF || trigger == OFF) return 0;
  if (senseCount >= SENSE_MAX) { VF("WRN: Senses::add(); senseCount exceeded ignoring pin "); VL(pin); return 0; }
  if (trigger < 0 || trigger >= SENSE_MAX_TRIGGER) { VF("WRN: Senses::add(); trigger value invalid ignoring pin "); VL(pin); return 0; }
  if (initState != INPUT && initState != INPUT_PULLUP && initState != INPUT_PULLDOWN) {
    VF("WRN: Senses::add(); initState value invalid ignoring pin "); VL(pin);
    return 0;
  }
  sense[senseCount] = new Sense(pin, initState, trigger);
  senseCount++;
  return senseCount;
}

int Senses::read(uint8_t handle) {
  if (handle == 0) return false;
  return sense[handle - 1]->read();
}

void Senses::poll() {
  for (int i = 0; i < senseCount; i++) sense[i]->poll();
}

Senses senses;
