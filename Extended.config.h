/* ---------------------------------------------------------------------------------------------------------------------------------
 * Extended configuration for OnStepX INFREQUENTLY USED controller options
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

// =================================================================================================================================
// CONTROLLER ======================================================================================================================

// DEBUG ------------------------------------------------------------ see https://onstep.groups.io/g/main/wiki/6-Configuration#DEBUG
// Enable additional debugging and/or status messages on the specified SERIAL_DEBUG port
// Note that the SERIAL_DEBUG port cannot be used for normal communication with OnStep
#define DEBUG                         OFF //    OFF, Use ON for background error messages only, use VERBOSE for all           Infreq
                                          //         error and status messages, use CONSOLE for VT100 debug console,
                                          //         or use PROFILER for VT100 task profiler.
#define DEBUG_ECHO_COMMANDS           OFF //    OFF, Use ON to log command/responses to the debug serial port.                Option
#define SERIAL_DEBUG               Serial // Serial, Use any h/w serial port. Serial1 or Serial2, etc. as supported.          Option
#define SERIAL_DEBUG_BAUD            9600 //   9600, n. Where n=9600,19200,57600,115200 (common baud rates.)                  Option

// SERIAL PORT COMMAND CHANNELS ------------------------------------ see https://onstep.groups.io/g/main/wiki/6-Configuration#SERIAL
#define SERIAL_A_BAUD_DEFAULT        9600 //   9600, n. Where n=9600,19200,57600,115200 (common baud rates.)                  Infreq
#define SERIAL_D_BAUD_DEFAULT         OFF //    OFF, n. See (src/HAL/) for your MCU Serial port # etc.                        Option

// ESP32 VIRTUAL SERIAL BLUETOOTH AND IP COMMAND CHANNELS --------------------------------------------------------------------------
#define SERIAL_BT_MODE                OFF //    OFF, Use SLAVE to enable the interface (ESP32 only.)                          Option
#define SERIAL_BT_NAME           "OnStep" //         "OnStep", Bluetooth device name.                                         Adjust
#define SERIAL_IP_MODE                OFF //    OFF, Use ACCESS_POINT or STATION to enable the interface (ESP32 only.)        Option

// EXTERNAL GPIO SUPPORT -----------------------------------------------------------------------------------------------------------
#define GPIO_DEVICE                   OFF //    OFF, DS2413 for 2-ch 1-wire GPIO, MCP23008 for 8-ch I2C GPIO, or MCP23017     Option
                                          //         for a 16-ch I2C GPIO. The device can be used for most OnStep features.
                                          //         Channels are assigned to pin#'s starting at 256 (for channel 0, etc.)

// =================================================================================================================================
// MOUNT ===========================================================================================================================

// AXIS1 RA/AZM -------------------------------- see https://onstep.groups.io/g/main/wiki/Configuration---Rotator-and-Focusers#AXIS1

#define AXIS1_DRIVER_DECAY            OFF //    OFF, Tracking decay mode default override. TMC default is STEALTHCHOP.        Infreq
#define AXIS1_DRIVER_DECAY_GOTO       OFF //    OFF, Decay mode goto default override. TMC default is SPREADCYCLE.            Infreq

// for SERVO_II and SERVO_SD driver models, PID and encoder settings:
#define AXIS1_SERVO_P                 2.0 //    2.0, Proportional; scale of immediate response to position error.             Adjust 
#define AXIS1_SERVO_I                 5.0 //    5.0, Integral; rate of increasing response to position error over time.       Adjust
#define AXIS1_SERVO_D                 1.0 //    1.0, Derivative; overshoot supression.                                        Adjust
#define AXIS1_SERVO_ENCODER        ENC_AB // ..._AB, Encoder type, ENC_AB, ENC_CW_CCW, ENC_PULSE_DIR, ENC_PULSE_ONLY.         Adjust

// other Axis1 settings:
#define AXIS1_POWER_DOWN              OFF //    OFF, ON Powers off 30sec after movement stops or 10min after last<=1x guide.  Option

#define AXIS1_WRAP                    OFF //    OFF, ON Allows unlimited RA/Azm range and ignores min/max/meridian limits.    Option

#define AXIS1_LIMIT_MIN              -180 //   -180, n. Where n= -90..-360 (degrees.) Minimum "Hour Angle" or Azimuth.        Adjust
#define AXIS1_LIMIT_MAX               180 //    180, n. Where n=  90.. 360 (degrees.) Maximum "Hour Angle" or Azimuth.        Adjust

#define AXIS1_SENSE_HOME              OFF //    OFF, HIGH or LOW enables & state clockwise home position, as seen from front. Option
#define AXIS1_SENSE_LIMIT_MIN         OFF //    OFF, HIGH or LOW state on limit sense switch stops movement.                  Option
#define AXIS1_SENSE_LIMIT_MAX         OFF //    OFF, HIGH or LOW state on limit sense switch stops movement.                  Option
                                          //         Digital, optionally add: |HYST(n) Where n=0..1023 stability time in ms.
                                          //         Analog capable sense inputs also allow adding:
                                          //         |THLD(n) Where n=1..1023 (ADU) for Analog threshold.
                                          //         |HYST(n) Where n=0..1023 (ADU) for +/- Hystersis range.

// AXIS2 DEC/ALT ------------------------------- see https://onstep.groups.io/g/main/wiki/Configuration---Rotator-and-Focusers#AXIS2

#define AXIS2_DRIVER_DECAY            OFF //    OFF, Tracking decay mode default override. TMC default is STEALTHCHOP.        Infreq
#define AXIS2_DRIVER_DECAY_GOTO       OFF //    OFF, Decay mode goto default override. TMC default is SPREADCYCLE.            Infreq

// for SERVO_II and SERVO_SD driver models, PID and encoder settings:
#define AXIS2_SERVO_P                 2.0 //    2.0, Proportional; scale of immediate response to position error.             Adjust 
#define AXIS2_SERVO_I                 5.0 //    5.0, Integral; rate of increasing response to position error over time.       Adjust
#define AXIS2_SERVO_D                 1.0 //    1.0, Derivative; overshoot supression.                                        Adjust
#define AXIS2_SERVO_ENCODER        ENC_AB // ..._AB, Encoder type, ENC_AB, ENC_CW_CCW, ENC_PULSE_DIR, ENC_PULSE_ONLY.         Adjust

#define AXIS2_POWER_DOWN              OFF //    OFF, ON Powers off 30sec after movement stops or 10min after last<=1x guide.  Option

#define AXIS2_LIMIT_MIN               -90 //    -90, n. Where n=-90..0 (degrees.) Minimum allowed declination.                Infreq
#define AXIS2_LIMIT_MAX                90 //     90, n. Where n=0..90 (degrees.) Maximum allowed declination.                 Infreq

#define AXIS2_SENSE_HOME              OFF //    OFF, HIGH or LOW enables & state clockwise home position, as seen from above. Option
#define AXIS2_SENSE_LIMIT_MIN         OFF //    OFF, HIGH or LOW state on limit sense switch stops movement.                  Option
#define AXIS2_SENSE_LIMIT_MAX         OFF //    OFF, HIGH or LOW state on limit sense switch stops movement.                  Option

#define AXIS2_TANGENT_ARM             OFF //    OFF, ON +limit range above. Set cntr w/[Reset Home] Return cntr w/[Find Home] Infreq
#define AXIS2_TANGENT_ARM_CORRECTION  OFF //    OFF, ON enables tangent arm geometry correction for Axis2.                    Infreq

// MOUNT COORDINATES ------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#MOUNT_TYPE
#define MOUNT_COORDS          TOPOCENTRIC // ...RIC, Applies refraction to coordinates to/from OnStep, except exactly         Infreq
                                          //              at the poles. Use TOPO_STRICT to apply refraction even in that case.
                                          //              Use OBSERVED_PLACE for no refraction.

// ALIGN ---------------------------------------------------------------------------------------------------------------------------
#define ALIGN_MAX_STARS              AUTO //   AUTO, Uses HAL specified default (either 6 or 9 stars.)                        Infreq
                                          //              Or use n. Where n=1 (for Sync only) or 3 to 9 (for Goto Assist.)

// USER FEEDBACK -------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#USER_FEEDBACK
#define STATUS_BUZZER_DEFAULT         OFF //    OFF, ON default starts w/buzzer enabled.                                      Option
#define STATUS_BUZZER_MEMORY          OFF //    OFF, ON to remember buzzer setting across power cycles.                       Option

// ST4 INTERFACE ------------------------------------------------------ see https://onstep.groups.io/g/main/wiki/6-Configuration#ST4
// *** It is up to you to verify the interface meets the electrical specifications of any connected device, use at your own risk ***
#define ST4_HAND_CONTROL               ON //     ON, ON for hand controller special features and SHC support.                 Option
                                          //         Hold [E]+[W] btns >2s: Guide rate   [E]-  [W]+  [N] trk on/off [S] sync
                                          //         Hold [N]+[S] btns >2s: Usr cat item [E]-  [W]+  [N] goto [S] snd on/off
#define ST4_HAND_CONTROL_FOCUSER       ON //     ON, ON alternate to above: Focuser move [E]f1 [W]f2 [N]-     [S]+            Option

// GUIDING BEHAVIOUR ---------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#GUIDING
#define GUIDE_TIME_LIMIT                0 //      0, No guide time limit. Or n. Where n=1..120 second time limit guard.       Adjust
#define GUIDE_DISABLE_BACKLASH        OFF //    OFF, Disable backlash takeup during guiding at <= 1X                          Option
#define SEPARATE_PULSE_GUIDE_RATE      ON //     ON, Uses a separate rate (stored in NV) for pulse guiding                    Infreq

// PEC ---------------------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#PEC
#define PEC_BUFFER_SIZE_LIMIT         720 //    720, Seconds of PEC buffer allowed.                                           Adjust

// SLEWING BEHAVIOUR ---------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SLEWING
#define SLEW_GOTO                      ON //     ON, Use OFF to disable mount Goto features.                                  Infreq

// TRACKING BEHAVIOUR -------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#TRACKING
#define TRACK_AUTOSTART               OFF //    OFF, ON Start with tracking enabled.                                          Option
#define TRACK_REFRACTION_RATE_DEFAULT OFF //    OFF, ON Start w/atmospheric refract. compensation (RA axis/Eq mounts only.)   Option
#define TRACK_BACKLASH_RATE            25 //     25, n. Where n=2..100 (x sidereal rate) during backlash takeup.              Option
                                          //         Too fast motors stall/gears slam or too slow and sluggish in backlash.

// SLEWING BEHAVIOUR ---------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SLEWING
#define SLEW_RATE_MEMORY              OFF //    OFF, ON Remembers rates set across power cycles.                              Option

// PIER SIDE BEHAVIOUR -------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SYNCING
#define MFLIP_SKIP_HOME                ON //     ON, ON Goto directly to the destination without visiting home position.      Option
#define MFLIP_PAUSE_HOME_MEMORY       OFF //    OFF, ON Remember meridian flip pause at home setting across power cycles.     Option
#define MFLIP_AUTOMATIC_MEMORY         ON //     ON, ON Remember automatic meridian flip setting across power cycles.         Option

#define PIER_SIDE_SYNC_CHANGE_SIDES   OFF //    OFF, ON Allows sync to change pier side, for GEM mounts.                      Option
#define PIER_SIDE_PREFERRED_DEFAULT  BEST //   BEST, Stays on current side if possible. EAST or WEST switch if possible.      Option

// PARKING BEHAVIOUR ---------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#PARKING
#define STRICT_PARKING                OFF //    OFF, ON Un-parking is only allowed if successfully parked.                    Option

// =================================================================================================================================
// ROTATOR =========================================================================================================================

// AXIS3 ROTATOR ------------------------------- see https://onstep.groups.io/g/main/wiki/Configuration---Rotator-and-Focusers#AXIS3

#define AXIS3_DRIVER_DECAY            OFF //    OFF, Tracking decay mode default override. TMC default is STEALTHCHOP.        Infreq
#define AXIS3_DRIVER_DECAY_GOTO       OFF //    OFF, Decay mode goto default override. TMC default is SPREADCYCLE.            Infreq

// for SERVO_II and SERVO_SD driver models, PID and encoder settings:
#define AXIS3_SERVO_P                 2.0 //    2.0, Proportional; scale of immediate response to position error.             Adjust 
#define AXIS3_SERVO_I                 5.0 //    5.0, Integral; rate of increasing response to position error over time.       Adjust
#define AXIS3_SERVO_D                 1.0 //    1.0, Derivative; overshoot supression.                                        Adjust
#define AXIS3_SERVO_ENCODER        ENC_AB // ..._AB, Encoder type, ENC_AB, ENC_CW_CCW, ENC_PULSE_DIR, ENC_PULSE_ONLY.         Adjust

#define AXIS3_POWER_DOWN              OFF //    OFF, ON Powers off the motor at stand-still.                                  Option

#define AXIS3_LIMIT_MIN              -180 //   -180, n. Where n=-360..0 (degrees.) Minimum allowed rotator angle.             Infreq
#define AXIS3_LIMIT_MAX               180 //    180, n. Where n=0..360 (degrees.) Maximum allowed rotator angle.              Infreq

#define AXIS3_SENSE_HOME              OFF //    OFF, HIGH or LOW enables & state clockwise home position, as seen from above. Option
#define AXIS3_SENSE_LIMIT_MIN         OFF //    OFF, HIGH or LOW state on limit sense switch stops movement.                  Option
#define AXIS3_SENSE_LIMIT_MAX         OFF //    OFF, HIGH or LOW state on limit sense switch stops movement.                  Option

// USER FEEDBACK -------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#USER_FEEDBACK
#define STATUS_ROTATOR_LED            OFF //    OFF, ON Flashes proportional to the rate of movement (2Hz = 1deg/s.)          Option

// =================================================================================================================================
// FOCUSERS ========================================================================================================================

// AXIS4 FOCUSER 1 -------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#AXIS4

#define AXIS4_DRIVER_DECAY            OFF //    OFF, Tracking decay mode default override. TMC default is STEALTHCHOP.        Infreq
#define AXIS4_DRIVER_DECAY_GOTO       OFF //    OFF, Decay mode goto default override. TMC default is SPREADCYCLE.            Infreq

// for SERVO_II and SERVO_SD driver models, PID and encoder settings:
#define AXIS4_SERVO_P                 2.0 //    2.0, Proportional; scale of immediate response to position error.             Adjust 
#define AXIS4_SERVO_I                 5.0 //    5.0, Integral; rate of increasing response to position error over time.       Adjust
#define AXIS4_SERVO_D                 1.0 //    1.0, Derivative; overshoot supression.                                        Adjust
#define AXIS4_SERVO_ENCODER        ENC_AB // ..._AB, Encoder type, ENC_AB, ENC_CW_CCW, ENC_PULSE_DIR, ENC_PULSE_ONLY.         Adjust

#define AXIS4_POWER_DOWN              OFF //    OFF, ON Powers off 30sec after movement stops.                                Option

#define AXIS4_LIMIT_MIN                 0 //      0, n. Where n=0..500 (millimeters.) Minimum allowed position.               Adjust
#define AXIS4_LIMIT_MAX                50 //     50, n. Where n=0..500 (millimeters.) Maximum allowed position.               Adjust

#define AXIS4_SENSE_HOME              OFF //    OFF, HIGH or LOW enables & state clockwise home position, as seen from above. Option
#define AXIS4_SENSE_LIMIT_MIN         OFF //    OFF, HIGH or LOW state on limit sense switch stops movement.                  Option
#define AXIS4_SENSE_LIMIT_MAX         OFF //    OFF, HIGH or LOW state on limit sense switch stops movement.                  Option

#define AXIS4_SLEW_RATE_MINIMUM         2 //      2, n. Where n=1..10 (um/s.) Minimum microns/second.                         Adjust

// AXIS5 FOCUSER 2 -----------------------------------------------------------------------------------------------------------------
// Up to 6 focusers can be present (AXIS4 to AXIS9) simply copy the above text for focuser 1 and rename to AXIS5_ for focuser 2, etc

// USER FEEDBACK -------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#USER_FEEDBACK
#define STATUS_FOCUSER_LED            OFF //    OFF, ON Flashes proportional to the rate of movement (2Hz = 500um/s.)         Option

// SENSORS -------------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SENSORS
#define FOCUSER_TEMPERATURE           OFF //    OFF, THERMISTOR, DS1820, n. Where n is the DS1820 s/n for focuser temp.       Adjust

// =================================================================================================================================
// AUXILIARY FEATURES ==============================================================================================================

// FEATURES ----------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-ConfigurationMaster#AUXILIARY

#define FEATURE1_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust
#define FEATURE1_ACTIVE_STATE        HIGH //   HIGH, LOW to invert so "ON" is 0V and "OFF" is Vcc (3.3V or 5V.)               Adjust

#define FEATURE2_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust
#define FEATURE2_ACTIVE_STATE        HIGH //   HIGH, LOW to invert so "ON" is 0V and "OFF" is Vcc (3.3V or 5V.)               Adjust

#define FEATURE3_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust
#define FEATURE3_ACTIVE_STATE        HIGH //   HIGH, LOW to invert so "ON" is 0V and "OFF" is Vcc (3.3V or 5V.)               Adjust

#define FEATURE4_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust
#define FEATURE4_ACTIVE_STATE        HIGH //   HIGH, LOW to invert so "ON" is 0V and "OFF" is Vcc (3.3V or 5V.)               Adjust

#define FEATURE5_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust
#define FEATURE5_ACTIVE_STATE        HIGH //   HIGH, LOW to invert so "ON" is 0V and "OFF" is Vcc (3.3V or 5V.)               Adjust

#define FEATURE6_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust
#define FEATURE6_ACTIVE_STATE        HIGH //   HIGH, LOW to invert so "ON" is 0V and "OFF" is Vcc (3.3V or 5V.)               Adjust

#define FEATURE7_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust
#define FEATURE7_ACTIVE_STATE        HIGH //   HIGH, LOW to invert so "ON" is 0V and "OFF" is Vcc (3.3V or 5V.)               Adjust

#define FEATURE8_DEFAULT_VALUE        OFF //    OFF, ON, n. Where n=0..255 for ANALOG_OUT purpose.                            Adjust
#define FEATURE8_ACTIVE_STATE        HIGH //   HIGH, LOW to invert so "ON" is 0V and "OFF" is Vcc (3.3V or 5V.)               Adjust

// ---------------------------------------------------------------------------------------------------------------------------------
