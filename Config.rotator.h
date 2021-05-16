/* ---------------------------------------------------------------------------------------------------------------------------------
 * Configuration for OnStepX rotator options
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

// AXIS3 ROTATOR ------------------------------- see https://onstep.groups.io/g/main/wiki/Configuration---Rotator-and-Focusers#AXIS3
#define AXIS3_DRIVER_MODEL            OFF //    OFF, (See above.) Stepper driver model.                                       Option

#define AXIS3_DRIVER_MICROSTEPS       OFF //    OFF, n. Microstep mode when tracking.                                         Option
#define AXIS3_DRIVER_IHOLD           HALF //   HALF, n, (mA.) Current standstill. HALF uses IRUN/2.0                          Option
#define AXIS3_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current tracking, appropriate for stepper/driver/etc.           Option
#define AXIS3_DRIVER_POWER_DOWN       OFF //    OFF, ON Powers off the motor at stand-still.                                  Option
#define AXIS3_DRIVER_REVERSE          OFF //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.    Option
#define AXIS3_DRIVER_STATUS           OFF //    OFF, ON, HIGH, or LOW.  Polling for driver status info/fault detection.       Option

#define AXIS3_LIMIT_MIN              -180 //   -180, n. Where n=-360..0 (degrees.) Minimum allowed rotator angle.             Infreq
#define AXIS3_LIMIT_MAX               180 //    180, n. Where n=0..360 (degrees.) Maximum allowed rotator angle.              Infreq

#define AXIS3_SENSE_HOME              OFF //    OFF, ON for HIGH clockwise of home position, as seen from above. Or use LOW.  Option
#define AXIS3_SENSE_LIMIT_MIN         OFF //    OFF, ON for HIGH limit sense switch state stops movement. Or use LOW.         Option
#define AXIS3_SENSE_LIMIT_MAX         OFF //    OFF, ON for HIGH limit sense switch state stops movement. Or use LOW.         Option

#define AXIS3_STEPS_PER_DEGREE       64.0 //   64.0, n. Number of steps per degree for rotator/de-rotator.                    Adjust
                                          //         Alt/Az de-rotation: n = (num_circumference_pixels * 2)/360, minimum
#define AXIS3_SLEW_RATE_DESIRED       1.0 //    1.0, n, (degrees/second) Maximum speed depends on processor.                  Adjust

// ---------------------------------------------------------------------------------------------------------------------------------
