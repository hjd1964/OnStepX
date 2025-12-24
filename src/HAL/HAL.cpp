// HAL global variables
#include <Arduino.h>

// Must match the externs in HAL_FAST_TICKS.h exactly
uint32_t _halTicksPerSecond = 1000000UL;
uint32_t _halTicksPerMicro  = 1;
