/* ---------------------------------------------------------------------------------------------------------------------------------
 * Configuration for OnStepX auxiliary mount options
 *
 *          For more information on setting OnStep up see http://www.stellarjourney.com/index.php?r=site/equipment_onstep 
 *                      and join the OnStep Groups.io at https://groups.io/g/onstep
 * 
 *           *** Read the compiler warnings and errors, they are there to help guard against invalid configurations ***
 *
 * ---------------------------------------------------------------------------------------------------------------------------------
 * ADJUST THE FOLLOWING TO CONFIGURE YOUR CONTROLLER FEATURES ----------------------------------------------------------------------
 * <-Req'd = always must set, <-Often = usually must set, Option = optional, Adjust = adjust as req'd, Infreq = infrequently changed
*/
//      Parameter Name              Value   Default  Notes                                                                      Hint

// Stepper driver models (also see ~/OnStepX/src/motion/StepDrivers.h for additional infrequently used models and more info.): 
// A4988, DRV8825, LV8729, S109, TMC2130*, and TMC5160**
// * = SSS TMC2130 if you choose to set stepper driver current (in mA) set Vref pot. 2.5V instead of by motor current as usual.
// ** = SSS TMC5160 you must set stepper driver current (in mA) w/ AXISn_DRIVER_IRUN (IHOLD & IGOTO are optional)

// Settings for driver Microsteps, IRUN, Reverse, Limit Min, and Limit Max are stored in NV (EEPROM.) These runtime settings
// can be changed (or reverted to the defaults below) from the SmartWebServer's Config webpage.  If runtime axis settings are 
// enabled changes to these settings below may be ignored as runtime settings from NV (EEPROM) are used instead.

// AXIS1 RA/AZM -------------------------------- see https://onstep.groups.io/g/main/wiki/Configuration---Rotator-and-Focusers#AXIS1
#define AXIS1_DRIVER_MODEL            OFF //    OFF, (See above.) Stepper driver model.                                      <-Often

#define AXIS1_DRIVER_DECAY            OFF //    OFF, Tracking decay mode default override. TMC default is STEALTHCHOP.        Infreq
#define AXIS1_DRIVER_DECAY_GOTO       OFF //    OFF, Decay mode goto default override. TMC default is SPREADCYCLE.            Infreq
#define AXIS1_DRIVER_MICROSTEPS       OFF //    OFF, n. Microstep mode when tracking.                                        <-Often
#define AXIS1_DRIVER_MICROSTEPS_GOTO SAME //   SAME, n. Microstep mode used during gotos. SAME uses MICROSTEPS.               Option
#define AXIS1_DRIVER_POWER_DOWN       OFF //    OFF, ON Powers off 30sec after movement stops or 10min after last<=1x guide.  Option
#define AXIS1_DRIVER_IHOLD           HALF //   HALF, n, (mA.) Current during standstill. HALF uses IRUN/2.0                   Option
#define AXIS1_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current during tracking, appropriate for stepper/driver/etc.    Option
#define AXIS1_DRIVER_IGOTO           SAME //   SAME, n, (mA.) Current during slews. SAME uses IRUN.                           Option
#define AXIS1_DRIVER_REVERSE          OFF //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.   <-Often
#define AXIS1_DRIVER_STATUS           OFF //    OFF, ON, HIGH, or LOW.  Polling for driver status info/fault detection.       Option

#define AXIS1_LIMIT_MIN              -180 //   -180, n. Where n= -90..-270 (degrees.) Minimum "Hour Angle" for Eq modes.      Adjust
                                          //         n. Where n=-180..-360 (degrees.) Minimum Azimuth for AltAzm mode.
#define AXIS1_LIMIT_MAX               180 //    180, n. Where n=  90.. 270 (degrees.) Maximum "Hour Angle" for Eq modes.      Adjust
                                          //         n. Where n= 180.. 360 (degrees.) Maximum Azimuth for AltAzm mode.

#define AXIS1_SENSE_HOME              OFF //    OFF, HIGH or LOW enables & state clockwise home position, as seen from front. Option
#define AXIS1_SENSE_LIMIT_MIN         OFF //    OFF, HIGH or LOW state on limit sense switch stops movement.                  Option
#define AXIS1_SENSE_LIMIT_MAX         OFF //    OFF, HIGH or LOW state on limit sense switch stops movement.                  Option
                                          //         Digital, optionally add: |HYST(n) Where n=0..1023 stability time in ms.
                                          //         Analog capable sense inputs also allow adding:
                                          //         |THLD(n) Where n=1..1023 (ADU) for Analog threshold.
                                          //         |HYST(n) Where n=0..1023 (ADU) for +/- Hystersis range.

