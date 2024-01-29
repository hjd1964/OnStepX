// -----------------------------------------------------------------------------------
// windowing average filter

#include "windowing.h"

WindowingFilter::WindowingFilter(int size) {
  this->size = size;
  values = new long[size]; 
}

long WindowingFilter::update(long value) {
  if (!initialized) {
    sum = value*size;
    index = 0;
    for (int i = 0; i < size; i++) values[i] = value;
    initialized = true;
  }
  sum -= values[index % size];
  values[index % size] = value;
  sum += value;
  index++;
  return sum/size;
}
