// -----------------------------------------------------------------------------------
// combined digital and analog threshold read, 10 bit analog read resolution only
// analog mode reads uses software schmitt trigger with threshold/hysteresis-band
// digital mode reads have basic hf EMI/RFI noise filtering
#pragma once

#include <Arduino.h>

#define OFF -1
#define ON  -2

// default provision for up to 16 sense inputs is allowed, to change use:
// #define SENSE_MAX 30 (for example)
#ifndef SENSE_MAX
  #define SENSE_MAX 16
#endif

class Sense {
  public:
    Sense(int8_t pin, int8_t inputMode, bool isAnalog, int threshold, int hysteresis);
    int read();
    void poll();

  private:
    void reset();

    int8_t pin;
    int8_t inputMode = OFF;
    bool isAnalog;
    int threshold;
    int hysteresis;
    bool invert;
    int8_t triggerMode;
    int lastValue = LOW;
    int lastResult = LOW;
    int stableSample = 0;
    unsigned long stableStartMs = 0;
};

class Senses {
  public:
    // add process sense input
    // \param pin         MCU pin#
    // \param inputMode   pin mode (OFF, ON, HIGH, LOW) for OFF, INPUT_PULLUP, INPUT_PULLDOWN
    // \param isAnalog    specifices if pin is analog (true or false) where false is digital
    // \param threshold   value (0 to 1023) where result switches from LOW to HIGH
    // \param hysteresis  range +/- (0 to 1023) where result is ignored (uses last valid reading)
    //                    for digital input stable period in ms
    // \param callback    function to call on the triggerMode event
    // \param triggerMode pin state change event RISING, FALLING, CHANGE
    // \returns           handle to this sense if successful, 0 otherwise
    uint8_t add(int8_t pin, int8_t inputMode, bool isAnalog, int threshold, int hysteresis);

    // call repeatedly to check inputs for changes
    void poll();

    // read the sense's associated input and process state as configured
    // \param handle      sense handle to read current input state from
    int read(uint8_t handle);

  private:
    uint8_t senseCount = 0;
    Sense *sense[SENSE_MAX];
};

extern Senses senses;
