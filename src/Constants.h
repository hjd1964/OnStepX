// -----------------------------------------------------------------------------------
// Constants
#pragma once

// Configuration options ----------------------------------------------------------------------------------------------------

// PINMAPS
#define PINMAP_FIRST                1
#define Classic                     1      // original pin-map
#define ClassicShield               2      // " for Steve's shield where ST4 port is on the Mega2560's "alternate pins"
#define ClassicInstein              3      // "  w/Instein ST4 port... this isn't tested, I have no idea if it's safe to use!

#define InsteinESP1                 4      // ESP32 Instein PINMAP (latest variant September/2020)

#define Ramps14                     10     // Ramps v1.4 3D printer shields/boards for Mega2560, a 5-axis design
#define Ramps15                     10     // Ramps v1.5 3D printer shields/boards for Mega2560, a 5-axis design
#define MksGenL                     11     // Ramps clone, all-in-one
#define MksGenL1                    11     // Ramps clone, all-in-one
#define MksGenL2                    12     // as above, SPI bus to all stepper drivers
#define MksGenL21                   13     // as above, SPI bus to all stepper drivers, the best Mega2560 based board

#define FYSETC_S6                   15     // FYSETC S6 Version 1.2, 3D printer board, a 6-axis design
#define FYSETC_S6_1                 15     // FYSETC S6 Version 1.2
#define FYSETC_S6_2                 16     // FYSETC S6 Version 2.0, 3D printer board, a 6-axis design

#define MiniPCB                     20     // small 2-axis design for embedded or mounting behind a panel, Teensy3.2
#define MiniPCB13                   21     // improved version 1.3 adds better support for ESP-01 flashing and optional I2C

#define MiniPCB2                    30     // 2-axis design for small alum. case, Teensy3.2

#define MaxPCB                      40     // first generation custom 4-axis board, Teensy3.5/Teensy3.6
#define MaxPCB2                     41     // improved second generation for alum. case, Teensy3.5/Teensy3.6
#define MaxPCB3                     42     // improved third generation for alum. case, Teensy4.1
#define MaxSTM3                     43     // update to the MaxPCB3 using an Blackpill F411CE instead of the Teensy3.5/3.6
#define MaxSTM3I                    44     // as above but using an onboard STM32F411CE with M24C64 EEPROM as default

#define MaxESP2                     50     // similar to MaxPCB2 except 3 axes and uses cheaper/more available ESP32 MCU
#define MaxESP3                     51     // adds 4th axis and option to flash the WeMos D1 Mini WiFi through OnStep
#define GregsESP32                  52
#define CNC3                        53     // Arduino CNC Sheild on WeMos D1 R32 (ESP32)

#define STM32Blue                   60     // Khalid and Dave's PCB for STM32 Blue pill (STM32F103CB and STM32F303CC)

#define PINMAP_LAST                 60

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
#define MOUNT_TYPE_LAST             3

// MOUNT COORDS
#define MOUNT_COORDS_FIRST          1
#define OBSERVED_PLACE              1
#define TOPOCENTRIC                 2
#define TOPO_STRICT                 3
#define ASTROMETRIC_J2000           4
#define MOUNT_COORDS_LAST           4

// DRIVER (step/direction interface, usually for stepper motors)
#define DRIVER_FIRST                0
#define A4988                       0      // allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x
#define DRV8825                     1      // allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x
#define S109                        2      // allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x
#define LV8729                      3      // allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,64x,128x
#define RAPS128                     4      // allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,64x,128x
#define TMC2100                     5      // allows M0,M1    bit patterens for 1x,2x,4x,16x   (spreadCycle only, no 256x intpol)
#define TMC2208                     6      // allows M0,M1    bit patterens for 2x,4x,8x,16x   (stealthChop default, uses 256x intpol)
#define TMC2209                     7      // allows M0,M1    bit patterens for 8x,16x,32x,64x (M2 sets spreadCycle/stealthChop, uses 256x intpol)
#define ST820                       8      // allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,128x,256x
#define TMC2130                     9      // uses TMC protocol SPI comms   for 1x,2x...,256x  (SPI sets spreadCycle/stealthChop etc.)
#define TMC5160                     10     // uses TMC protocol SPI comms   for 1x,2x...,256x  (SPI sets spreadCycle/stealthChop etc.)
#define GENERIC                     11     // generic s/d driver allows     for 1x,2x,4x,8x,16x,32x,64x,128x,256x (no mode switching)
#define DRIVER_LAST                 11

// DRIVER DECAY MODE
#define DRIVER_DECAY_MODE_FIRST     1
#define MIXED                       2
#define FAST                        3
#define SLOW                        4
#define SPREADCYCLE                 5
#define STEALTHCHOP                 6
#define DRIVER_DECAY_MODE_LAST      6

