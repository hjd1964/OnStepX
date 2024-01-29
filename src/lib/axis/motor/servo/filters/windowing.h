// -----------------------------------------------------------------------------------
// windowing average filter
#pragma once

class WindowingFilter {
  public:
    WindowingFilter(int size);
    
    long update(long value);

  private:
    bool initialized = false;
    int size;
    long *values;
    long long index;
    long long sum;
};
