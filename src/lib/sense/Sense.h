// -----------------------------------------------------------------------------------
// combined digital and analog threshold read, 10 bit analog read resolution only
// analog mode reads uses software schmitt trigger with threshold/hysteresis-band
// digital mode reads have basic hf EMI/RFI noise filtering
#pragma once
#include "../../Common.h"

#ifndef THLD
  #define THLD(v) ((v)<<1)  // 10 bit analog threshold, bits 1 through 10
#endif
#ifndef HYST
  #define HYST(v) ((v)<<11) // 10 bit hysteresis, bits 11 through 20
#endif
#ifndef INPUT_PULLDOWN
  #define INPUT_PULLDOWN INPUT
#endif

// default provision for up to 16 sense inputs is allowed, to change use:
// #define SENSE_MAX 30 (for example)
#ifndef SENSE_MAX
  #define SENSE_MAX 8
#endif

// largest possible trigger value == 2^21
#define SENSE_MAX_TRIGGER 2097152

class SenseInput {
  public:
    SenseInput(int pin, int initState, int32_t trigger);

    int isOn();
    int changed();
    inline void reverse(bool state) { reverseState = state; }

    void poll();

  private:
    void reset();

    int pin;
    int activeState = OFF;
    bool reverseState = false;
    bool isAnalog;
    int threshold;
    int hysteresis;
    int triggerMode;
    int lastValue = LOW;
    int lastChangedValue = LOW;
    int lastResult = LOW;
    int stableSample = 0;
    unsigned long stableStartMs = 0;
};

class Sense {
  public:
    // add sense input
    // \param pin        MCU pin to watch
    // \param initState  initialization state: INPUT, INPUT_PULLUP, or INPUT_PULLDOWN
    // \param trigger    triggered state: HIGH or LOW, optional
    //                   analog threshold/hysteresis |THLD(t) |HIST(h) are 10 bit values
    // \param force      allocates even if pin is OFF
    // \returns          handle to this sense if successful, 0 otherwise
    uint8_t add(int pin, int initState, int32_t trigger, bool force = false);

    // read the sense associated input pin and return state as configured
    // \param handle      sense handle
    int isOn(uint8_t handle);

    // check the sense associated input pin and return true if it has changed since last read
    // \param handle      sense handle
    int changed(uint8_t handle);

    // reverse the on state for this pin
    // \param handle      sense handle
    void reverse(uint8_t handle, bool state);

    // call repeatedly to check inputs for changes
    void poll();

  private:
    uint8_t senseCount = 0;
    SenseInput *senseInput[SENSE_MAX];
};

extern Sense sense;
