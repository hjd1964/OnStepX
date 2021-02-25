// -----------------------------------------------------------------------------------
// Constants
#pragma once

// Firmware version ----------------------------------------------------------------------------------------------------------------
#define FirmwareName                "OnStepX"
#define FirmwareVersionMajor        10
#define FirmwareVersionMinor        02     // minor version 00 to 99
#define FirmwareVersionPatch        "a"    // for example major.minor patch: 10.03c
#define FirmwareVersionConfig       1      // internal, for tracking configuration file changes
#define FirmwareDate                __DATE__
#define FirmwareTime                __TIME__

// Configuration options -----------------------------------------------------------------------------------------------------------

// Pinmaps
#define PINMAP_FIRST                1
#define Classic                     1      // Original pin-map
#define ClassicShield               2      // " for Steve's shield where ST4 port is on the Mega2560's "alternate pins"
#define ClassicInstein              3      // "  w/Instein ST4 port... this isn't tested, I have no idea if it's safe to use!
#define InsteinESP1                 4      // ESP32 Instein PINMAP (latest variant September/2020)

// RAMPS - 3D printer shields/boards that work with OnStep, all are 5-axis designs
#define Ramps14                     10     // Ramps v1.4 shield for Mega2560
#define Ramps15                     10     // Ramps v1.5
#define MksGenL                     11     // Like Ramps above but better, Mega2560 built-in, crystal oscillator, 24V support
#define MksGenL1                    11
#define MksGenL2                    12     // Adds SPI bus to all stepper drivers (TMC only)
#define MksGenL21                   13     // As above except Focuser2 CS is on pin12

// FYSETC S6 - 3D printer board that works with OnStep, a 6-axis design with 5-axes supported
#define FYSETC_S6                   15     // FYSETC S6 Version 1.2
#define FYSETC_S6_1                 15     // FYSETC S6 Version 1.2
#define FYSETC_S6_2                 16     // FYSETC S6 Version 2.0

// Mini - Small 2-axis design suitable for embedded or mounting behind a panel can even be built with connectors up
#define MiniPCB                     20     // All for Teensy3.2
#define MiniPCB13                   21     // MiniPCB v1.3 adds better support for ESP-01 flashing and optional I2C

// Mini2 - Small 2-axis design for aluminum mini-case
#define MiniPCB2                    30     // Teensy3.2

// Max - Larger 4-axis design suitable for embedded or mounting behind a panel
#define MaxPCB                      40     // First custom "full feature" board. Teensy3.5/Teensy3.6

// Max2 & Max3 - Larger 4-axis design for aluminum case
#define MaxPCB2                     41     // Improved "full feature" board, Teensy3.5/Teensy3.6
#define MaxPCB3                     42     // Adds SPI bus to all stepper drivers (TMC only,) flashes WeMos D1 Mini through OnStep
#define MaxSTM3                     43     // Update to the MaxPCB3 using an Blackpill F411CE instead of the Teensy3.5/3.6
#define MaxSTM3I                    44     // As above but using an onboard STM32F411CE with M24C64 EEPROM as default

#define MaxESP2                     50     // Similar to MaxPCB2 except 3 axes and uses cheaper/more available ESP32 MCU
#define MaxESP3                     51     // Adds 4th axis and option to flash the WeMos D1 Mini WiFi through OnStep

// Arduino CNC Sheild on WeMos D2 R32 (ESP32)
#define CNC3                        52     // Similar features to MaxESP3

// Khalid and Dave's PCB for STM32 Blue pill (STM32F303CC)
#define STM32Blue                   60

// Etc.
#define UNO                         70     // Arduino UNO for focuser (etc?) only

#define PINMAP_LAST                 70
// ---------------------------------------------------------------------------------------------------------------------------------


// Settings ------------------------------------------------------------------------------------------------------------------------

// basic values
#define OFF                         -1
#define ON                          -2
#define ON_BOTH                     -3
#define ON_PULLUP                   -4
#define ON_PULLDOWN                 -5

#define EAST                        -10
#define WEST                        -11
#define BEST                        -12

// debug values
#define CONSOLE                     -20
#define PROFILER                    -21

// pins
#define SHARED                      -30

// mount types
#define MOUNT_TYPE_FIRST            1
#define GEM                         1      // German Equatorial Mount, meridian flips enabled
#define FORK                        2      // Fork Mount, meridian flips disabled
#define ALTAZM                      3      // Altitude Azimuth Mounts, Dobsonians, etc.
#define MOUNT_TYPE_LAST             3

// coordinate modes
#define COORD_MODE_FIRST            1
#define OBSERVED_PLACE              1
#define TOPOCENTRIC                 2
#define TOPO_STRICT                 3
#define ASTROMETRIC_J2000           4
#define COORD_MODE_LAST             4

