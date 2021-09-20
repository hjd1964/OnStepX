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
#define AUX                         -3
#define SLAVE                       -4
#define ACCESS_POINT                -5
#define STATION                     -6
#define STATION_DHCP                -7
#define BOTH                        -8

// pier side
#define EAST                        1      // same as PSS_EAST
#define WEST                        2      // same as PSS_WEST
#define BEST                        3      // same as PSS_BEST

// debug values
#define CONSOLE                     -20
#define PROFILER                    -21

// pins
#define SHARED                      -30
#define DAC_PIN(v)                  ((v)+0x100)
#define GPIO_PIN(v)                 ((v)+0x200)

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

// motor drivers
#define SERVO                      -1      // general purpose flag for a SERVO driver motor
#define STEP_DIR                   -2      // general purpose flag for a STEP_DIR driver motor

// step/dir drivers (usually for stepper motors)
#define SD_DRIVER_FIRST             0
#define A4988                       0      // DRIVER, allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x
#define DRV8825                     1      // DRIVER, allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x
#define S109                        2      // DRIVER, allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x
#define LV8729                      3      // DRIVER, allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,64x,128x
#define RAPS128                     4      // DRIVER, allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,64x,128x
#define TMC2100                     5      // DRIVER, allows M0,M1    bit patterens for 1x,2x,4x,16x   (spreadCycle only, no 256x intpol)
#define TMC2208                     6      // DRIVER, allows M0,M1    bit patterens for 2x,4x,8x,16x   (stealthChop default, uses 256x intpol)
#define TMC2209                     7      // DRIVER, allows M0,M1    bit patterens for 8x,16x,32x,64x (M2 sets spreadCycle/stealthChop, uses 256x intpol)
#define ST820                       8      // DRIVER, allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,128x,256x
#define TMC2130                     9      // DRIVER, uses TMC protocol SPI comms   for 1x,2x...,256x  (SPI sets spreadCycle/stealthChop etc.)
#define TMC5160                     10     // DRIVER, uses TMC protocol SPI comms   for 1x,2x...,256x  (SPI sets spreadCycle/stealthChop etc.)
#define GENERIC                     11     // DRIVER, generic s/d driver allows     for 1x,2x,4x,8x,16x,32x,64x,128x,256x (no mode switching)
#define SD_DRIVER_LAST              11

// servo drivers (usually for DC motors equipped with encoders)
#define SERVO_DRIVER_FIRST          100
#define SERVO_PD                    100    // SERVO, pwm and direction connections
#define SERVO_II                    101    // SERVO, dual pwm input connections
#define SERVO_DRIVER_LAST           101

// encoder types (must match Encoder library)
#define ENC_FIRST                   1
#define ENC_AB                      1      // AB quadrature encoder
#define ENC_CW_CCW                  2      // clockwise/counter-clockwise encoder
#define ENC_PULSE_DIR               3      // pulse/direction encoder
#define ENC_PULSE                   4      // pulse only encoder
#define ENC_LAST                    4

// stepper driver decay modes
#define MIXED                       0
#define FAST                        1
#define SLOW                        2
#define SPREADCYCLE                 3
#define STEALTHCHOP                 4

// etc.
#define INVALID                     -127

// various Time and Location sources supported
#define DS3231                      1      // DS3231 RTC on I2C
#define DS3234                      2      // DS3234 RTC on SPI (DS3234_CS_PIN) Makuna library
#define TEENSY                      4      // TEENSY3.2 RTC (Built-in)
#define GPS                         5      // GPS device
#define SoftSerial                  6      // placeholder for a software serial port object
#define HardSerial                  7      // placeholder for a hardware serial port object

// various Weather sensors supported
#define BME280                      1      // BME280 on I2C (at default address 0x77)
#define BME280_0x77                 1      // BME280 on I2C (at address 0x77)
#define BME280_0x76                 2      // BME280 on I2C (at address 0x76)
#define BME280_SPI                  3      // BME280 on SPI (default CS)
#define BMP280                      4      // BMP280 on I2C (at default address 0x77)
#define BMP280_0x77                 4      // BMP280 on I2C (at address 0x77)
#define BMP280_0x76                 5      // BMP280 on I2C (at address 0x76)
#define BMP280_SPI                  6      // BMP280 on SPI (default CS)

// auxiliary features
#define SWITCH                      1      // control an simple on/off switch
#define ANALOG_OUTPUT               2      // control an analog (pwm) output, depends on MCU support
#define ANALOG_OUT                  2
#define DEW_HEATER                  3      // control an dew heater
#define INTERVALOMETER              4      // control an camera shutter
#define SWITCH_UNPARKED             5

// various temperature sensing devices
#define DS1820     0x2800000000000000      // DS18B20 1-wire temperature sensors for focusing and dew heaters
#define DS18B20    0x2800000000000000      // (as above)
#define DS18S20    0x1000000000000000      // (as above except for DS18S20)
#define DS_MASK    0x3F00000000000000      // not for use in Config.h
#define THERMISTOR                  1      // General purpose thermistor sensor, type 1 (see Config.common.h)
#define THERMISTOR1                 1      // (as above)
#define THERMISTOR2                 2      // General purpose thermistor sensor, type 2 (see Config.common.h)

// various GPIO devices
// these can work for most digital I/O EXCEPT: STEP/DIR, 1-WIRE/I2C/SPI (CS is ok), the ST4 port, and the PPS pin
#define DS2413                      1      // DS2413 2-channel GPIO for dew heaters etc. pin# 1000 and 1001
#define MCP23008                    2      // MCP23008 8-channel GPIO for dew heaters etc. pin# 1000 to 1007
#define MCP23017                    3      // MCP23017 16-channel GPIO for dew heaters etc. pin# 1000 to 1015

