// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire ds2413 device support

#include "ds2413.h"

#ifdef DS2413_DEVICES_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;

#include "../1wire/1Wire.h"
#include <DallasGPIO.h>               // my DallasGPIO library https://github.com/hjd1964/Arduino-DS2413GPIO-Control-Library
DallasGPIO DS2413GPIO(&oneWire);

#include "ds2413.h"

void ds2413PollWrapper() { gpio.poll(); }

// scan for DS18B20 and DS2413 devices on the 1-wire bus
bool Ds2413::init() {
  bool success = true;

  // clear then pre-load any user defined DS2413 addresses
  ds2413_device_count = 0;
  for (int i = 0; i < 4; i++) {
    if (ds2413_device[i] > 255 && ds2413_device[i] != DS1820) {
      for (int j = 0; j < 8; j++) ds2413_address[i][j] = (ds2413_device[i] >> (7 - j)*8) & 0xff;
      ds2413_device_count++;
    } else for (int j = 0; j < 8; j++) ds2413_address[i][j] = 0;
  }

  // scan the 1-wire bus and record the devices found
  oneWire.reset_search();

  // only search out DS2413's or DS1820's IF none are explicitly specified
  #if defined(DS2413_DEVICES_PRESENT) || DEBUG_MODE == VERBOSE
    bool searchDS2413 = ds2413_device_count == 0;
  #endif
  #if DEBUG_MODE == VERBOSE
    bool ds2413_detected = false;
  #endif

  VLF("*********************************************");
  VLF("MSG: Dallas/Maxim 1-wire DS2413 device s/n's:");

  uint8_t address[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  while (oneWire.search(address)) {
    if (oneWire.crc8(address, 7) == address[7]) {
      if (address[0] == 0x3A) {
        if (searchDS2413) {
          if (ds2413_index <= 3) { if (ds2413_device[ds2413_index] != DS2413) ds2413_index++; }
          if (ds2413_index <= 3) { for (int j = 0; j < 8; j++) ds2413_address[ds2413_index][j] = address[j]; ds2413_device_count++; }
          ds2413_index++;
        }
        #if DEBUG_MODE == VERBOSE
          ds2413_detected = true;
          VF("DS2413:  0x"); for (int j = 0; j < 8; j++) { if (address[j] < 16) V("0"); Serial.print(address[j], HEX); }
          if (searchDS2413) {
            if (ds2413_index <= 4) { VF(" auto-assigned to FEATURE"); V((ds2413_index - 1)*2 + 1); V("_PIN"); } else { VF(" not assigned"); }
          } else VF(" auto-assign disabled");
          VL("");
        #endif
      }
    }
  }

  #if DEBUG_MODE == VERBOSE
    if (!ds2413_detected) VLF("No DS2413 devices found");
  #endif

  VLF("*********************************************");

  #ifdef DS2413_DEVICES_PRESENT
    if (ds2413_device_count > 0) {
      ds2413_found = true;
      VF("MSG: DS2413, start device monitor task (rate 20ms priority 7)... ");
      tasks.add(20, 0, true, 7, ds2413PollWrapper, "ds2413");
    } else success = false;
  #endif

  return success;
}

// read DS2413 devices
void Ds2413::poll() {
  static int ds2413_index = 0;
  if (ds2413_found) {
    int chan1 = ds2413_index*2;
    int chan2 = ds2413_index*2 + 1;
    if ((ds2413_features[chan2] & DS_MASK) == DS2413 || (ds2413_features[chan1] & DS_MASK) == DS2413) {
      if (ds2413_last_state[chan2] != ds2413_this_state[chan2] || ds2413_last_state[chan1] != ds2413_this_state[chan1]) {

        // loop to set the GPIO
        // tasks.yield() during the 1-wire command sequence is ok since:
        //   1. only higher priority level tasks are allowed to run during a yield 
        //   2. all 1-wire task polling is run at the lowest priority level
        for (int i = 0; i < 20; i++) {
          if (DS2413GPIO.setStateByAddress(ds2413_address[ds2413_index], ds2413_this_state[chan2], ds2413_this_state[chan1], true)) {
            ds2413_last_state[chan2] = ds2413_this_state[chan2];
            ds2413_last_state[chan1] = ds2413_this_state[chan1];
            break;
          } else tasks.yield(20);
        }

        if (!DS2413GPIO.success()) {
          ds2413_failures[ds2413_index]++;
          VLF("WRN: DS2413 comms: features "); V(chan1 + 1); V("/"); VL(chan2 + 1);
        }
      }
    }
  }
  ds2413_index++;
  if (ds2413_index > 3) ds2413_index = 0;
}

// four DS2413 1-wire GPIO's are supported, this sets each output on or off
// index 1 is auxiliary feature #1, etc. each DS2413 has two GPIO's
int Ds2413::getChannel(int index) {
  if (index >= 1 && index <= 8) return ds2413_state[index - 1]; else return 0;
}

// four DS2413 1-wire GPIO's are supported, this sets each output on or off
// index 1 is auxiliary feature #1, etc. each DS2413 has two GPIO's
void Ds2413::setChannel(int index, bool state) {
  if (index >= 1 && index <= 8) ds2413_state[index - 1] = state;
}

// four DS2413 1-wire GPIO's are supported, this gets the status of each
// index 1 is auxiliary feature #1, etc. each DS2413 has two GPIO's
bool Ds2413::failureChannel(int index) {
  if (index >= 1 || index <= 8) return ds2413_failures[(index - 1)/2] > 2; else return true;
}

Ds2413 gpio;

#endif
