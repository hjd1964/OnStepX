//--------------------------------------------------------------------------------------------------
// OnStepX focuser control
#pragma once

#include "../../Common.h"

#ifdef FOCUSER_PRESENT

#include "../../lib/axis/Axis.h"
#include "../../libApp/commands/ProcessCmds.h"
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
  #if NV_ENDURANCE == NVE_VHIGH
    #define FOCUSER_WRITE_DELAY 5
  #elif NV_ENDURANCE == NVE_HIGH
    #define FOCUSER_WRITE_DELAY 60
  #else
    #define FOCUSER_WRITE_DELAY 300
  #endif
#endif

#pragma pack(1)
typedef struct Tcf {
  bool enabled;
  float coef;       // in um/°C
  int16_t deadband; // in steps
  float t0;         // in °C, temperature when first enabled
} Tcf;

#define FocuserSettingsSize 20
typedef struct FocuserSettings {
  Tcf tcf;
  ParkState parkState;
  int16_t backlash;  // in steps
  float position;    // in microns
  int16_t gotoRate;  // in microns/s
} FocuserSettings;
#pragma pack()

class Focuser {
  public:
    void init();
    void begin();

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // poll focusers to handle parking and TCF
    void monitor();

    // poll focuser buttons to start/stop movement
    #if FOCUSER_BUTTON_SENSE_IN != OFF && FOCUSER_BUTTON_SENSE_OUT != OFF
      void buttons();
    #endif

  private:

    // get focuser temperature in deg. C
    float getTemperature();

    // check for DC motor focuser
    bool isDC(int index);

    // get DC power in %
    int getDcPower(int index);

    // set DC power in %
    bool setDcPower(int index, int value);

    // get TCF enable
    bool getTcfEnable(int index);

    // set TCF enable
    CommandError setTcfEnable(int index, bool value);

    // get TCF coefficient, in microns per deg. C
    float getTcfCoef(int index);

    // set TCF coefficient, in microns per deg. C
    bool setTcfCoef(int index, float value);

    // get TCF deadband, in steps
    int getTcfDeadband(int index);

    // set TCF deadband, in steps
    bool setTcfDeadband(int index, int value);

    // get TCF T0, in deg. C
    float getTcfT0(int index);

    // set TCF T0, in deg. C
    bool setTcfT0(int index, float value);

    // get backlash in microns
    int getBacklash(int index);

    // set backlash in microns
    CommandError setBacklash(int index, int value);

    // set move rate
    void setMoveRate(int index, int value);

    // start move in the specified direction
    CommandError move(int index, Direction dir);

    // get goto rate, 1 for 0.5x base, 2 for 0.66x base, 3 for base, 4 for 1.5x base, 5 for 2x base
    int getGotoRate(int index);

    // set goto rate, 1 for 0.5x base, 2 for 0.66x base, 3 for base, 4 for 1.5x base, 5 for 2x base
    void setGotoRate(int index, int value);

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

    long target[FOCUSER_MAX]; // in steps

    unsigned long writeTime[FOCUSER_MAX];

    uint8_t parkHandle[FOCUSER_MAX];

    bool homing[FOCUSER_MAX];

    unsigned long secs = 0;

    // the default focuser is the first found
    int active = -1;

    #if FOCUSER_BUTTON_SENSE_IN != OFF && FOCUSER_BUTTON_SENSE_OUT != OFF
      uint8_t inButtonHandle = 0;
      uint8_t outButtonHandle = 0;
    #endif
};

#if AXIS4_DRIVER_MODEL != OFF
  #ifdef AXIS4_STEP_DIR_PRESENT
    extern StepDirMotor motor4;
  #elif defined(AXIS4_SERVO_PRESENT)
    extern ServoMotor motor4;
  #endif
  extern Axis axis4;
#endif

#if AXIS5_DRIVER_MODEL != OFF
  #ifdef AXIS5_STEP_DIR_PRESENT
    extern StepDirMotor motor5;
  #elif defined(AXIS5_SERVO_PRESENT)
    extern ServoMotor motor5;
  #endif
  extern Axis axis5;
#endif

#if AXIS6_DRIVER_MODEL != OFF
  #ifdef AXIS6_STEP_DIR_PRESENT
    extern StepDirMotor motor6;
  #elif defined(AXIS6_SERVO_PRESENT)
    extern ServoMotor motor6;
  #endif
  extern Axis axis6;
#endif

#if AXIS7_DRIVER_MODEL != OFF
  #ifdef AXIS7_STEP_DIR_PRESENT
    extern StepDirMotor motor7;
  #elif defined(AXIS7_SERVO_PRESENT)
    extern ServoMotor motor7;
  #endif
  extern Axis axis7;
#endif

#if AXIS8_DRIVER_MODEL != OFF
  #ifdef AXIS8_STEP_DIR_PRESENT
    extern StepDirMotor motor8;
  #elif defined(AXIS8_SERVO_PRESENT)
    extern ServoMotor motor8;
  #endif
  extern Axis axis8;
#endif

#if AXIS9_DRIVER_MODEL != OFF
  #ifdef AXIS9_STEP_DIR_PRESENT
    extern StepDirMotor motor9;
  #elif defined(AXIS9_SERVO_PRESENT)
    extern ServoMotor motor9;
  #endif
  extern Axis axis9;
#endif

extern Focuser focuser;

#endif