#define DEFAULT_POWER_DOWN_TIME 30000      // default standstill time (in ms) to power down an axis (see AXISn_DRIVER_POWER_DOWN)

#define NV_DEFAULT                 ON      // uses HAL specified default for remembering settings when powered off

// Macros --------------------------------------------------------------------------------------------------------------------------

// misc. math
#define RAD_DEG_RATIO               57.29577951308232L
#define RAD_DEG_RATIO_F             57.295780F
#define RAD_HOUR_RATIO              3.819718634205488L
#define RAD_HOUR_RATIO_F            3.8197186F
#define SIDEREAL_RATIO              1.002737909350795L
#define SIDEREAL_RATIO_F            1.0027379F
#define SIDEREAL_PERIOD             15956313.06126534L
#define SIDEREAL_RATE_HZ            60.16427456104770L
#define Deg10                       0.174532925199432L
#define Deg20                       0.349065850398865L
#define Deg45                       0.785398163397448L
#define Deg60                       1.047197551196597L
#define Deg85                       1.483529864195180L
#define Deg90                       1.570796326794896L
#define Deg180                      3.141592653589793L
#define Deg360                      6.283185307179586L

// for handling degenerate spherical coordinates near the poles
#define TenthArcSec                 0.000000484813681L
#define OneArcSec                   0.000004848136811L
#define SmallestFloat               0.0000005F

// conversion math
#define degToRad(x)                 ((x)/(double)RAD_DEG_RATIO)
#define degToRadF(x)                ((x)/(double)RAD_DEG_RATIO_F)
#define radToDeg(x)                 ((x)*(double)RAD_DEG_RATIO)
#define radToDegF(x)                ((x)*(double)RAD_DEG_RATIO_F)
#define hrsToRad(x)                 ((x)/(double)RAD_HOUR_RATIO)
#define radToHrs(x)                 ((x)*(double)RAD_HOUR_RATIO)
#define arcsecToRad(x)              ((x)/(double)206264.8062470963L)
#define radToArcsec(x)              ((x)*(double)206264.8062470963L)
#define siderealToRad(x)            ((x)/(double)13750.98708313975L)
#define siderealToRadF(x)           ((x)/13750.987F)
#define radToSidereal(x)            ((x)*(double)13750.98708313975L)
// conversion factor to go to/from Hz for sidereal interval
#define hzToSubMicros(x)            ((x)*(double)266666.666666667L)
#define hzToSidereal(x)             ((x)/(double)SIDEREAL_RATE_HZ)
#define siderealToHz(x)             ((x)*(double)SIDEREAL_RATE_HZ)
#define fequal(x,y)                 (fabs((x)-(y))<SmallestFloat)
#define fgt(x,y)                    ((x)-(y)>SmallestFloat)
#define flt(x,y)                    ((y)-(x)>SmallestFloat)

#define THLD(v)                     ((v)<<1)  // 10 bit analog threshold, bits 1 through 10
#define HYST(v)                     ((v)<<11) // 10 bit hysteresis, bits 11 through 20
#ifndef INPUT_PULLDOWN
  #define INPUT_PULLDOWN INPUT
#endif

// task manager
#define TASKS_SKIP_MISSED
#define TASKS_HWTIMER1_ENABLE
#define TASKS_HWTIMER2_ENABLE
#define TASKS_HWTIMER3_ENABLE

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

// directions in OnStepX
enum Direction: uint8_t {DIR_NONE, DIR_FORWARD, DIR_REVERSE, DIR_BOTH};

// NV addresses
#define INIT_NV_KEY                 583927926UL

#define NV_KEY                      0      // bytes: 4   , addr:   0..  3
#define NV_SITE_NUMBER              4      // bytes: 1   , addr:   4..  4
#define NV_SITE_BASE                5      // bytes: 40*4, addr:   5..164
#define NV_SITE_JD_BASE             165    // bytes: 16  , addr: 165..180

#define NV_MOUNT_SETTINGS_BASE      181    // bytes: 10  , addr: 181..190
#define NV_MOUNT_TYPE_BASE          191    // bytes: 1   , addr: 191..191
#define NV_MOUNT_GOTO_BASE          192    // bytes: 5   , addr: 192..196
#define NV_MOUNT_GUIDE_BASE         197    // bytes: 2   , addr: 197..198
#define NV_MOUNT_LIMITS_BASE        199    // bytes: 16  , addr: 199..214
#define NV_MOUNT_PARK_BASE          215    // bytes: 15  , addr: 215..229
#define NV_MOUNT_PEC_BASE           230    // bytes: 6   , addr: 230..235

#define NV_ALIGN_MODEL_BASE         236    // bytes: 32  , addr: 236..267
#define NV_AXIS_SETTINGS_REVERT     268    // bytes: 2   , addr: 268..269
#define NV_AXIS_SETTINGS_BASE       270    // bytes: 25*9, addr: 270..494
#define NV_FOCUSER_SETTINGS_BASE    495    // bytes: 18*9, addr: 495..656
#define NV_ROTATOR_SETTINGS_BASE    657    // bytes: 7   , addr: 657..663
#define NV_FEATURE_SETTINGS_BASE    664    // bytes: 3 *8, addr: 664..688
#define NV_PEC_BUFFER_BASE          700    // Bytes: ?   , addr: 700..700 + (PEC_BUFFER_SIZE_LIMIT - 1)
