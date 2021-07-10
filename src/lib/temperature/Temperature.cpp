// -----------------------------------------------------------------------------------
// Dallas/Maxim 1-Wire DS1820 device support

#include "Temperature.h"
#include "../weather/Weather.h"

#ifdef TEMPERATURE_PRESENT

bool Temperature::init() {
  return false;
}

float Temperature::getChannel(int index) {
  index = index;
  return weather.getTemperature();
}

Temperature temperature;

#endif
