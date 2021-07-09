// -----------------------------------------------------------------------------------
// Constants
#pragma once

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
#define SAME                        -1
#define HALF                        -1
#define AUTO                        -1
#define ON                          -2
#define ON_BOTH                     -3
#define ON_PULLUP                   -4
#define ON_PULLDOWN                 -5
#define TMC_SPI                     -6
#define SLAVE                       -7
#define ACCESS_POINT                -8
#define STATION                     -9
#define STATION_DHCP                -10

// pier side
#define EAST                        1      // same as PSS_EAST
#define WEST                        2      // same as PSS_WEST
#define BEST                        3      // same as PSS_BEST

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

// various Time and Location sources supported
#define DS3231                      1 // DS3231 RTC on I2C
#define DS3234                      2 // DS3234 RTC on SPI (DS3234_CS_PIN) Makuna library
#define TEENSY                      4 // TEENSY3.2 RTC (Built-in)
#define GPS                         5 // GPS device

// various Weather sensors supported
#define BME280                      1 // BME280 on I2C (at default address 0x77)
#define BME280_0x77                 1 // BME280 on I2C (at address 0x77)
#define BME280_0x76                 2 // BME280 on I2C (at address 0x76)
#define BME280_SPI                  3 // BME280 on SPI (default CS)
#define BMP280                      4 // BMP280 on I2C (at default address 0x77)
#define BMP280_0x77                 4 // BMP280 on I2C (at address 0x77)
#define BMP280_0x76                 5 // BMP280 on I2C (at address 0x76)
#define BMP280_SPI                  6 // BMP280 on SPI (default CS)

// auxiliary features
#define SWITCH                      1 // control an simple on/off switch
#define ANALOG_OUTPUT               2 // control an analog (pwm) output, depends on MCU support
#define ANALOG_OUT                  2
#define DEW_HEATER                  3 // control an dew heater
#define INTERVALOMETER              4 // control an camera shutter
#define SWITCH_UNPARKED             5

// various Dallas/Maxim 1-wire devices supported
#define DS1820     0x2800000000000000 // DS1820 "generic" temperature sensors (DS18B20 or DS18S20) for focusing and dew heaters
#define DS2413     0x3A00000000000000 // DS2413 GPIO pins for dew heaters
#define CHAIN      0x3A00000000000001 // DS2413 second GPIO

#define DS_MASK    0x3F00000000000000 // not for use in Config.h
#define DS18S20    0x1000000000000000 // not for use in Config.h

// Macros --------------------------------------------------------------------------------------------------------------------------

// misc. math
#define RAD_DEG_RATIO_L             57.29577951308232L
#define RAD_DEG_RATIO               57.29577951308232
#define RAD_DEG_RATIO_F             57.295780F
#define RAD_HOUR_RATIO_L            3.819718634205488L
#define RAD_HOUR_RATIO              3.819718634205488
#define RAD_HOUR_RATIO_F            3.8197186F
#define SIDEREAL_RATIO_L            1.002737909350795L
#define SIDEREAL_RATIO              1.002737909350795
#define SIDEREAL_RATIO_F            1.0027379F
#define SIDEREAL_PERIOD             15956313.06126534
#define SIDEREAL_RATE_HZ            60.16427456104770
#define Deg10                       0.174532925199432
#define Deg20                       0.349065850398865
#define Deg45                       0.785398163397448
#define Deg60                       1.047197551196597
#define Deg85                       1.483529864195180
#define Deg90                       1.570796326794896
#define Deg180                      3.141592653589793
#define Deg360                      6.283185307179586

// for handling degenerate spherical coordinates near the poles
#define TenthArcSec                 0.00000048481368
#define OneArcSec                   0.0000048481368F
#define SmallestFloat               0.0000001F

// conversion math
#define degToRad(x)                 ((x)/RAD_DEG_RATIO)
#define radToDeg(x)                 ((x)*RAD_DEG_RATIO)
#define hrsToRad(x)                 ((x)/RAD_HOUR_RATIO)
#define radToHrs(x)                 ((x)*RAD_HOUR_RATIO)
#define csToRad(x)                  ((x)/1375098.708313976)
#define radToCs(x)                  ((x)*1375098.708313976)
#define csToHours(x)                ((x)/360000.0)
#define hoursToCs(x)                ((x)*360000.0)
#define csToDays(x)                 ((x)/8640000.0)
#define daysToCs(x)                 ((x)*8640000.0)
#define arcsecToRad(x)              ((x)/206264.8062470963)
#define radToArcsec(x)              ((x)*206264.8062470963)
#define siderealToRad(x)            ((x)/13750.98708313975)
#define siderealToRadF(x)           ((x)/13750.987F)
#define radToSidereal(x)            ((x)*13750.98708313975)
//#define siderealToRad(x)            ((((x)*SIDEREAL_RATIO*15.0)/3600.0)/RAD)
// conversion factor to go to/from Hz for sidereal interval
#define hzToSubMicros(x)            ((x)*266666.666666667)
#define hzToSidereal(x)             ((x)/SIDEREAL_RATE_HZ)
#define siderealToHz(x)             ((x)*SIDEREAL_RATE_HZ)
#define fequal(x,y)                 (fabs((x)-(y))<SmallestFloat)
#define fgt(x,y)                    ((x)-(y)>SmallestFloat)
#define flt(x,y)                    ((y)-(x)>SmallestFloat)

