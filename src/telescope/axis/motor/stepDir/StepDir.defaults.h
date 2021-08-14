// -----------------------------------------------------------------------------------
// axis step/dir driver defaults
#pragma once
#include "../../../../Common.h"

// check/flag SD stepper drivers
#if AXIS1_DRIVER_MODEL >= SD_DRIVER_FIRST && AXIS1_DRIVER_MODEL <= SD_DRIVER_LAST
  #define AXIS1_DRIVER_SD
#endif
#if AXIS2_DRIVER_MODEL >= SD_DRIVER_FIRST && AXIS2_DRIVER_MODEL <= SD_DRIVER_LAST
  #define AXIS2_DRIVER_SD
#endif
#if AXIS3_DRIVER_MODEL >= SD_DRIVER_FIRST && AXIS3_DRIVER_MODEL <= SD_DRIVER_LAST
  #define AXIS3_DRIVER_SD
#endif
#if AXIS4_DRIVER_MODEL >= SD_DRIVER_FIRST && AXIS4_DRIVER_MODEL <= SD_DRIVER_LAST
  #define AXIS4_DRIVER_SD
#endif
#if AXIS5_DRIVER_MODEL >= SD_DRIVER_FIRST && AXIS5_DRIVER_MODEL <= SD_DRIVER_LAST
  #define AXIS5_DRIVER_SD
#endif
#if AXIS6_DRIVER_MODEL >= SD_DRIVER_FIRST && AXIS6_DRIVER_MODEL <= SD_DRIVER_LAST
  #define AXIS6_DRIVER_SD
#endif
#if AXIS7_DRIVER_MODEL >= SD_DRIVER_FIRST && AXIS7_DRIVER_MODEL <= SD_DRIVER_LAST
  #define AXIS7_DRIVER_SD
#endif
#if AXIS8_DRIVER_MODEL >= SD_DRIVER_FIRST && AXIS8_DRIVER_MODEL <= SD_DRIVER_LAST
  #define AXIS8_DRIVER_SD
#endif
#if AXIS9_DRIVER_MODEL >= SD_DRIVER_FIRST && AXIS9_DRIVER_MODEL <= SD_DRIVER_LAST
  #define AXIS9_DRIVER_SD
#endif

#if defined(AXIS1_DRIVER_SD) || defined(AXIS2_DRIVER_SD) || defined(AXIS3_DRIVER_SD) || defined(AXIS4_DRIVER_SD) || defined(AXIS5_DRIVER_SD) || defined(AXIS6_DRIVER_SD) || defined(AXIS7_DRIVER_SD) || defined(AXIS8_DRIVER_SD) || defined(AXIS9_DRIVER_SD)
  #define SD_DRIVER_PRESENT
#endif

// check/flag TMC stepper drivers
#if AXIS1_DRIVER_MODEL == TMC2130 || AXIS1_DRIVER_MODEL == TMC5160
  #define AXIS1_DRIVER_TMC_SPI
#endif
#if AXIS2_DRIVER_MODEL == TMC2130 || AXIS2_DRIVER_MODEL == TMC5160
  #define AXIS2_DRIVER_TMC_SPI
#endif
#if AXIS3_DRIVER_MODEL == TMC2130 || AXIS3_DRIVER_MODEL == TMC5160
  #define AXIS3_DRIVER_TMC_SPI
#endif
#if AXIS4_DRIVER_MODEL == TMC2130 || AXIS4_DRIVER_MODEL == TMC5160
  #define AXIS4_DRIVER_TMC_SPI
#endif
#if AXIS5_DRIVER_MODEL == TMC2130 || AXIS5_DRIVER_MODEL == TMC5160
  #define AXIS5_DRIVER_TMC_SPI
#endif
#if AXIS6_DRIVER_MODEL == TMC2130 || AXIS6_DRIVER_MODEL == TMC5160
  #define AXIS6_DRIVER_TMC_SPI
#endif
#if AXIS7_DRIVER_MODEL == TMC2130 || AXIS7_DRIVER_MODEL == TMC5160
  #define AXIS7_DRIVER_TMC_SPI
#endif
#if AXIS8_DRIVER_MODEL == TMC2130 || AXIS8_DRIVER_MODEL == TMC5160
  #define AXIS8_DRIVER_TMC_SPI
#endif
#if AXIS9_DRIVER_MODEL == TMC2130 || AXIS9_DRIVER_MODEL == TMC5160
  #define AXIS9_DRIVER_TMC_SPI
#endif

#if defined(AXIS1_DRIVER_TMC_SPI) || defined(AXIS2_DRIVER_TMC_SPI) || defined(AXIS3_DRIVER_TMC_SPI) || \
    defined(AXIS4_DRIVER_TMC_SPI) || defined(AXIS5_DRIVER_TMC_SPI) || defined(AXIS6_DRIVER_TMC_SPI) || \
    defined(AXIS7_DRIVER_TMC_SPI) || defined(AXIS8_DRIVER_TMC_SPI) || defined(AXIS9_DRIVER_TMC_SPI)
  #define TMC_DRIVER_PRESENT
