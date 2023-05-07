// -----------------------------------------------------------------------------------
// Constants
#pragma once

// Configuration options ----------------------------------------------------------------------------------------------------

// PINMAPS
#define PINMAP_FIRST                1

#define FYSETC_E4                   1      // FYSETC E4 Version 1.0, *** EXPERIMENTAL, MAY BE REMOVED AT ANY TIME ***

#define FYSETC_S6                   2      // FYSETC S6 Version 1.2, 3D printer board, a 6-axis design
#define FYSETC_S6_1                 3      // FYSETC S6 Version 1.2
#define FYSETC_S6_2                 4      // FYSETC S6 Version 2.0, 3D printer board, a 6-axis design

#define MiniPCB                     5      // small 2-axis design for embedded or mounting behind a panel, Teensy3.2
#define MiniPCB13                   6      // improved version 1.3 adds better support for ESP-01 flashing and optional I2C

#define MiniPCB2                    7      // 2-axis design for small alum. case, Teensy3.2

#define MaxPCB                      8      // first generation custom 4-axis board, Teensy3.5/Teensy3.6
#define MaxPCB2                     9      // improved second generation for alum. case, Teensy3.5/Teensy3.6
#define MaxPCB3                     10     // improved third generation for alum. case, Teensy4.1
#define MaxPCB4                     11     // for Teensy4.1 w/TMC2209 support
#define MaxSTM3                     12     // update to the MaxPCB3 using an Blackpill F411CE instead of the Teensy3.5/3.6
#define MaxSTM3I                    13     // as above but using an onboard STM32F411CE with M24C64 EEPROM as default

#define MaxESP3                     14     // adds 4th axis and option to flash the WeMos D1 Mini WiFi through OnStep
#define MaxESP4                     15     // for ESP32S w/TMC2209 support
#define CNC3                        16     // Arduino CNC Sheild on WeMos D1 R32 (ESP32)
#define MicroScope                  17     // MicroScope PCB (ESP32, experimental and may be removed at any point!, USE AY YOUR OWN RISK!!!)

#define STM32Blue                   18     // Khalid and Dave's PCB for STM32 Blue pill (STM32F103CB and STM32F303CC)

#define PINMAP_LAST                 18

// WEATHER sensors (temperature, pressure, and humidity)
#define WEATHER_FIRST               1
#define BME280                      1      // BME280 on I2C (at default address 0x77)
#define BME280_0x77                 1      // BME280 on I2C (at address 0x77)
#define BME280_0x76                 2      // BME280 on I2C (at address 0x76)
#define BME280_SPI                  3      // BME280 on SPI (default CS)
#define BMP280                      4      // BMP280 on I2C (at default address 0x77)
#define BMP280_0x77                 4      // BMP280 on I2C (at address 0x77)
#define BMP280_0x76                 5      // BMP280 on I2C (at address 0x76)
#define BMP280_SPI                  6      // BMP280 on SPI (default CS)
#define WEATHER_LAST                6

// STEP WAVE FORM
#define STEP_WAVE_FORM_FIRST        1
#define SQUARE                      1
#define PULSE                       2
#define STEP_WAVE_FORM_LAST         2

// MOUNT TYPE
#define MOUNT_TYPE_FIRST            1
#define GEM                         1      // German Equatorial Mount, meridian flips enabled
#define FORK                        2      // Fork Mount, meridian flips disabled
#define ALTAZM                      3      // Altitude Azimuth Mounts, Dobsonians, etc.
#define GEM_TA                      4      // GEM, w/tangent arm Declination
#define GEM_TAC                     5      // GEM, w/tangent arm Declination and geometry correction
#define FORK_TA                     6      // FORK, w/tangent arm Declination
#define FORK_TAC                    7      // FORK, w/tangent arm Declination and geometry correction
#define ALTAZM_UNL                  8      // ALTAZM, w/unlimited Azmiuth motion
#define MOUNT_TYPE_LAST             8

// MOUNT COORDS
#define MOUNT_COORDS_FIRST          1
#define OBSERVED_PLACE              1
#define TOPOCENTRIC                 2
#define TOPO_STRICT                 3
#define ASTROMETRIC_J2000           4
#define MOUNT_COORDS_LAST           4

// TIME LOCATION SOURCE devices supported
#define TLS_FIRST                   1
#define DS3231                      1      // DS3231 RTC on I2C
#define DS3234                      2      // DS3234 RTC on SPI (DS3234_CS_PIN) Makuna library
#define SD3031                      3      // SD3031 RTC on I2C
#define TEENSY                      4      // TEENSY3.2 RTC (Built-in)
#define GPS                         5      // GPS device
#define NTP                         6      // NTP
#define TLS_LAST                    6

