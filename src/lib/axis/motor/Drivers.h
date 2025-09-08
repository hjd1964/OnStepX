#pragma once

#include <Arduino.h>

typedef struct DriverOutputStatus {
  bool shortToGround;
  bool openLoad;
} DriverOutputStatus;

typedef struct DriverStatus {
  bool active;
  DriverOutputStatus outputA;
  DriverOutputStatus outputB;
  bool overTemperatureWarning;
  bool overTemperature;
  bool standstill;
  bool fault;
} DriverStatus;

// codes for locale specific name generation of certain terms in SWS, etc.
// any AxisParameter can instead just pass a string of up to 18 chars directly
#define AXPN_STEPS_PER_DEG "$1"
#define AXPN_LIMIT_DEGS_MIN "$2"
#define AXPN_LIMIT_DEGS_MAX "$3"
#define AXPN_STEPS_PER_UM "$4"
#define AXPN_LIMIT_UM_MIN "$5"
#define AXPN_LIMIT_UM_MAX "$6"
#define AXPN_REVERSE "$7"
#define AXPN_MICROSTEPS "$8"
#define AXPN_MICROSTEPS_GOTO "$9"
#define AXPN_DECAY_MODE "$10"
#define AXPN_DECAY_MODE_GOTO "$11"
#define AXPN_CURRENT_HOLD "$12"
#define AXPN_CURRENT_RUN "$13"
#define AXPN_CURRENT_GOTO "$14"
#define AXPN_INTERPOLATE "$15"
#define AXPN_PID_P "$16"
#define AXPN_PID_I "$17"
#define AXPN_PID_D "$18"
#define AXPN_PID_SLEWING_P "$19"
#define AXPN_PID_SLEWING_I "$20"
#define AXPN_PID_SLEWING_D "$21"
#define AXPN_RADS_PER_COUNT "$22"

#define AXPN_1 "Steps/degree"
#define AXPN_2 "Min. limit degs"
#define AXPN_3 "Max. limit degs"
#define AXPN_4 "Steps/um"
#define AXPN_5 "Min. limit um"
#define AXPN_6 "Max. limit um"
#define AXPN_7 "Reverse"
#define AXPN_8 "Microsteps"
#define AXPN_9 "Microsteps Goto"
#define AXPN_10 "Decay mode"
#define AXPN_11 "Decay mode Goto"
#define AXPN_12 "mA Hold"
#define AXPN_13 "mA Run"
#define AXPN_14 "mA Goto"
#define AXPN_15 "256x Interpolate"
#define AXPN_16 "P tracking"
#define AXPN_17 "I tracking"
#define AXPN_18 "D tracking"
#define AXPN_19 "P slewing"
#define AXPN_20 "I slewing"
#define AXPN_21 "D slewing"
#define AXPN_22 "Rads/count"

// AXP_FLOAT_RAD and AXP_FLOAT_RAD_INV automatically translate to/from AXP_FLOAT (in degrees) for commands
enum AxisParameterType: uint8_t {AXP_INVALID,
                                 AXP_BOOLEAN, AXP_BOOLEAN_IMMEDIATE,
                                 AXP_INTEGER, AXP_INTEGER_IMMEDIATE,
                                 AXP_FLOAT, AXP_FLOAT_IMMEDIATE,
                                 AXP_FLOAT_RAD, AXP_FLOAT_RAD_INV,
                                 AXP_POW2,
                                 AXP_DECAY};

typedef struct AxisParameter {
  float value;
  float valueNv;
  float valueDefault;
  float min;
  float max;
  AxisParameterType type;
  const char name[18];
} AxisParameter;
