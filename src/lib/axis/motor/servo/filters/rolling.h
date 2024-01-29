// -----------------------------------------------------------------------------------
// rolling average filter
#pragma once

class RollingFilter {
  public:
    RollingFilter(int size);

    long update(long value);

  private:
    bool initialized = false;
    int size;
    long long result;
};
