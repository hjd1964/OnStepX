// handle PPS interrupt
#pragma once

#include "../../Common.h"

#if defined(TIME_LOCATION_PPS_SENSE) && (TIME_LOCATION_PPS_SENSE) != OFF

#define PPS_SECS_TO_AVERAGE 40   // running average of 40 samples (1 per second)
#define PPS_WINDOW_MICROS 20000  // +/- window in microseconds to meet synced criteria (2%)

#if !defined(PPS_SENSE_PIN) || PPS_SENSE_PIN == OFF
  #error "Configuration (Config.h): PPS_SENSE_PIN must be defined for TIME_LOCATION_PPS_SENSE ON"
#endif

class Pps {
  public:
    // attach interrupt and start PPS
    void init();

    volatile bool synced = false;

    volatile unsigned long averageSubMicros = 16000000UL;
    long averageSubMicrosSkew = 0;
    volatile unsigned long lastMicros = 0UL;
  private:
};

extern Pps pps;

#endif
