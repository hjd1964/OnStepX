// Library general purpose constants
#pragma once

// misc.
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
#define SoftSerial                  -9
#define HardSerial                  -10
#define CONSOLE                     -11
#define PROFILER                    -12
#define VERBOSE                     -13
#define REMOTE                      -14
#define WIFI                        -15
#define ETHERNET_W5100              -16
#define ETHERNET_W5500              -17
#define SHARED                      -18
#define STANDARD                    -19
#define PERSISTENT                  -20
#define INVALID                     -127

// NV/EEPROM
#define NV_KEY_VALUE                111111111UL
#define NV_DEFAULT                  ON  // use the HAL specified default
#define NVE_LOW                     0   // low (< 100K writes)
#define NVE_MID                     1   // mid (~ 100K writes)
#define NVE_HIGH                    2   // high (~ 1M writes)
#define NVE_VHIGH                   3   // very high (> 1M writes)
#ifndef NV_ENDURANCE
  #define NV_ENDURANCE NVE_MID
#endif

// angular
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
