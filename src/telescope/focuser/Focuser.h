//--------------------------------------------------------------------------------------------------
// OnStepX focuser control
#pragma once

#include "../../Common.h"

#ifdef FOCUSER_PRESENT

#include "../../commands/ProcessCmds.h"
#include "../Telescope.h"

#if AXIS9_DRIVER_MODEL != OFF
  #define FOCUSER_MAX 6
#else
  #if AXIS8_DRIVER_MODEL != OFF
    #define FOCUSER_MAX 5
  #else
    #if AXIS7_DRIVER_MODEL != OFF
      #define FOCUSER_MAX 4
    #else
      #if AXIS6_DRIVER_MODEL != OFF
        #define FOCUSER_MAX 3
      #else
        #if AXIS5_DRIVER_MODEL != OFF
          #define FOCUSER_MAX 2
        #else
          #if AXIS4_DRIVER_MODEL != OFF
            #define FOCUSER_MAX 1
          #else
            #define FOCUSER_MAX 0
          #endif
        #endif
      #endif
    #endif
  #endif
#endif

// time to write position to nv after last movement of Focuser
#ifndef FOCUSER_WRITE_DELAY
  #if NV_ENDURANCE == VHIGH
    #define FOCUSER_WRITE_DELAY 5000L
  #elif NV_ENDURANCE == HIGH
    #define FOCUSER_WRITE_DELAY 60000L
  #else
    #define FOCUSER_WRITE_DELAY 300000L
  #endif
#endif

#pragma pack(1)
typedef struct Tcf {
  bool enabled;
  float coef;       // in um/°C
  int16_t deadband; // in steps
  float t0;         // in °C, temperature when first enabled
} Tcf;

#define FocuserSettingsSize 18
typedef struct FocuserSettings {
  Tcf tcf;
  ParkState parkState;
  int16_t backlash;  // in steps
  float position;    // in microns
} FocuserSettings;
#pragma pack()

class Focuser {
  public:
    void init();

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // poll focusers as required
    void monitor();

    // poll for park completion
    void parkMonitor(int index);
  
  private:

    // get focuser temperature in deg. C
    float getTemperature();

    // check for DC motor focuser
    bool  isDC(int index);

    // get DC power in %
    int getDcPower(int index);

    // set DC power in %
    bool setDcPower(int index, int value);

    // get TCF enable
    bool  getTcfEnable(int index);

    // set TCF enable
    CommandError setTcfEnable(int index, bool value);

    // get TCF coefficient, in microns per deg. C
    float getTcfCoef(int index);

    // set TCF coefficient, in microns per deg. C
    bool  setTcfCoef(int index, float value);

    // get TCF deadband, in steps
    int   getTcfDeadband(int index);

    // set TCF deadband, in steps
    bool  setTcfDeadband(int index, int value);

    // get TCF T0, in deg. C
    float getTcfT0(int index);

    // set TCF T0, in deg. C
    bool  setTcfT0(int index, float value);

    // get backlash in microns
    int  getBacklash(int index);

    // set backlash in microns
    CommandError setBacklash(int index, int value);

    // start slew in the specified direction
    CommandError slew(int index, Direction dir);

    // move focuser to a specific location (in steps)
    CommandError gotoTarget(int index, long target);

    // park focuser at its current location
    CommandError park(int index);

    // unpark focuser
    CommandError unpark(int index);

    void readSettings(int index);
    void writeSettings(int index);

    int moveRate[FOCUSER_MAX];
    long tcfSteps[FOCUSER_MAX];

    FocuserSettings settings[FOCUSER_MAX];

    long target[FOCUSER_MAX];

    bool wasSlewing[FOCUSER_MAX];
    unsigned long lastSlewTime[FOCUSER_MAX];

    uint8_t parkHandle[FOCUSER_MAX];
};

extern Focuser focuser;

#endif
