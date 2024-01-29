// -----------------------------------------------------------------------------------
// rolling average filter

#include "rolling.h"

RollingFilter::RollingFilter(int size) {
  this->size = size;
}

long RollingFilter::update(long value) {
  if (!initialized) {
     result = value;
     initialized = true;
  }
  result = (result*(size - 1) + value)/size;
  return result;
}
