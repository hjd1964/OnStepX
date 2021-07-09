// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire DS1820 device support

#include "Ds1820.h"

#ifdef DS1820_DEVICES_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;

#include "../1wire/1Wire.h"
#include <DallasTemperature.h>        // my DallasTemperature library https://github.com/hjd1964/Arduino-Temperature-Control-Library
DallasTemperature DS18B20(&oneWire);

#include "..\weather\Weather.h"

void ds1820PollWrapper() { temperature.poll(); }

// scan for DS1820 devices on the 1-wire bus
bool Ds1820::init() {
  static bool initialized = false;
  if (initialized) return ds1820_found;

  // clear then pre-load any user defined DS1820 addresses
  ds1820_device_count = 0;
  for (int i = 0; i < 9; i++) {
    if (ds1820_device[i] != OFF && ds1820_device[i] != DS1820) {
      for (int j = 0; j < 8; j++) ds1820_address[i][j] = (ds1820_device[i] >> (7 - j)*8) & 0xff;
      ds1820_device_count++;
    } else for (int j = 0; j < 8; j++) ds1820_address[i][j] = 0;
  }

  // scan the 1-wire bus and record the devices found
  oneWire.reset_search();

  // only search out DS2413's or DS1820's IF none are explicitly specified
  bool searchDS1820 = ds1820_device_count == 0;
  #if DEBUG_MODE == VERBOSE
    bool ds1820_detected = false;
  #endif

  VLF("*********************************************");
  VLF("MSG: Dallas/Maxim 1-wire DS1820 device s/n's:");

  uint8_t address[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  while (oneWire.search(address)) {
    if (oneWire.crc8(address, 7) == address[7]) {
      if (address[0] == 0x10 || address[0] == 0x28) {
        if (searchDS1820) {
          if (ds1820_index <= 8) { if (ds1820_device[ds1820_index] != DS1820) ds1820_index++; }
          if (ds1820_index <= 8) { for (int j = 0; j < 8; j++) { ds1820_address[ds1820_index][j] = address[j]; ds1820_device_count++; } }
          ds1820_index++;
        }
        #if DEBUG_MODE == VERBOSE
          ds1820_detected = true;
          if (address[0] == 0x10) { VF("DS18S20: 0x"); } else { VF("DS18B20: 0x"); }
          for (int j = 0; j < 8; j++) { if (address[j] < 16) V("0"); Serial.print(address[j],HEX); }
          if (searchDS1820) {
            if (ds1820_index == 1) { VF(" auto-assigned to FOCUSER_TEMPERATURE"); } else
            if (ds1820_index <= 9) { VF(" auto-assigned to FEATURE"); V(ds1820_index - 1); V("_TEMP"); } else { VF(" not assigned"); }
          } else { VF(" auto-assign disabled"); }
          VL("");
        #endif
      }
  }
  }

  #if DEBUG_MODE == VERBOSE
    if (!ds1820_detected) VLF("No DS1820 devices found");
  #endif

  VLF("*********************************************");

  DS18B20.setWaitForConversion(false);
  if (ds1820_device_count > 0) {
    ds1820_found = true;
    VF("MSG: DS1820, start device monitor task (rate 60ms priority 7)... ");
    if (tasks.add(60, 0, true, 7, ds1820PollWrapper, "ds1820")) { VL("success"); } else { VL("FAILED!"); }
  } else ds1820_found = false;

  return ds1820_found;
}

// read DS18B20 devices
void Ds1820::poll() {
  static int ds1820_index = 0;
  static unsigned long requestTime = 0;

  if (ds1820_found) {
    if (ds1820_index == 0) { DS18B20.requestTemperatures(true); requestTime = millis(); }
    if ((long)(millis() - requestTime) < 200) return;

    // loop to read the temperature
    // tasks.yield() during the 1-wire command sequence is ok since:
    //   1. only higher priority level tasks are allowed to run during a yield 
    //   2. all 1-wire task polling is run at the lowest priority level
    float t = NAN;
    for (int i = 0; i < 20; i++) {
      t = DS18B20.getTempC(ds1820_address[ds1820_index], true);
      if (polling(t)) tasks.yield(60); else break;
    }

    featureTemperature[ds1820_index] = validated(t);
    if (isnan(averageFeatureTemperature[ds1820_index])) averageFeatureTemperature[ds1820_index] = featureTemperature[ds1820_index];
    averageFeatureTemperature[ds1820_index] = (averageFeatureTemperature[ds1820_index]*9.0F + featureTemperature[ds1820_index])/10.0F;
  }
  ds1820_index++;
  if (ds1820_index > 8) ds1820_index = 0;
}

// nine DS1820 1-wire temperature sensors are supported, this gets
// the temperature in deg. C otherwise it falls back to the weather sensor temperature
// index 0 is the focuser temperature, 1 is auxiliary feature #1, etc.
float Ds1820::getChannel(int index) {
  if (index >= 0 && index <= 7) {
    if (ds1820_device[index] == OFF) averageFeatureTemperature[index] = weather.getTemperature();
    return averageFeatureTemperature[index];
  } else return NAN;
}

bool Ds1820::polling(float f) {
  return (fabs(f - DEVICE_POLLING_C) < 0.001);
}

float Ds1820::validated(float f) {
  if (fabs(f - DEVICE_DISCONNECTED_C) < 0.001) return NAN;
  if (f < -100 || f > 70) return NAN;
  return f;
}

Ds1820 temperature;

#endif
