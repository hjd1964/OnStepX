// handle PPS interrupt
#pragma once

#if TIME_LOCATION_PPS_SENSE == ON

#define PPS_SECS_TO_AVERAGE 40   // running average of 40 samples (1 per second)
#define PPS_WINDOW_MICROS 20000  // +/- window in microseconds to meet synced criteria (2%)

class Pps {
  public:
    // attach interrupt and start PPS
    void init();

    volatile bool synced = false;

    volatile unsigned long averageMicros = 1000000;
    volatile unsigned long lastMicros = 1000000;
  private:
};

extern Pps pps;

#endif
