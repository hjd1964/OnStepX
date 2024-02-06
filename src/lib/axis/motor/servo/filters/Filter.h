// -----------------------------------------------------------------------------------
// servo filter
#pragma once

class Filter {
  public:
    virtual long update(long encoderCounts, long motorCounts, bool isTracking) { return encoderCounts; }

  private:
    bool initialized = false;
};
