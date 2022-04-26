/* ---------------------------------------------------------------------------------------------------------------------------------
 * Configuration for OnStepX controller options
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

// PINMAP ---------------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#PINMAP
#define PINMAP                        MaxESP_RCX400 //    OFF, Choose from: MksGenL2, MiniPCB2, MaxPCB2, MaxESP3, CNC3, STM32Blue,     <-Req'd
                                          //         MaxSTM3, FYSETC_S6_2, etc.  Other boards and more info. in ~/src/Constants.h

// SERIAL PORT COMMAND CHANNELS ------------------------------------ see https://onstep.groups.io/g/main/wiki/6-Configuration#SERIAL
#define SERIAL_B_BAUD_DEFAULT        9600 //   9600, n. See (src/HAL/) for your MCU Serial port # etc.                        Option
#define SERIAL_B_ESP_FLASHING         ON //    OFF, ON Upload ESP8266 WiFi firmware through SERIAL_B with :ESPFLASH# cmd.    Option
#define SERIAL_C_BAUD_DEFAULT         OFF //    OFF, n. See (src/HAL/) for your MCU Serial port # etc.                        Option

// USER FEEDBACK -------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#USER_FEEDBACK
#define STATUS_LED                    OFF //    OFF, Steady illumination if no error, blinks w/error code otherwise.          Option

// RETICLE CONTROL ------------------------------------------------ see https://onstep.groups.io/g/main/wiki/6-Configuration#RETICLE
#define RETICLE_LED_DEFAULT           OFF //    OFF, n. Where n=0..255 (0..100%) activates feature sets default brightness.   Option
#define RETICLE_LED_MEMORY            OFF //    OFF, ON Remember reticle brightness across power cycles.                      Option

// WEATHER SENSOR ------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SENSORS
#define WEATHER                       BME280_0x76 //    OFF, BME280 (I2C 0x77,) BME280_0x76, BME280_SPI (see pinmap for CS.)          Option
                                          //         BMP280 (I2C 0x77,) BMP280_0x76, BMP280_SPI (see pinmap for CS.)
                                          //         BME280 or BMP280 for temperature, pressure.  BME280 for humidity also.

// MOTION ---------------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#MOTION
#define STEP_WAVE_FORM             SQUARE // SQUARE, PULSE Step signal wave form faster rates. SQUARE best signal integrity.  Adjust
                                          //         Applies to all axes.

// =================================================================================================================================
// MOUNT ===========================================================================================================================

// Driver models (Step/Dir and Servo) see "~/OnStepX/Constants.h" for a complete list.
// Typically: A4988, DRV8825, LV8729, S109, TMC2130*, or TMC5160
// Settings for TMC SPI and SERVO drivers are in Extended.config.h
// * = SSS TMC2130 set Vref to 2.5V (IRUN defaults to 300mA.)

// Settings for driver Microsteps, IRUN, Reverse, Limit Min, and Limit Max are stored in NV (EEPROM.) These runtime settings
// can be changed (or reverted to the defaults below) from the SmartWebServer's Config webpage.  If runtime axis settings are
// enabled changes to these settings below may be ignored as runtime settings from NV (EEPROM) are used instead.

// AXIS1 RA/AZM -------------------------------- see https://onstep.groups.io/g/main/wiki/Configuration---Rotator-and-Focusers#AXIS1
#define AXIS1_DRIVER_MODEL            GENERIC //    OFF, Enter motor driver model (above) in both axes to activate the mount.    <-Often
#define AXIS1_DRIVER_MICROSTEPS       1 //    OFF, n. Microstep mode when tracking.                                        <-Often
#define AXIS1_DRIVER_MICROSTEPS_GOTO  OFF //    OFF, n. Microstep mode used during gotos. OFF uses _DRIVER_MICROSTEPS.        Option
#define AXIS1_DRIVER_STATUS           OFF //    OFF, ON, HIGH, or LOW.  For driver status info/fault detection.               Option

// for TMC2130, TMC5160, and TMC2209U STEP/DIR driver models:
#define AXIS1_DRIVER_IHOLD            OFF //    OFF, n, (mA.) Current during standstill. OFF uses IRUN/2.0                    Option
#define AXIS1_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current during tracking, appropriate for stepper/driver/etc.    Option
#define AXIS1_DRIVER_IGOTO            OFF //    OFF, n, (mA.) Current during slews. OFF uses IRUN.                            Option

#define AXIS1_STEPS_PER_DEGREE      49777.77778 //  12800, n. Number of steps per degree:                                          <-Req'd
                                          //         n = (stepper_steps * micro_steps * overall_gear_reduction)/360.0
#define AXIS1_REVERSE                 OFF //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.   <-Often

// AXIS2 DEC/ALT ------------------------------- see https://onstep.groups.io/g/main/wiki/Configuration---Rotator-and-Focusers#AXIS2
#define AXIS2_DRIVER_MODEL            GENERIC //    OFF, Enter motor driver model (above) in both axes to activate the mount.    <-Often
#define AXIS2_DRIVER_MICROSTEPS       1 //    OFF, n. Microstep mode when tracking.                                        <-Often
#define AXIS2_DRIVER_MICROSTEPS_GOTO  OFF //    OFF, n. Microstep mode used during gotos. OFF uses _DRIVER_MICROSTEPS.        Option
#define AXIS2_DRIVER_STATUS           OFF //    OFF, ON, HIGH, or LOW.  Polling for driver status info/fault detection.       Option

// for TMC2130, TMC5160, and TMC2209U STEP/DIR driver models:
#define AXIS2_DRIVER_IHOLD            OFF //    OFF, n, (mA.) Current during standstill. OFF uses IRUN/2.0                    Option
#define AXIS2_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current during tracking, appropriate for stepper/driver/etc.    Option
#define AXIS2_DRIVER_IGOTO            OFF //    OFF, n, (mA.) Current during slews. OFF uses IRUN.                            Option

#define AXIS2_STEPS_PER_DEGREE      49777.77778 //  12800, n. Number of steps per degree:                                          <-Req'd
                                          //         n = (stepper_steps * micro_steps * overall_gear_reduction)/360.0
#define AXIS2_REVERSE                 ON //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.   <-Often

#define AXIS2_HOME_DEFAULT             20

// MOUNT ------------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#MOUNT_TYPE
#define MOUNT_TYPE                    ALTAZM //    GEM, GEM for German Equatorial, FORK for Equatorial Fork, or ALTAZM          <-Req'd
                                          //         Dobsonian etc. mounts. GEM Eq mounts perform meridian flips.

// TIME AND LOCATION -------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#TLS
#define TIME_LOCATION_SOURCE          DS3231 //    OFF, DS3231 (I2c,) DS3234 (Spi,) TEENSY (T3.2 internal,) or GPS source.       Option
                                          //         Provides Date/Time, and if available, PPS & Lat/Long also.
#define TIME_LOCATION_PPS_SENSE       HIGH //    OFF, HIGH senses PPS (pulse per second,) signal rising edge, or use LOW for   Option
                                          //         falling edge, or use BOTH for rising and falling edges.
                                          //         Better tracking accuracy especially for Mega2560's w/ceramic resonator.

// USER FEEDBACK -------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#USER_FEEDBACK
#define STATUS_MOUNT_LED              OFF //    OFF, ON Flashes proportional to rate of movement or solid on for slews.       Option
#define STATUS_BUZZER                 OFF //    OFF, ON, n. Where n=100..6000 (Hz freq.) for speaker. ON for piezo buzzer.    Option

// ST4 INTERFACE ------------------------------------------------------ see https://onstep.groups.io/g/main/wiki/6-Configuration#ST4
// *** It is up to you to verify the interface meets the electrical specifications of any connected device, use at your own risk ***
#define ST4_INTERFACE                 OFF //    OFF, ON enables interface. <= 1X guides unless hand control mode.             Option
                                          //         During goto btn press: aborts slew or continue meridian flip pause home

// SENSORS -------------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SENSORS
#define LIMIT_SENSE                   LOW //    OFF, HIGH or LOW state on limit sense switch stops movement.                  Option

// PARK -------------------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#PARK
#define PARK_SENSE                    OFF //    OFF, HIGH or LOW state indicates mount is in the park orientation.            Option
#define PARK_SIGNAL                   OFF //    OFF, HIGH or LOW state park input signal triggers parking.                    Option
#define PARK_STATUS                   OFF //    OFF, signals with a HIGH or LOW state when successfully parked.               Option 

// PEC ---------------------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#PEC
#define PEC_STEPS_PER_WORM_ROTATION     51200 //      0, n. Steps per worm rotation (0 disables else 720 sec buffer allocated.)  <-Req'd
                                          //         n = (AXIS1_STEPS_PER_DEGREE*360)/reduction_final_stage

#define PEC_SENSE                     OFF //    OFF, HIGH. Senses the PEC signal rising edge or use LOW for falling edge.     Option
                                          //         Ignored in ALTAZM mode.

// SLEWING BEHAVIOUR ---------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#SLEWING
#define SLEW_RATE_BASE_DESIRED        1.0 //    1.0, n. Desired slew rate in deg/sec. Adjustable at run-time from            <-Req'd
                                          //         1/2 to 2x this rate, and as performace considerations require.
#define SLEW_ACCELERATION_DIST        5.0 //    5.0, n, (degrees.) Approx. distance for acceleration (and deceleration.)      Adjust
#define SLEW_RAPID_STOP_DIST          2.0 //    2.0, n, (degrees.) Approx. distance required to stop when a slew              Adjust
                                          //         is aborted or a limit is exceeded.

// =================================================================================================================================
// ROTATOR =========================================================================================================================

// Driver models (Step/Dir and Servo) see "~/OnStepX/Constants.h" for a complete list.
// A4988, DRV8825, LV8729, SSS TMC2100, TMC2130*
// Settings for TMC SPI and SERVO drivers are in Extended.config.h
// * = SSS TMC2130 set Vref to 2.5V (IRUN defaults to 300mA.)

// Settings for driver Microsteps, IRUN, Reverse, Limit Min, and Limit Max are stored in NV (EEPROM.) These runtime settings
// can be changed (or reverted to the defaults below) from the SmartWebServer's Config webpage.  If runtime axis settings are 
// enabled changes to these settings below may be ignored as runtime settings from NV (EEPROM) are used instead.

// AXIS3 ROTATOR ------------------------------- see https://onstep.groups.io/g/main/wiki/Configuration---Rotator-and-Focusers#AXIS3
#define AXIS3_DRIVER_MODEL            OFF //    OFF, Enter motor driver model (above) to activate the rotator.                Option
#define AXIS3_DRIVER_MICROSTEPS       OFF //    OFF, n. Microstep mode when tracking.                                         Option
#define AXIS3_DRIVER_STATUS           OFF //    OFF, ON, HIGH, or LOW.  For driver status info/fault detection.               Option

// for TMC2130, TMC5160, and TMC2209U STEP/DIR driver models:
#define AXIS3_DRIVER_IHOLD            OFF //    OFF, n, (mA.) Current during standstill. OFF uses IRUN/2.0                    Option
#define AXIS3_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current during tracking, appropriate for stepper/driver/etc.    Option
#define AXIS3_DRIVER_IGOTO            OFF //    OFF, n, (mA.) Current during slews. OFF uses IRUN.                            Option

#define AXIS3_STEPS_PER_DEGREE       64.0 //   64.0, n. Number of steps per degree for rotator/de-rotator.                    Adjust
                                          //         Alt/Az de-rotation: n = (num_circumference_pixels * 2)/360, minimum
#define AXIS3_SLEW_RATE_DESIRED       1.0 //    1.0, n, (degrees/second) Maximum speed depends on processor.                  Adjust
#define AXIS3_REVERSE                 OFF //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.    Option

// =================================================================================================================================
// FOCUSERS ========================================================================================================================

// Driver models (Step/Dir and Servo) see "~/OnStepX/Constants.h" for a complete list.
// Typically: A4988, DRV8825, LV8729, SSS TMC2100, TMC2130*
// Settings for TMC SPI and SERVO drivers are in Extended.config.h
// * = SSS TMC2130 set Vref to 2.5V (IRUN defaults to 300mA.)

// Settings for driver Microsteps, IRUN, Reverse, Limit Min, and Limit Max are stored in NV (EEPROM.) These runtime settings
// can be changed (or reverted to the defaults below) from the SmartWebServer's Config webpage.  If runtime axis settings are
// enabled changes to these settings below may be ignored as runtime settings from NV (EEPROM) are used instead.

// AXIS4 FOCUSER 1 -------------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#AXIS4
#define AXIS4_DRIVER_MODEL            GENERIC //    OFF, Enter motor driver model (above) to activate the focuser.                Option
#define AXIS4_DRIVER_MICROSTEPS       OFF //    OFF, n. Microstep mode when tracking.                                         Option
#define AXIS4_DRIVER_STATUS           OFF //    OFF, ON, HIGH, or LOW.  For driver status info/fault detection.               Option

// for TMC2130, TMC5160, and TMC2209U STEP/DIR driver models:
#define AXIS4_DRIVER_IHOLD            OFF //    OFF, n, (mA.) Current during standstill. OFF uses IRUN/2.0                    Option
#define AXIS4_DRIVER_IRUN             OFF //    OFF, n, (mA.) Current during tracking, appropriate for stepper/driver/etc.    Option
#define AXIS4_DRIVER_IGOTO            OFF //    OFF, n, (mA.) Current during slews. OFF uses IRUN.                            Option

#define AXIS4_STEPS_PER_MICRON        0.2 //    0.5, n. Steps per micrometer. Figure this out by testing or other means.      Adjust
#define AXIS4_SLEW_RATE_DESIRED       500 //    500, n, Where n=200..5000 (um/s.) Desired (maximum) microns/second.           Adjust
#define AXIS4_REVERSE                 OFF //    OFF, ON Reverses movement direction, or reverse wiring instead to correct.    Option

// AXIS5 FOCUSER 2 -----------------------------------------------------------------------------------------------------------------
// Up to 6 focusers can be present (AXIS4 to AXIS9) simply copy the above text for focuser 1 and rename to AXIS5_ for focuser 2, etc

// =================================================================================================================================
// AUXILIARY FEATURES ==============================================================================================================

// FEATURES ----------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-ConfigurationMaster#AUXILIARY
// Note: Temporarily set DEBUG mode to VERBOSE and use "FEATURE1_TEMP DS1820" to list the DS18B20 device serial numbers.

#define FEATURE1_PURPOSE              SWITCH //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE1_NAME          "FEATURE1" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE1_TEMP                 OFF //    OFF, THERMISTOR or n. Where n is the ds18b20 s/n. For DEW_HEATER temperature. Adjust
#define FEATURE1_PIN                  512 //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust

#define FEATURE2_PURPOSE              SWITCH //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE2_NAME          "FAN" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE2_TEMP                 OFF //    OFF, THERMISTOR or n. Where n is the ds18b20 s/n. For DEW_HEATER temperature. Adjust
#define FEATURE2_PIN                  516 //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust

#define FEATURE3_PURPOSE              SWITCH //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE3_NAME          "FEATURE3" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE3_TEMP                 OFF //    OFF, THERMISTOR or n. Where n is the ds18b20 s/n. For DEW_HEATER temperature. Adjust
#define FEATURE3_PIN                  517 //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust

#define FEATURE4_PURPOSE              OFF //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE4_NAME          "FEATURE4" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE4_TEMP                 OFF //    OFF, THERMISTOR or n. Where n is the ds18b20 s/n. For DEW_HEATER temperature. Adjust
#define FEATURE4_PIN                  OFF //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust

#define FEATURE5_PURPOSE              OFF //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE5_NAME          "FEATURE5" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE5_TEMP                 OFF //    OFF, THERMISTOR or n. Where n is the ds18b20 s/n. For DEW_HEATER temperature. Adjust
#define FEATURE5_PIN                  OFF //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust

#define FEATURE6_PURPOSE              OFF //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE6_NAME          "FEATURE6" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE6_TEMP                 OFF //    OFF, THERMISTOR or n. Where n is the ds18b20 s/n. For DEW_HEATER temperature. Adjust
#define FEATURE6_PIN                  OFF //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust

#define FEATURE7_PURPOSE              OFF //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE7_NAME          "FEATURE7" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE7_TEMP                 OFF //    OFF, THERMISTOR or n. Where n is the ds18b20 s/n. For DEW_HEATER temperature. Adjust
#define FEATURE7_PIN                  OFF //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust

#define FEATURE8_PURPOSE              OFF //    OFF, SWITCH, ANALOG_OUT, DEW_HEATER, INTERVALOMETER.                          Option
#define FEATURE8_NAME          "FEATURE8" // "FE..", Name of feature being controlled.                                        Adjust
#define FEATURE8_TEMP                 OFF //    OFF, THERMISTOR or n. Where n is the ds18b20 s/n. For DEW_HEATER temperature. Adjust
#define FEATURE8_PIN                  OFF //    OFF, AUX for auxiliary pin, n. Where n is the pin#.                           Adjust

// ---------------------------------------------------------------------------------------------------------------------------------
#define FileVersionConfig 5
#include "Extended.config.h"
