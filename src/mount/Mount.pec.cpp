//--------------------------------------------------------------------------------------------------
// telescope mount control, PEC

#include "../Common.h"
#include "Mount.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF && AXIS1_PEC == ON

#include "../tasks/OnTask.h"
extern Tasks tasks;
#include "../coordinates/Site.h"
#include "../telescope/Telescope.h"
#include "../lib/sense/Sense.h"

#if PEC_SENSE == OFF
  bool wormSenseFirst = true;
#else
  bool wormSenseFirst = false;
#endif

inline void mountPecWrapper() { telescope.mount.pecPoll(); }

void Mount::pecInit() {
  // read the pec settings
  if (PecSize < sizeof(Pec)) { DL("ERR: Mount::initPec(); PecSize error NV subsystem writes disabled"); nv.readOnly(true); }
  nv.readBytes(NV_MOUNT_PEC_BASE, &pec, PecSize);

  wormRotationSeconds = pec.wormRotationSteps/stepsPerSiderealSecondAxis1;
  pecBufferSize = ceil(pec.wormRotationSteps/(axis1.getStepsPerMeasure()/240.0));

  if (pecBufferSize != 0) {
    if (pecBufferSize < 61) {
      pecBufferSize = 0;
      //initError = true;
      DLF("ERR: Mount::initPec(); invalid pecBufferSize PEC disabled");
    }
    if (pecBufferSize + NV_PEC_BUFFER_BASE >= nv.size - 1) {
      pecBufferSize = 0;
      //initError = true;
      DLF("ERR: Mount::initPec(); pecBufferSize exceeds available NV, PEC disabled");
    }
  }
  if (wormRotationSeconds > pecBufferSize) wormRotationSeconds = pecBufferSize;

  pecBuffer = (int8_t*)malloc(pecBufferSize * sizeof(*pecBuffer));
  if (pecBuffer == NULL) {
    pecBufferSize = 0;
    VLF("WRN: Mount, buffer exceeds available RAM PEC disabled");
  } else {
    long allocatedSize = pecBufferSize * sizeof(*pecBuffer);
    VF("MSG: Mount, allocated PEC buffer "); V(allocatedSize); VLF(" bytes");
  }

  bool pecBufferNeedsInit = true;
  for (int i = 0; i < pecBufferSize; i++) {
    pecBuffer[i] = nv.read(NV_PEC_BUFFER_BASE + i);
    if (pecBuffer[i] != 0) pecBufferNeedsInit = false;
  }
  if (pecBufferNeedsInit) for (int i = 0; i < pecBufferSize; i++) nv.write(NV_PEC_BUFFER_BASE + i, (int8_t)0);

  #if PEC_SENSE == OFF
    park.wormSensePositionSteps = 0;
    pec.state = PEC_NONE;
  #else
    VLF("MSG: Mount, adding pec sense");
    pecSenseHandle = senses.add(PEC_SENSE_PIN, PEC_SENSE_INIT, PEC_SENSE);
  #endif

  if (pec.state > PEC_RECORD) {
    pec.state = PEC_NONE;
    //initError = true;
    DLF("ERR: Mount::initPec(); bad NV pec.state");
  }

  if (!pec.recorded) pec.state = PEC_NONE;

  // start pec monitor task if it's not running
  if (pecMonitorHandle == 0) {
    VF("MSG: Mount, start pec monitor task... ");
    pecMonitorHandle = tasks.add(10, 0, true, 1, mountPecWrapper, "MntPec");
    if (pecMonitorHandle) { VL("success"); } else { VL("FAILED!"); }
  }
}

