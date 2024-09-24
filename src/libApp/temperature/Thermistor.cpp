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

      #ifdef ESP32
        int counts = round((analogReadMilliVolts(devicePin[index])/3300.0F)*(float)ANALOG_READ_RANGE);
      #else
        int counts = analogRead(devicePin[index]);
      #endif

      // calculate the device resistance
      float resistance;

      // handle special case where we add a resistor (say 10k) parallel to the thermistor (say 10k 3950)
      #ifdef THERMISTOR_RPARALLEL
        float voltage = (counts/(float)ANALOG_READ_RANGE)*3.3F;
        float RtLow = settings[thermistorType].rNom/30.0F;
        float RtHigh = settings[thermistorType].rNom*30.0F;
        float Rpr = 1.0F/THERMISTOR_RPARALLEL;
        bool vOutLowChanged = true;
        bool vOutHighChanged = true;
        float vOutLow, vOutHigh;
        for (int i = 0; i < 100; i++) {
          if (vOutLowChanged) {
            resistance = 1.0F/(Rpr + 1.0F/RtLow);
            vOutLow = 3.3F * (resistance/(settings[thermistorType].rSeries + resistance));
            vOutLowChanged = false;
          }
          if (vOutHighChanged) {
            resistance = 1.0F/(Rpr + 1.0F/RtHigh);
            vOutHigh = 3.3F * (resistance/(settings[thermistorType].rSeries + resistance));
            vOutHighChanged = false;
          }
          if (voltage < vOutLow) { RtLow /= 1.5F; vOutLowChanged = true; } else
          if (voltage > vOutHigh) { RtHigh *= 1.5F; vOutHighChanged = true; } else
          if (voltage > (vOutLow + vOutHigh)/2.0F) { RtLow = (RtLow*4.0F + RtHigh)/5.0F; vOutLowChanged = true; } else
          if (voltage < (vOutLow + vOutHigh)/2.0F) { RtHigh = (RtLow + RtHigh*4.0F)/5.0F; vOutHighChanged = true; };
        }
        resistance = 1.0F/((1.0F/resistance) - Rpr);
      #else
        resistance = (float)(ANALOG_READ_RANGE)/counts - 1.0F;
        resistance = settings[thermistorType].rSeries/resistance;
      #endif

      // convert to temperature in degrees C
      float f = log(resistance/settings[thermistorType].rNom)/settings[thermistorType].beta;
      f += 1.0F/(settings[thermistorType].tNom + 273.15F);
      float temperature = 1.0F/f - 273.15F;

      // constrain to a reasonable range, outside of this something is definately wrong
      if (temperature < THERMISTOR_TEMPERATURE_MINIMUM || temperature > THERMISTOR_TEMPERATURE_MAXIMUM) temperature = NAN;

      // do a running average on the temperature
      if (!isnan(temperature)) {
        if (isnan(averageTemperature[index])) averageTemperature[index] = temperature;
        averageTemperature[index] = (averageTemperature[index]*19.0F + temperature)/20.0F;
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
