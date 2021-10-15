// -----------------------------------------------------------------------------------
// Auxiliary Features

#include "Features.h"

#ifdef FEATURES_PRESENT

#include "../../lib/tasks/OnTask.h"

#include "../../lib/weather/Weather.h"
#include "../../telescope/Telescope.h"

void featuresPollWrapper() { features.poll(); }

void Features::init() {
  for (int i = 0; i < 8; i++) {
    if (device[i].pin == AUX) device[i].pin = auxPins[i];

    if (device[i].value == ON) device[i].value = 1; else
      if (device[i].value < 0 || device[i].value > 255) device[i].value = 0;

    if (device[i].purpose == SWITCH) {
      pinModeEx(device[i].pin, OUTPUT);
      digitalWriteEx(device[i].pin, device[i].value == device[i].active);
    } else

    if (device[i].purpose == ANALOG_OUTPUT) {
      pinModeEx(device[i].pin, OUTPUT);
      analogWrite(device[i].pin, analog8BitToAnalogRange(device[i].value));
    } else

    if (device[i].purpose == DEW_HEATER) {
      device[i].dewHeater = new DewHeater;
      device[i].dewHeater->init(NV_FEATURE_SETTINGS_BASE + i*3);
      pinModeEx(device[i].pin, OUTPUT);
      device[i].dewHeater->enable(device[i].value);
    } else

    if (device[i].purpose == INTERVALOMETER) {
      device[i].intervalometer = new Intervalometer;
      device[i].intervalometer->init(NV_FEATURE_SETTINGS_BASE + i*3);
      pinModeEx(device[i].pin, OUTPUT);
      digitalWriteEx(device[i].pin, device[i].value == device[i].active?HIGH:LOW);
    }
  }

  VF("MSG: Auxiliary, start feature monitor task (rate 20ms priority 6)... ");
  if (tasks.add(20, 0, true, 6, featuresPollWrapper, "AuxPoll")) { VLF("success"); } else { VLF("FAILED!"); }

  temperature.init();
}

void Features::poll() {
  for (int i = 0; i < 8; i++) {
    if (device[i].purpose == DEW_HEATER) {
      device[i].dewHeater->poll(temperature.getChannel(i + 1) - weather.getDewPoint());
      digitalWriteEx(device[i].pin, device[i].dewHeater->isOn() == device[i].active);
//    if (gpio.failure(i)) device[i].dewHeater->enable(false);
    } else

    if (device[i].purpose == INTERVALOMETER) {
      device[i].intervalometer->poll();
      digitalWriteEx(device[i].pin, device[i].intervalometer->isOn() == device[i].active);
//    if (gpio.failure(i)) device[i].intervalometer->enable(false);
    }
  }
}

Features features;

#endif
