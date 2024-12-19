// -----------------------------------------------------------------------------------
// learning filter (experimental)

#include "Learning.h"

#if AXIS1_SERVO_FLTR == LEARNING

#include "..\..\..\..\tasks\OnTask.h"

void learningAnalyze() { filterAxis1.analyze(); }

LearningFilter::LearningFilter(int windowSize, int frameWidth) {
  this->windowSize = windowSize;
  this->frameWidth = frameWidth;

  deltas = new long[this->frameWidth];
  counts = new long[this->frameWidth];

  if (deltas == NULL || counts == NULL) { active = false; return; } else active = true;

  countsPerSecond = (AXIS1_STEPS_PER_DEGREE/240.0F)*SIDEREAL_RATIO_F;
  countsPerSample = countsPerSecond/4.0F;
  samplesPerSecond = countsPerSample/countsPerSecond;
}

long LearningFilter::update(long encoderCounts, long motorCounts, bool learn) {
  if (!active) return encoderCounts;

  long delta = encoderCounts - motorCounts;

  if (!initialized) {
    reset();

    avgDelta = delta;
    avgEncoderCounts = encoderCounts;

    VF("MSG: Filter, learning start polling task (rate 5s priority 7)... ");
    if (tasks.add(2000, 0, true, 7, learningAnalyze, "Learn")) { VLF("success"); } else { VLF("FAILED!"); }

    initialized = true;
  }

  avgDelta = (avgDelta*(windowSize - 1) + delta)/windowSize;
  avgEncoderCounts = (avgEncoderCounts*(windowSize - 1) + encoderCounts)/windowSize;

  index = lround(avgEncoderCounts/countsPerSample);

  // take a samples
  if (learn && index != lastIndex) {
    lastIndex = index;

    deltas[index % frameWidth] = avgDelta;
    counts[index % frameWidth] = avgEncoderCounts;

   // DL(cos(((index - phase)/avgPeriod)*PI*2));
    if (lock) {
      correction = cos((((index - avgPhase) % (long)avgPeriod)/avgPeriod)*PI*2.0F) * (avgAmplitude*0.4);
//      correction = 0;
    } else correction = 0;
  } else correction = 0;

  avgDelta -= correction;

  return avgDelta + motorCounts;
}

void LearningFilter::analyze() {
  if (!active) return;

  // locate highest delta, an start of phase
  // locate lowest delta, an half-phase
  long maxDelta = 0;
  long minDelta = 0;
  long maxCount = 0;
  long minCount = 0;
  long p = 0, t = 0;
  for (int i = 0; i < frameWidth; i++) {
    int j = (index - i) % frameWidth;
    if (j < 0) j += frameWidth;
    if (deltas[j] >= maxDelta) { maxDelta = deltas[j]; maxCount = counts[j]; p = j; }
    if (deltas[j] <= minDelta) { minDelta = deltas[j]; minCount = counts[j]; t = j; }
    Y;
  }
  phase = p;

  avgMaxDelta = ((avgMaxDelta*19.0F) + maxDelta)/20.0F;
  avgMinDelta = ((avgMinDelta*19.0F) + minDelta)/20.0F;
  avgAmplitude = (avgMaxDelta - avgMinDelta)/2.0F;

  float error, delta;
  error = 0;
  for (int i = 0; i < frameWidth; i++) {
    int j = (phase - i) % frameWidth;
    if (j < 0) j += frameWidth;
    delta = deltas[j];
    error += delta*delta;
    Y;
  }
  error /= frameWidth;
  float nativeError = error;

  float bestError = 100000;
  float bestPeriod = 0;
  for (int period = 200; period > 20; period -= 1) {
    error = 0;
    for (int i = 0; i < frameWidth; i++) {
      int j = (phase - i) % frameWidth;
      if (j < 0) j += frameWidth;
      delta = deltas[j] - cos(((i/(float)frameWidth)*(period/20.0F))*(float)(PI)*2.0F)*avgAmplitude;
      error += delta*delta;
      Y;
    }
    error /= frameWidth;

    if (error < bestError) {
      bestPeriod = frameWidth/(period/20.0F);
      bestError = error;
    }
  }

  if (phase != lastPhase) {
    lastPhase = phase;
    avgPhase += avgPhaseShift;
  } else {
    if (avgPhaseShift = 0) avgPhaseShift = lastPhase - phase;
    avgPhaseShift = ((avgPhaseShift*19.0F) + (lastPhase - phase))/20.0F;

    if (avgPhase == 0) avgPhase = phase;
    avgPhase = ((avgPhase*19.0F) + phase)/20.0F;
  }

  if (avgPeriod == 0) avgPeriod = bestPeriod;
  avgPeriod = ((avgPeriod*19.0F) + bestPeriod)/20.0F;

  if (bestError < nativeError &&
      avgPeriod > 0 &&
      avgPhase > 0 &&
      bestPeriod/avgPeriod > 0.8 && bestPeriod/avgPeriod < 1.2 &&
      phase/avgPhase > 0.9 && phase/avgPhase < 1.1)
  {
    lock = true; 
  } else {
    lock = false;
  }

  if (lock) { D("Lock: true "); } else { D("Lock: false"); }
  D(", Qual: "); D(sqrt(nativeError)/sqrt(bestError)*100.0); D("%");
  D(", Period: "); D(bestPeriod*samplesPerSecond); D(", Period avg: "); D(avgPeriod*samplesPerSecond);
  D(", Phase: ");     D((phase % (long)avgPeriod)/avgPeriod);
  D(", Phase avg: "); DL((avgPhase % (long)avgPeriod)/avgPeriod);


//  VF("MSG: Native error (sd)      "); VL(sqrt(nativeError));
//  VF("MSG: Best error             "); VL(sqrt(bestError));
//  VF("MSG: Default frame (samples)"); VL(frameWidth);
//  VF("MSG: B Period (samples)     "); VL(bestPeriod);
//  VF("MSG: A Period (samples)     "); VL(avgPeriod);
//  VF("MSG: Phase (samples)        "); VL(avgPhase);
//  VF("MSG: Phase cos()            "); VL(cos((avgPhase/avgPeriod)*PI*2.0F));
//  VF("MSG: Amplitude (counts)     "); VL(avgAmplitude);
//  VL("");
  
}

void LearningFilter::reset() {
  if (!active) return;

  phase = 0;
  avgMaxDelta = 0;
  avgMinDelta = 0;
  avgAmplitude = 0;
  avgPeriod = 0;
  for (int i = 0; i < frameWidth; i++) {
    counts[i] = 0;
    deltas[i] = 0;
  }
}

LearningFilter filterAxis1(AXIS1_SERVO_FLTR_WSIZE, 240);

#endif
