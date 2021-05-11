/* ---------------------------------------------------------------------------------------------------------------------------------
 * Configuration for OnStepX focuser optiona
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

// AXIS4 FOCUSER 1 -------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#AXIS4
#define AXIS4_STEPS_PER_MICRON        0.5 //    0.5, n. Steps per micrometer. Figure this out by testing or other means.      Adjust
#define AXIS4_SLEW_RATE_DESIRED       500 //    500, n, Where n=200..5000 (um/s.) Max microns/second. In DC mode, max pwr %   Adjust

#define AXIS4_DRIVER_MODEL            OFF //    OFF, TMC2130, TMC5160. Leave OFF for all drivers models except these.         Option
#define AXIS4_DRIVER_MICROSTEPS       OFF //    OFF, n. Microstep mode when tracking.                   For TMC2130, TMC5160. Option
#define AXIS4_DRIVER_IHOLD            OFF //    OFF, n, (mA.) Current standstill. OFF uses IRUN/2.0.                  "       Option
#define AXIS4_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current tracking, appropriate for stepper/driver/etc.   "       Option
#define AXIS4_DRIVER_POWER_DOWN       OFF //    OFF, ON Powers off the motor at stand-still.                                  Option
#define AXIS4_DRIVER_REVERSE          OFF //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.    Option
#define AXIS4_DRIVER_STATUS           OFF //    OFF, ON, HIGH, or LOW.  Polling for driver status info/fault detection.       Option
#define AXIS4_DRIVER_DC_MODE          OFF //    OFF, DRV8825 for pwm dc motor control on stepper driver outputs.              Option

#define AXIS4_LIMIT_MIN_RATE           50 //     50, n. Where n=1..1000 (um/s.) Minimum microns/second. In DC mode, min pwr.  Adjust
#define AXIS4_LIMIT_MIN                 0 //      0, n. Where n=0..500 (millimeters.) Minimum allowed position.               Adjust
#define AXIS4_LIMIT_MAX                50 //     50, n. Where n=0..500 (millimeters.) Maximum allowed position.               Adjust

// AXIS6 FOCUSER 2 -----------------------------------------------------------------------------------------------------------------
// Up to 6 focusers can be present (AXIS4 to AXIS9) simply copy the above text for focuser 1 and rename to AXIS5_ for focuser 2, etc

// ---------------------------------------------------------------------------------------------------------------------------------