#define AXIS1_STEPS_PER_DEGREE    12800.0 //  12800, n. Number of steps per degree:                                          <-Req'd
                                          //         n = (stepper_steps * micro_steps * overall_gear_reduction)/360.0

// AXIS2 DEC/ALT ------------------------------- see https://onstep.groups.io/g/main/wiki/Configuration---Rotator-and-Focusers#AXIS2
#define AXIS2_DRIVER_MODEL            OFF //    OFF, (See above.) Stepper driver model.                                      <-Often

#define AXIS2_DRIVER_DECAY            OFF //    OFF, Tracking decay mode default override. TMC default is STEALTHCHOP.        Infreq
#define AXIS2_DRIVER_DECAY_GOTO       OFF //    OFF, Decay mode goto default override. TMC default is SPREADCYCLE.            Infreq
#define AXIS2_DRIVER_MICROSTEPS       OFF //    OFF, n. Microstep mode when tracking.                                        <-Often
#define AXIS2_DRIVER_MICROSTEPS_GOTO SAME //   SAME, n. Microstep mode used during gotos. SAME uses MICROSTEPS.               Option
#define AXIS2_DRIVER_IHOLD           HALF //   HALF, n, (mA.) Current during standstill. HALF uses IRUN/2.0                   Option
#define AXIS2_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current during tracking, appropriate for stepper/driver/etc.    Option
#define AXIS2_DRIVER_IGOTO           SAME //   SAME, n, (mA.) Current during slews. SAME uses IRUN.                           Option
#define AXIS2_DRIVER_POWER_DOWN       OFF //    OFF, ON Powers off 30sec after movement stops or 10min after last<=1x guide.  Option
#define AXIS2_DRIVER_REVERSE          OFF //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.   <-Often
#define AXIS2_DRIVER_STATUS           OFF //    OFF, ON, HIGH, or LOW.  Polling for driver status info/fault detection.       Option

#define AXIS2_LIMIT_MIN               -90 //    -90, n. Where n=-90..0 (degrees.) Minimum allowed declination.                Infreq
#define AXIS2_LIMIT_MAX                90 //     90, n. Where n=0..90 (degrees.) Maximum allowed declination.                 Infreq

#define AXIS2_SENSE_HOME              OFF //    OFF, HIGH or LOW enables & state clockwise home position, as seen from above. Option
#define AXIS2_SENSE_LIMIT_MIN         OFF //    OFF, HIGH or LOW state on limit sense switch stops movement.                  Option
#define AXIS2_SENSE_LIMIT_MAX         OFF //    OFF, HIGH or LOW state on limit sense switch stops movement.                  Option

#define AXIS2_TANGENT_ARM             OFF //    OFF, ON +limit range above. Set cntr w/[Reset Home] Return cntr w/[Find Home] Infreq

#define AXIS2_STEPS_PER_DEGREE    12800.0 //  12800, n. Number of steps per degree:                                          <-Req'd
                                          //         n = (stepper_steps * micro_steps * overall_gear_reduction)/360.0

// MOUNT TYPE -------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#MOUNT_TYPE
#define MOUNT_TYPE                    GEM //    GEM, GEM for German Equatorial, FORK for Equatorial Fork, or ALTAZM          <-Req'd
                                          //         Dobsonian etc. mounts. GEM Eq mounts perform meridian flips.

// TIME AND LOCATION -------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#TLS
#define TIME_LOCATION_SOURCE          OFF //    OFF, DS3231 (I2c,) DS3234 (Spi,) TEENSY (T3.2 internal,) or GPS source.       Option
                                          //         Provides Date/Time, and if available, PPS & Lat/Long also.
#define TIME_LOCATION_PPS_SENSE       OFF //    OFF, ON enables PPS (pulse per second,) senses signal rising edge.            Option
                                          //         Better tracking accuracy especially for Mega2560's w/ceramic resonator.

// USER FEEDBACK -------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#USER_FEEDBACK
#define LED_MOUNT_STATUS              OFF //    OFF, ON Flashes proportional to rate of movement or solid on for slews.       Option
#define BUZZER                        OFF //    OFF, ON, n. Where n=100..6000 (Hz freq.) for speaker. ON for piezo buzzer.    Option
#define BUZZER_DEFAULT                OFF //    OFF, ON default starts w/buzzer enabled.                                      Option
#define BUZZER_MEMORY                 OFF //    OFF, ON to remember buzzer setting across power cycles.                       Option

