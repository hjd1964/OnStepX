// -----------------------------------------------------------------------------------
// Spot temperature measurements
#pragma once

#include "../../Common.h"

#include "Ds1820.h"
#ifndef DS1820_DEVICES_PRESENT

#include "Thermistor.h"
#ifndef THERMISTOR_DEVICES_PRESENT

#define TEMPERATURE_PRESENT

class Temperature {
  public:
    bool init();

    float getChannel(int index);
   
  private:
};

extern Temperature temperature;

#endif
#endif
