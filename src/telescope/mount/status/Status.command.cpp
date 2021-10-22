// ---------------------------------------------------------------------------------------------------
// Mount status LED and buzzer

#include "Status.h"

#if defined(MOUNT_PRESENT) && SLEW_GOTO == ON

#include "../Mount.h"
#include "../coordinates/Transform.h"
#include "../../../lib/tls/PPS.h"
#include "../goto/Goto.h"
#include "../guide/Guide.h"
#include "../home/Home.h"
#include "../park/Park.h"
#include "../limits/Limits.h"
#include "../status/Status.h"

bool Status::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  UNUSED(supressFrame);

  if (command[0] == 'G') {

    // :GU#       Get telescope Status
    //            Returns: s#
    if (command[1] == 'U' && parameter[0] == 0)  {
      int i = 0;
      if (!mount.isTracking())                 reply[i++]='n';                     // [n]ot tracking
      if (goTo.state == GS_NONE)               reply[i++]='N';                     // [N]o goto
      if (park.state == PS_UNPARKED)           reply[i++]='p'; else                // not [p]arked
      if (park.state == PS_PARKING)            reply[i++]='I'; else                // parking [I]n-progress
      if (park.state == PS_PARKED)             reply[i++]='P'; else                // [P]arked
      if (park.state == PS_PARK_FAILED)        reply[i++]='F';                     // park [F]ailed
      if (mount.isSyncToEncoders())            reply[i++]='e';                     // sync to [e]ncoders only
      if (mount.isHome())                      reply[i++]='H';                     // at [H]ome
      #if TIME_LOCATION_PPS_SENSE == ON
        if (pps.synced)                        reply[i++]='S';                     // PPS [S]ync
      #endif
      if (guide.state == GU_PULSE_GUIDE)       reply[i++]='G';                     // pulse [G]uide active
      if (guide.state != GU_NONE)              reply[i++]='g';                     // [g]uide active

      if (mount.settings.rc == RC_REFR_RA)    { reply[i++]='r'; reply[i++]='s'; }  // [r]efr enabled [s]ingle axis
      if (mount.settings.rc == RC_REFR_BOTH)  { reply[i++]='r'; }                  // [r]efr enabled
      if (mount.settings.rc == RC_FULL_RA)    { reply[i++]='t'; reply[i++]='s'; }  // on[t]rack enabled [s]ingle axis
      if (mount.settings.rc == RC_FULL_BOTH)  { reply[i++]='t'; }                  // on[t]rack enabled

      if (goTo.isHomePaused())                 reply[i++]='w';                     // [w]aiting at home 
      if (goTo.isHomePauseEnabled())           reply[i++]='u';                     // pa[u]se at home enabled?
      if (sound.enabled)                       reply[i++]='z';                     // bu[z]zer enabled?
      if (goTo.isAutoFlipEnabled())            reply[i++]='a';                     // [a]uto meridian flip
      #if AXIS1_PEC == ON
        if (pec.recorded)                      reply[i++]='R';                     // PEC data has been [R]ecorded
        if (transform.mountType != ALTAZM)     reply[i++]="/,~;^"[(int)pec.state]; // PEC State (/)gnore, ready (,)lay, (~)laying, ready (;)ecord, (^)ecording
      #endif
      if (transform.mountType == GEM)          reply[i++]='E'; else                // GEM
      if (transform.mountType == FORK)         reply[i++]='K'; else                // FORK
      if (transform.mountType == ALTAZM)       reply[i++]='A';                     // ALTAZM

      Coordinate current = mount.getMountPosition(CR_MOUNT);
      if (current.pierSide == PIER_SIDE_NONE)  reply[i++]='o'; else                // pier side n[o]ne
      if (current.pierSide == PIER_SIDE_EAST)  reply[i++]='T'; else                // pier side eas[T]
      if (current.pierSide == PIER_SIDE_WEST)  reply[i++]='W';                     // pier side [W]est

      reply[i++]='0' + guide.settings.pulseRateSelect;                             // provide pulse-guide rate
      reply[i++]='0' + guide.settings.axis1RateSelect;                             // provide guide rate

      reply[i++]='0' + limits.errorCode();                                          // provide general error code
      reply[i++]=0;

      *numericReply = false;
    } else

    // :Gu#       Get bit packed telescope status
    //            Returns: s#
    if (command[1] == 'u' && parameter[0] == 0)  {
      memset(reply, (char)0b10000000, 9);
      if (!mount.isTracking())                     reply[0]|=0b10000001;           // Not tracking
      if (goTo.state == GS_NONE)                   reply[0]|=0b10000010;           // No goto
      #if TIME_LOCATION_PPS_SENSE == ON
        if (pps.synced)                            reply[0]|=0b10000100;           // PPS sync
      #endif
      if (guide.state == GU_PULSE_GUIDE)           reply[0]|=0b10001000;           // pulse guide active

      if (mount.settings.rc == RC_REFR_RA)         reply[0]|=0b11010000;           // Refr enabled Single axis
      if (mount.settings.rc == RC_REFR_BOTH)       reply[0]|=0b10010000;           // Refr enabled
      if (mount.settings.rc == RC_FULL_RA)         reply[0]|=0b11100000;           // OnTrack enabled Single axis
      if (mount.settings.rc == RC_FULL_BOTH)       reply[0]|=0b10100000;           // OnTrack enabled
      if (mount.settings.rc == RC_NONE) {
        double r = siderealToHz(mount.trackingRate);
        if (fequal(r, 57.900))                     reply[1]|=0b10000001; else      // Lunar rate selected
        if (fequal(r, 60.000))                     reply[1]|=0b10000010; else      // Solar rate selected
        if (fequal(r, 60.136))                     reply[1]|=0b10000011;           // King rate selected
      }

      if (mount.isSyncToEncoders())                reply[1]|=0b10000100;           // sync to encoders only
      if (guide.state != GU_NONE)                  reply[1]|=0b10001000;           // guide active
      if (mount.isHome())                          reply[2]|=0b10000001;           // At home
      if (goTo.isHomePaused())                     reply[2]|=0b10000010;           // Waiting at home
      if (goTo.isHomePauseEnabled())               reply[2]|=0b10000100;           // Pause at home enabled?
      if (sound.enabled)                           reply[2]|=0b10001000;           // Buzzer enabled?
      if (goTo.isAutoFlipEnabled())                reply[2]|=0b10010000;           // Auto meridian flip

      if (transform.mountType == GEM)              reply[3]|=0b10000001; else      // GEM
      if (transform.mountType == FORK)             reply[3]|=0b10000010; else      // FORK
      if (transform.mountType == ALTAZM)           reply[3]|=0b10001000;           // ALTAZM

      Coordinate current = mount.getMountPosition(CR_MOUNT);
      if (current.pierSide == PIER_SIDE_NONE)      reply[3]|=0b10010000; else      // Pier side none
      if (current.pierSide == PIER_SIDE_EAST)      reply[3]|=0b10100000; else      // Pier side east
      if (current.pierSide == PIER_SIDE_WEST)      reply[3]|=0b11000000;           // Pier side west

      #if AXIS1_PEC == ON
        if (transform.mountType != ALTAZM)
          reply[4] = (int)pec.state|0b10000000;                                    // PEC state: 0 ignore, 1 ready play, 2 playing, 3 ready record, 4 recording
        if (pec.recorded)                          reply[4]|=0b11000000;           // PEC state: data has been recorded
      #endif
      reply[5] = (int)park.state|0b10000000;                                       // Park state: 0 not parked, 1 parking in-progress, 2 parked, 3 park failed
      reply[6] = (int)guide.settings.pulseRateSelect|0b10000000;                   // Pulse-guide selection
      reply[7] = (int)guide.settings.axis1RateSelect|0b10000000;                   // Guide selection
      reply[8] = limits.errorCode()|0b10000000;                                    // General error
      reply[9] = 0;
      *numericReply = false;
    } else return false;
  } else

  // :SX97,[n]#     Set buzzer state
  //                Return: see below
  if (command[0] == 'S' && command[1] == 'X' && parameter[0] == '9' && parameter[1] == '7'  && parameter[2] == ','  && parameter[4] == 0) {
    if (parameter[3] == '0' || parameter[3] == '1') {
      buzzer = parameter[3] - '0';
      #if STATUS_BUZZER_MEMORY == ON
        nv.write(NV_MOUNT_STATUS_BASE, (uint8_t)buzzer);
      #endif
      sound.enabled = buzzer;
    } else *commandError = CE_PARAM_RANGE;
  } else return false;

  return true;
}

#endif
