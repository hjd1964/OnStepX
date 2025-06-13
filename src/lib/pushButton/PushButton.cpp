// -----------------------------------------------------------------------------------------------------------------------------
// Handle push buttons

#include "PushButton.h"

#include "../gpioEx/GpioEx.h"

#ifndef ANALOG_READ_RANGE
  #define ANALOG_READ_RANGE 1023
#endif

Button::Button(int pin, int initState, int32_t trigger) {
  this->pin = pin;

  long activeState             = (trigger & 0b00000000000000000000000000000001);
  int16_t analogThresholdValue = (trigger & 0b00000000000000000000011111111110) >> 1;
  int16_t analogDownValue      = (trigger & 0b01111111111000000000000000000000) >> 21;
  hysteresis                   = (trigger & 0b00000000000111111111100000000000) >> 11;

  isAnalogThreshold = analogThresholdValue != 0;
  isAnalogValue = analogDownValue != 0;

  if (isAnalogThreshold) { analogCompareValue = analogThresholdValue; debounceMs = 30; } else
  if (isAnalogValue) { analogCompareValue = analogDownValue; debounceMs = 30; } else debounceMs = hysteresis;

  if (activeState == LOW) { UP = HIGH; DOWN = LOW; } else
  if (activeState == HIGH) { UP = LOW; DOWN = HIGH; }

  if (isAnalogThreshold || isAnalogValue) {
    if (analogCompareValue > ANALOG_READ_RANGE) {
      analogCompareValue = ANALOG_READ_RANGE;
      DF("WRN: Button::Button(), AnalogCompareValue for pin "); D(pin); DF(" is above analog range setting to "); DL(analogCompareValue);
    }
	  if (analogCompareValue - hysteresis < 0) {
      hysteresis = analogCompareValue;
      DF("WRN: Button::Button(), AnalogCompareValue - hysteresis for pin "); D(pin); DF(" is below analog range setting Hysteresis to "); DL(hysteresis);
    } else
  	if (analogCompareValue + hysteresis > ANALOG_READ_RANGE) {
      hysteresis = ANALOG_READ_RANGE - analogCompareValue;
      DF("WRN: Button::Button(), AnalogCompareValue + Hysteresis for pin "); D(pin); DF(" is above analog range setting Hysteresis to "); DL(hysteresis);
    }
  }

  pinModeEx(pin, initState);
}

void Button::poll() {
  int lastState = state;
  if (isAnalogThreshold || isAnalogValue) {
    #ifdef ESP32
      int analogValue = round((analogReadMilliVolts(pin)/3300.0F)*(float)ANALOG_READ_RANGE);
    #else
      int analogValue = analogRead(pin);
    #endif

    if (isAnalogThreshold) {
      if (DOWN == HIGH) { if (analogValue >= analogCompareValue + hysteresis) state = HIGH; else state = LOW; }
      if (DOWN == LOW) { if (analogValue <= analogCompareValue - hysteresis) state = LOW; else state = HIGH; }
    } else

    if (isAnalogValue) {
      if (analogValue >= analogCompareValue - hysteresis && analogValue <= analogCompareValue + hysteresis) state = DOWN; else state = UP;
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
