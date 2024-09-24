// -----------------------------------------------------------------------------------------------------------------------------
// Handle push buttons

#include "../../Common.h"
#include "PushButton.h"

#ifndef ANALOG_READ_RANGE
  #define ANALOG_READ_RANGE 1023
#endif

Button::Button(int pin, int initState, int32_t trigger) {
  this->pin = pin;

  long activeState = (trigger & 0b0000000000000000000001);
  threshold        = (trigger & 0b0000000000011111111110) >> 1;
  hysteresis       = (trigger & 0b0111111111100000000000) >> 11;

  isAnalog = threshold != 0;
  if (isAnalog) debounceMs = 30; else debounceMs = hysteresis;

  if (activeState == LOW) { UP = HIGH; DOWN = LOW; } else
  if (activeState == HIGH) { UP = LOW; DOWN = HIGH; }

  if (isAnalog) {
    if (threshold < 0) {
      threshold = 0;
      VF("WRN: Button::Button(), Threshold for pin "); V(pin); VLF(" is below Analog range setting to "); VL(threshold);
    } else
    if (threshold > ANALOG_READ_RANGE) {
      threshold = ANALOG_READ_RANGE;
      VF("WRN: Button::Button(), Threshold for pin "); V(pin); VF(" is above Analog range setting to "); VL(threshold);
    }
	  if (threshold - hysteresis < 0) {
      hysteresis = threshold;
      VF("WRN: Button::Button(), Threshold - Hysteresis for pin "); V(pin); VF(" is below Analog range setting Hysteresis to "); VL(hysteresis);
    } else
  	if (threshold + hysteresis > ANALOG_READ_RANGE) {
      hysteresis = ANALOG_READ_RANGE - threshold;
      VF("WRN: Button::Button(), Threshold + Hysteresis for pin "); V(pin); VF(" is above Analog range setting Hysteresis to "); VL(hysteresis);
    }
  }

  pinModeEx(pin, initState);
}

void Button::poll() {
  int lastState = state;
  if (isAnalog) {
    #ifdef ESP32
      int analogValue = round((analogReadMilliVolts(pin)/3300.0F)*(float)ANALOG_READ_RANGE);
    #else
      int analogValue = analogRead(pin);
    #endif

    if (DOWN == HIGH) {
      if (analogValue >= threshold + hysteresis) state = HIGH; else state = LOW;
    }

    if (DOWN == LOW) {
      if (analogValue <= threshold - hysteresis) state = LOW; else state = HIGH;
    }

  } else state = digitalReadEx(pin);
  if (lastState != state) {
    avgPulseDuration = ((avgPulseDuration*49.0) + (double)(millis() - stableStartMs))/50.0;
    stableStartMs = millis();
  }
  stableMs = (long)(millis() - stableStartMs);
  if (stableMs > 3000UL) {
    avgPulseDuration = ((avgPulseDuration*4.0) + 2000.0)/5.0;
  }
  if (stableMs > debounceMs) {
    if (lastStableState == UP && state == DOWN) { 
      static unsigned long lastPressTime = 0;
      if (pressed && millis() - lastPressTime < 500) doublePressed = true;
      pressed = true;
      lastPressTime = millis();
    }
    lastStableState = state;
  }
}

bool Button::isDown() {
  if (stableMs > debounceMs && state == DOWN) return true; else return false;
}

bool Button::wasPressed(bool peek) {
  if (pressed) {
    if (!peek) pressed = false;
    return true;
  } else return false;
}

bool Button::wasDoublePressed(bool peek) {
  if (doublePressed) {
    if (!peek) {
      doublePressed = false;
      pressed = false;
    } return true;
  } else return false;
}

bool Button::wasClicked(bool peek) {
  if (isUp()) return wasPressed(peek); else return false;
}

void Button::clearPress() {
  pressed = false;
}

bool Button::isUp() {
  if (stableMs > debounceMs && state == UP) return true; else return false;
}

long Button::timeDown() {
  if (stableMs > debounceMs && state == DOWN) return stableMs; else return 0;
}

long Button::timeUp() {
  if (stableMs > debounceMs && state == UP) return stableMs; else return 0;
}

bool Button::hasTone() {
  if (fabs(avgPulseDuration - 40.0) < TONE_FREQ_THRESHOLD) return true; else return false;
}

double Button::toneFreq() {
  if (avgPulseDuration >= 0.1 && avgPulseDuration <= 10000.0) return 0.5/(avgPulseDuration/1000.0); else return NAN;
}
