// -----------------------------------------------------------------------------------------------------------------------------
// Handle push buttons

#pragma once

class Button {
  public:
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
    unsigned long debounceMs = 0;
    unsigned long stableStartMs = 0;
    unsigned long stableMs = 0;
    bool pressed = false;
    bool doublePressed = false;
    double avgPulseDuration = 2000.0;
    int UP = HIGH;
    int DOWN = LOW;
};
