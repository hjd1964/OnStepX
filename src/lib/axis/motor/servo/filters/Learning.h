// -----------------------------------------------------------------------------------
// learning filter (experimental)
#pragma once

#include "../../../../../Common.h"
#include "Filter.h"

#if AXIS1_SERVO_FLTR == LEARNING

class LearningFilter : public Filter {
  public:
    // windowSize: in counts for rolling average
    // frameWidth: in seconds for learning window
    // learn: true when the (high accuracy) motion critical to the process is stable
    LearningFilter(int windowSize, int frameWidth);

    long update(long encoderCounts, long motorCounts, bool learn);

    void analyze();

    void reset();

  private:
    bool active = false;
    bool initialized = false;
    bool lock = false;
    int windowSize; // in samples
    int frameWidth; // in samples

    float countsPerSecond;
    float countsPerSample;
    float samplesPerSecond;
    long phase = 0;
    long lastPhase = 0;
    long avgPhase = 0;
    long avgPhaseShift = 0;
    long correction = 0;
    float avgMaxDelta = 0;
    float avgMinDelta = 0;
    float avgAmplitude = 0;
    float avgPeriod = 0; // in samples
    long *deltas;
    long *counts;
    long index;
    long long avgDelta;
    long long avgEncoderCounts;
    long lastIndex = 0;
};

extern LearningFilter filterAxis1;

#endif
