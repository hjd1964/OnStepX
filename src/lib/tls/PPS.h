// handle PPS interrupt
#pragma once

#if TIME_LOCATION_PPS_SENSE == ON

#define PPS_SECS_TO_AVERAGE 40   // running average of 40 samples (1 per second)
#define PPS_WINDOW_MICROS 20000  // +/- window in microseconds to meet synced criteria (2%)

class Pps {
  public:
    // attach interrupt
    void init();

    volatile unsigned long averageMicros = 0;
    volatile unsigned long lastMicros = 0;
    volatile bool synced = false;
  private:
};

extern Pps pps;

#endif
