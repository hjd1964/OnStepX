// -----------------------------------------------------------------------------------------------------------------------------
// Handle push buttons

#include "../../Common.h"
#include "PushButton.h"

Button::Button(int8_t pin, int8_t initState, int32_t trigger) {
  this->pin = pin;

  long activeState = (trigger & 0b0000000000000000000001);
  long threshold   = (trigger & 0b0000000000011111111110) >> 1;
  long hysteresis  = (trigger & 0b0111111111100000000000) >> 11;
  bool isAnalog    = threshold != 0;

  if (isAnalog) {
    VLF("WRN: Button::Button(); Analog sense not supported.");
  }

  debounceMs = hysteresis;

  if (activeState == LOW) { UP = HIGH; DOWN = LOW; } else
  if (activeState == HIGH) { UP = LOW; DOWN = HIGH; }

  pinModeEx(pin, initState);

  //reset();
}

void Button::poll() {
  int lastState = state;
  state = digitalReadEx(pin);
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
  if (fabs(avgPulseDuration - 40.0) < 7.0) return true; else return false;
}

double Button::toneFreq() {
  if (avgPulseDuration >= 0.1 && avgPulseDuration <= 10000.0) return 0.5/(avgPulseDuration/1000.0); else return NAN;
}
