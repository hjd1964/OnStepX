// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire DS2413 device support

#include "Ds2413.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == DS2413

#include "../1wire/1Wire.h"

#include <DallasGPIO.h>               // my DallasGPIO library https://github.com/hjd1964/Arduino-DS2413GPIO-Control-Library
DallasGPIO DS2413GPIO(&oneWire);

#include "../tasks/OnTask.h"

void ds2413Wrapper() { gpio.poll(); }

// scan for a DS2413 device on the 1-wire bus
bool Ds2413::init() {
  static bool initialized = false;
  if (initialized) return found;

  // scan the 1-wire bus and record the devices found
  oneWire.reset_search();

  #if DEBUG == VERBOSE
    bool detected = false;
  #endif

  VLF("*********************************************");
  VLF("MSG: Dallas/Maxim 1-wire DS2413 device s/n's:");

  uint8_t addressFound[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  while (oneWire.search(addressFound)) {
    if (oneWire.crc8(addressFound, 7) == addressFound[7]) {
      if (addressFound[0] == 0x3A || addressFound[0] == 0xBA) {
        #if DEBUG == VERBOSE
          VF("DS2413: 0x");
          for (int i = 0; i < 8; i++) {
            if (addressFound[i] < 16) { V("0"); }
            if (DEBUG != OFF) SERIAL_DEBUG.print(addressFound[i], HEX);
          }
          if (deviceCount < DS2413_MAX_DEVICES) {
            VF(" allocated to GPIO("); V(deviceCount*2); VF(") and GPIO("); V(deviceCount*2 + 1); VLF(")");
          } else { VLF(" not assigned"); }

          detected = true;
        #endif

        if (deviceCount < DS2413_MAX_DEVICES) {
          for (int i = 0; i < 8; i++) address[deviceCount][i] = addressFound[i];
          deviceCount++;
        }

      }
    }
  }

  #if DEBUG == VERBOSE
    if (!detected) { VLF("No DS2413 devices found"); }
  #endif

  VLF("*********************************************");

  if (deviceCount > 0) {
    found = true;
    VF("MSG: GPIO, start DS2413 monitor task (rate 100ms priority 7)... ");
    if (tasks.add(100, 0, true, 7, ds2413Wrapper, "ds2413")) { VLF("success"); } else { VLF("FAILED!"); }
  } else found = false;

  lastValidPin = deviceCount*2 - 1;

  initialized = true;
  return found;
}

// no command processing
bool Ds2413::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  UNUSED(reply);
  UNUSED(command);
  UNUSED(parameter);
  UNUSED(supressFrame);
  UNUSED(numericReply);
  UNUSED(commandError);
  return false;
}

// set GPIO pin mode for INPUT or OUTPUT (both pins of any device must be in the same mode)
void Ds2413::pinMode(int pin, int mode) {
  if (found && pin >= 0 && pin <= lastValidPin) {
    if (mode == INPUT_PULLUP) mode = INPUT;
    this->mode[pin/2] = mode;
  }
}

// get GPIO pin state
int Ds2413::digitalRead(int pin) {
  if (found && (long)(millis() - goodUntil[pin/2]) < 0 && pin >= 0 && pin <= lastValidPin) {
    return state[pin];
  } else return -1;
}

// set GPIO pin state
void Ds2413::digitalWrite(int pin, int value) {
  if (found && pin >= 0 && pin <= lastValidPin) state[pin] = value;
}

// update the DS2413
void Ds2413::poll() {
  if (found) {
    // loop to get/set the GPIO
    // tasks.yield() during the 1-wire command sequence is ok since:
    //   1. only higher priority level tasks are allowed to run during a yield 
    //   2. all 1-wire task polling is run at the lowest priority level
    for (int index = 0; index < deviceCount; index++) {
      if (mode[index] == INPUT) {
        for (int i = 0; i < 22; i++) {
          if (DS2413GPIO.getStateByAddress(address[0 + index], &state[0 + index*2], &state[1 + index*2], true)) break; else tasks.yield(2);
        }
      } else
      if (mode[index] == OUTPUT) {
        if (lastState[0 + index*2] != state[0 + index*2] || lastState[1 + index*2] != state[1 + index*2]) {
          for (int i = 0; i < 22; i++) {
            if (DS2413GPIO.setStateByAddress(address[0 + index], state[0 + index*2], state[1 + index*2], true)) break; else tasks.yield(2);
          }
        }
      }
      lastState[0 + index*2] = state[0 + index*2];
      lastState[1 + index*2] = state[1 + index*2];

      if (DS2413GPIO.success()) {
        goodUntil[index] = millis() + 5000;
      } else {
        state[0 + index*2] = state[1 + index*2] = INVALID;
        DF("ERR: DS2413 0x");
        for (int i = 0; i < 8; i++) {
          if (address[0 + index][i] < 16) { V("0"); }
          if (DEBUG != OFF) SERIAL_DEBUG.print(address[0 + index][i], HEX);
        }
        DLF(" GPIO comms failure");
      }
    }
  }
}

Ds2413 gpio;

#endif
