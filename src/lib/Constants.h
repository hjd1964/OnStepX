// Library general purpose constants
#pragma once

// misc.
#define AUTO                        -1
#define OFF                         -1
#define ON                          -2
#define AUX                         -4
#define MASTER                      -5
#define SLAVE                       -6
#define BOTH                        -7
#define SoftSerial                  -8
#define HardSerial                  -9
#define CONSOLE                     -10
#define PROFILER                    -11
#define VERBOSE                     -12
#define REMOTE                      -13
#define WIFI_ACCESS_POINT           -14    // shorthand for easy WIFI mode settings
#define WIFI_STATION                -15    // shorthand for easy WIFI mode settings
#define BLUETOOTH                   -16    // shorthand for easy bluetooth mode settings
#define SHARED                      -17
#define STANDARD                    -18
#define PERSISTENT                  -19
#define ERRORS_ONLY                 -20
#define MINIMUM                     -21
#define MIDDLE                      -22
#define MAXIMUM                     -23
#define INVALID                     -127

// CAN interface options
#define CAN_SAN                     0
#define CAN_ESP32                   1
#define CAN_MCP2515                 2
#define CAN0_TEENSY4                3
#define CAN1_TEENSY4                4
#define CAN2_TEENSY4                5
#define CAN3_TEENSY4                6

// network interface options
#define ETHERNET_FIRST              1
#define ETHERNET_W5100              1
#define ETHERNET_W5500              2
#define ETHERNET_TEENSY41           3
#define ETHERNET_LAST               3
#define WIFI_FIRST                  4
#define WIFI                        4
#define WIFI_LAST                   4

// driver (step/dir interface, usually for stepper motors)
// changes here must be reflected in the StepDirDriver.cpp file, etc.
#define STEP_DIR_DRIVER_FIRST       0
#define A4988                       0      // allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x
#define DRV8825                     1      // allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x
#define GENERIC                     2      // generic s/d driver allows     for 1x,2x,4x,8x,16x,32x,64x,128x,256x (using just the M2 pin)
#define LV8729                      3      // allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,64x,128x
#define RAPS128                     4      // allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,64x,128x
#define S109                        5      // allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x
#define ST820                       6      // allows M0,M1,M2 bit patterens for 1x,2x,4x,8x,16x,32x,128x,256x
#define TMC2100                     7      // allows M0,M1    bit patterens for 1x,2x,4x,16x   (spreadCycle only, no 256x intpol)
#define TMC2130S                    8      // allows M0,M1    bit patterens for 1x,2x,4x,16x   (spreadCycle only, no 256x intpol)
#define TMC2208S                    9      // allows M0,M1    bit patterens for 2x,4x,8x,16x   (stealthChop default, uses 256x intpol)
#define TMC2225S                    10     // allows M0,M1    bit patterens for 4x,8x,16x,32x  (M2 sets spreadCycle/stealthChop, uses 256x intpol)
#define TMC2209S                    11     // allows M0,M1    bit patterens for 8x,16x,32x,64x (M2 sets spreadCycle/stealthChop, uses 256x intpol)
#define TMC2226S                    12     // allows M0,M1    bit patterens for 8x,16x,32x,64x (M2 sets spreadCycle/stealthChop, uses 256x intpol)
#define TMC_DRIVER_FIRST            13
#define TMC2130                     13     // uses TMC software SPI comms   for 1x,2x...,256x  (SPI sets spreadCycle/stealthChop etc.)
#define TMC2160                     14     // uses TMC software SPI comms   for 1x,2x...,256x  (SPI sets spreadCycle/stealthChop etc.)
#define TMC2660                     15     // uses TMC software SPI comms   for 1x,2x...,256x  (SPI sets spreadCycle/stealthChop etc.)
#define TMC5160                     16     // uses TMC software SPI comms   for 1x,2x...,256x  (SPI sets spreadCycle/stealthChop etc.)
#define TMC5161                     17     // uses TMC software SPI comms   for 1x,2x...,256x  (SPI sets spreadCycle/stealthChop etc.)
#define TMC_UART_DRIVER_FIRST       18
#define TMC2208                     18     // uses TMC software UART comms  for 1x,2x...,256x  (UART sets spreadCycle/stealthChop etc, dedicated RX/TX UART to each driver using index 0)
#define TMC2225                     19     // uses TMC software UART comms  for 1x,2x...,256x  (UART sets spreadCycle/stealthChop etc, dedicated RX/TX UART to each driver using index 0)
#define TMC2209                     20     // uses TMC software UART comms  for 1x,2x...,256x  (UART sets spreadCycle/stealthChop etc, dedicated RX/TX UART to each driver using index 0)
#define TMC2226                     21     // uses TMC software UART comms  for 1x,2x...,256x  (UART sets spreadCycle/stealthChop etc, dedicated RX/TX UART to each driver using index 0)
#define STEP_DIR_DRIVER_LAST        21

// driver (step/dir) decay mode
#define DRIVER_DECAY_MODE_FIRST     1
#define MIXED                       1
#define FAST                        2
#define SLOW                        3
#define SPREADCYCLE                 4
#define STEALTHCHOP                 5
#define DRIVER_DECAY_MODE_LAST      6

// servo driver (usually for DC motors equipped with encoders)
// changes here must be reflected in the ServoDriver.cpp file, etc.
#define SERVO_DRIVER_FIRST          100
#define SERVO_PE                    100    // SERVO, direction (phase) and pwm (enable) connections
#define SERVO_EE                    101    // SERVO, dual pwm input (enable/enable) connections
#define SERVO_TMC2130_DC            102    // TMC2130 DC motor control
#define SERVO_TMC5160_DC            103    // TMC5160 DC motor control
#define SERVO_TMC2209               104    // TMC2209 stepper driver using VACTUAL velocity control
#define SERVO_TMC5160               105    // TMC5160 stepper driver using VMAX velocity control
#define SERVO_KTECH                 106    // KTech servo motor using velocity control
#define SERVO_DRIVER_LAST           106

