// -----------------------------------------------------------------------------------
// axis servo driver motion default settings
#pragma once

#include "../../../../Common.h"

// assign DRIVER_TYPE
#if AXIS1_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS1_DRIVER_SERVO
#endif
#if AXIS2_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS2_DRIVER_SERVO
#endif
#if AXIS3_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS3_DRIVER_SERVO
#endif
#if AXIS4_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS4_DRIVER_SERVO
#endif
#if AXIS5_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS5_DRIVER_SERVO
#endif
#if AXIS6_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS6_DRIVER_SERVO
#endif
#if AXIS7_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS7_DRIVER_SERVO
#endif
#if AXIS8_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS8_DRIVER_SERVO
#endif
#if AXIS9_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS9_DRIVER_SERVO
#endif

#if defined(AXIS1_DRIVER_SERVO) || defined(AXIS2_DRIVER_SERVO) || defined(AXIS3_DRIVER_SERVO) || defined(AXIS4_DRIVER_SERVO) || defined(AXIS5_DRIVER_SERVO) || defined(AXIS6_DRIVER_SERVO) || defined(AXIS7_DRIVER_SERVO) || defined(AXIS8_DRIVER_SERVO) || defined(AXIS9_DRIVER_SERVO)
  #define SERVO_DRIVER_PRESENT
#endif

// common axis driver settings, RA/AZM
#ifdef AXIS1_DRIVER_SERVO
#ifndef AXIS1_DRIVER_STATUS
#define AXIS1_DRIVER_STATUS           OFF    // driver status reporting (ON for TMC SPI or HIGH/LOW for fault pin)
#endif
#ifndef AXIS1_DRIVER_SERVO_P
#define AXIS1_DRIVER_SERVO_P          2      // P = porportional
#endif
#ifndef AXIS1_DRIVER_SERVO_I
#define AXIS1_DRIVER_SERVO_I          5      // I = integral
#endif
#ifndef AXIS1_DRIVER_SERVO_D
#define AXIS1_DRIVER_SERVO_D          1      // D = derivative
#endif
#ifndef AXIS1_PARAMETER1      
#define AXIS1_PARAMETER1              AXIS1_DRIVER_SERVO_I
#endif
#ifndef AXIS1_PARAMETER2 
#define AXIS1_PARAMETER2              AXIS1_DRIVER_SERVO_P
#endif
#ifndef AXIS1_ENABLE_STATE
#define AXIS1_ENABLE_STATE            LOW    // default state of ENable pin for motor power on
#endif
#ifndef AXIS1_DC_IN1_STATE
#define AXIS1_DC_IN1_STATE            LOW    // IN1 also doubles for DIR
#endif
#ifndef AXIS1_DC_IN2_STATE
#define AXIS1_DC_IN2_STATE            LOW    // IN2 also doubles for PHASE
#endif
#endif

// common axis driver settings, DEC/ALT
#ifdef AXIS2_DRIVER_SERVO
#ifndef AXIS2_DRIVER_STATUS
#define AXIS2_DRIVER_STATUS           OFF
#endif
#ifndef AXIS2_DRIVER_SERVO_P
#define AXIS2_DRIVER_SERVO_P          2
#endif
#ifndef AXIS2_DRIVER_SERVO_I
#define AXIS2_DRIVER_SERVO_I          5
#endif
#ifndef AXIS2_DRIVER_SERVO_D
#define AXIS2_DRIVER_SERVO_D          1
#endif
#ifndef AXIS2_PARAMETER1      
#define AXIS2_PARAMETER1              AXIS2_DRIVER_SERVO_I
#endif
#ifndef AXIS2_PARAMETER2 
#define AXIS2_PARAMETER2              AXIS2_DRIVER_SERVO_P
#endif
#ifndef AXIS2_ENABLE_STATE
#define AXIS2_ENABLE_STATE            LOW
#endif
#ifndef AXIS2_DC_IN1_STATE
#define AXIS2_DC_IN1_STATE            LOW
#endif
#ifndef AXIS2_DC_IN2_STATE
#define AXIS2_DC_IN2_STATE            LOW
#endif
#endif

