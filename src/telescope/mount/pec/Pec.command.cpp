//--------------------------------------------------------------------------------------------------
// telescope mount control, commands

#include "Pec.h"

#ifdef MOUNT_PRESENT

#include "../../../lib/tasks/OnTask.h"

#include "../site/Site.h"

bool Pec::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  *supressFrame = false;
  *commandError = CE_NONE;

  if (command[0] == 'G' && command[1] == 'X' && parameter[2] == 0) {
    #if AXIS1_PEC == ON
      // :GX91#     Get PEC analog value
      //            Returns: n#
      if (parameter[0] == '9' && parameter[1] == '1') {
        sprintf(reply, "%d", 0);
        *numericReply = false;
      } else
    #endif

    // :GXE6#     Get number of steps per sidereal second
    //            Returns: n#
    if (parameter[0] == 'E' && parameter[1] == '6') {
      sprintF(reply, "%0.6f", (axis1.getStepsPerMeasure()/RAD_DEG_RATIO_F)/240.0F);
      *numericReply = false;
    } else

    // :GXE7#     Get PEC worm rotation steps (from NV)
    //            Returns: n#
    if (parameter[0] == 'E' && parameter[1] == '7') {
      PecSettings temp;
      nv.readBytes(NV_MOUNT_PEC_BASE, &temp, sizeof(PecSettings));
      sprintf(reply, "%ld", temp.wormRotationSteps);
      *numericReply = false;
    } else

    // :GXE8#     Get PEC buffer size in seconds
    //            Returns: n#
    if (parameter[0] == 'E' && parameter[1] == '8') {
      sprintf(reply, "%ld", bufferSize);
      *numericReply = false;
    } else return false;
  } else

  #if AXIS1_PEC == ON
    // :SXE7,[n]#
    //            Set PEC steps per worm rotation [n]
    //            Return: 0 on failure or 1 on success
    if (command[0] == 'S' && command[1] == 'X' && parameter[0] == 'E' && parameter[1] == '7' && parameter[2] == ',') {
      if (parameter[2] != ',') { *commandError = CE_PARAM_FORM; return true; } 
      long l = atol(&parameter[3]);
      if (l >= 0 && l < 129600000) {
        PecSettings temp = settings;
        temp.wormRotationSteps = l;
        nv.updateBytes(NV_MOUNT_PEC_BASE, &temp, sizeof(PecSettings));
      } else *commandError = CE_PARAM_RANGE;
    } else
  #endif

  // V - PEC Readout
  if (command[0] == 'V') {
    #if AXIS1_PEC == ON
      //  :VH#      PEC index sense position in sidereal seconds
      //            Returns: n#
      if (command[1] == 'H' && parameter[0] == 0) {
        long s = lroundf(wormSenseSteps/stepsPerSiderealSecond);
        while (s > wormRotationSeconds) s -= wormRotationSeconds;
        while (s < 0) s += wormRotationSeconds;
        sprintf(reply,"%05ld",s);
        *numericReply = false;
      } else

      // :VR[n]#    Read PEC table entry rate adjustment (in steps +/-) for worm segment n (in seconds)
      //            Returns: sn#
      // :VR#       Read PEC table entry rate adjustment (in steps +/-) for currently playing segment and its rate adjustment (in steps +/-)
      //            Returns: sn,n#
      if (command[1] == 'R') {
        int16_t i, j;
        bool conv_result = true;
        if (parameter[0] == 0) i = bufferIndex; else conv_result = convert.atoi2(parameter, &i);
        if (conv_result) {
          if (i >= 0 && i < bufferSize) {
            if (parameter[0] == 0) {
              i -= 1;
              if (i < 0) i += wormRotationSeconds;
              if (i >= wormRotationSeconds) i -= wormRotationSeconds;
              j = buffer[i];
              sprintf(reply,"%+04i,%03i", j, i);
            } else {
              j = buffer[i];
              sprintf(reply,"%+04i", j);
            }
          } else *commandError = CE_PARAM_RANGE;
        } else *commandError = CE_PARAM_FORM;
        *numericReply = false;
      } else

      // :Vr[n]#    Read out RA PEC ten byte frame in hex format starting at worm segment n (in seconds)
      //            Returns: x0x1x2x3x4x5x6x7x8x9# (hex one byte integers)
      //            Ten rate adjustment factors for 1s worm segments in steps +/- (steps = x0 - 128, etc.)
      if (command[1] == 'r') {
        int16_t i, j;
        if (convert.atoi2(parameter, &i)) {
          if (i >= 0 && i < bufferSize) {
            j = 0;
            uint8_t b;
            char s[3] = "  ";
            for (j = 0; j < 10; j++) {
              if (i + j < bufferSize) b = (int)buffer[i + j] + 128; else b = 128;
              sprintf(s, "%02X", b);
              strcat(reply, s);
            }
          } else *commandError = CE_PARAM_RANGE;
        } else *commandError = CE_PARAM_FORM;
        *numericReply = false;
      } else
    #endif

    // :VS#       Get PEC number of steps per sidereal second of worm rotation
    //            Returns: n.n#
    if (command[1] == 'S' && parameter[0] == 0) {
      stepsPerSiderealSecond = (axis1.getStepsPerMeasure()/RAD_DEG_RATIO_F)/240.0F;
      sprintF(reply, "%0.6f", stepsPerSiderealSecond);
      *numericReply = false;
    } else

    // :VW#       Get pec worm rotation steps
    //            Returns: n#
    if (command[1] == 'W' && parameter[0] == 0) {
      long steps = 0;
      #if AXIS1_PEC == ON
        steps = settings.wormRotationSteps;
      #endif
      sprintf(reply, "%06ld", steps);
      *numericReply = false;
    } else return false;
  } else

  // W - PEC Write
  if (command[0] == 'W') {
    #if AXIS1_PEC == ON
      // :WR+#      Move PEC Table ahead by one sidereal second
      //            Return: 0 on failure
      //                    1 on success
      if (command[1] == 'R' && parameter[0] == '+' && parameter[1] == 0) {
        int8_t i = buffer[wormRotationSeconds - 1];
        memmove(&buffer[1], &buffer[0], wormRotationSeconds - 1);
        buffer[0] = i;
      } else

      // :WR-#      Move PEC Table back by one sidereal second
      //            Return: 0 on failure
      //                    1 on success
      if (command[1] == 'R' && parameter[0] == '-' && parameter[1] == 0) {
        int8_t i = buffer[0];
        memmove(&buffer[0], &buffer[1], wormRotationSeconds - 1);
        buffer[wormRotationSeconds - 1] = i;
      } else

      // :WR[n,sn]# Write PEC table entry for worm segment [n] (in sidereal seconds)
      // where [sn] is the correction in steps +/- for this 1 second segment
      //            Returns: Nothing
      if (command[1] == 'R') {
        char *parameter2 = strchr(parameter, ',');
        if (parameter2) {
          int16_t i, j;
          parameter2[0] = 0;
          parameter2++;
          if (convert.atoi2(parameter, &i)) {
            if (i >= 0 && i < bufferSize) {
              if (convert.atoi2(parameter2, &j)) {
                if (j >= -128 && j <= 127) {
                  buffer[i] = j;
                  settings.recorded = true;
                } else *commandError = CE_PARAM_RANGE;
              } else *commandError = CE_PARAM_FORM;
            } else *commandError = CE_PARAM_RANGE;
          } else *commandError = CE_PARAM_FORM;
        } else *commandError = CE_PARAM_FORM;
        *numericReply = false;
      } else
    #endif
    return false;
  } else

  // $QZ - PEC Control
  if (command[0] == '$' && command[1] == 'Q' && parameter[0] == 'Z' && parameter[2] == 0) {
    *numericReply = false;
    #if AXIS1_PEC == ON
      // :$QZ+#     Enable RA PEC compensation 
      //            Returns: nothing
      if (parameter[1] == '+') {
        if (settings.state == PEC_NONE && settings.recorded) settings.state = PEC_READY_PLAY; else *commandError = CE_0;
        nv.updateBytes(NV_MOUNT_PEC_BASE, &settings, sizeof(PecSettings));
      } else
      // :$QZ-#     Disable RA PEC Compensation
      //            Returns: nothing
      if (parameter[1] == '-') {
        settings.state = PEC_NONE;
        nv.updateBytes(NV_MOUNT_PEC_BASE, &settings, sizeof(PecSettings));
      } else
      // :$QZ/#     Ready Record PEC
      //            Returns: nothing
      if (parameter[1] == '/') {
        if (settings.state == PEC_NONE && mount.isTracking()) {
          settings.state = PEC_READY_RECORD;
          nv.updateBytes(NV_MOUNT_PEC_BASE, &settings, sizeof(PecSettings));
        } else *commandError = CE_0;
      } else
      // :$QZZ#     Clear the PEC data buffer
      //            Return: Nothing
      if (parameter[1] == 'Z') {
        for (int i = 0; i < bufferSize; i++) buffer[i] = 0;
        settings.state = PEC_NONE;
        settings.recorded = false;
        nv.updateBytes(NV_MOUNT_PEC_BASE, &settings, sizeof(PecSettings));
      } else
      // :$QZ!#     Write PEC data to NV
      //            Returns: nothing
      if (parameter[1] == '!') {
        settings.recorded = true;
        nv.updateBytes(NV_MOUNT_PEC_BASE, &settings, sizeof(PecSettings));
        for (int i = 0; i < bufferSize; i++) nv.update(NV_PEC_BUFFER_BASE + i, buffer[i]);
      } else
    #endif
    // :$QZ?#     Get PEC status
    //            Returns: s#, one of "IpPrR" (I)gnore, get ready to (p)lay, (P)laying, get ready to (r)ecord, (R)ecording
    //                         or an optional (.) to indicate an index detect
    if (parameter[1] == '?') {
      const char *pecStateStr = "IpPrR";
      uint8_t state = 0;
      #if AXIS1_PEC == ON
        state = settings.state;
      #endif
      reply[0] = pecStateStr[state]; reply[1] = 0; reply[2] = 0;
      #if AXIS1_PEC == ON
        if (wormIndexSenseThisSecond) reply[1] = '.';
      #endif
    } else { *numericReply = true; return false; }
  } else return false;

  return true;
}

#endif