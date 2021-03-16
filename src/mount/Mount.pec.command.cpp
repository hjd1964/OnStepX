//--------------------------------------------------------------------------------------------------
// telescope mount control, commands
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../lib/nv/NV.h"
extern NVS nv;
#include "../pinmaps/Models.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../debug/Debug.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;

#include "../coordinates/Convert.h"
#include "../coordinates/Transform.h"
#include "../coordinates/Site.h"
#include "../commands/ProcessCmds.h"
#include "../motion/StepDrivers.h"
#include "../motion/Axis.h"
#include "Mount.h"

bool Mount::commandPec(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {

  // :$QZ?      Get PEC status
  //            Returns: s#, one of "IpPrR" (I)gnore, get ready to (p)lay, (P)laying, get ready to (r)ecord, (R)ecording
  //                         or an optional (.) to indicate an index detect
  if (cmd2("$QZ?")) {
    const char *pecStateStr = "IpPrR";
    uint8_t state = 0;
    #if AXIS1_PEC == ON
      state = pec.state;
      if (pecIndexSensedSinceLast) { reply[1] = '.'; pecIndexSensedSinceLast = false; }
    #endif
    reply[0] = pecStateStr[state]; reply[1] = 0; reply[2] = 0;
  } else

  // :VS#       Get PEC number of steps per sidereal second of worm rotation
  //            Returns: n.n#
  if (cmd("VS") || cmd2("GXE6")) {
    sprintF(reply, "%0.6f", stepsPerSiderealSecondAxis1);
    *numericReply = false;
  } else

  // :VW#       Get pec worm rotation steps
  //            Returns: n#
  if (cmd("VW")) {
    long steps = 0;
    #if AXIS1_PEC == ON
      steps = pec.wormRotationSteps;
    #endif
    sprintf(reply, "%06ld", steps);
    *numericReply = false;
  } else

  if (cmdGX("GXE")) {
    switch (parameter[1]) {
      // :GXE7#     Get pec worm rotation steps (from NV)
      //            Returns: n#
      case '7':
        Pec tempPec;
        nv.readBytes(NV_PEC_BASE, &tempPec, PecSize);
        sprintf(reply, "%ld", tempPec.wormRotationSteps);
        *numericReply = false;
      break;
      // :GXE8#     Get pec buffer size in seconds
      //            Returns: n#
      case '8': sprintf(reply,"%ld",lround(pecBufferSize)); *numericReply = false; break;
      default: return false;
    }
  } else

  #if AXIS1_PEC == ON

    // :GX91#     Get PEC analog value
    //            Returns: n#
    if (cmd2("GX91")) {
      sprintf(reply, "%d", pecAnalogValue);
      *numericReply = false;
    } else

    // :SXE7,[n]#
    //            Set PEC steps per worm rotation [n]
    //            Return: 0 on failure or 1 on success
    if (cmd2("SXE7")) {
      long l = strtol(&parameter[3], NULL, 10);
      if (AXIS1_PEC == OFF) l = 0;
      if (l >= 0 && l < 129600000) {
        Pec tempPec = pec;
        tempPec.worm.rotationSteps = l;
        nv.updateBytes(NV_PEC_BASE, &tempPec, PecSize);
      } else *commandError = CE_PARAM_RANGE;
    } else

    // V - PEC Readout
    // :VR[n]#    Read PEC table entry rate adjustment (in steps +/-) for worm segment n (in seconds)
    //            Returns: sn#
    // :VR#       Read PEC table entry rate adjustment (in steps +/-) for currently playing segment and its rate adjustment (in steps +/-)
    //            Returns: sn,n#
    if (cmdP("VR")) {
      int16_t i, j;
      bool conv_result = true;
      if (parameter[0] == 0) i = pecIndex; else conv_result = transform.site.convert.atoi2(parameter, &i);
      if (conv_result) {
        if (i >= 0 && i < pecBufferSize) {
          if (parameter[0] == 0) {
            i -= 1;
            if (i < 0) i += wormRotationSeconds;
            if (i >= wormRotationSeconds) i -= wormRotationSeconds;
            j = pecBuffer[i]-128; sprintf(reply,"%+04i,%03i", j, i);
          } else {
            j = pecBuffer[i]-128; sprintf(reply,"%+04i", j);
          }
        } else *commandError = CE_PARAM_RANGE;
      } else *commandError = CE_PARAM_FORM;
      *numericReply = false;
    } else
    // :Vr[n]#    Read out RA PEC ten byte frame in hex format starting at worm segment n (in seconds)
    //            Returns: x0x1x2x3x4x5x6x7x8x9# (hex one byte integers)
    //            Ten rate adjustment factors for 1s worm segments in steps +/- (steps = x0 - 128, etc.)
    if (cmdP("Vr")) {
      int16_t i, j;
      if (transform.site.convert.atoi2(parameter, &i)) {
        if (i >= 0 && i < pecBufferSize) {
          j = 0;
          uint8_t b;
          char s[3] = "  ";
          for (j = 0; j < 10; j++) {
            if (i + j < pecBufferSize) b = pecBuffer[i + j]; else b = 128;
            sprintf(s,"%02X",b);
            strcat(reply, s);
          }
        } else *commandError = CE_PARAM_RANGE;
      } else *commandError = CE_PARAM_FORM;
      *numericReply = false;
    } else

    //  :VH#      PEC index sense position in sidereal seconds
    //            Returns: n#
    if (command[0] == 'V' && command[1] == 'H' && parameter[0] == 0) {
      long s = lround(pec.worm.sensePositionSteps/stepsPerSiderealSecondAxis1);
      while (s > wormRotationSeconds) s -= wormRotationSeconds;
      while (s < 0) s += wormRotationSeconds;
      sprintf(reply,"%05ld",s);
      *numericReply = false;
    } else

    // :WR+#      Move PEC Table ahead by one sidereal second
    //            Return: 0 on failure
    //                    1 on success
    if (cmd1("WR+")) {
      int8_t i = pecBuffer[wormRotationSeconds - 1];
      memmove(&pecBuffer[1], &pecBuffer[0], wormRotationSeconds - 1);
      pecBuffer[0] = i;
    }

    // :WR-#      Move PEC Table back by one sidereal second
    //            Return: 0 on failure
    //                    1 on success
    if (cmd1("WR-")) { 
      int8_t i = pecBuffer[0];
      memmove(&pecBuffer[0], &pecBuffer[1], wormRotationSeconds - 1);
      pecBuffer[wormRotationSeconds - 1] = i;
    }

    // :WR[n,sn]# Write PEC table entry for worm segment [n] (in sidereal seconds)
    // where [sn] is the correction in steps +/- for this 1 second segment
    //            Returns: Nothing
    if (cmd("WR")) {
      char *parameter2 = strchr(parameter, ',');
      if (parameter2) {
        int16_t i, j;
        parameter2[0] = 0;
        parameter2++;
        if (transform.site.convert.atoi2(parameter, &i)) {
          if (i >= 0 && i < pecBufferSize) {
            if (transform.site.convert.atoi2(parameter2, &j)) {
              if (j >= -128 && j <= 127) {
                pecBuffer[i] = j;
                pec.recorded = true;
              } else *commandError = CE_PARAM_RANGE;
            } else *commandError = CE_PARAM_FORM;
          } else *commandError = CE_PARAM_RANGE;
        } else *commandError = CE_PARAM_FORM;
      } else *commandError = CE_PARAM_FORM;
      *numericReply = false;
    } else

    // $QZ - PEC Control
    if (cmdGX("$QZ")) {
      *numericReply = false;
      // :$QZ+      Enable RA PEC compensation 
      //            Returns: nothing
      if (parameter[1] == '+') {
        if (pec.recorded) pec.state = PEC_READY_PLAY;
        nv.updateBytes(NV_PEC_BASE, &pec, PecSize);
      } else
      // :$QZ-      Disable RA PEC Compensation
      //            Returns: nothing
      if (parameter[1] == '-') {
        pec.state = PEC_NONE; 
        nv.updateBytes(NV_PEC_BASE, &pec, PecSize);
      } else
      // :$QZ/      Ready Record PEC
      //            Returns: nothing
      if (parameter[1] == '/' && trackingState == TS_SIDEREAL) {
        pec.state = PEC_READY_RECORD;
        nv.updateBytes(NV_PEC_BASE, &pec, PecSize);
      } else
      // :$QZZ      Clear the PEC data buffer
      //            Return: Nothing
      if (parameter[1] == 'Z') { 
        for (int i = 0; i < pecBufferSize; i++) pecBuffer[i] = 128;
        pec.state = PEC_NONE;
        pec.recorded = false;
        nv.updateBytes(NV_PEC_BASE, &pec, PecSize);
      } else
      // :$QZ!      Write PEC data to NV
      //            Returns: nothing
      if (parameter[1] == '!') {
        pec.recorded = true;
        nv.updateBytes(NV_PEC_BASE, &pec, PecSize);
        for (int i = 0; i < pecBufferSize; i++) nv.update(NV_PEC_BUFFER_BASE + i, pecBuffer[i]);
      } else { *numericReply = true; *commandError = CE_CMD_UNKNOWN; }
    } else
  #endif
    return false;

  return true;
}

#endif