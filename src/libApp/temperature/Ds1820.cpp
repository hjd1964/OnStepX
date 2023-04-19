// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire DS1820 device support

#include "Ds1820.h"

#ifdef DS1820_DEVICES_PRESENT

#include "../../lib/tasks/OnTask.h"

#include "../../lib/1wire/1Wire.h"
#include <DallasTemperature.h>        // my Dallas Temperature library https://github.com/hjd1964/Arduino-DS1820-Temperature-Library
DallasTemperature DS18X20(&oneWire);

#include "../weather/Weather.h"

void ds1820Wrapper() { temperature.poll(); }

// scan for DS18B20 devices on the 1-wire bus and prepare for operation
bool Ds1820::init() {
  static bool initialized = false;
  if (initialized) return found;

  // clear then pre-load any user defined DS1820 addresses
  deviceCount = 0;
  for (int i = 0; i < 9; i++) {
    // check that it's not OFF and also not an (auto assigned) DS1820, so must be an DS1820 address
    if (device[i] != (uint64_t)OFF && device[i] != DS1820) {
      for (int j = 0; j < 8; j++) address[i][j] = (device[i] >> (7 - j)*8) & 0xff;
      deviceCount++;
    } else for (int j = 0; j < 8; j++) address[i][j] = 0;
  }

  // scan the 1-wire bus and record the devices found
  oneWire.reset_search();

  // only search out DS1820's IF none are explicitly specified
  bool search = deviceCount == 0;
  #if DEBUG == VERBOSE
    bool detected = false;
  #endif

  VLF("*********************************************");
  VLF("MSG: Dallas/Maxim 1-wire DS1820 device s/n's:");

  int index = 0;
  uint8_t addressfound[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  while (oneWire.search(addressfound)) {
    if (oneWire.crc8(addressfound, 7) == addressfound[7]) {
      if (addressfound[0] == 0x10 || addressfound[0] == 0x28) {
        if (search) {
          if (index <= 8) { while (device[index] != DS1820 && index <= 8) index++; }
          if (index <= 8) { for (int j = 0; j < 8; j++) { address[index][j] = addressfound[j]; deviceCount++; } }
          index++;
        }

        #if DEBUG == VERBOSE
          if (addressfound[0] == 0x10) { VF("DS18S20: 0x"); } else { VF("DS18B20: 0x"); }
          for (int i = 0; i < 8; i++) {
            if (addressfound[i] < 16) { V("0"); }
            if (DEBUG != OFF) SERIAL_DEBUG.print(addressfound[i], HEX);
          }
          if (search) {
            if (index == 1) { VLF(" auto-assigned to FOCUSER_TEMPERATURE"); } else
            if (index <= 9) { VF(" auto-assigned to FEATURE"); V(index - 1); VLF("_TEMP"); } else { VLF(" not assigned"); }
          } else { VLF(" auto-assign disabled"); }
          detected = true;
        #endif
      }
    }
  }

  #if DEBUG == VERBOSE
    if (!detected) { VLF("No DS1820 devices found"); }
  #endif

  VLF("*********************************************");

  DS18X20.setWaitForConversion(false);
  if (deviceCount > 0) {
    found = true;
    VF("MSG: Temperature, start DS1820 monitor task (rate 250ms priority 7)... ");
    if (tasks.add(250, 0, true, 7, ds1820Wrapper, "ds1820")) { VLF("success"); } else { VLF("FAILED!"); }
  } else found = false;

  initialized = true;
  return found;
}

// read devices
void Ds1820::poll() {
  static int index = -1;
  static unsigned long requestTime = 0;

  if (found) {
    if (index < 0) { requestTime = millis(); if (DS18X20.requestTemperatures(true)) index++; }
    if ((long)(millis() - requestTime) < 200) return;

    if (device[index] != (uint64_t)OFF) {
      // loop to read the temperature
      // tasks.yield() during the 1-wire command sequence is ok since:
      //   1. only higher priority level tasks are allowed to run during a yield 
      //   2. all 1-wire task polling is run at the lowest priority level
      float rawTemperature = NAN;
      for (int i = 0; i < 22; i++) {
        rawTemperature = DS18X20.getTempC(address[index], true);
        if (polling(rawTemperature)) tasks.yield(2); else break;
      }

      float temperature = validated(rawTemperature);
      if (!isnan(temperature)) {
        if (isnan(averageTemperature[index])) averageTemperature[index] = temperature;
        averageTemperature[index] = (averageTemperature[index]*9.0F + temperature)/10.0F;
        goodUntil[index] = millis() + 30000;
      } else {
        // we must get a reading at least once every 30 seconds otherwise flag the failure with a NAN
        if ((long)(millis() - goodUntil[index]) > 0) averageTemperature[index] = NAN;
      }
    }
  }
  index++;
  if (index > 8) index = -1;
}

// nine temperature sensors are supported, this gets the averaged temperature
// in deg. C otherwise it falls back to the weather sensor temperature
// index 0 is the ambient temperature, 1 through 8 are point temperatures #1, #2, etc.
// returns NAN if no temperature source is available or if a communications failure
// results in no valid readings for > 30 seconds
float Ds1820::getChannel(int index) {
  if (found && index >= 0 && index <= 7) {
    if (device[index] == (uint64_t)OFF) averageTemperature[index] = weather.getTemperature();
    return averageTemperature[index];
  } else return NAN;
}

// checks for polling status code from DS1820 library
bool Ds1820::polling(float f) {
  return (fabs(f - DEVICE_POLLING_C) < 0.001F);
}

// checks for validated status code from DS1820 library
float Ds1820::validated(float f) {
  if (fabs(f - DEVICE_DISCONNECTED_C) < 0.001F) return NAN;
  if (f < -100 || f > 70) return NAN;
  return f;
}

Ds1820 temperature;

#endif