// SERVO DRIVER (usually for DC motors equipped with encoders)
#define SERVO_DRIVER_FIRST          100
#define SERVO_PE                    100    // SERVO, direction (phase) and enable (pwm) connections
#define SERVO_II                    101    // SERVO, dual pwm input connections
#define SERVO_DRIVER_LAST           101

// SERVO ENCODER (must match Encoder library)
#define SERVO_ENCODER_FIRST         1
#define ENC_AB                      1      // AB quadrature encoder
#define ENC_CW_CCW                  2      // clockwise/counter-clockwise encoder
#define ENC_PULSE_DIR               3      // pulse/direction encoder
#define ENC_PULSE_ONLY              4      // pulse only encoder
#define SERVO_ENCODER_LAST          4

// servo feedback (must match Encoder library)
#define SERVO_FEEDBACK_FIRST        1
#define FB_PID                      1      // PID feedback
#define SERVO_FEEDBACK_LAST         1

// TIME LOCATION SOURCE devices supported
#define TLS_FIRST                   1
#define DS3231                      1      // DS3231 RTC on I2C
#define DS3234                      2      // DS3234 RTC on SPI (DS3234_CS_PIN) Makuna library
#define TEENSY                      3      // TEENSY3.2 RTC (Built-in)
#define GPS                         4      // GPS device
#define TLS_LAST                    4

// PIER SIDE
#define PIER_SIDE_FIRST             1
#define EAST                        1      // same as PSS_EAST
#define WEST                        2      // same as PSS_WEST
#define BEST                        3      // same as PSS_BEST
#define PIER_SIDE_LAST              3

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
#define ANALOG_OUT                  2
#define DEW_HEATER                  3      // control an dew heater
#define INTERVALOMETER              4      // control an camera shutter
#define AUX_FEATURE_PURPOSE_LAST    4

// GPIO devices
// these can work for most digital I/O EXCEPT: STEP/DIR, 1-WIRE/I2C/SPI (CS is ok), the ST4 port, and the PPS pin
#define GPIO_FIRST                  1
#define DS2413                      1      // DS2413 2-channel GPIO for dew heaters etc. pin# 1000 and 1001
#define MCP23008                    2      // MCP23008 8-channel GPIO for dew heaters etc. pin# 1000 to 1007
#define MCP23017                    3      // MCP23017 16-channel GPIO for dew heaters etc. pin# 1000 to 1015
#define GPIO_LAST                   3

// --------------------------------------------------------------------------------------------------------------------------

// a short string describing this product
#define PROD_ABV                    "OnStepX"

// task manager
#define TASKS_MAX                   32     // up to 32 tasks
#define TASKS_SKIP_MISSED
#define TASKS_HWTIMER1_ENABLE
#define TASKS_HWTIMER2_ENABLE
#define TASKS_HWTIMER3_ENABLE

// enable library features
#define SERIAL_LOCAL_PRESENT
#define SERIAL_ST4_SERVER_PRESENT

// NV -------------------------------------------------------------------------------------------------------------------
#define INIT_NV_KEY                 583927929UL

#define NV_KEY                      0      // bytes: 4   , 4
#define NV_SITE_NUMBER              4      // bytes: 1   , 1
#define NV_SITE_BASE                5      // bytes: 40*4, 160
#define NV_SITE_JD_BASE             165    // bytes: 16  , 16

#define NV_MOUNT_SETTINGS_BASE      181    // bytes: 10  , 10
#define NV_MOUNT_TYPE_BASE          191    // bytes: 1   , 1
#define NV_MOUNT_GOTO_BASE          192    // bytes: 5   , 5
#define NV_MOUNT_GUIDE_BASE         197    // bytes: 3   , 3
#define NV_MOUNT_LIMITS_BASE        200    // bytes: 16  , 16
#define NV_MOUNT_PARK_BASE          216    // bytes: 15  , 15
#define NV_MOUNT_PEC_BASE           231    // bytes: 6   , 6
#define NV_MOUNT_STATUS_BASE        237    // bytes: 1   , 1

#define NV_ALIGN_MODEL_BASE         238    // bytes: 32  , 32
#define NV_AXIS_SETTINGS_REVERT     270    // bytes: 2   , 2
#define NV_AXIS_SETTINGS_BASE       272    // bytes: 45*9, 405
#define NV_FOCUSER_SETTINGS_BASE    677    // bytes: 18*9, 162
#define NV_ROTATOR_SETTINGS_BASE    839    // bytes: 7   , 7
#define NV_FEATURE_SETTINGS_BASE    846    // bytes: 3 *8, 24
#define NV_PEC_BUFFER_BASE          870    // Bytes: ?   , ? + (PEC_BUFFER_SIZE_LIMIT - 1)
