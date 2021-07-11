// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire ds2413 device support

#include "Ds2413.h"

#ifdef DS2413_DEVICES_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;

#include "../1wire/1Wire.h"
#include <DallasGPIO.h>               // my DallasGPIO library https://github.com/hjd1964/Arduino-DS2413GPIO-Control-Library
DallasGPIO DS2413GPIO(&oneWire);

void ds2413Wrapper() { gpio.poll(); }

// scan for DS2413 devices on the 1-wire bus
bool Ds2413::init() {
  static bool initialized = false;
  if (initialized) return found;

  // clear then pre-load any user defined DS2413 addresses
  deviceCount = 0;
  for (int i = 0; i < 4; i++) {
    // check that it's not a pin (or OFF) and also not an (auto assigned) DS2413, so must be an DS2413 address
    if (device[i*2] > 255 && device[i*2] != DS2413) {
      for (int j = 0; j < 8; j++) address[i][j] = (device[i*2] >> (7 - j)*8) & 0xff;
      deviceCount++;
    } else for (int j = 0; j < 8; j++) address[i][j] = 0;
  }

  // scan the 1-wire bus and record the devices found
  oneWire.reset_search();

  // only search out DS2413's or DS1820's IF none are explicitly specified
  bool search = deviceCount == 0;
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
        if (search) {
          if (index <= 3) { if (device[index*2] != DS2413) index++; }
          if (index <= 3) { for (int j = 0; j < 8; j++) address[index][j] = addressFound[j]; deviceCount++; }
          index++;
        }
        #if DEBUG_MODE == VERBOSE
          detected = true;
          VF("DS2413:  0x"); for (int j = 0; j < 8; j++) { if (addressFound[j] < 16) { V("0"); } SERIAL_DEBUG.print(addressFound[j], HEX); }
          if (search) {
            if (index <= 4) { VF(" auto-assigned to FEATURE"); V((index - 1)*2 + 1); V("_PIN"); } else { VF(" not assigned"); }
          } else { VF(" auto-assign disabled"); }
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
    VF("MSG: DS2413, start device monitor task (rate 20ms priority 7)... ");
    if (tasks.add(20, 0, true, 7, ds2413Wrapper, "ds2413")) { VL("success"); } else { VL("FAILED!"); }
  } else found = false;

  initialized = true;
  return found;
}

// read DS2413 devices, designed for a 20ms polling interval
void Ds2413::poll() {

  static int index = 0;
  if (found) {

    int chan1 = index*2;
    int chan2 = index*2 + 1;
    if ((device[chan2] & DS_MASK) == DS2413 || (device[chan1] & DS_MASK) == DS2413) {

      int state1 = state[chan1];
      int state2 = state[chan2];
      if (lastState[chan2] != state2 || lastState[chan1] != state1) {

        // loop to set the GPIO
        // tasks.yield() during the 1-wire command sequence is ok since:
        //   1. only higher priority level tasks are allowed to run during a yield 
        //   2. all 1-wire task polling is run at the lowest priority level
        for (int i = 0; i < 20; i++) {
          if (DS2413GPIO.setStateByAddress(address[index], state2, state1, true)) break; else tasks.yield(20);
        }

        if (DS2413GPIO.success()) {
          goodUntil[index] = millis() + 5000;
        } else {
          state1 = state2 = INVALID;
          DLF("ERR: DS2413 comms failure "); V(chan1 + 1); V("/"); VL(chan2 + 1);
        }

        lastState[chan2] = state2;
        lastState[chan1] = state1;
      }
    }
  }
  index++;
  if (index > 3) index = 0;
}

// four DS2413 1-wire GPIO's are supported, this gets the last set value
// index 0 is auxiliary feature #1, etc. each DS2413 has two GPIO's for 8 total
int Ds2413::getChannel(int index) {
  if (index >= 0 && index <= 7) return state[index]; else return 0;
}

// four DS2413 1-wire GPIO's are supported, this sets each output on or off
// index 0 is auxiliary feature #1, etc. each DS2413 has two GPIO's for 8 total
void Ds2413::setChannel(int index, bool value) {
  if (index >= 0 && index <= 7) state[index] = value;
}

// four DS2413 1-wire GPIO's are supported, this gets the status of each
// index 0 is auxiliary feature #1, etc. each DS2413 has two GPIO's for 8 total
// returns true if there was a communications failure detected in the last 5 seconds
bool Ds2413::failure(int index) {
  if (index >= 0 || index <= 7) {
    if ((long)(millis() - goodUntil[index/2]) > 0) return true; else return false;
  } else return true;
}

Ds2413 gpio;

#endif