void Mount::pecPoll() {
  // PEC is only active when we're tracking at the sidereal rate with a guide rate that makes sense
  if (trackingState != TS_SIDEREAL ||
      park.state != PS_PARKED ||
     (guideState != GU_NONE && guideState != GU_PULSE_GUIDE)) { pecDisable(); return; }

  // keep track of our current step position, and when the step position on the worm wraps during playback
  long axis1Steps = axis1.getMotorCoordinateSteps();

  #if PEC_SENSE == OFF
    wormSenseFirst = true;
  #else
    static int lastState;
    lastState = wormIndexState;
    wormIndexState = senses.read(pecSenseHandle);

    // digital or analog pec sense, with 60 second delay before redetect
    long dist; if (wormSenseSteps > axis1Steps) dist = wormSenseSteps - axis1Steps; else dist = axis1Steps - wormSenseSteps;
    if (dist > stepsPerSiderealSecondAxis1*60.0 && wormIndexState != lastState && wormIndexState == true) {
      wormSenseSteps = axis1Steps;
      wormSenseFirst = true;
      pecBufferStart = true;
      wormIndexSenseThisSecond = true;
    } else pecBufferStart = false;

    if (wormIndexSenseThisSecond && dist > stepsPerSiderealSecondAxis1) wormIndexSenseThisSecond = false;
  #endif

  if (pec.state == PEC_NONE) { pecRateAxis1 = 0.0; return; }
  if (!wormSenseFirst) return;

  // worm step position corrected for any index found
  #if PEC_SENSE == OFF
    static long lastWormRotationSteps = wormRotationSteps;
  #endif
  wormRotationSteps = axis1Steps - wormSenseSteps;
  wormRotationSteps = ((wormRotationSteps % pec.wormRotationSteps) + pec.wormRotationSteps) % pec.wormRotationSteps;
  #if PEC_SENSE == OFF
    if (wormRotationSteps - lastWormRotationSteps < 0) pecBufferStart = true; else pecBufferStart = false;
  #endif

  // handle playing back and recording PEC
  noInterrupts();
  unsigned long lastCs = centisecondLAST; // local apparent sidereal time in centi-seconds
  interrupts();

  // start playing PEC
  if (pec.state == PEC_READY_PLAY) {
    // makes sure the index is at the start of a second before resuming play
    if ((long)fmod(wormRotationSteps, stepsPerCentisecondAxis1) == 0) {
      pec.state = PEC_PLAY;
      pecBufferIndex = wormRotationSteps/stepsPerCentisecondAxis1;
      wormRotationStartTimeCs = lastCs;
    }
  } else
  // start recording PEC
  if (pec.state == PEC_READY_RECORD) {
    if ((long)fmod(wormRotationSteps, stepsPerCentisecondAxis1) == 0) {
      pec.state = PEC_RECORD;
      pecBufferIndex = wormRotationSteps/stepsPerCentisecondAxis1;
      pecFirstRecording = !pec.recorded;
      wormRotationStartTimeCs = lastCs;
      pecRecordStopTimeCs = wormRotationStartTimeCs + wormRotationSeconds*100;
      pecAccGuideAxis1 = 0;
    }
  } else
  // and once the PEC data is all stored, indicate that it's valid and start using it
  if (pec.state == PEC_RECORD && lastCs - pecRecordStopTimeCs > 0) {
    pec.state = PEC_PLAY;
    pec.recorded = true;
    pecCleanup();
  }

  // reset the buffer index to match the worm index
  if (pecBufferStart && pec.state != PEC_RECORD) {
    pecBufferIndex = 0;
    wormRotationStartTimeCs = lastCs;
  }

  // Increment the PEC index once a second and make it go back to zero when the
  // worm finishes a rotation, this code works when crossing zero
  if (lastCs - wormRotationStartTimeCs >= 100) { wormRotationStartTimeCs = lastCs; pecBufferIndex++; }
  pecBufferIndex = ((pecBufferIndex % wormRotationSeconds) + wormRotationSeconds) % wormRotationSeconds;

  // accumulate guide steps for PEC
  if (guideRateAxis1 != 0.0) {
    if (guideActionAxis1 == GA_FORWARD) pecAccGuideAxis1 += stepsPerCentisecondAxis1*guideRateAxis1;
    if (guideActionAxis1 == GA_REVERSE) pecAccGuideAxis1 -= stepsPerCentisecondAxis1*guideRateAxis1;
  }
  
  // falls in whenever the pecIndex changes, which is once a sidereal second
  static long lastPecBufferIndex = 0;
  if (pecBufferIndex != lastPecBufferIndex) {
    lastPecBufferIndex = pecBufferIndex;

    // assume no change to tracking rate
    pecRateAxis1 = 0.0;

    if (pec.state == PEC_RECORD) {
      // get guide steps taken from the accumulator
      int i = lround(pecAccGuideAxis1);

      // stay within +/- one sidereal rate for corrections
      if (i < -stepsPerSiderealSecondAxis1) i = -stepsPerSiderealSecondAxis1;
      if (i >  stepsPerSiderealSecondAxis1) i =  stepsPerSiderealSecondAxis1;

      // remove steps from the accumulator
      pecAccGuideAxis1 -= i;

      // apply weighted average
      if (!pecFirstRecording) i = (i + pecBuffer[pecBufferIndex]*2)/3;

      // restrict to valid range and store
      if (i < -127) i = -127; else if (i >  127) i =  127;
      pecBuffer[pecBufferIndex] = i;
    }

    if (pec.state == PEC_PLAY) {
      // adjust one second before the value was recorded, an estimate of the latency between image acquisition and response
      // if sending values directly to OnStep from PECprep, etc. be sure to account for this
      // number of steps ahead or behind for this 1 second slot, up to +/-127
      int j = pecBufferIndex - 1; if (j == -1) j += wormRotationSeconds;
      int i = pecBuffer[j];
      if (i >  stepsPerSiderealSecondAxis1) i =  stepsPerSiderealSecondAxis1;
      if (i < -stepsPerSiderealSecondAxis1) i = -stepsPerSiderealSecondAxis1;
      pecRateAxis1 = (float)i/stepsPerSiderealSecondAxis1;
    }
  }
}
 
