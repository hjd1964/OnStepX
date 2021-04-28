// -----------------------------------------------------------------------------------
// Step driver mode control pins
#pragma once

typedef struct DriverPins {
  int8_t m0;
  int8_t m1;
  int8_t m2;
  int8_t m3;
  int8_t decay;
  int8_t fault;
} DriverPins;

#define mosi m0
#define sck  m1
#define cs   m2
#define miso m3
