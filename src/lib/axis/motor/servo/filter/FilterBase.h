// -----------------------------------------------------------------------------------
// servo filter
#pragma once

#include "../../../../../Common.h"

class Filter {
  public:
    virtual long update(long encoderCounts, long motorCounts, bool isTracking) { return encoderCounts; }

  private:
    bool initialized = false;
};