#endif

// common axis driver settings, RA/AZM
#ifdef AXIS1_DRIVER_SD
#ifndef AXIS1_DRIVER_MICROSTEPS
#define AXIS1_DRIVER_MICROSTEPS       OFF  // normal microstep mode 
#endif
#ifndef AXIS1_DRIVER_MICROSTEPS_GOTO
#define AXIS1_DRIVER_MICROSTEPS_GOTO  OFF  // microstep mode to use during slews
#endif
#ifndef AXIS1_DRIVER_DECAY
#define AXIS1_DRIVER_DECAY            OFF  // OFF for default, TMC STEALTHCHOP
#endif
#ifndef AXIS1_DRIVER_DECAY_GOTO
#define AXIS1_DRIVER_DECAY_GOTO       OFF  // OFF for default, TMC SPREADCYCLE
#endif
#ifndef AXIS1_DRIVER_IHOLD
#define AXIS1_DRIVER_IHOLD            HALF // in mA
#endif
#ifndef AXIS1_DRIVER_IRUN
#define AXIS1_DRIVER_IRUN             OFF  // in mA
#endif
#ifndef AXIS1_DRIVER_IGOTO
#define AXIS1_DRIVER_IGOTO            SAME // in mA
#endif
#ifndef AXIS1_DRIVER_STATUS
#define AXIS1_DRIVER_STATUS           OFF  // driver status reporting (ON for TMC SPI or HIGH/LOW for fault pin)
#endif
#ifndef AXIS1_SUBDIVISIONS
#define AXIS1_SUBDIVISIONS            AXIS1_DRIVER_MICROSTEPS
#endif
#ifndef AXIS1_CURRENT
#define AXIS1_CURRENT                 AXIS1_DRIVER_IRUN
#endif
#ifndef AXIS1_ENABLE_STATE
#define AXIS1_ENABLE_STATE            LOW  // default state of ENable pin for motor power on
#endif
#ifndef AXIS1_STEP_STATE
#define AXIS1_STEP_STATE              HIGH // default signal transition state for a step
#endif
#endif

// common axis driver settings, DEC/ALT
#ifdef AXIS2_DRIVER_SD
#ifndef AXIS2_DRIVER_MICROSTEPS
#define AXIS2_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS2_DRIVER_MICROSTEPS_GOTO
#define AXIS2_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS2_DRIVER_DECAY
#define AXIS2_DRIVER_DECAY            OFF
#endif
#ifndef AXIS2_DRIVER_DECAY_GOTO
#define AXIS2_DRIVER_DECAY_GOTO       OFF
#endif
#ifndef AXIS2_DRIVER_IHOLD
#define AXIS2_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS2_DRIVER_IRUN
#define AXIS2_DRIVER_IRUN             OFF
#endif
#ifndef AXIS2_DRIVER_IGOTO
#define AXIS2_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS2_DRIVER_STATUS
#define AXIS2_DRIVER_STATUS           OFF
#endif
#ifndef AXIS2_SUBDIVISIONS
#define AXIS2_SUBDIVISIONS            AXIS2_DRIVER_MICROSTEPS
#endif
#ifndef AXIS2_CURRENT
#define AXIS2_CURRENT                 AXIS2_DRIVER_IRUN
#endif
#ifndef AXIS2_ENABLE_STATE
#define AXIS2_ENABLE_STATE            LOW
#endif
#ifndef AXIS2_STEP_STATE
#define AXIS2_STEP_STATE              HIGH
#endif
#endif

// common axis driver settings, ROTATOR
#ifdef AXIS3_DRIVER_SD
#ifndef AXIS3_DRIVER_MICROSTEPS
#define AXIS3_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS3_DRIVER_MICROSTEPS_GOTO
#define AXIS3_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS3_DRIVER_DECAY
#define AXIS3_DRIVER_DECAY            OFF
#endif
#ifndef AXIS3_DRIVER_DECAY_GOTO
#define AXIS3_DRIVER_DECAY_GOTO       OFF
#endif
#ifndef AXIS3_DRIVER_IHOLD
#define AXIS3_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS3_DRIVER_IRUN
#define AXIS3_DRIVER_IRUN             OFF
#endif
#ifndef AXIS3_DRIVER_IGOTO
#define AXIS3_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS3_DRIVER_STATUS
#define AXIS3_DRIVER_STATUS           OFF
#endif
#ifndef AXIS3_SUBDIVISIONS
#define AXIS3_SUBDIVISIONS            AXIS3_DRIVER_MICROSTEPS
#endif
#ifndef AXIS3_CURRENT
#define AXIS3_CURRENT                 AXIS3_DRIVER_IRUN
#endif
#ifndef AXIS3_ENABLE_STATE
#define AXIS3_ENABLE_STATE            LOW
#endif
#ifndef AXIS3_STEP_STATE
#define AXIS3_STEP_STATE              HIGH
#endif
#endif