// common axis driver settings, ROTATOR
#ifdef AXIS3_DRIVER_SERVO
#ifndef AXIS3_DRIVER_STATUS
#define AXIS3_DRIVER_STATUS           OFF
#endif
#ifndef AXIS3_DRIVER_SERVO_P
#define AXIS3_DRIVER_SERVO_P          2
#endif
#ifndef AXIS3_DRIVER_SERVO_I
#define AXIS3_DRIVER_SERVO_I          5
#endif
#ifndef AXIS3_DRIVER_SERVO_D
#define AXIS3_DRIVER_SERVO_D          1
#endif
#ifndef AXIS3_PARAMETER1
#define AXIS3_PARAMETER1              AXIS3_DRIVER_SERVO_I
#endif
#ifndef AXIS3_PARAMETER2
#define AXIS3_PARAMETER2              AXIS3_DRIVER_SERVO_P
#endif
#ifndef AXIS3_ENABLE_STATE
#define AXIS3_ENABLE_STATE            LOW
#endif
#ifndef AXIS3_DC_IN1_STATE
#define AXIS3_DC_IN1_STATE            LOW
#endif
#ifndef AXIS3_DC_IN2_STATE
#define AXIS3_DC_IN2_STATE            LOW
#endif
#endif

// common axis driver settings, FOCUSER1
#ifdef AXIS4_DRIVER_SERVO
#ifndef AXIS4_DRIVER_STATUS
#define AXIS4_DRIVER_STATUS           OFF
#endif
#ifndef AXIS4_DRIVER_SERVO_P
#define AXIS4_DRIVER_SERVO_P          2
#endif
#ifndef AXIS4_DRIVER_SERVO_I
#define AXIS4_DRIVER_SERVO_I          5
#endif
#ifndef AXIS4_DRIVER_SERVO_D
#define AXIS4_DRIVER_SERVO_D          1
#endif
#ifndef AXIS4_PARAMETER1      
#define AXIS4_PARAMETER1              AXIS4_DRIVER_SERVO_I
#endif
#ifndef AXIS4_PARAMETER2 
#define AXIS4_PARAMETER2              AXIS4_DRIVER_SERVO_P
#endif
#ifndef AXIS4_ENABLE_STATE
#define AXIS4_ENABLE_STATE            LOW
#endif
#ifndef AXIS4_DC_IN1_STATE
#define AXIS4_DC_IN1_STATE            LOW
#endif
#ifndef AXIS4_DC_IN2_STATE
#define AXIS4_DC_IN2_STATE            LOW
#endif
#endif

// common axis driver settings, FOCUSER2
#ifdef AXIS5_DRIVER_SERVO
#ifndef AXIS5_DRIVER_STATUS
#define AXIS5_DRIVER_STATUS           OFF
#endif
#ifndef AXIS5_DRIVER_SERVO_P
#define AXIS5_DRIVER_SERVO_P          2
#endif
#ifndef AXIS5_DRIVER_SERVO_I
#define AXIS5_DRIVER_SERVO_I          5
#endif
#ifndef AXIS5_DRIVER_SERVO_D
#define AXIS5_DRIVER_SERVO_D          1
#endif
#ifndef AXIS5_PARAMETER1
#define AXIS5_PARAMETER1              AXIS5_DRIVER_SERVO_I
#endif
#ifndef AXIS5_PARAMETER2 
#define AXIS5_PARAMETER2              AXIS5_DRIVER_SERVO_P
#endif
#ifndef AXIS5_ENABLE_STATE
#define AXIS5_ENABLE_STATE            LOW
#endif
#ifndef AXIS5_DC_IN1_STATE
#define AXIS5_DC_IN1_STATE            LOW
#endif
#ifndef AXIS5_DC_IN2_STATE
#define AXIS5_DC_IN2_STATE            LOW
#endif
#endif

// common axis driver settings, FOCUSER3
#ifdef AXIS6_DRIVER_SERVO
#ifndef AXIS6_DRIVER_STATUS
#define AXIS6_DRIVER_STATUS           OFF
#endif
#ifndef AXIS6_DRIVER_SERVO_P
#define AXIS6_DRIVER_SERVO_P          2
#endif
#ifndef AXIS6_DRIVER_SERVO_I
#define AXIS6_DRIVER_SERVO_I          5
#endif
#ifndef AXIS6_DRIVER_SERVO_D
#define AXIS6_DRIVER_SERVO_D          1
#endif
#ifndef AXIS6_PARAMETER1      
#define AXIS6_PARAMETER1              AXIS6_DRIVER_SERVO_I
#endif
#ifndef AXIS6_PARAMETER2 
#define AXIS6_PARAMETER2              AXIS6_DRIVER_SERVO_P
#endif
#ifndef AXIS6_ENABLE_STATE
#define AXIS6_ENABLE_STATE            LOW
#endif
#ifndef AXIS6_DC_IN1_STATE
#define AXIS6_DC_IN1_STATE            LOW
#endif
#ifndef AXIS6_DC_IN2_STATE
#define AXIS6_DC_IN2_STATE            LOW
#endif
#endif

