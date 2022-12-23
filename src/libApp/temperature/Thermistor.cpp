// -----------------------------------------------------------------------------------
// Thermistor device support

#include "Thermistor.h"

#ifdef THERMISTOR_DEVICES_PRESENT

#include "../../lib/tasks/OnTask.h"

#include "../weather/Weather.h"

#ifndef ANALOG_READ_RANGE
  #define ANALOG_READ_RANGE 1023
#endif

void thermistorWrapper() { temperature.poll(); }

// prepare for operation
bool Thermistor::init() {
  static bool initialized = false;
  if (initialized) return found;

  deviceCount = 1;

  if (deviceCount > 0) {
    found = true;
    VF("MSG: Temperature, start Thermistor monitor task (rate 500ms priority 6)... ");
    if (tasks.add(500, 0, true, 6, thermistorWrapper, "therm")) { VLF("success"); } else { VLF("FAILED!"); }
  } else found = false;

  found = true;
  initialized = true;

  return found;
}

// read devices, designed for a 0.5s polling interval
void Thermistor::poll() {
  static int index = 0;

  if (found) {
    int thermistorType = -1;
    if (device[index] == THERMISTOR1) thermistorType = 0; else
    if (device[index] == THERMISTOR2) thermistorType = 1;

    if (thermistorType >= 0 && devicePin[index] != OFF) {
      // get the total resistance
      int r = analogRead(devicePin[index]);

      // calculate the device resistance
      float resistance = (float)(ANALOG_READ_RANGE)/r - 1.0F;
      resistance = settings[thermistorType].rSeries / resistance;

      // convert to temperature in degrees C
      float f = log(resistance/settings[thermistorType].rNom)/settings[thermistorType].beta;
      f += 1.0F/(settings[thermistorType].tNom + 273.15F);
      float temperature = 1.0F/f - 273.15F;

      // constrain to a reasonable range, outside of this something is definately wrong
      if (temperature < -60.0F || temperature > 60.0F) temperature = NAN;

      // do a running average on the temperature
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
  if (index > 8) index = 0;
}

// nine temperature sensors are supported, this gets the averaged temperature
// in deg. C otherwise it falls back to the weather sensor temperature
// index 0 is the ambient temperature, 1 through 8 are point temperatures #1, #2, etc.
// returns NAN if no temperature source is available or if a communications failure
// results in no valid readings for > 30 seconds
float Thermistor::getChannel(int index) {
  if (found && index >= 0 && index <= 7) {
    if (device[index] == (uint64_t)OFF) averageTemperature[index] = weather.getTemperature();
    return averageTemperature[index];
  } else return NAN;
}

Thermistor temperature;

#endif
