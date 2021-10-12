//--------------------------------------------------------------------------------------------------
// telescope mount control
#pragma once

#include "../../Common.h"

#ifdef MOUNT_PRESENT

#include "../../lib/axis/Axis.h"
#include "../../lib/commands/ProcessCmds.h"
#include "coordinates/Transform.h"

enum RateCompensation: uint8_t {RC_NONE, RC_REFR_RA, RC_REFR_BOTH, RC_FULL_RA, RC_FULL_BOTH};
#if TRACK_REFRACTION_RATE_DEFAULT == ON
  #define RC_DEFAULT RC_REFR_RA
#else
  #define RC_DEFAULT RC_NONE
#endif

enum TrackingState: uint8_t    {TS_NONE, TS_SIDEREAL};
enum CoordReturn: uint8_t      {CR_MOUNT, CR_MOUNT_EQU, CR_MOUNT_ALT, CR_MOUNT_HOR, CR_MOUNT_ALL};

#pragma pack(1)
#define MountSettingsSize 10
typedef struct Backlash {
  float axis1;
  float axis2;
} Backlash;
typedef struct MountSettings {
  RateCompensation rc;
  bool syncToEncoders;
  Backlash backlash;
} MountSettings;
#pragma pack()

extern Axis axis1;
extern Axis axis2;

class Mount {
  public:
    void init();

    bool command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError);

    // get current equatorial position (Native coordinate system)
    Coordinate getPosition(CoordReturn coordReturn = CR_MOUNT_EQU);

    // get current equatorial position (Mount coordinate system)
    Coordinate getMountPosition(CoordReturn coordReturn = CR_MOUNT_EQU);

    // returns true if either of the mount motor drivers reported a fault
    inline bool isFault() { return axis1.fault() || axis2.fault(); }

    // returns true if the mount is at the home (startup) position
    inline void setHome(bool state) { atHome = state; };

    // returns true if the mount is at the home (startup) position
    inline bool isHome() { return atHome; }

    // returns true if the mount is slewing (doing a goto or guide > 2X)
    inline bool isSlewing() { return axis1.isSlewing() || axis2.isSlewing(); }

    // enables or disables tracking, enabling tracking powers on the motors if necessary
    void tracking(bool state);

    // returns true if the mount is tracking
    inline bool isTracking() { return trackingState == TS_SIDEREAL; }

    // enables or disables power to the mount motors
    // first enable starts the mount status indications
    void enable(bool state);

    // returns true if the mount motors are powered on
    inline bool isEnabled() { return axis1.isEnabled() || axis2.isEnabled(); }

    // allow syncing to the encoders instead of from them
    void syncToEncoders(bool state);

    // returns true if syncing only from OnStep to the Encoders
    inline bool isSyncToEncoders() { return settings.syncToEncoders; }

    // updates the tracking rates, etc. as appropriate for the mount state
    // called once a second by poll() but available here for immediate action
    void update();

    void poll();

    float trackingRate = 1.0F;
    MountSettings settings = {RC_DEFAULT, false, { 0, 0 }};

  private:
    // alternate tracking rate calculation method
    float ztr(float a);

    // update where we are pointing *now*
    // CR_MOUNT for Horizon or Equatorial mount coordinates, depending on mount
    // CR_MOUNT_EQU for Equatorial mount coordinates, depending on mode
    // CR_MOUNT_ALT for altitude (a) and Horizon or Equatorial mount coordinates, depending on mode
    // CR_MOUNT_HOR for Horizon mount coordinates, depending on mode
    void updatePosition(CoordReturn coordReturn);

    // current position in Mount coordinates (Observed Place with no corrections except index offset)
    // coordinates are either Horizon (a, z) or Equatorial (h, d) depending on the mountType
    // also includes Mount normalized axis coordinates (a1, a2) where a2 is an instrument coordinate in tangent arm mode
    Coordinate current;

    TrackingState trackingState = TS_NONE;
    float trackingRateAxis1     = 0.0F;
    float trackingRateAxis2     = 0.0F;

    bool atHome = true;
};

#ifdef AXIS1_DRIVER_SD
  extern StepDirMotor motor1;
#elif defined(AXIS1_SERVO)
  extern ServoMotor motor1;
#endif
extern IRAM_ATTR void moveAxis1();
extern IRAM_ATTR void moveFFAxis1();
extern IRAM_ATTR void moveFRAxis1();
extern IRAM_ATTR void pollAxis1();
extern Axis axis1;

#ifdef AXIS2_DRIVER_SD
  extern StepDirMotor motor2;
#elif defined(AXIS2_SERVO)
  extern ServoMotor motor2;
#endif
extern IRAM_ATTR void moveAxis2();
extern IRAM_ATTR void moveFFAxis2();
extern IRAM_ATTR void moveFRAxis2();
extern IRAM_ATTR void pollAxis2();
extern Axis axis2;

extern Mount mount;

#endif
