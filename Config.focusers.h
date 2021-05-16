/* ---------------------------------------------------------------------------------------------------------------------------------
 * Configuration for OnStepX focuser options
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
// A4988, DRV8825, LV8729, SSS TMC2100, TMC2130*
// * = SSS TMC2130 if you choose to set stepper driver current (in mA) set Vref pot. 2.5V instead of by motor current as usual.

// AXIS4 FOCUSER 1 -------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#AXIS4
#define AXIS4_DRIVER_MODEL            OFF //    OFF, (See above.) Stepper driver model.                                       Option

#define AXIS4_DRIVER_MICROSTEPS       OFF //    OFF, n. Microstep mode when tracking.                                         Option
#define AXIS4_DRIVER_IHOLD           HALF //   HALF, n, (mA.) Current standstill. HALF uses IRUN/2.0                          Option
#define AXIS4_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current during tracking, appropriate for stepper/driver/etc.    Option
#define AXIS4_DRIVER_POWER_DOWN       OFF //    OFF, ON Powers off the motor at stand-still.                                  Option
#define AXIS4_DRIVER_REVERSE          OFF //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.    Option
#define AXIS4_DRIVER_STATUS           OFF //    OFF, ON, HIGH, or LOW.  Polling for driver status info/fault detection.       Option
#define AXIS4_DRIVER_DC_MODE          OFF //    OFF, ON for pwm dc motor control on stepper driver outputs.                   Option

#define AXIS4_LIMIT_MIN                 0 //      0, n. Where n=0..500 (millimeters.) Minimum allowed position.               Adjust
#define AXIS4_LIMIT_MAX                50 //     50, n. Where n=0..500 (millimeters.) Maximum allowed position.               Adjust

#define AXIS4_SENSE_HOME              OFF //    OFF, ON for HIGH inside of home position. Or use LOW.                         Option
#define AXIS4_SENSE_LIMIT_MIN         OFF //    OFF, ON for HIGH limit sense switch state stops movement. Or use LOW.         Option
#define AXIS4_SENSE_LIMIT_MAX         OFF //    OFF, ON for HIGH limit sense switch state stops movement. Or use LOW.         Option

#define AXIS4_STEPS_PER_MICRON        0.5 //    0.5, n. Steps per micrometer. Figure this out by testing or other means.      Adjust
#define AXIS4_SLEW_RATE_MINIMUM        50 //     50, n. Where n=1..1000 (um/s.) Minimum microns/second. In DC mode, min pwr.  Adjust
#define AXIS4_SLEW_RATE_DESIRED       500 //    500, n, Where n=200..5000 (um/s.) Max microns/second. In DC mode, max pwr %   Adjust

// AXIS6 FOCUSER 2 -----------------------------------------------------------------------------------------------------------------
// Up to 6 focusers can be present (AXIS4 to AXIS9) simply copy the above text for focuser 1 and rename to AXIS5_ for focuser 2, etc

// ---------------------------------------------------------------------------------------------------------------------------------
