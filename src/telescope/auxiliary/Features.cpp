// -----------------------------------------------------------------------------------
// Auxiliary Features

#include "Features.h"

#ifdef FEATURES_PRESENT

#include "../../lib/tasks/OnTask.h"
#include "../../lib/nv/Nv.h"

#include "../../libApp/weather/Weather.h"
#include "../../telescope/Telescope.h"

void featuresPollWrapper() { features.poll(); }

void Features::init() {
  for (int i = 0; i < 8; i++) {
    if (device[i].pin == AUX) device[i].pin = auxPins[i];

    if (device[i].value == ON) device[i].value = 1; else
      if (device[i].value < 0 || device[i].value > 255) device[i].value = 0;

    if (device[i].memory == ON) {
      if (device[i].purpose == SWITCH ||
          device[i].purpose == ANALOG_OUTPUT ||
          device[i].purpose == DEW_HEATER) {
        device[i].value = nv.readI(NV_FEATURE_SETTINGS_BASE + i*5 + 3);
      }
    }

    if (device[i].purpose == SWITCH || device[i].purpose == MOMENTARY_SWITCH || device[i].purpose == HIDDEN_SWITCH) {
      pinModeEx(device[i].pin, OUTPUT);
      digitalWriteEx(device[i].pin, device[i].value == device[i].active);
      if (device[i].purpose == MOMENTARY_SWITCH && device[i].value) momentarySwitchTime[i] = 50;
      if (device[i].purpose == HIDDEN_SWITCH) device[i].purpose = OFF;
    } else

    #ifdef COVER_SWITCH_SERVO_PRESENT
      if (device[i].purpose == COVER_SWITCH) {
        cover[i].servo = new Servo;
        #ifdef ESP32
          cover[i].servo->setPeriodHertz(COVER_SWITCH_SERVO_PERIOD_HZ);
        #endif
        cover[i].servo->attach(device[i].pin, COVER_SWITCH_SERVO_MIN, COVER_SWITCH_SERVO_MAX);
        cover[i].servo->write(COVER_SWITCH_SERVO_CLOSED_DEG);
        cover[i].position = COVER_SWITCH_SERVO_CLOSED_DEG;
      } else
    #endif

    if (device[i].purpose == ANALOG_OUTPUT) {
      pinModeEx(device[i].pin, OUTPUT);
      analogWriteEx(device[i].pin, analog8BitToAnalogRange(device[i].value));
    } else

    if (device[i].purpose == DEW_HEATER) {
      device[i].dewHeater = new DewHeater;
      device[i].dewHeater->init(i);
      pinModeEx(device[i].pin, OUTPUT);
      device[i].dewHeater->enable(device[i].value);
    } else

    if (device[i].purpose == INTERVALOMETER) {
      device[i].intervalometer = new Intervalometer;
      device[i].intervalometer->init(i);
      pinModeEx(device[i].pin, OUTPUT);
      digitalWriteEx(device[i].pin, device[i].value == device[i].active);
    }
  }

  VF("MSG: Auxiliary, start feature monitor task (rate 20ms priority 6)... ");
  if (tasks.add(20, 0, true, 6, featuresPollWrapper, "AuxPoll")) { VLF("success"); } else { VLF("FAILED!"); }
}

void Features::poll() {
  for (int i = 0; i < 8; i++) {
    if (device[i].memory == ON) {
      if (device[i].purpose == SWITCH ||
          device[i].purpose == ANALOG_OUTPUT ||
          device[i].purpose == DEW_HEATER) {
          nv.write(NV_FEATURE_SETTINGS_BASE + i*5 + 3, (int16_t)device[i].value);
      }
    }

    if (device[i].purpose == MOMENTARY_SWITCH) {
      if (momentarySwitchTime[i] > 0) {
        momentarySwitchTime[i]--;
        if (momentarySwitchTime[i] == 0) {
          device[i].value = 0;
          digitalWriteEx(device[i].pin, device[i].value == device[i].active);
        }
      }
    } else

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

  #ifdef COVER_SWITCH_SERVO_PRESENT
    // default moves at 25 degrees/second
    static int toggle = 0;
    if (toggle++ % (100/COVER_SWITCH_SERVO_SPEED_PERCENT) == 0) {
      for (int i = 0; i < 8; i++) {
        if (device[i].purpose == COVER_SWITCH) {
          VL(cover[i].position);
          if (cover[i].position > cover[i].target) cover[i].position--; else
          if (cover[i].position < cover[i].target) cover[i].position++;
          cover[i].servo->write(cover[i].position);
        }
      }
    }
  #endif

}

Features features;

#endif