// SENSORS -------------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SENSORS
#define LIMIT_SENSE                   OFF //    OFF, HIGH or LOW state on limit sense switch stops movement.                  Option

// ST4 INTERFACE ------------------------------------------------------ see https://onstep.groups.io/g/main/wiki/6-Configuration#ST4
// *** It is up to you to verify the interface meets the electrical specifications of any connected device, use at your own risk ***
#define ST4_INTERFACE                 OFF //    OFF, ON enables interface. <= 1X guides unless hand control mode.             Option
                                          //         During goto btn press: aborts slew or continue meridian flip pause home
#define ST4_HAND_CONTROL              OFF //    OFF, ON for hand controller special features and SHC support.                 Option
                                          //         Hold [E]+[W] btns >2s: Guide rate   [E]-  [W]+  [N] trk on/off [S] sync
                                          //         Hold [N]+[S] btns >2s: Usr cat item [E]-  [W]+  [N] goto [S] snd on/off
#define ST4_HAND_CONTROL_FOCUSER      OFF //    OFF, ON alternate to above: Focuser move [E]f1 [W]f2 [N]-     [S]+            Option

// PEC ---------------------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#PEC
#define PEC_STEPS_PER_WORM_ROTATION     0 //      0, n. Number steps per worm rotation (PEC Eq mode only, 0 disables PEC.)   <-Req'd
                                          //         n = (AXIS1_STEPS_PER_DEGREE*360)/reduction_final_stage

#define PEC_SENSE                     OFF //    OFF, HIGH. Senses the PEC signal rising edge or use LOW for falling edge.     Option
                                          //         Ignored in ALTAZM mode.

// GUIDING BEHAVIOUR ---------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#GUIDING
#define GUIDE_TIME_LIMIT                0 //      0, No guide time limit. Or n. Where n=1..120 second time limit guard.       Adjust
#define GUIDE_DISABLE_BACKLASH        OFF //    OFF, Disable backlash takeup during guiding at <= 1X                          Option

// TRACKING BEHAVIOUR -------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#TRACKING
#define TRACK_AUTOSTART               OFF //    OFF, ON Start with tracking enabled.                                          Option
#define TRACK_REFRACTION_RATE_DEFAULT OFF //    OFF, ON Start w/atmospheric refract. compensation (RA axis/Eq mounts only.)   Option
#define TRACK_BACKLASH_RATE            25 //     25, n. Where n=2..50 (x sidereal rate) during backlash takeup.               Option
                                          //         Too fast motors stall/gears slam or too slow and sluggish in backlash.

// SLEWING BEHAVIOUR ---------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SLEWING
#define SLEW_RATE_BASE_DESIRED        1.0 //    1.0, n. Desired slew rate in deg/sec. Adjustable at run-time from            <-Req'd
                                          //         1/2 to 2x this rate, and as MCU performace considerations require.
#define SLEW_RATE_MEMORY              OFF //    OFF, ON Remembers rates set across power cycles.                              Option
#define SLEW_ACCELERATION_DIST        5.0 //    5.0, n, (degrees.) Approx. distance for acceleration (and deceleration.)      Adjust
#define SLEW_RAPID_STOP_DIST          2.0 //    2.0, n, (degrees.) Approx. distance required to stop when a slew              Adjust
                                          //         is aborted or a limit is exceeded.

// PIER SIDE BEHAVIOUR -------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SYNCING
#define MFLIP_SKIP_HOME               OFF //    OFF, ON Goto directly to the destination without visiting home position.      Option
#define MFLIP_PAUSE_HOME_MEMORY       OFF //    OFF, ON Remember meridian flip pause at home setting across power cycles.     Option
#define MFLIP_AUTOMATIC_MEMORY        OFF //    OFF, ON Remember automatic meridian flip setting across power cycles.         Option

#define PIER_SIDE_SYNC_CHANGE_SIDES   OFF //    OFF, ON Allows sync to change pier side, for GEM mounts.                      Option
#define PIER_SIDE_PREFERRED_DEFAULT  BEST //   BEST, Stays on current side if possible. EAST or WEST switch if possible.      Option

// PARKING BEHAVIOUR ---------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#PARKING
#define STRICT_PARKING                OFF //    OFF, ON Un-parking is only allowed if successfully parked.                    Option

// ---------------------------------------------------------------------------------------------------------------------------------
