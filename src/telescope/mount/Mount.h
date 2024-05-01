//--------------------------------------------------------------------------------------------------
// telescope mount control
#pragma once

#include "../../Common.h"

#ifdef MOUNT_PRESENT

#include "../../lib/axis/Axis.h"
#include "../../libApp/commands/ProcessCmds.h"
#include "coordinates/Transform.h"
#include "home/Home.h"

enum RateCompensation: uint8_t {RC_NONE, RC_REFRACTION, RC_REFRACTION_DUAL, RC_MODEL, RC_MODEL_DUAL};

#if TRACK_COMPENSATION_DEFAULT == OFF
  #define RC_DEFAULT RC_NONE
#elif TRACK_COMPENSATION_DEFAULT == REFRACTION
  #define RC_DEFAULT RC_REFRACTION
#elif TRACK_COMPENSATION_DEFAULT == REFRACTION_DUAL
  #define RC_DEFAULT RC_REFRACTION_DUAL
#elif TRACK_COMPENSATION_DEFAULT == MODEL
  #define RC_DEFAULT RC_FULL
#elif TRACK_COMPENSATION_DEFAULT == MODEL_DUAL
  #define RC_DEFAULT RC_MODEL_DUAL
#endif

enum TrackingState: uint8_t    {TS_NONE, TS_SIDEREAL};

#pragma pack(1)
#define MountSettingsSize 9
typedef struct Backlash {
  float axis1;
  float axis2;
} Backlash;
typedef struct MountSettings {
  RateCompensation rc;
  Backlash backlash;
} MountSettings;
#pragma pack()

extern Axis axis1;
extern Axis axis2;

class Mount {
  public:
    void init();
    void begin();

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // get current equatorial position (Native coordinate system)
    Coordinate getPosition(CoordReturn coordReturn = CR_MOUNT_EQU);

    // get current equatorial position (Mount coordinate system)
    Coordinate getMountPosition(CoordReturn coordReturn = CR_MOUNT_EQU);

    // returns true if either of the mount motor drivers report a fault
    inline bool motorFault() { return axis1.motorFault() || axis2.motorFault(); }

    // returns true if the mount is at the home (startup) position
    inline bool isHome() {
      return abs(axis1.getInstrumentCoordinate() - home.getPosition(CR_MOUNT).a1) <= arcsecToRad(AXIS1_HOME_TOLERANCE) &&
             abs(axis2.getInstrumentCoordinate() - home.getPosition(CR_MOUNT).a2) <= arcsecToRad(AXIS2_HOME_TOLERANCE);
    }

    // returns true if the mount is slewing (doing a goto or guide > 2X)
    inline bool isSlewing() { return axis1.isSlewing() || axis2.isSlewing(); }

    // one time initialization of tracking
    void autostart();
    void autostartPostponed();

    // enables or disables tracking, enabling tracking powers on the motors if necessary
    void tracking(bool state);

    // returns true if the mount is tracking
    inline bool isTracking() { return trackingState == TS_SIDEREAL; }

    // enables or disables power to the mount motors
    // first enable starts the mount status indications
    void enable(bool state);

    // returns true if the mount motors are powered on
    inline bool isEnabled() { return axis1.isEnabled() && axis2.isEnabled(); }

    // true if syncing only from OnStep to the Encoders
    bool syncFromOnStepToEncoders = false;

    // updates the tracking rates, etc. as appropriate for the mount state
    // called once a second by poll() but available here for immediate action
    void update();

    void poll();

    float trackingRate = 1.0F;            // in sidereal units 1x = 15 arc-seconds/sidereal second
    float trackingRateAxis1 = 0.0F;       // in sidereal units 1x = 15 arc-seconds/sidereal second
    float trackingRateAxis2 = 0.0F;       // in sidereal units 1x = 15 arc-seconds/sidereal second
    float trackingRateOffsetRA = 0.0F;    // in sidereal units 1x = 15 arc-seconds/sidereal second
    float trackingRateOffsetDec = 0.0F;   // in sidereal units 1x = 15 arc-seconds/sidereal second

    MountSettings settings = {RC_DEFAULT, { 0, 0 }};

  private:
    // alternate tracking rate calculation method
    float ztr(float a);

    // update where we are pointing *now*
    void updatePosition(CoordReturn coordReturn);

    // current position in Mount coordinates (Observed Place with no corrections except index offset)
    // coordinates are either Horizon (a, z) or Equatorial (h, d) depending on the mountType
    // also includes Mount normalized axis coordinates (a1, a2) where a2 is an instrument coordinate in tangent arm mode
    Coordinate current;

    TrackingState trackingState = TS_NONE;
};

#ifdef AXIS1_STEP_DIR_PRESENT
  extern StepDirMotor motor1;
#elif defined(AXIS1_SERVO_PRESENT)
  extern ServoMotor motor1;
#elif defined(AXIS1_ODRIVE_PRESENT)
  extern ODriveMotor motor1;
#endif
extern Axis axis1;

#ifdef AXIS2_STEP_DIR_PRESENT
  extern StepDirMotor motor2;
#elif defined(AXIS2_SERVO_PRESENT)
  extern ServoMotor motor2;
#elif defined(AXIS2_ODRIVE_PRESENT)
  extern ODriveMotor motor2;
#endif
extern Axis axis2;

extern Mount mount;

#endif
