//--------------------------------------------------------------------------------------------------
// OnStepX focuser control
#pragma once

#include "../../Common.h"

#if AXIS4_DRIVER_MODEL != OFF || AXIS5_DRIVER_MODEL != OFF || AXIS6_DRIVER_MODEL != OFF || AXIS7_DRIVER_MODEL != OFF || AXIS8_DRIVER_MODEL != OFF || AXIS9_DRIVER_MODEL != OFF

#include "../../commands/ProcessCmds.h"
#include "../motion/StepDrivers.h"
#include "../motion/Axis.h"

#define FOCUSER_PRESENT
#define FOCUSER_MAX 6

#pragma pack(1)
typedef struct Tcf {
  bool enabled;
  float coef;
  int16_t deadband;
  float t0;
} Tcf;

#define FocuserSettingsSize 14
typedef struct Settings {
  Tcf tcf;
  uint8_t dcPower;
  int16_t backlash;
} Settings;
#pragma pack()

class Focuser {
  public:
    Focuser();

    // initialize all focusers
    void init(bool validKey);

    // process focuser commands
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
    // get TCF deadband, in microns
    int   getTcfDeadband(int index);
    // set TCF deadband, in microns
    bool  setTcfDeadband(int index, int value);
    // get TCF T0, in deg. C
    float getTcfT0(int index);
    // set TCF T0, in deg. C
    bool  setTcfT0(int index, float value);
    // poll TCF to move the focusers as required
    void tcfPoll();

    // get backlash in microns
    int   getBacklash(int index);
    // set backlash in microns
    bool  setBacklash(int index, int value);

    Axis *axis[6];

  private:
    void readSettings(int index);
    void writeSettings(int index);

    int driverModel[FOCUSER_MAX]      = { AXIS4_DRIVER_MODEL, AXIS5_DRIVER_MODEL, AXIS6_DRIVER_MODEL, AXIS7_DRIVER_MODEL, AXIS8_DRIVER_MODEL, AXIS9_DRIVER_MODEL };
    int slewRateDesired[FOCUSER_MAX]  = { AXIS4_SLEW_RATE_DESIRED, AXIS5_SLEW_RATE_DESIRED, AXIS6_SLEW_RATE_DESIRED, AXIS7_SLEW_RATE_DESIRED, AXIS8_SLEW_RATE_DESIRED, AXIS9_SLEW_RATE_DESIRED };
    int accelerationRate[FOCUSER_MAX] = { AXIS4_ACCELERATION_RATE, AXIS5_ACCELERATION_RATE, AXIS6_ACCELERATION_RATE, AXIS7_ACCELERATION_RATE, AXIS8_ACCELERATION_RATE, AXIS9_ACCELERATION_RATE };
    int rapidStopRate[FOCUSER_MAX]    = { AXIS4_RAPID_STOP_RATE, AXIS5_RAPID_STOP_RATE, AXIS6_RAPID_STOP_RATE, AXIS7_RAPID_STOP_RATE, AXIS8_RAPID_STOP_RATE, AXIS9_RAPID_STOP_RATE };
    int moveRate[FOCUSER_MAX];

    float tcfSteps[FOCUSER_MAX];

    Settings settings[FOCUSER_MAX];
};

#endif
