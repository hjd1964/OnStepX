// -----------------------------------------------------------------------------------
// inactive GPIO standin
#pragma once

#include "../../common.h"
#include "Ds2413.h"

#ifndef DS2413_DEVICES_PRESENT

#define GPIO_PRESENT

class Gpio {
  public:
    bool init();

    int getChannel(int index);

    void setChannel(int index, bool state);

    bool failure(int index);
  
  private:
};

extern Gpio gpio;

#endif
