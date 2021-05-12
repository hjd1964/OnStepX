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

// AXIS3 ROTATOR ------------------------------- see https://onstep.groups.io/g/main/wiki/Configuration---Rotator-and-Focusers#AXIS3
#define AXIS3_STEPS_PER_DEGREE       64.0 //   64.0, n. Number of steps per degree for rotator/de-rotator.                    Adjust
                                          //         Alt/Az de-rotation: n = (num_circumference_pixels * 2)/360, minimum
#define AXIS3_SLEW_RATE_DESIRED       1.0 //    1.0, n, (degrees/second) Maximum speed depends on processor.                  Adjust

#define AXIS3_DRIVER_MODEL            OFF //    OFF, TMC2130, TMC5160. Leave OFF for all drivers models except these.         Option
#define AXIS3_DRIVER_MICROSTEPS       OFF //    OFF, n. Microstep mode when tracking.                   For TMC2130, TMC5160. Option
#define AXIS3_DRIVER_IHOLD            OFF //    OFF, n, (mA.) Current standstill. OFF uses IRUN/2.0.                  "       Option
#define AXIS3_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current tracking, appropriate for stepper/driver/etc.   "       Option
#define AXIS3_DRIVER_POWER_DOWN       OFF //    OFF, ON Powers off the motor at stand-still.                                  Option
#define AXIS3_DRIVER_REVERSE          OFF //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.    Option
#define AXIS3_DRIVER_STATUS           OFF //    OFF, ON, HIGH, or LOW.  Polling for driver status info/fault detection.       Option

#define AXIS3_LIMIT_MIN              -180 //   -180, n. Where n=-360..0 (degrees.) Minimum allowed rotator angle.             Infreq
#define AXIS3_LIMIT_MAX               180 //    180, n. Where n=0..360 (degrees.) Maximum allowed rotator angle.              Infreq

// ---------------------------------------------------------------------------------------------------------------------------------