// common axis driver settings, FOCUSER1
#ifdef AXIS4_DRIVER_SD
#ifndef AXIS4_DRIVER_MICROSTEPS
#define AXIS4_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS4_DRIVER_MICROSTEPS_GOTO
#define AXIS4_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS4_DRIVER_DECAY
#define AXIS4_DRIVER_DECAY            OFF
#endif
#ifndef AXIS4_DRIVER_DECAY_GOTO
#define AXIS4_DRIVER_DECAY_GOTO       OFF
#endif
#ifndef AXIS4_DRIVER_IHOLD
#define AXIS4_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS4_DRIVER_IRUN
#define AXIS4_DRIVER_IRUN             OFF
#endif
#ifndef AXIS4_DRIVER_IGOTO
#define AXIS4_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS4_DRIVER_STATUS
#define AXIS4_DRIVER_STATUS           OFF
#endif
#ifndef AXIS4_SUBDIVISIONS
#define AXIS4_SUBDIVISIONS            AXIS4_DRIVER_MICROSTEPS
#endif
#ifndef AXIS4_CURRENT
#define AXIS4_CURRENT                 AXIS4_DRIVER_IRUN
#endif
#ifndef AXIS4_ENABLE_STATE
#define AXIS4_ENABLE_STATE            LOW
#endif
#ifndef AXIS4_STEP_STATE
#define AXIS4_STEP_STATE              HIGH
#endif
#endif

// common axis driver settings, FOCUSER2
#ifdef AXIS5_DRIVER_SD
#ifndef AXIS5_DRIVER_MICROSTEPS
#define AXIS5_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS5_DRIVER_MICROSTEPS_GOTO
#define AXIS5_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS5_DRIVER_DECAY
#define AXIS5_DRIVER_DECAY            OFF
#endif
#ifndef AXIS5_DRIVER_DECAY_GOTO
#define AXIS5_DRIVER_DECAY_GOTO       OFF
#endif
#ifndef AXIS5_DRIVER_IHOLD
#define AXIS5_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS5_DRIVER_IRUN
#define AXIS5_DRIVER_IRUN             OFF
#endif
#ifndef AXIS5_DRIVER_IGOTO
#define AXIS5_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS5_DRIVER_STATUS
#define AXIS5_DRIVER_STATUS           OFF
#endif
#ifndef AXIS5_SUBDIVISIONS
#define AXIS5_SUBDIVISIONS            AXIS5_DRIVER_MICROSTEPS
#endif
#ifndef AXIS5_CURRENT
#define AXIS5_CURRENT                 AXIS5_DRIVER_IRUN
#endif
#ifndef AXIS5_ENABLE_STATE
#define AXIS5_ENABLE_STATE            LOW
#endif
#ifndef AXIS5_STEP_STATE
#define AXIS5_STEP_STATE              HIGH
#endif
#endif

// common axis driver settings, FOCUSER3
#ifdef AXIS6_DRIVER_SD
#ifndef AXIS6_DRIVER_MICROSTEPS
#define AXIS6_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS6_DRIVER_MICROSTEPS_GOTO
#define AXIS6_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS6_DRIVER_DECAY
#define AXIS6_DRIVER_DECAY            OFF
#endif
#ifndef AXIS6_DRIVER_DECAY_GOTO
#define AXIS6_DRIVER_DECAY_GOTO       OFF
#endif
#ifndef AXIS6_DRIVER_IHOLD
#define AXIS6_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS6_DRIVER_IRUN
#define AXIS6_DRIVER_IRUN             OFF
#endif
#ifndef AXIS6_DRIVER_IGOTO
#define AXIS6_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS6_DRIVER_STATUS
#define AXIS6_DRIVER_STATUS           OFF
#endif
#ifndef AXIS6_SUBDIVISIONS
#define AXIS6_SUBDIVISIONS            AXIS6_DRIVER_MICROSTEPS
#endif
#ifndef AXIS6_CURRENT
#define AXIS6_CURRENT                 AXIS6_DRIVER_IRUN
#endif
#ifndef AXIS6_ENABLE_STATE
#define AXIS6_ENABLE_STATE            LOW
#endif
#ifndef AXIS6_STEP_STATE
#define AXIS6_STEP_STATE              HIGH
#endif
#endif

