// -----------------------------------------------------------------------------------
// learning filter (experimental)
//
// This is not a conventional noise filter.
// It attempts to learn a repeatable cosine-like periodic error in the
// encoder-minus-motor delta for the RA axis and subtract part of that learned
// pattern back out. It should be treated as experimental cyclic-error
// compensation rather than a general-purpose smoothing filter.

#include "Learning.h"

#if AXIS1_SERVO_FLTR == LEARNING

#include "../../../../tasks/OnTask.h"

void learningAnalyze() { filterAxis1.analyze(); }

LearningFilter::LearningFilter(int smoothingWindowSize, int historyLength) {
  this->smoothingWindowSize = smoothingWindowSize;
  this->historyLength = historyLength;

  deltaSamples = new long[this->historyLength];

  if (deltaSamples == NULL) { active = false; return; } else active = true;

  trackingCountsPerSecond = (AXIS1_STEPS_PER_DEGREE/240.0F)*SIDEREAL_RATIO_F;
  sampleSpacingCounts = trackingCountsPerSecond/4.0F;
  sampleSpacingSeconds = sampleSpacingCounts/trackingCountsPerSecond;
}

long LearningFilter::wrappedSampleIndex(long sample) const {
  long wrapped = sample % historyLength;
  if (wrapped < 0) wrapped += historyLength;
  return wrapped;
}

float LearningFilter::periodicCorrection(long sample) const {
  if (!lock || averagePeriodSamples <= 0.0F) return 0.0F;

  float phaseOffset = fmodf((sample - averagePhaseSample), averagePeriodSamples);
  if (phaseOffset < 0.0F) phaseOffset += averagePeriodSamples;

  return cosf((phaseOffset/averagePeriodSamples)*PI*2.0F) * (averageAmplitude*0.4F);
}

long LearningFilter::update(long encoderCounts, long motorCounts, bool learningActive) {
  if (!active) return encoderCounts;

  long delta = encoderCounts - motorCounts;

  if (!initialized) {
    reset();

    smoothedDelta = delta;
    smoothedEncoderCounts = encoderCounts;

    VF("MSG: Filter, learning start polling task (rate 5s priority 7)... ");
    if (tasks.add(2000, 0, true, 7, learningAnalyze, "Learn")) { VLF("success"); } else { VLF("FAILED!"); }

    initialized = true;
  }

  smoothedDelta = (smoothedDelta*(smoothingWindowSize - 1) + delta)/smoothingWindowSize;
  smoothedEncoderCounts = (smoothedEncoderCounts*(smoothingWindowSize - 1) + encoderCounts)/smoothingWindowSize;

  sampleIndex = lround(smoothedEncoderCounts/sampleSpacingCounts);

  // Take samples only while the motion is stable enough to learn from.
  if (learningActive && sampleIndex != lastLearnedSampleIndex) {
    lastLearnedSampleIndex = sampleIndex;

    deltaSamples[wrappedSampleIndex(sampleIndex)] = smoothedDelta;
  }

  learnedCorrection = lroundf(periodicCorrection(sampleIndex));

  return (smoothedDelta - learnedCorrection) + motorCounts;
}

