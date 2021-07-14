// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire DS1820 device support

#include "Ds1820.h"

#ifdef DS1820_DEVICES_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;

#include "../1wire/1Wire.h"
#include <DallasTemperature.h>        // my DallasTemperature library https://github.com/hjd1964/Arduino-Temperature-Control-Library
DallasTemperature DS18B20(&oneWire);

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

  // only search out DS2413's or DS1820's IF none are explicitly specified
  bool search = deviceCount == 0;
  #if DEBUG_MODE == VERBOSE
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
          if (index <= 8) { if (device[index] != DS1820) index++; }
          if (index <= 8) { for (int j = 0; j < 8; j++) { address[index][j] = addressfound[j]; deviceCount++; } }
          index++;
        }
        #if DEBUG_MODE == VERBOSE
          detected = true;
          if (addressfound[0] == 0x10) { VF("DS18S20: 0x"); } else { VF("DS18B20: 0x"); }
          for (int j = 0; j < 8; j++) { if (addressfound[j] < 16) { V("0"); } SERIAL_DEBUG.print(addressfound[j], HEX); }
          if (search) {
            if (index == 1) { VF(" auto-assigned to FOCUSER_TEMPERATURE"); } else
            if (index <= 9) { VF(" auto-assigned to FEATURE"); V(index - 1); V("_TEMP"); } else { VF(" not assigned"); }
          } else { VF(" auto-assign disabled"); }
          VL("");
        #endif
      }
    }
  }

  #if DEBUG_MODE == VERBOSE
    if (!detected) { VLF("No DS1820 devices found"); }
  #endif

  VLF("*********************************************");

  DS18B20.setWaitForConversion(false);
  if (deviceCount > 0) {
    found = true;
    VF("MSG: Temperature, start DS1820 monitor task (rate 100ms priority 7)... ");
    if (tasks.add(100, 0, true, 7, ds1820Wrapper, "ds1820")) { VL("success"); } else { VL("FAILED!"); }
  } else found = false;

  return found;
}

// read devices, designed for a 0.1s polling interval
void Ds1820::poll() {
  static int index = 0;
  static unsigned long requestTime = 0;

  if (found) {
    if (index == 0) { DS18B20.requestTemperatures(true); requestTime = millis(); }
    if ((long)(millis() - requestTime) < 200) return;

    // loop to read the temperature
    // tasks.yield() during the 1-wire command sequence is ok since:
    //   1. only higher priority level tasks are allowed to run during a yield 
    //   2. all 1-wire task polling is run at the lowest priority level
    float rawTemperature = NAN;
    for (int i = 0; i < 20; i++) {
      rawTemperature = DS18B20.getTempC(address[index], true);
      if (polling(rawTemperature)) tasks.yield(100); else break;
    }

    float temperature = validated(rawTemperature);
    if (!isnan(temperature)) {
      if (isnan(averageTemperature[index])) averageTemperature[index] = temperature;
      averageTemperature[index] = (averageTemperature[index]*9.0F + temperature)/10.0F;
      goodUntil[index] = millis() + 30000;
    } else {
      // we must get a reading atleast once every 30 seconds otherwise flag the failure with a NAN
      if ((long)(millis() - goodUntil[index]) > 0) averageTemperature[index] = NAN;
    }
  }
  index++;
  if (index > 8) index = 0;
}

// nine temperature sensors are supported, this gets the averaged temperature
// in deg. C otherwise it falls back to the weather sensor temperature
// index 0 is the focuser temperature, 1 through 8 are auxiliary features #1, #2, etc.
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
  return (fabs(f - DEVICE_POLLING_C) < 0.001);
}

// checks for validated status code from DS1820 library
float Ds1820::validated(float f) {
  if (fabs(f - DEVICE_DISCONNECTED_C) < 0.001) return NAN;
  if (f < -100 || f > 70) return NAN;
  return f;
}

Ds1820 temperature;

#endif