// common axis driver settings, FOCUSER4
#ifdef AXIS7_DRIVER_SD
#ifndef AXIS7_DRIVER_MICROSTEPS
#define AXIS7_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS7_DRIVER_MICROSTEPS_GOTO
#define AXIS7_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS7_DRIVER_DECAY
#define AXIS7_DRIVER_DECAY            OFF
#endif
#ifndef AXIS7_DRIVER_DECAY_GOTO
#define AXIS7_DRIVER_DECAY_GOTO       OFF
#endif
#ifndef AXIS7_DRIVER_IHOLD
#define AXIS7_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS7_DRIVER_IRUN
#define AXIS7_DRIVER_IRUN             OFF
#endif
#ifndef AXIS7_DRIVER_IGOTO
#define AXIS7_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS7_DRIVER_STATUS
#define AXIS7_DRIVER_STATUS           OFF
#endif
#ifndef AXIS7_SUBDIVISIONS
#define AXIS7_SUBDIVISIONS            AXIS7_DRIVER_MICROSTEPS
#endif
#ifndef AXIS7_CURRENT
#define AXIS7_CURRENT                 AXIS7_DRIVER_IRUN
#endif
#ifndef AXIS7_ENABLE_STATE
#define AXIS7_ENABLE_STATE            LOW
#endif
#ifndef AXIS7_STEP_STATE
#define AXIS7_STEP_STATE              HIGH
#endif
#endif

// common axis driver settings, FOCUSER5
#ifdef AXIS8_DRIVER_SD
#ifndef AXIS8_DRIVER_MICROSTEPS
#define AXIS8_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS8_DRIVER_MICROSTEPS_GOTO
#define AXIS8_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS8_DRIVER_DECAY
#define AXIS8_DRIVER_DECAY            OFF
#endif
#ifndef AXIS8_DRIVER_DECAY_GOTO
#define AXIS8_DRIVER_DECAY_GOTO       OFF
#endif
#ifndef AXIS8_DRIVER_IHOLD
#define AXIS8_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS8_DRIVER_IRUN
#define AXIS8_DRIVER_IRUN             OFF
#endif
#ifndef AXIS8_DRIVER_IGOTO
#define AXIS8_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS8_DRIVER_STATUS
#define AXIS8_DRIVER_STATUS           OFF
#endif
#ifndef AXIS8_SUBDIVISIONS
#define AXIS8_SUBDIVISIONS            AXIS8_DRIVER_MICROSTEPS
#endif
#ifndef AXIS8_CURRENT
#define AXIS8_CURRENT                 AXIS8_DRIVER_IRUN
#endif
#ifndef AXIS8_ENABLE_STATE
#define AXIS8_ENABLE_STATE            LOW
#endif
#ifndef AXIS8_STEP_STATE
#define AXIS8_STEP_STATE              HIGH
#endif
#endif

// common axis driver settings, FOCUSER6
#ifdef AXIS9_DRIVER_SD
#ifndef AXIS9_DRIVER_MICROSTEPS
#define AXIS9_DRIVER_MICROSTEPS       OFF
#endif
#ifndef AXIS9_DRIVER_MICROSTEPS_GOTO
#define AXIS9_DRIVER_MICROSTEPS_GOTO  OFF
#endif
#ifndef AXIS9_DRIVER_DECAY
#define AXIS9_DRIVER_DECAY            OFF
#endif
#ifndef AXIS9_DRIVER_DECAY_GOTO
#define AXIS9_DRIVER_DECAY_GOTO       OFF
#endif
#ifndef AXIS9_DRIVER_IHOLD
#define AXIS9_DRIVER_IHOLD            HALF
#endif
#ifndef AXIS9_DRIVER_IRUN
#define AXIS9_DRIVER_IRUN             OFF
#endif
#ifndef AXIS9_DRIVER_IGOTO
#define AXIS9_DRIVER_IGOTO            SAME
#endif
#ifndef AXIS9_DRIVER_STATUS
#define AXIS9_DRIVER_STATUS           OFF
#endif
#ifndef AXIS9_SUBDIVISIONS
#define AXIS9_SUBDIVISIONS            AXIS9_DRIVER_MICROSTEPS
#endif
#ifndef AXIS9_CURRENT
#define AXIS9_CURRENT                 AXIS9_DRIVER_IRUN
#endif
#ifndef AXIS9_ENABLE_STATE
#define AXIS9_ENABLE_STATE            LOW
#endif
#ifndef AXIS9_STEP_STATE
#define AXIS9_STEP_STATE              HIGH
#endif
#endif

#if AXIS1_STEP_STATE == AXIS2_STEP_STATE == AXIS3_STEP_STATE == AXIS4_STEP_STATE == AXIS5_STEP_STATE == AXIS6_STEP_STATE == AXIS7_STEP_STATE == AXIS8_STEP_STATE == AXIS9_STEP_STATE == HIGH
  #define DRIVER_STEP_DEFAULTS
#endif
