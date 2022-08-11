// -----------------------------------------------------------------------------------------------------------------------------
// Handle push buttons
#pragma once

#include <Arduino.h>

// threshold in milliseconds (for tone detection, 40ms/7 = +/-17.5%)
#define TONE_FREQ_THRESHOLD 7.0

class Button {
  public:
    // create object
    // \param pin        MCU pin
    // \param initState  initialization state: INPUT, INPUT_PULLUP, or INPUT_PULLDOWN
    // \param trigger    triggered state: HIGH or LOW, optional
    //                   analog threshold/hysteresis |THLD(t) |HIST(h) are 10 bit values
    Button(int pin, int initState, int32_t trigger);

    // must be repeatedly called to check status of button
    void poll();

    // is the button down
    bool isDown();

    // was the button down since last checked
    bool wasPressed(bool peek = false);

    // was the button down since last checked
    bool wasDoublePressed(bool peek = false);

    // was the button down since last checked
    bool wasClicked(bool peek = false);

    // clear pressed state
    void clearPress();

    // is the button up
    bool isUp();

    // number of ms down
    long timeDown();

    // number of ms up
    long timeUp();

    // check to see if this button has the SHC tone (12.5 Hz nominal)
    bool hasTone();

    // in Hz (0.05 to 5000Hz range) returns NAN otherwise
    double toneFreq();

  private:
    int pin;
    int state = HIGH;
    int lastStableState = HIGH;
    int threshold;
    int hysteresis;
    unsigned long debounceMs = 0;
    unsigned long stableStartMs = 0;
    unsigned long stableMs = 0;
    bool isAnalog = false;
    bool pressed = false;
    bool doublePressed = false;
    double avgPulseDuration = 2000.0;
    int UP = HIGH;
    int DOWN = LOW;
};
