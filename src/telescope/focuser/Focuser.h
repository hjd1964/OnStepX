//--------------------------------------------------------------------------------------------------
// OnStepX focuser control
#pragma once

#include "../../common.h"
#include "../../commands/ProcessCmds.h"
#include "../motion/StepDrivers.h"
#include "../motion/Axis.h"

#if AXIS4_DRIVER_MODEL != OFF || AXIS5_DRIVER_MODEL != OFF || AXIS6_DRIVER_MODEL != OFF || AXIS7_DRIVER_MODEL != OFF || AXIS8_DRIVER_MODEL != OFF || AXIS9_DRIVER_MODEL != OFF

#pragma pack(1)
typedef struct Tcf {
  bool enabled;
  float coef;
  long deadband;
  float t0;
} Tcf;

#define FocuserSettingsSize 18
typedef struct Settings {
  Tcf tcf;
  uint8_t dcPower;
  float backlash;
} Settings;
#pragma pack()

class Focuser {
  public:
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

    // get backlash in microns
    int   getBacklash(int index);
    // set backlash in microns
    bool  setBacklash(int index, int value);

    #if AXIS4_DRIVER_MODEL != OFF
      Axis axis4;
    #endif
    #if AXIS5_DRIVER_MODEL != OFF
      Axis axis5;
    #endif
    #if AXIS6_DRIVER_MODEL != OFF
      Axis axis6;
    #endif
    #if AXIS7_DRIVER_MODEL != OFF
      Axis axis7;
    #endif
    #if AXIS8_DRIVER_MODEL != OFF
      Axis axis8;
    #endif
    #if AXIS9_DRIVER_MODEL != OFF
      Axis axis9;
    #endif

    Axis *focuserAxis[6] = { NULL, NULL, NULL, NULL, NULL, NULL };

  private:
    void readSettings(int index);
    void writeSettings(int index);

    int slewRateDesired[6]  = { AXIS4_SLEW_RATE_DESIRED, AXIS5_SLEW_RATE_DESIRED, AXIS6_SLEW_RATE_DESIRED, AXIS7_SLEW_RATE_DESIRED, AXIS8_SLEW_RATE_DESIRED, AXIS9_SLEW_RATE_DESIRED };
    int accelerationRate[6] = { AXIS4_ACCELERATION_RATE, AXIS5_ACCELERATION_RATE, AXIS6_ACCELERATION_RATE, AXIS7_ACCELERATION_RATE, AXIS8_ACCELERATION_RATE, AXIS9_ACCELERATION_RATE };
    int rapidStopRate[6]    = { AXIS4_RAPID_STOP_RATE, AXIS5_RAPID_STOP_RATE, AXIS6_RAPID_STOP_RATE, AXIS7_RAPID_STOP_RATE, AXIS8_RAPID_STOP_RATE, AXIS9_RAPID_STOP_RATE };
    int moveRate[6] = { 100, 100, 100, 100, 100, 100 };
    bool dcMode[6] = { AXIS4_DRIVER_DC_MODE == ON, AXIS5_DRIVER_DC_MODE == ON, AXIS6_DRIVER_DC_MODE == ON, AXIS7_DRIVER_DC_MODE == ON, AXIS8_DRIVER_DC_MODE == ON, AXIS9_DRIVER_DC_MODE == ON };

    Settings settings[6] = {
      { {false, 0.0, 1, 10.0}, 50, 0.0 },
      { {false, 0.0, 1, 10.0}, 50, 0.0 },
      { {false, 0.0, 1, 10.0}, 50, 0.0 },
      { {false, 0.0, 1, 10.0}, 50, 0.0 },
      { {false, 0.0, 1, 10.0}, 50, 0.0 },
      { {false, 0.0, 1, 10.0}, 50, 0.0 }
    };
};

#endif