// pins
#define pinModeEx(pin,mode)           { if (pin != OFF && pin != SHARED) { pinMode(pin,mode); } }
#define pinModeInitEx(pin,mode,state) { pinModeEx(pin,mode); digitalWrite(pin,state); }
#define digitalWriteEx(pin,value)     { if (pin != OFF && pin != SHARED) digitalWrite(pin,value); }
//#define digitalReadEx(pin)            ( ?(pin != OFF && pin != SHARED):digitalRead(pin):0 )
#define digitalReadEx(pin)            ( digitalRead(pin) )
#ifdef HAL_HAS_DIGITAL_FAST
  #define digitalReadF(pin)           ( digitalReadFast(pin) )
  #define digitalWriteF(pin,value)    { digitalWriteFast(pin,value); }
#else
  #define digitalReadF(pin)           ( digitalRead(pin) )
  #define digitalWriteF(pin,value)    { digitalWrite(pin,value); }
#endif

#define THLD(v)                       ((v)<<1)  // 10 bit analog threshold, bits 1 through 10
#define HYST(v)                       ((v)<<11) // 10 bit hysteresis, bits 11 through 20
#ifndef INPUT_PULLDOWN
  #define INPUT_PULLDOWN INPUT
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
// command of 4 chars with parameter
#define cmdP2(a) (command[0] == a[0] && command[1] == a[1] && parameter[0] == a[2] && parameter[1] == a[3])
// GX command of 3 chars, with end of string check
#define cmdGX(a) (command[0] == a[0] && command[1] == a[1] && parameter[0] == a[2] && parameter[2] == 0)
// SX, command of 3 chars, comma, then parameter
#define cmdSX(a) (command[0] == a[0] && command[1] == a[1] && parameter[0] == a[2] && parameter[2] == ',')

// task manager
#define TASKS_SKIP_MISSED
#define TASKS_HWTIMER1_ENABLE              // only the Mega2560 hardware timers are tested and seem to work
#define TASKS_HWTIMER2_ENABLE              // if the Teensy, etc. don't work comment these out to use the
#define TASKS_HWTIMER3_ENABLE              // software task scheduler instead

// NV write endurance constants
// low (< 100K writes)
#define NVE_LOW 0
// mid (~ 100K writes)
#define NVE_MID 1
// high (~ 1M writes)
#define NVE_HIGH 2
// very high (> 1M writes)
#define NVE_VHIGH 3
#define NV_ENDURANCE NVE_MID

// default time for spiral guides is 103.4 seconds
#define GUIDE_SPIRAL_TIME_LIMIT 103.4

// NV addresses
#define INIT_NV_KEY                 583927927UL

#define NV_KEY                      0      // bytes: 4   , addr:   0..  3
#define NV_SITE_NUMBER              4      // bytes: 1   , addr:   4..  4
#define NV_SITE_BASE                5      // bytes: 40*4, addr:   5..164
#define NV_SITE_JD_BASE             165    // bytes: 16  , addr: 165..180
#define NV_MOUNT_LIMITS_BASE        181    // bytes: 16  , addr: 181..196
#define NV_MOUNT_MISC_BASE          197    // bytes: 14  , addr: 197..210
#define NV_MOUNT_PEC_BASE           211    // bytes: 6   , addr: 211..216
#define NV_MOUNT_PARK_BASE          217    // bytes: 15  , addr: 217..231
#define NV_MOUNT_TYPE_BASE          232    // bytes: 1   , addr: 232..232
#define NV_ALIGN_MODEL_BASE         233    // bytes: 32  , addr: 233..264
#define NV_AXIS_SETTINGS_REVERT     264    // bytes: 2   , addr: 265..266
#define NV_AXIS_SETTINGS_BASE       267    // bytes: 21*9, addr: 267..455
#define NV_FOCUSER_SETTINGS_BASE    456    // bytes: 18*9, addr: 456..617
#define NV_ROTATOR_SETTINGS_BASE    618    // bytes: 2   , addr: 618..619
#define NV_PEC_BUFFER_BASE          650    // Bytes: ?   , addr: 650..650 + (PEC_BUFFER_SIZE_LIMIT - 1)
