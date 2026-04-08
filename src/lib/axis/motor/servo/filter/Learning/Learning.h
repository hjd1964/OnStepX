// -----------------------------------------------------------------------------------
// learning filter (experimental)
#pragma once

#include "../FilterBase.h"

#if AXIS1_SERVO_FLTR == LEARNING

class LearningFilter : public Filter {
  public:
    // smoothingWindowSize: sample count for the rolling average
    // historyLength: number of learned samples retained for periodic analysis
    // learningActive: true when the motion is stable enough to train on
    LearningFilter(int smoothingWindowSize, int historyLength);

    long update(long encoderCounts, long motorCounts, bool learningActive);

    void analyze();

    void reset();

  private:
    long wrappedSampleIndex(long sample) const;
    float periodicCorrection(long sample) const;

    bool active = false;
    bool initialized = false;
    bool lock = false;
    int smoothingWindowSize; // in samples
    int historyLength;       // in samples

    float trackingCountsPerSecond;
    float sampleSpacingCounts;
    float sampleSpacingSeconds;
    long phaseSample = 0;
    long averagePhaseSample = 0;
    long learnedCorrection = 0;
    float averagePeakDelta = 0;
    float averageValleyDelta = 0;
    float averageAmplitude = 0;
    float averagePeriodSamples = 0;
    long *deltaSamples;
    long sampleIndex = 0;
    long long smoothedDelta;
    long long smoothedEncoderCounts;
    long lastLearnedSampleIndex = 0;
};

extern LearningFilter filterAxis1;

#endif