void LearningFilter::analyze() {
  if (!active) return;

  // Find the strongest positive and negative error sample in the learned history.
  long maxDelta = deltaSamples[wrappedSampleIndex(sampleIndex)];
  long minDelta = maxDelta;
  long strongestPhaseSample = wrappedSampleIndex(sampleIndex);
  for (int i = 0; i < historyLength; i++) {
    int j = wrappedSampleIndex(sampleIndex - i);
    if (deltaSamples[j] >= maxDelta) { maxDelta = deltaSamples[j]; strongestPhaseSample = j; }
    if (deltaSamples[j] <= minDelta) { minDelta = deltaSamples[j]; }
    Y;
  }
  phaseSample = strongestPhaseSample;

  averagePeakDelta = ((averagePeakDelta*19.0F) + maxDelta)/20.0F;
  averageValleyDelta = ((averageValleyDelta*19.0F) + minDelta)/20.0F;
  averageAmplitude = (averagePeakDelta - averageValleyDelta)/2.0F;

  float error;
  float sampleDelta;
  error = 0;
  for (int i = 0; i < historyLength; i++) {
    int j = wrappedSampleIndex(phaseSample - i);
    sampleDelta = deltaSamples[j];
    error += sampleDelta*sampleDelta;
    Y;
  }
  error /= historyLength;
  float nativeError = error;

  float bestError = 100000;
  float bestPeriod = 0;
  for (int period = 200; period > 20; period -= 1) {
    error = 0;
    for (int i = 0; i < historyLength; i++) {
      int j = wrappedSampleIndex(phaseSample - i);
      sampleDelta = deltaSamples[j] - cosf(((i/(float)historyLength)*(period/20.0F))*(float)(PI)*2.0F)*averageAmplitude;
      error += sampleDelta*sampleDelta;
      Y;
    }
    error /= historyLength;

    if (error < bestError) {
      bestPeriod = historyLength/(period/20.0F);
      bestError = error;
    }
  }

  if (averagePhaseSample == 0) {
    averagePhaseSample = phaseSample;
  } else {
    float phaseForAverage = (float)phaseSample;
    float avgPhaseForAverage = (float)averagePhaseSample;

    // Smooth phase on a circular history buffer without jumping at wraparound.
    if (labs(phaseSample - averagePhaseSample) > historyLength/2) {
      if (phaseSample < averagePhaseSample) phaseForAverage += historyLength; else avgPhaseForAverage += historyLength;
    }

    averagePhaseSample = wrappedSampleIndex(lroundf((avgPhaseForAverage*19.0F + phaseForAverage)/20.0F));
  }
  if (averagePeriodSamples == 0) averagePeriodSamples = bestPeriod;
  averagePeriodSamples = ((averagePeriodSamples*19.0F) + bestPeriod)/20.0F;

  long phaseDistance = labs(phaseSample - averagePhaseSample);
  if (phaseDistance > historyLength/2) phaseDistance = historyLength - phaseDistance;
  const long phaseTolerance = max(2L, lroundf(averagePeriodSamples*0.1F));

  if (bestError < nativeError &&
      averagePeriodSamples > 0 &&
      bestPeriod/averagePeriodSamples > 0.8F && bestPeriod/averagePeriodSamples < 1.2F &&
      phaseDistance <= phaseTolerance)
  {
    lock = true; 
  } else {
    lock = false;
  }

  if (lock) { D("Lock: true "); } else { D("Lock: false"); }
  D(", Qual: "); D((bestError > 0.0F) ? sqrtf(nativeError/bestError)*100.0F : 0.0F); D("%");
  D(", Period s: "); D(bestPeriod*sampleSpacingSeconds); D(", Period avg s: "); D(averagePeriodSamples*sampleSpacingSeconds);
  D(", Phase: "); D((averagePeriodSamples > 0.0F) ? fmodf((float)phaseSample, averagePeriodSamples)/averagePeriodSamples : 0.0F);
  D(", Phase avg: "); DL((averagePeriodSamples > 0.0F) ? fmodf((float)averagePhaseSample, averagePeriodSamples)/averagePeriodSamples : 0.0F);


//  VF("MSG: Native error (sd)      "); VL(sqrt(nativeError));
//  VF("MSG: Best error             "); VL(sqrt(bestError));
//  VF("MSG: Default frame (samples)"); VL(historyLength);
//  VF("MSG: B Period (samples)     "); VL(bestPeriod);
//  VF("MSG: A Period (samples)     "); VL(averagePeriodSamples);
//  VF("MSG: Phase (samples)        "); VL(averagePhaseSample);
//  VF("MSG: Phase cos()            "); VL(cos((averagePhaseSample/averagePeriodSamples)*PI*2.0F));
//  VF("MSG: Amplitude (counts)     "); VL(averageAmplitude);
//  VL("");
  
}

void LearningFilter::reset() {
  if (!active) return;

  phaseSample = 0;
  averagePhaseSample = 0;
  learnedCorrection = 0;
  averagePeakDelta = 0;
  averageValleyDelta = 0;
  averageAmplitude = 0;
  averagePeriodSamples = 0;
  sampleIndex = 0;
  lastLearnedSampleIndex = 0;
  lock = false;
  for (int i = 0; i < historyLength; i++) {
    deltaSamples[i] = 0;
  }
}

LearningFilter filterAxis1(AXIS1_SERVO_FLTR_WSIZE, 240);

#endif
