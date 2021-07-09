// -----------------------------------------------------------------------------------
// Auxiliary Features

#include "Features.h"

#ifdef FEATURES_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;

#include "../../lib/weather/Weather.h"
#include "../../telescope/Telescope.h"

void featuresPollWrapper() { telescope.features.poll(); }

void Features::init(bool validKey) {
  for (int i = 0; i < 8; i++) {
    if (device[i].value == ON) device[i].value = 1; else
      if (device[i].value < 0 || device[i].value > 255) device[i].value = 0;

    if (device[i].purpose == SWITCH || device[i].purpose == SWITCH_UNPARKED) {
      if (device[i].pin >= 0 && device[i].pin <= 255) {
        pinMode(device[i].pin, OUTPUT);
        digitalWrite(device[i].pin, device[i].value == device[i].active);
      } else gpio.setChannel(i, device[i].value == device[i].active);
    } else

    if (device[i].purpose == ANALOG_OUTPUT) {
      if (device[i].pin >= 0 && device[i].pin <= 255) {
        pinMode(device[i].pin, OUTPUT);
        analogWrite(device[i].pin, device[i].value);
      }
    } else

    if (device[i].purpose == DEW_HEATER) {
      device[i].dewHeater = new DewHeater;
      device[i].dewHeater->init(NV_FEATURE_SETTINGS_BASE + i*3, validKey);
      if (device[i].pin >= 0 && device[i].pin <= 255) pinMode(device[i].pin, OUTPUT);
      device[i].dewHeater->enable(device[i].value);
    } else

    if (device[i].purpose == INTERVALOMETER) {
      device[i].intervalometer = new Intervalometer;
      device[i].intervalometer->init(NV_FEATURE_SETTINGS_BASE + i*3, validKey);
      if (device[i].pin >= 0 && device[i].pin <= 255) {
        pinMode(device[i].pin, OUTPUT);
        digitalWrite(device[i].pin, device[i].value == device[i].active?HIGH:LOW);
      }
    }
  }

  VF("MSG: Auxiliary, start feature monitor task (rate 20ms priority 7)... ");
  if (tasks.add(20, 0, true, 7, featuresPollWrapper, "auxPoll")) { VL("success"); } else { VL("FAILED!"); }

  temperature.init();
  gpio.init();
}

void Features::poll() {
  for (int i = 0; i < 8; i++) {
    if (device[i].purpose == DEW_HEATER) {
      device[i].dewHeater->poll(temperature.getChannel(i + 1) - weather.getDewPoint());
      if (isDS2413(device[i].pin)) {
        gpio.setChannel(i, device[i].dewHeater->isOn() == device[i].active);
        if (gpio.failure(i)) device[i].dewHeater->enable(false);
      } else {
        if (device[i].pin >= 0 && device[i].pin <= 255) {
          digitalWrite(device[i].pin, device[i].dewHeater->isOn() == device[i].active);
        }
      }
    } else

    if (device[i].purpose == INTERVALOMETER) {
      device[i].intervalometer->poll();
      if (isDS2413(device[i].pin)) {
        gpio.setChannel(i, device[i].intervalometer->isOn() == device[i].active);
        if (gpio.failure(i)) device[i].intervalometer->enable(false);
      } else {
        if (device[i].pin >= 0 && device[i].pin <= 255) {
          digitalWrite(device[i].pin, device[i].intervalometer->isOn() == device[i].active);
        }
      }
    }
  }
}

// work-around for Mega2560 64 bit conditional comparison bug ((device[i].pin & DS_MASK) == DS2413)
bool Features::isDS2413(int64_t v) {
  uint32_t l1 = (v & DS_MASK) & 0xffff;
  uint32_t h1 = (v & DS_MASK) >> 32;
  uint32_t l2 = DS2413 & 0xffff;
  uint32_t h2 = DS2413 >> 32;
  return (l1 == l2 && h1 == h2);
}

#endif
