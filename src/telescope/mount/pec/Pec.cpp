//--------------------------------------------------------------------------------------------------
// telescope mount control, PEC

#include "Pec.h"

#ifdef MOUNT_PRESENT 

#if AXIS1_PEC == ON
  #include "../../../lib/tasks/OnTask.h"

  #include "../../../lib/sense/Sense.h"
  #include "../../Telescope.h"
  #include "../goto/Goto.h"
  #include "../guide/Guide.h"
  #include "../park/Park.h"

  #if (PEC_SENSE) == OFF
    bool wormSenseFirst = true;
  #else
    bool wormSenseFirst = false;
  #endif

  inline void pecWrapper() { pec.poll(); }

  void Pec::init() {
    // confirm the data structure size
    if (PecSettingsSize < sizeof(PecSettings)) { nv.initError = true; DL("ERR: Pec::init(), PecSettingsSize error"); }

    // write the default settings to NV
    if (!nv.hasValidKey() || nv.isNull(NV_MOUNT_PEC_BASE, sizeof(PecSettings))) {
      VLF("MSG: Mount, PEC writing defaults to NV");
      nv.writeBytes(NV_MOUNT_PEC_BASE, &settings, sizeof(PecSettings));
    }

    // read the settings
    nv.readBytes(NV_MOUNT_PEC_BASE, &settings, sizeof(PecSettings));

    stepsPerSiderealSecond = (axis1.getStepsPerMeasure()/RAD_DEG_RATIO)/240.0L;
    stepsPerSiderealSecondI = lroundf(stepsPerSiderealSecond);
    stepsPerMicroSecond = (stepsPerSiderealSecond*SIDEREAL_RATIO)/1000000.0L;

    wormRotationSeconds = round(settings.wormRotationSteps/stepsPerSiderealSecond);
    bufferSize = wormRotationSeconds;

    if (bufferSize > 0) {
      if (bufferSize < 61) {
        bufferSize = 0;
        initError.value = true;
        DLF("WRN: Pec::init(), invalid bufferSize - PEC disabled");
      } else
      if (bufferSize + NV_PEC_BUFFER_BASE >= nv.size - 1) {
        bufferSize = 0;
        initError.value = true;
        DLF("WRN: Pec::init(), bufferSize exceeds available NV - PEC disabled");
      } else {
        buffer = (int8_t*)malloc(bufferSize * sizeof(*buffer));
        if (buffer == NULL) {
          bufferSize = 0;
          initError.value = true;
          VLF("WRN: Pec::init(), bufferSize exceeds available RAM - PEC disabled");
        } else {
          VF("MSG: Mount, PEC allocated buffer "); V(bufferSize * (long)sizeof(*buffer)); VLF(" bytes");

          bool bufferNeedsInit = true;
          for (int i = 0; i < bufferSize; i++) {
            buffer[i] = nv.read(NV_PEC_BUFFER_BASE + i);
            if (buffer[i] != 0) bufferNeedsInit = false;
          }
          if (bufferNeedsInit) for (int i = 0; i < bufferSize; i++) nv.write(NV_PEC_BUFFER_BASE + i, (int8_t)0);

          if (settings.state > PEC_RECORD) {
            settings.state = PEC_NONE;
            initError.value = true;
            DLF("ERR: Pec::init(), bad NV settings.state");
          }

          if (!settings.recorded) settings.state = PEC_NONE;

          #if (PEC_SENSE) == OFF
            #if GOTO_FEATURE == ON
              park.settings.wormSensePositionSteps = 0;
            #endif
            settings.state = PEC_NONE;
          #else
            VLF("MSG: Mount, PEC adding sense");
            senseHandle = sense.add(PEC_SENSE_PIN, PEC_SENSE_INIT, PEC_SENSE);
          #endif

          VF("MSG: Mount, PEC start monitor task (rate 10ms priority 3)... ");
          monitorHandle = tasks.add(10, 0, true, 3, pecWrapper, "MntPec");
          if (monitorHandle) { VLF("success"); } else { VLF("FAILED!"); }
        }
      }
    }
    if (bufferSize <= 0) { bufferSize = 0; settings.state = PEC_NONE; settings.recorded = false; }
    if (wormRotationSeconds > bufferSize) wormRotationSeconds = bufferSize;
  }

  void Pec::poll() {
    // PEC is only active when we're tracking at the sidereal rate with a guide rate that makes sense
    #if GOTO_FEATURE == ON
      if (park.state > PS_UNPARKED) { disable(); return; }
    #endif
    if (!mount.isTracking() || guide.state > GU_PULSE_GUIDE) { disable(); return; }

    // keep track of our current step position, and when the step position on the worm wraps during playback
    long axis1Steps = axis1.getMotorPositionSteps();

    #if (PEC_SENSE) == OFF
      wormSenseFirst = true;
    #else
      static int lastState;
      lastState = wormIndexState;
      wormIndexState = sense.isOn(senseHandle);

      // digital or analog pec sense, with 60 second delay before redetect
      long dist;
      if (wormSenseSteps > axis1Steps) dist = wormSenseSteps - axis1Steps; else dist = axis1Steps - wormSenseSteps;

      if (wormIndexSenseThisSecond && dist > stepsPerSiderealSecond*2.0) wormIndexSenseThisSecond = false;

      if (dist > stepsPerSiderealSecond*60.0 && wormIndexState != lastState && wormIndexState == true) {
        VLF("MSG: Mount, PEC index detected");
        wormSenseSteps = axis1Steps;
        wormSenseFirst = true;
        bufferStart = true;
        wormIndexSenseThisSecond = true;
      } else bufferStart = false;

    #endif

    if (settings.state == PEC_NONE) { rate = 0.0F; return; }
    if (!wormSenseFirst) return;

    // worm step position corrected for any index found
    #if (PEC_SENSE) == OFF
      static long lastWormRotationSteps = wormRotationSteps;
    #endif
    wormRotationSteps = axis1Steps - wormSenseSteps;
    while (wormRotationSteps >= settings.wormRotationSteps) wormRotationSteps -= settings.wormRotationSteps;
    while (wormRotationSteps < 0) wormRotationSteps += settings.wormRotationSteps;

    #if (PEC_SENSE) == OFF
      if (wormRotationSteps - lastWormRotationSteps < 0) {
        VLF("MSG: Mount, PEC virtual index detected");
        bufferStart = true;
      } else bufferStart = false;
    #endif

    // handle playing back and recording PEC
    noInterrupts();
    unsigned long lastFs = fracLAST;
    interrupts();

    // start playing PEC
    if (settings.state == PEC_READY_PLAY) {
      // makes sure the index is at the start of a second before resuming play
      if ((long)fmod(wormRotationSteps, stepsPerSiderealSecond) == 0) {
        VLF("MSG: Mount, PEC started playing");
        settings.state = PEC_PLAY;
        bufferIndex = lroundf(wormRotationSteps/stepsPerSiderealSecond);
        wormRotationStartTimeFs = lastFs;
      }
    } else
    // start recording PEC
    if (settings.state == PEC_READY_RECORD) {
      if ((long)fmod(wormRotationSteps, stepsPerSiderealSecond) == 0) {
        VF("MSG: Mount, PEC started recording at ");
        settings.state = PEC_RECORD;
        bufferIndex = lroundf(wormRotationSteps/stepsPerSiderealSecond);
        firstRecording = !settings.recorded;
        wormRotationStartTimeFs = lastFs;
        V(wormRotationStartTimeFs);
        recordStopTimeFs = wormRotationStartTimeFs + (uint32_t)(wormRotationSeconds*(long)FRACTIONAL_SEC);
        V(" and stopping at "); VL(recordStopTimeFs);
        accGuideAxis1 = 0.0L;
      }
    } else
    // and once the PEC data is all stored, indicate that it's valid and start using it
    if (settings.state == PEC_RECORD && (long)(lastFs - recordStopTimeFs) > 0) {
      VLF("MSG: Mount, PEC recording complete switched to playing");
      settings.state = PEC_PLAY;
      settings.recorded = true;
      cleanup();
    }

    // reset the buffer index to match the worm index
    if (bufferStart && settings.state != PEC_RECORD) {
      bufferIndex = 0;
      wormRotationStartTimeFs = lastFs;
    }

    // Increment the PEC index once a second and make it go back to zero when the
    // worm finishes a rotation, this code works when crossing zero
    if (lastFs - wormRotationStartTimeFs >= FRACTIONAL_SEC) { wormRotationStartTimeFs = lastFs; bufferIndex++; }
    bufferIndex = ((bufferIndex % wormRotationSeconds) + wormRotationSeconds) % wormRotationSeconds;

    // accumulate guide steps for PEC
    if (guide.rateAxis1 != 0.0F) {
      if (accGuideStartTime != 0) accGuideAxis1 += stepsPerMicroSecond*(micros() - accGuideStartTime)*guide.rateAxis1;
      accGuideStartTime = micros();
      if (accGuideStartTime == 0) accGuideStartTime = 1;
    } else accGuideStartTime = 0;

    // falls in whenever the pecIndex changes, which is once a sidereal second
    static long lastBufferIndex = 0;
    if (bufferIndex != lastBufferIndex) {
      lastBufferIndex = bufferIndex;

      // assume no change to tracking rate
      rate = 0.0F;

      if (settings.state == PEC_RECORD) {
        // get guide steps taken from the accumulator
        int i = round(accGuideAxis1);

        // stay within +/- one sidereal rate for corrections
        if (i < -stepsPerSiderealSecondI) i = -stepsPerSiderealSecondI;
        if (i >  stepsPerSiderealSecondI) i =  stepsPerSiderealSecondI;

        // apply weighted average
        if (!firstRecording) i = (i + (int)buffer[bufferIndex]*2)/3;

        // restrict to valid range and store
        if (i < -127) i = -127; else if (i >  127) i = 127;

        // remove steps from the accumulator
        accGuideAxis1 -= i;

        buffer[bufferIndex] = i;
      }

      if (settings.state == PEC_PLAY) {
        // adjust one second before the value was recorded, an estimate of the latency between image acquisition and response
        // if sending values directly to OnStep from PECprep, etc. be sure to account for this
        // number of steps ahead or behind for this 1 second slot, up to +/-127
        int j = bufferIndex - 1; if (j == -1) j += wormRotationSeconds;
        int i = buffer[j];
        if (i >  stepsPerSiderealSecondI) i =  stepsPerSiderealSecondI;
        if (i < -stepsPerSiderealSecondI) i = -stepsPerSiderealSecondI;
        rate = i/stepsPerSiderealSecond;
      }
    }
  }

  // disable PEC
  void Pec::disable() {
    // give up recording if we stop tracking at the sidereal rate
    // don't zero the PEC offset, we don't want things moving and it really doesn't matter 
    if (settings.state == PEC_RECORD || settings.state == PEC_READY_RECORD) {
      VLF("MSG: Mount, PEC recording stopped");
      settings.state = PEC_NONE;
      rate = 0.0F;
    } 
    // get ready to re-index when tracking comes back
    if (settings.state == PEC_PLAY) {
      VLF("MSG: Mount, PEC playing paused");
      settings.state = PEC_READY_PLAY;
      rate = 0.0F;
    } 
  }

  // applies low pass filter to smooth noise in PEC data and linear regression
  void Pec::cleanup() {
    VLF("MSG: Mount, applying low pass filter to PEC data");
    int i,J1,J4,J9,J17;
    for (int scc = 3; scc < wormRotationSeconds + 3; scc++) {
      i = buffer[scc % wormRotationSeconds];

      J1 = lroundf(i*0.01F);
      J4 = lroundf(i*0.04F);
      J9 = lroundf(i*0.09F);
      J17 = lroundf(i*0.17F);
      buffer[(scc - 4) % wormRotationSeconds] = (buffer[(scc - 4) % wormRotationSeconds]) + J1;
      buffer[(scc - 3) % wormRotationSeconds] = (buffer[(scc - 3) % wormRotationSeconds]) + J4;
      buffer[(scc - 2) % wormRotationSeconds] = (buffer[(scc - 2) % wormRotationSeconds]) + J9;
      buffer[(scc - 1) % wormRotationSeconds] = (buffer[(scc - 1) % wormRotationSeconds]) + J17;
      buffer[(scc    ) % wormRotationSeconds] = (buffer[(scc    ) % wormRotationSeconds]) - (J17+J17+J9+J9+J4+J4+J1+J1);
      buffer[(scc + 1) % wormRotationSeconds] = (buffer[(scc + 1) % wormRotationSeconds]) + J17;
      buffer[(scc + 2) % wormRotationSeconds] = (buffer[(scc + 2) % wormRotationSeconds]) + J9;
      buffer[(scc + 3) % wormRotationSeconds] = (buffer[(scc + 3) % wormRotationSeconds]) + J4;
      buffer[(scc + 4) % wormRotationSeconds] = (buffer[(scc + 4) % wormRotationSeconds]) + J1;
    }

    // linear regression
    VLF("MSG: Mount, applying linear regression to PEC data");
    // the number of steps added should equal the number of steps subtracted (from the cycle)
    // first, determine how far we've moved ahead or backward in steps
    long stepsSum = 0; for (int scc = 0; scc < wormRotationSeconds; scc++) stepsSum += buffer[scc];

    // this is the correction coefficient for a given location in the sequence
    float Ccf = (float)stepsSum/wormRotationSeconds;

    // now, apply the correction to the sequence to make the PEC adjustments null out
    // this process was simulated in a spreadsheet and the roundoff error might leave us at +/- a step which is tacked on at the beginning
    long lp2 = 0; stepsSum = 0; 
    for (int scc = 0; scc < wormRotationSeconds; scc++) {
      // the correction, "now"
      long lp1 = lroundf(-scc*Ccf);
      
      // if the correction increases or decreases then add or subtract that many steps
      buffer[scc] = buffer[scc] + (lp1 - lp2);

      // sum the values for a final adjustment, if necessary
      stepsSum += buffer[scc];
      lp2 = lp1;
    }
    buffer[0] -= stepsSum;

    // a reality check, make sure the buffer data looks good, if not forget it
    if (stepsSum < -2 || stepsSum > 2) {
      DF("ERR: Pec::cleanup(), linear regression residual "); D(stepsSum);
      if (stepsSum > 2) { DLF(" > threshold of 2"); } else { DLF(" < threshold of -2"); }
      settings.recorded = false;
      settings.state = PEC_NONE;
    }
  }

#endif

  Pec pec;

#endif
