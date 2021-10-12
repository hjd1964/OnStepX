// -----------------------------------------------------------------------------------------------------------------------------
// Weather related functions
#pragma once

#include "../../Common.h"

enum WeatherSensor: uint8_t {WS_NONE, WS_BMP280, WS_BME280};

class Weather {
  public:
    bool init();

    // designed for a 2s polling interval
    void poll();

    // get temperature in deg. C
    float getTemperature();

    // set temperature in deg. C
    bool setTemperature(float t);
    
    // get barometric pressure in hPa/mb
    float getPressure();
    
    // get barometric pressure in hPa/mb
    bool setPressure(float p);
    
    // get relative humidity in %
    float getHumidity();
    
    // set relative humidity in %
    bool setHumidity(float h);

    // get dew point in deg. C
    // accurate to +/- 1 deg. C for RH above 50%
    float getDewPoint();

  private:
    WeatherSensor weatherSensor = WS_NONE;

    bool success = false;

    float temperature = NAN;
    float averageTemperature = NAN;
    float pressure = NAN;
    float humidity = NAN;
};

extern Weather weather;
