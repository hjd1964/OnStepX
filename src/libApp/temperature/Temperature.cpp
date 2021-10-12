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
  #if WEATHER == BME280 || WEATHER == BME280_0x76 || WEATHER == BME280_SPI \
      WEATHER == BMP280 || WEATHER == BMP280_0x76 || WEATHER == BMP280_SPI
    return weather.getTemperature();
  #else
    return NAN;
  #endif
}

Temperature temperature;

#endif