// PIER SIDE
#define PIER_SIDE_FIRST             1
#define EAST                        1      // same as PSS_EAST
#define WEST                        2      // same as PSS_WEST
#define BEST                        3      // same as PSS_BEST
#define PIER_SIDE_LAST              3

// COMPENSATED TRACKING
#define COMPENSATED_TRACKING_FIRST  1
#define REFRACTION                  1      // refraction compensated tracking RA only
#define REFRACTION_DUAL             2      // refraction compensated tracking both axes
#define MODEL                       3      // pointing model compensated tracking RA only
#define MODEL_DUAL                  4      // pointing model compensated tracking both axes
#define COMPENSATED_TRACKING_LAST   4

// TEMPERATURE sensing devices
#define TEMPERATURE_FIRST           1
#define DS1820     0x2800000000000000      // DS18B20 1-wire temperature sensors for focusing and dew heaters
#define DS18B20    0x2800000000000000      // as above
#define DS18S20    0x1000000000000000      // as above, except for DS18S20
#define DS_MASK    0x3F00000000000000      // not for use in Config.h
#define THERMISTOR                  1      // General purpose thermistor sensor, type 1 (see Config.defaults.h)
#define THERMISTOR1                 1      // as above
#define THERMISTOR2                 2      // General purpose thermistor sensor, type 2 (see Config.defaults.h)
#define TEMPERATURE_LAST            2

// AUXILIARY FEATURE purpose
#define AUX_FEATURE_PURPOSE_FIRST   1
#define SWITCH                      1      // control an simple on/off switch
#define ANALOG_OUTPUT               2      // control an analog (pwm) output, depends on MCU support
#define ANALOG_OUT                  2      // as above
#define DEW_HEATER                  3      // control an dew heater
#define INTERVALOMETER              4      // control an camera shutter
#define MOMENTARY_SWITCH            5      // control an simple momentary on/off switch
#define HIDDEN_SWITCH               6      // control an hidden on/off switch (for controlling a pin state at boot)
#define AUX_FEATURE_PURPOSE_LAST    6

// --------------------------------------------------------------------------------------------------------------------------

// a short string describing this product
#define PROD_ABV                    "OnStepX"

// task manager
#define TASKS_MAX                   48     // up to 48 tasks
#define TASKS_SKIP_MISSED                  // just skip missed tasks if too late
#define TASKS_HWTIMERS              3      // up to 3 hardware timers

// default start of axis class hardware timers
#define AXIS_HARDWARE_TIMER_BASE    2      // in the OnStepX timer#1 is the sidereal clock

// enable library features
#define SERIAL_LOCAL_PRESENT
#define SERIAL_ST4_SERVER_PRESENT

// NV -------------------------------------------------------------------------------------------------------------------
#define INIT_NV_KEY                 583928934UL

#define NV_KEY                      0      // bytes: 4   , 4
#define NV_SITE_NUMBER              4      // bytes: 1   , 1
#define NV_SITE_BASE                5      // bytes: 40*4, 160
#define NV_SITE_JD_BASE             165    // bytes: 16  , 16

#define NV_MOUNT_SETTINGS_BASE      181    // bytes: 9   , 9
#define NV_MOUNT_TYPE_BASE          190    // bytes: 1   , 1
#define NV_MOUNT_GOTO_BASE          191    // bytes: 6   , 6
#define NV_MOUNT_GUIDE_BASE         197    // bytes: 3   , 3
#define NV_MOUNT_LIMITS_BASE        200    // bytes: 16  , 16
#define NV_MOUNT_PARK_BASE          216    // bytes: 15  , 15
#define NV_MOUNT_PEC_BASE           231    // bytes: 6   , 6
#define NV_MOUNT_STATUS_BASE        237    // bytes: 1   , 1

#define NV_ALIGN_MODEL_BASE         238    // bytes: 32  , 32
#define NV_AXIS_SETTINGS_REVERT     270    // bytes: 2   , 2
#define NV_AXIS_SETTINGS_BASE       272    // bytes: 45*9, 405
#define NV_FOCUSER_SETTINGS_BASE    677    // bytes: 20*6, 120
#define NV_ROTATOR_SETTINGS_BASE    797    // bytes: 11  , 11
#define NV_FEATURE_SETTINGS_BASE    808    // bytes: 3 *8, 24
#define NV_TELESCOPE_SETTINGS_BASE  832    // bytes: 2   , 2

#define NV_LAST                     833
