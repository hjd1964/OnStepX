// -----------------------------------------------------------------------------------
// Spot temperature measurements

#include "Temperature.h"

#ifdef TEMPERATURE_PRESENT

#include "../weather/Weather.h"

bool Temperature::init() {
  return false;
}

float Temperature::getChannel(int index) {
  UNUSED(index);
  return weather.getTemperature();
}

Temperature temperature;

#endif