// step modes
#define STEP_MODE_FIRST             1
#define SQUARE                      1
#define PULSE                       2
#define STEP_MODE_LAST              2

// etc.
#define INVALID                     -127

// Mutexes -------------------------------------------------------------------------------------------------------------------------
#define MX_CLOCK_CMD                0
#define MX_TELESCOPE_CMD            1
#define MX_TRANSFORM_CMD            2

// Macros --------------------------------------------------------------------------------------------------------------------------

// misc. math
#define RAD                         57.29577951308232
#define RAD_HOUR_RATIO              3.819718634205488
#define SIDEREAL_RATIO              1.002737909350795
#define SIDEREAL_PERIOD             15956313.06126534
#define SIDEREAL_RATE_HZ            60.16427456104770
#define Deg10                       0.174532925199432
#define Deg20                       0.349065850398865
#define Deg45                       0.785398163397448
#define Deg60                       1.047197551196597
#define Deg90                       1.570796326794896
#define Deg180                      3.141592653589793
#define Deg360                      6.283185307179586

// for handling degenerate spherical coordinates near the poles
#define SmallestRad                 0.000005

// conversion math
#define degToRad(x)                 ((x)/RAD)
#define radToDeg(x)                 ((x)*RAD)
#define hrsToRad(x)                 ((x)/RAD_HOUR_RATIO)
#define radToHrs(x)                 ((x)*RAD_HOUR_RATIO)
#define csToRad(x)                  ((x)/1375098.708313976)
#define radToCs(x)                  ((x)*1375098.708313976)
#define csToHours(x)                ((x)/360000.0)
#define hoursToCs(x)                ((x)*360000.0)
#define csToDays(x)                 ((x)/8640000.0)
#define daysToCs(x)                 ((x)*8640000.0)
#define arcsecToRad(x)              ((x)/(RAD*3600.0))
#define radToArcsec(x)              ((x)*(RAD*3600.0))
#define siderealToRad(x)            ((x)/(RAD*240.0))
#define radToSidereal(x)            ((x)*(RAD*240.0))
//#define siderealToRad(x)            ((((x)*SIDEREAL_RATIO*15.0)/3600.0)/RAD)
// conversion factor to go to/from Hz for sidereal interval
#define hzToSubMicros(x)            ((x)*266666.666666667)    // (x*(16000000.0/60.0))
#define hzToSidereal(x)             ((x)/SIDEREAL_RATE_HZ)
#define siderealToHz(x)             ((x)*SIDEREAL_RATE_HZ)
#define fequal(x,y)                 (fabs((x)-(y))<0.00000001)

// pins
#define pinModeEx(pin,mode)           { if (pin != OFF && pin != SHARED) { pinMode(pin,mode); } }
#define pinModeInitEx(pin,mode,state) { pinModeEx(pin,mode); digitalWrite(pin,state); }
#define digitalWriteEx(pin,value)     { if (pin != OFF && pin != SHARED) digitalWrite(pin,value); }
//#define digitalReadEx(pin)            ( ?(pin != OFF && pin != SHARED):digitalRead(pin):0 )
#define digitalReadEx(pin)            ( digitalRead(pin) )
#ifdef HAL_HAS_DIGITAL_FAST
  #define digitalWriteF(pin,value)    { digitalWriteFast(pin,value); }
#else
  #define digitalWriteF(pin,value)    { digitalWrite(pin,value); }
#endif

// commands
// command of 2 chars
#define cmd(a)  (command[0] == a[0] && command[1] == a[1] && parameter[0] == 0)
// command of 3 chars
#define cmd1(a) (command[0] == a[0] && command[1] == a[1] && parameter[0] == a[2] && parameter[1] == 0)
// command of 4 chars
#define cmd2(a) (command[0] == a[0] && command[1] == a[1] && parameter[0] == a[2] && parameter[1] == a[3] && parameter[2] == 0)
// command of 2 chars with high precision option
#define cmdH(a) (command[0] == a[0] && command[1] == a[1] && (command[2] == 0 || (command[2] == 'H' && command[3] == 0)))
// command of 2 chars with parameter
#define cmdP(a) (command[0] == a[0] && command[1] == a[1])

// task manager
#define TASKS_SKIP_MISSED
#define TASKS_HWTIMER1_ENABLE              // only the Mega2560 hardware timers are tested and seem to work
#define TASKS_HWTIMER2_ENABLE              // if the Teensy, etc. don't work comment these out to use the
#define TASKS_HWTIMER3_ENABLE              // software task scheduler instead
