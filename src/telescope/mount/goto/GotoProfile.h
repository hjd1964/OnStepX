//--------------------------------------------------------------------------------------------------
// telescope mount goto, single axis slew motion profile model
#pragma once

#include <math.h>

// Models Axis::poll() AR_RATE_BY_DISTANCE motion: v = clamp(sqrt(2*a*min(dOrigin, dTarget)), vLow, vHigh)
// all units are radians and seconds, distance is unsigned
class GotoProfile {
  public:
    // \param distance: total distance to travel
    // \param accel: acceleration rate
    // \param vLow: lowest rate, from the backlash and minimum frequencies
    // \param vHigh: slew rate
    void init(float distance, float accel, float vLow, float vHigh) {
      this->distance = distance;
      this->accel = accel;
      this->vHigh = vHigh;
      this->vLow = vLow < vHigh ? vLow : vHigh;

      half = distance/2.0F;
      xLow = clampHalf((this->vLow*this->vLow)/(2.0F*accel));
      xHigh = clampHalf((vHigh*vHigh)/(2.0F*accel));
      if (xHigh < xLow) xHigh = xLow;

      tLow = this->vLow > 0.0F ? xLow/this->vLow : 0.0F;
      sLow = sqrtf(2.0F*xLow/accel);
      tHigh = tLow + (sqrtf(2.0F*xHigh/accel) - sLow);
      tHalf = tHigh + (vHigh > 0.0F ? (half - xHigh)/vHigh : 0.0F);
    }

    // total time to complete the motion
    inline float duration() { return 2.0F*tHalf; }

    // distance travelled at time t, from 0 to distance
    float position(float t) {
      if (t <= 0.0F) return 0.0F;
      if (t >= duration()) return distance;
      if (t > tHalf) return distance - halfPosition(duration() - t);
      return halfPosition(t);
    }

    // highest rate reached during the motion
    inline float rateMax() { return vHigh > vLow ? vHigh : vLow; }

  private:
    inline float clampHalf(float x) { return x < half ? x : half; }

    float halfPosition(float t) {
      if (t <= tLow) return vLow*t;
      if (t <= tHigh) {
        float s = t - tLow + sLow;
        return 0.5F*accel*s*s;
      }
      return xHigh + vHigh*(t - tHigh);
    }

    float distance = 0.0F;
    float accel = 1.0F;
    float vLow = 0.0F;
    float vHigh = 0.0F;
    float half = 0.0F;
    float xLow = 0.0F;
    float xHigh = 0.0F;
    float tLow = 0.0F;
    float sLow = 0.0F;
    float tHigh = 0.0F;
    float tHalf = 0.0F;
};
