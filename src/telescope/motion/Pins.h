// -----------------------------------------------------------------------------------
// Step driver mode control pins
#pragma once

#include <Arduino.h>

typedef struct DriverPins {
  int16_t m0;
  int16_t m1;
  int16_t m2;
  int16_t m3;
  int16_t decay;
  int16_t fault;
} DriverPins;

#define mosi m0
#define sck  m1
#define cs   m2
#define miso m3
