//--------------------------------------------------------------------------------------------------
// OnStepX focuser control
#pragma once

#include "../../Common.h"

#ifdef FOCUSER_PRESENT

#include "../../commands/ProcessCmds.h"
#include "../axis/Axis.h"

#define FOCUSER_MAX 6

#pragma pack(1)
typedef struct Tcf {
  bool enabled;
  float coef;
  int16_t deadband; // in steps
  float t0;
} Tcf;

#define FocuserSettingsSize 18
typedef struct FocuserSettings {
  Tcf tcf;
  uint8_t dcPower;   // in %
  int16_t backlash;  // in steps
  float position;    // in microns
} FocuserSettings;
#pragma pack()

class Focuser {
  public:
    void init();

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // get focuser temperature in deg. C
    float getTemperature();

    // check for DC motor focuser
    bool  isDC(int index);

    // get DC power in %
    int   getDcPower(int index);

    // set DC power in %
    bool  setDcPower(int index, int value);

    // get TCF enable
    bool  getTcfEnable(int index);

    // set TCF enable
    bool  setTcfEnable(int index, bool value);

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
    bool setBacklash(int index, int value);

    // move focuser to a specific location
    CommandError gotoTarget(int index, long target);

    // park focuser at its current position
    void park(int index);

    // unpark focuser
    void unpark(int index);

    // poll TCF to move the focusers as required
    void poll();

    Axis *axis[6];

  private:
    void readSettings(int index);
    void writeSettings(int index);

    int driverModel[FOCUSER_MAX]      = { AXIS4_DRIVER_MODEL, AXIS5_DRIVER_MODEL, AXIS6_DRIVER_MODEL, AXIS7_DRIVER_MODEL, AXIS8_DRIVER_MODEL, AXIS9_DRIVER_MODEL };
    int slewRateDesired[FOCUSER_MAX]  = { AXIS4_SLEW_RATE_DESIRED, AXIS5_SLEW_RATE_DESIRED, AXIS6_SLEW_RATE_DESIRED, AXIS7_SLEW_RATE_DESIRED, AXIS8_SLEW_RATE_DESIRED, AXIS9_SLEW_RATE_DESIRED };
    int slewRateMinimum[FOCUSER_MAX]  = { AXIS4_SLEW_RATE_MINIMUM, AXIS5_SLEW_RATE_MINIMUM, AXIS6_SLEW_RATE_MINIMUM, AXIS7_SLEW_RATE_MINIMUM, AXIS8_SLEW_RATE_MINIMUM, AXIS9_SLEW_RATE_MINIMUM };
    int accelerationTime[FOCUSER_MAX] = { AXIS4_ACCELERATION_TIME, AXIS5_ACCELERATION_TIME, AXIS6_ACCELERATION_TIME, AXIS7_ACCELERATION_TIME, AXIS8_ACCELERATION_TIME, AXIS9_ACCELERATION_TIME };
    int rapidStopTime[FOCUSER_MAX]    = { AXIS4_RAPID_STOP_TIME, AXIS5_RAPID_STOP_TIME, AXIS6_RAPID_STOP_TIME, AXIS7_RAPID_STOP_TIME, AXIS8_RAPID_STOP_TIME, AXIS9_RAPID_STOP_TIME };
    bool powerDown[FOCUSER_MAX]       = { AXIS4_POWER_DOWN == ON, AXIS5_POWER_DOWN == ON, AXIS6_POWER_DOWN == ON, AXIS7_POWER_DOWN == ON, AXIS8_POWER_DOWN == ON, AXIS9_POWER_DOWN == ON };

    int moveRate[FOCUSER_MAX];
    long tcfSteps[FOCUSER_MAX];

    FocuserSettings settings[FOCUSER_MAX];

    long target[FOCUSER_MAX] = { 0, 0, 0, 0, 0, 0 };
    unsigned long afterSlewWait[FOCUSER_MAX] = { 0, 0, 0, 0, 0, 0 };
};

extern Focuser focuser;

#endif
