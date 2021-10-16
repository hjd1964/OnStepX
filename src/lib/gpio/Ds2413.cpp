// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire DS2413 device support

#include "Ds2413.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == DS2413

#include "../1wire/1Wire.h"
#include <DallasGPIO.h>               // my DallasGPIO library https://github.com/hjd1964/Arduino-DS2413GPIO-Control-Library
DallasGPIO DS2413GPIO(&oneWire);

void ds2413Wrapper() { gpio.poll(); }

// scan for a DS2413 device on the 1-wire bus
bool Ds2413::init() {
  static bool initialized = false;
  if (initialized) return found;

  // scan the 1-wire bus and record the devices found
  oneWire.reset_search();

  #if DEBUG_MODE == VERBOSE
    bool detected = false;
  #endif

  VLF("*********************************************");
  VLF("MSG: Dallas/Maxim 1-wire DS2413 device s/n's:");

  int index = 0;
  uint8_t addressFound[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  while (oneWire.search(addressFound)) {
    if (oneWire.crc8(addressFound, 7) == addressFound[7]) {
      if (addressFound[0] == 0x3A) {
        if (index <= 1) { for (int j = 0; j < 8; j++) address[j] = addressFound[j]; deviceCount++; }
        index++;
        #if DEBUG_MODE == VERBOSE
          detected = true;
          VF("DS2413:  0x"); for (int j = 0; j < 8; j++) { if (addressFound[j] < 16) { V("0"); } SERIAL_DEBUG.print(addressFound[j], HEX); }
          if (index <= 1) { VF(" auto-assigned to FEATURE"); V((index - 1)*2 + 1); V("_PIN"); } else { VF(" not assigned"); }
          VL("");
        #endif
      }
    }
  }

  #if DEBUG_MODE == VERBOSE
    if (!detected) { VLF("No DS2413 devices found"); }
  #endif

  VLF("*********************************************");

  if (deviceCount > 0) {
    found = true;
    VF("MSG: Gpio, start DS2413 monitor task (rate 20ms priority 7)... ");
    if (tasks.add(20, 0, true, 6, ds2413Wrapper, "ds2413")) { VLF("success"); } else { VLF("FAILED!"); }
  } else found = false;

  initialized = true;
  return found;
}

// set GPIO pin (0 or 1) mode for INPUT or OUTPUT (both pins must be in the same mode)
void Ds2413::pinMode(int pin, int mode) {
  if (found && pin >= 0 && pin <= 1) {
    if (mode == INPUT_PULLUP) mode = INPUT;
    this->mode = mode;
  }
}

// get GPIO pin (0 or 1) state
int Ds2413::digitalRead(int pin) {
  if (found && (long)(millis() - goodUntil) < 0 && pin >= 0 && pin <= 1) {
    return state[pin];
  } else return -1;
}

// set GPIO pin (0 or 1) state
void Ds2413::digitalWrite(int pin, int value) {
  if (found && pin >= 0 && pin <= 1) state[pin] = value;
}

// update the DS2413, designed for a 20ms polling interval
void Ds2413::poll() {
  if (found) {
    // loop to set the GPIO
    // tasks.yield() during the 1-wire command sequence is ok since:
    //   1. only higher priority level tasks are allowed to run during a yield 
    //   2. all 1-wire task polling is run at the lowest priority level
    if (mode == INPUT) {
      for (int i = 0; i < 20; i++) {
        if (DS2413GPIO.getStateByAddress(address, &state[1], &state[0], true)) break; else tasks.yield(20);
      }
    } else
    if (mode == OUTPUT) {
      if (lastState[0] != state[0] || lastState[1] != state[1]) {
        for (int i = 0; i < 20; i++) {
          if (DS2413GPIO.setStateByAddress(address, state[1], state[0], true)) break; else tasks.yield(20);
        }
      }

      if (DS2413GPIO.success()) {
        goodUntil = millis() + 5000;
      } else {
        state[0] = state[1] = INVALID;
        DLF("ERR: DS2413 GPIO comms failure");
      }

      lastState[0] = state[0];
      lastState[1] = state[1];
    }
  }
}

Ds2413 gpio;

#endif
