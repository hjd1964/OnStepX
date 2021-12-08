// -----------------------------------------------------------------------------------
// Pulse Per Second precision timer skew

#include "PPS.h"

#if defined(TIME_LOCATION_PPS_SENSE) && TIME_LOCATION_PPS_SENSE != OFF

#include "../tasks/OnTask.h"

void ppsIsr() {
  unsigned long t = micros();
  unsigned long oneSecond = t - pps.lastMicros;
  if (oneSecond > 1000000 - PPS_WINDOW_MICROS && oneSecond < 1000000 + PPS_WINDOW_MICROS) {
    pps.averageMicros = (pps.averageMicros*(PPS_SECS_TO_AVERAGE - 1) + oneSecond)/PPS_SECS_TO_AVERAGE;
    pps.synced = true;
    tasks.setPeriodRatioSubMicros(pps.averageMicros*16UL);
  } else {
    pps.synced = false;
    tasks.setPeriodRatioSubMicros(16000000UL);
  }
  pps.lastMicros = t;
}

void Pps::init() {
    VLF("MSG: PPS, attaching ISR to sense input");
    pinMode(PPS_SENSE_PIN, INPUT);
    #if TIME_LOCATION_PPS_SENSE == HIGH
      attachInterrupt(digitalPinToInterrupt(PPS_SENSE_PIN), ppsIsr, RISING);
    #elif TIME_LOCATION_PPS_SENSE == LOW
      attachInterrupt(digitalPinToInterrupt(PPS_SENSE_PIN), ppsIsr, FALLING);
    #elif TIME_LOCATION_PPS_SENSE == BOTH
      attachInterrupt(digitalPinToInterrupt(PPS_SENSE_PIN), ppsIsr, CHANGE);
    #endif
}

Pps pps;

#endif