// integrated motor drivers
#define MOTOR_DRIVER_FIRST          200
#define ODRIVE                      200    // First generation ODrive (axis 1 and 2 only)
#define KTECH                       201    // KTech CAN driver (axis 1 and 2 only)
#define MOTOR_DRIVER_LAST           201

// servo encoder (must match Encoder library)
#define ENC_FIRST                   1
#define AB                          1      // AB quadrature encoder
#define AB_ESP32                    2      // AB quadrature encoder (using fast ESP32 hardware decode)
#define CW_CCW                      3      // clockwise/counter-clockwise encoder
#define PULSE_DIR                   4      // pulse/direction encoder
#define PULSE_ONLY                  5      // pulse only encoder (uses hint for direction)
#define VIRTUAL                     6      // virtual encoder (uses hints for velocity and direction)
#define AS37_H39B_B                 7      // Broadcom AS37-H39B-B BISS-C interface encoder
#define JTW_24BIT                   8      // JTW Trident BISS-C interface 24bit encoder
#define JTW_26BIT                   9      // JTW Trident BISS-C interface 26bit encoder
#define LIKA_ASC85                  10     // Lika ASC85 BISS-C interface 25bit encoder (experimental)
#define KTECH_IME                   11     // KTech in motor encoder
#define SERIAL_BRIDGE               12     // serial bridge to encoders
#define ENC_LAST                    12

// encoder filter types
#define ENC_FILT_FIRST              1
#define KALMAN                      1      // more advanced, predictive
#define ROLLING                     2      // basic, rolling average
#define WINDOWING                   3      // basic, average
#define LEARNING                    4      // learning, for RA axis only (experimental may be removed)
#define ENC_FILT_LAST               4

// servo feedback (must match Encoder library)
#define SERVO_FEEDBACK_FIRST        1
#define PID                         1      // PID feedback
#define DUAL_PID                    2      // Dual PID feedback
#define SERVO_FEEDBACK_LAST         2

// driver (step/dir) and servo, misc.
#define ODRIVER                     -10    // general purpose flag for a ODRIVE driver motor
#define SERVO                       -11    // general purpose flag for a SERVO driver motor
#define STEP_DIR                    -12    // general purpose flag for a STEP_DIR driver motor

// NV/EEPROM
#define NV_KEY_VALUE                111111111UL

#define NV_DEF                      0
#define NV_DEFAULT                  0  // PLATFORM SPECIFIC (HAL)
#define NV_EEPROM                   1  // EEPROM.h
#define NV_ESP                      2  // ESP8266/ESP32 EEPROM emulation with commits
#define NV_M0                       3  // Arduino M0 EEPROM emulation with commits
#define NV_2416                     4  // 2KB I2C EEPROM AT DEFAULT ADDRESS 0x50
#define NV_2432                     5  // 4KB I2C EEPROM AT DEFAULT ADDRESS 0x50
#define NV_2464                     6  // 8KB I2C EEPROM AT DEFAULT ADDRESS 0x50
#define NV_24128                    7  // 16KB I2C EEPROM AT DEFAULT ADDRESS 0x50
#define NV_24256                    8  // 32KB I2C EEPROM AT DEFAULT ADDRESS 0x50
#define NV_AT24C32                  9  // 4KB I2C EEPROM AT DEFAULT ADDRESS 0x57 (ZS042 module for instance)
#define NV_MB85RC32                10  // 4KB I2C FRAM AT DEFAULT ADDRESS 0x50
#define NV_MB85RC64                11  // 8KB I2C FRAM AT DEFAULT ADDRESS 0x50
#define NV_MB85RC256               12  // 32KB I2C FRAM AT DEFAULT ADDRESS 0x50

#define NVE_LOW                     0   // low (< 100K writes)
#define NVE_MID                     1   // mid (~ 100K writes)
#define NVE_HIGH                    2   // high (~ 1M writes)
#define NVE_VHIGH                   3   // very high (> 300M writes, FRAM)

// GPIO devices (pin# 512 up to 543)
// these can work for most digital I/O EXCEPT: STEP/DIR, 1-WIRE/I2C/SPI (CS is ok), the ST4 port, and the PPS pin
#define GPIO_FIRST                  1
#define DS2413                      1      // DS2413 2-channel GPIO
#define MCP23008                    2      // MCP23008 8-channel GPIO
#define MCP23017                    3      // MCP23017 16-channel GPIO
#define X9555                       4      // TCA/PCA9555 16-channel GPIO
#define X8574                       5      // PCF8574 8-channel GPIO (up to 4)
#define X8575                       6      // PCF8575 16-channel GPIO
#define SWS                         7      // Smart Web Server 8-channel GPIO (usually on the four Encoder pins, if otherwise unused)
#define SSR74HC595                  8      // 74HC595 8,16,24,32-channel GPIO (serial shift register, output only)
#define GPIO_LAST                   8

// angular
#define RAD_DEG_RATIO               57.29577951308232L
#define RAD_DEG_RATIO_F             57.295780F
#define RAD_HOUR_RATIO              3.819718634205488L
#define RAD_HOUR_RATIO_F            3.8197186F
#define SIDEREAL_RATIO              1.002737909350795L
#define SIDEREAL_RATIO_F            1.0027379F
#define SIDEREAL_PERIOD             15956313.06126534L
#define SIDEREAL_RATE_HZ            60.16427456104770L
#define SOLAR_RATE_HZ               60.0L
#define LUNAR_RATE_HZ               57.9L
#define KING_RATE_HZ                60.136L
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