// common axis driver settings, FOCUSER4
#ifdef AXIS7_DRIVER_SERVO
#ifndef AXIS7_DRIVER_STATUS
#define AXIS7_DRIVER_STATUS           OFF
#endif
#ifndef AXIS7_DRIVER_SERVO_P
#define AXIS7_DRIVER_SERVO_P          2
#endif
#ifndef AXIS7_DRIVER_SERVO_I
#define AXIS7_DRIVER_SERVO_I          5
#endif
#ifndef AXIS7_DRIVER_SERVO_D
#define AXIS7_DRIVER_SERVO_D          1
#endif
#ifndef AXIS7_PARAMETER1      
#define AXIS7_PARAMETER1              AXIS7_DRIVER_SERVO_I
#endif
#ifndef AXIS7_PARAMETER2 
#define AXIS7_PARAMETER2              AXIS7_DRIVER_SERVO_P
#endif
#ifndef AXIS4_ENABLE_STATE
#define AXIS7_ENABLE_STATE            LOW
#endif
#ifndef AXIS7_DC_IN1_STATE
#define AXIS7_DC_IN1_STATE            LOW
#endif
#ifndef AXIS7_DC_IN2_STATE
#define AXIS7_DC_IN2_STATE            LOW
#endif
#endif

// common axis driver settings, FOCUSER5
#ifdef AXIS8_DRIVER_SERVO
#ifndef AXIS8_DRIVER_STATUS
#define AXIS8_DRIVER_STATUS           OFF
#endif
#ifndef AXIS8_DRIVER_SERVO_P
#define AXIS8_DRIVER_SERVO_P          2
#endif
#ifndef AXIS8_DRIVER_SERVO_I
#define AXIS8_DRIVER_SERVO_I          5
#endif
#ifndef AXIS8_DRIVER_SERVO_D
#define AXIS8_DRIVER_SERVO_D          1
#endif
#ifndef AXIS8_PARAMETER1      
#define AXIS8_PARAMETER1              AXIS8_DRIVER_SERVO_I
#endif
#ifndef AXIS8_PARAMETER2 
#define AXIS8_PARAMETER2              AXIS8_DRIVER_SERVO_P
#endif
#ifndef AXIS8_ENABLE_STATE
#define AXIS8_ENABLE_STATE            LOW
#endif
#ifndef AXIS8_DC_IN1_STATE
#define AXIS8_DC_IN1_STATE            LOW
#endif
#ifndef AXIS8_DC_IN2_STATE
#define AXIS8_DC_IN2_STATE            LOW
#endif
#endif

// common axis driver settings, FOCUSER6
#ifdef AXIS9_DRIVER_SERVO
#ifndef AXIS9_DRIVER_STATUS
#define AXIS9_DRIVER_STATUS           OFF
#endif
#ifndef AXIS9_DRIVER_SERVO_P
#define AXIS9_DRIVER_SERVO_P          2
#endif
#ifndef AXIS9_DRIVER_SERVO_I
#define AXIS9_DRIVER_SERVO_I          5
#endif
#ifndef AXIS9_DRIVER_SERVO_D
#define AXIS9_DRIVER_SERVO_D          1
#endif
#ifndef AXIS9_PARAMETER1      
#define AXIS9_PARAMETER1              AXIS9_DRIVER_SERVO_I
#endif
#ifndef AXIS9_PARAMETER2 
#define AXIS9_PARAMETER2              AXIS9_DRIVER_SERVO_P
#endif
#ifndef AXIS9_ENABLE_STATE
#define AXIS9_ENABLE_STATE            LOW
#endif
#ifndef AXIS9_DC_IN1_STATE
#define AXIS9_DC_IN1_STATE            LOW
#endif
#ifndef AXIS9_DC_IN2_STATE
#define AXIS9_DC_IN2_STATE            LOW
#endif
#endif