void Mount::pecDisable() {
  // give up recording if we stop tracking at the sidereal rate
  // don't zero the PEC offset, we don't want things moving and it really doesn't matter 
  if (pec.state == PEC_RECORD) { pec.state = PEC_NONE; pecRateAxis1 = 0.0; } 
  // get ready to re-index when tracking comes back
  if (pec.state == PEC_PLAY)   { pec.state = PEC_READY_PLAY; pecRateAxis1 = 0.0; } 
}

void Mount::pecCleanup() {
  // low pass filter smooths noise in Pec data
  int i,J1,J4,J9,J17;
  for (int scc = 3; scc < wormRotationSeconds + 3; scc++) {
    i = pecBuffer[scc % wormRotationSeconds];

    J1 = lround(i*0.01F);
    J4 = lround(i*0.04F);
    J9 = lround(i*0.09F);
    J17 = lround(i*0.17F);
    pecBuffer[(scc - 4) % wormRotationSeconds] = (pecBuffer[(scc - 4) % wormRotationSeconds]) + J1;
    pecBuffer[(scc - 3) % wormRotationSeconds] = (pecBuffer[(scc - 3) % wormRotationSeconds]) + J4;
    pecBuffer[(scc - 2) % wormRotationSeconds] = (pecBuffer[(scc - 2) % wormRotationSeconds]) + J9;
    pecBuffer[(scc - 1) % wormRotationSeconds] = (pecBuffer[(scc - 1) % wormRotationSeconds]) + J17;
    pecBuffer[(scc    ) % wormRotationSeconds] = (pecBuffer[(scc    ) % wormRotationSeconds]) - (J17+J17+J9+J9+J4+J4+J1+J1);
    pecBuffer[(scc + 1) % wormRotationSeconds] = (pecBuffer[(scc + 1) % wormRotationSeconds]) + J17;
    pecBuffer[(scc + 2) % wormRotationSeconds] = (pecBuffer[(scc + 2) % wormRotationSeconds]) + J9;
    pecBuffer[(scc + 3) % wormRotationSeconds] = (pecBuffer[(scc + 3) % wormRotationSeconds]) + J4;
    pecBuffer[(scc + 4) % wormRotationSeconds] = (pecBuffer[(scc + 4) % wormRotationSeconds]) + J1;
  }
  
  // linear regression
  // the number of steps added should equal the number of steps subtracted (from the cycle)
  // first, determine how far we've moved ahead or backward in steps
  long stepsSum = 0; for (int scc = 0; scc < wormRotationSeconds; scc++) stepsSum += pecBuffer[scc];

  // this is the correction coefficient for a given location in the sequence
  float Ccf = round((float)stepsSum/wormRotationSeconds);

  // now, apply the correction to the sequence to make the PEC adjustments null out
  // this process was simulated in a spreadsheet and the roundoff error might leave us at +/- a step which is tacked on at the beginning
  long lp2 = 0; stepsSum = 0; 
  for (int scc = 0; scc < wormRotationSeconds; scc++) {
    // the correction, "now"
    long lp1 = lround(-scc*Ccf);
    
    // if the correction increases or decreases then add or subtract that many steps
    pecBuffer[scc] = pecBuffer[scc] + (lp1 - lp2);

    // sum the values for a final adjustment, if necessary
    stepsSum += pecBuffer[scc];
    lp2 = lp1;
  }
  pecBuffer[0] -= stepsSum;

  // a reality check, make sure the buffer data looks good, if not forget it
  if (stepsSum > 2 || stepsSum < -2) { pec.recorded = false; pec.state = PEC_NONE; }
}

#endif
