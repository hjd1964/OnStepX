// -----------------------------------------------------------------------------------
// axis servo driver motion default settings
#pragma once

#include "../../../../Common.h"

// assign DRIVER_TYPE
#if AXIS1_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS1_SERVO
#endif
#if AXIS2_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS2_SERVO
#endif
#if AXIS3_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS3_SERVO
#endif
#if AXIS4_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS4_SERVO
#endif
#if AXIS5_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS5_SERVO
#endif
#if AXIS6_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS6_SERVO
#endif
#if AXIS7_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS7_SERVO
#endif
#if AXIS8_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS8_SERVO
#endif
#if AXIS9_DRIVER_MODEL >= SERVO_DRIVER_FIRST
  #define AXIS9_SERVO
#endif

#if defined(AXIS1_SERVO) || defined(AXIS2_SERVO) || defined(AXIS3_SERVO) || defined(AXIS4_SERVO) || defined(AXIS5_SERVO) || defined(AXIS6_SERVO) || defined(AXIS7_SERVO) || defined(AXIS8_SERVO) || defined(AXIS9_SERVO)
  #define SERVO_DRIVER_PRESENT
#endif

// common axis driver settings, RA/AZM
#ifndef AXIS1_SERVO_P
#define AXIS1_SERVO_P                 2.0    // P = proportional
#endif
#ifndef AXIS1_SERVO_I
#define AXIS1_SERVO_I                 5.0    // I = integral
#endif
#ifndef AXIS1_SERVO_D
#define AXIS1_SERVO_D                 1.0    // D = derivative
#endif
#ifndef AXIS1_SERVO_ENCODER
#define AXIS1_SERVO_ENCODER           ENC_AB
#endif
#ifndef AXIS1_SERVO_ENCODER_TRIGGER
#define AXIS1_SERVO_ENCODER_TRIGGER   CHANGE // ignored for ENC_AB
#endif
#ifndef AXIS1_SERVO_PH1_STATE
#define AXIS1_SERVO_PH1_STATE         LOW    // default state motor driver IN1 (SERVO_II) or PHASE (SERVO_PD) pin
#endif
#ifndef AXIS1_SERVO_PH2_STATE
#define AXIS1_SERVO_PH2_STATE         LOW    // default state motor driver IN2 or DIR pin
#endif
#ifndef AXIS1_DRIVER_STATUS
#define AXIS1_DRIVER_STATUS           OFF    // driver status reporting (ON for TMC SPI or HIGH/LOW for fault pin)
#endif
#ifndef AXIS1_PARAMETER1      
#define AXIS1_PARAMETER1              1      // subdivisions always 1
#endif
#ifndef AXIS1_PARAMETER2 
#define AXIS1_PARAMETER2              0      // not used
#endif
#ifndef AXIS1_ENABLE_STATE
#define AXIS1_ENABLE_STATE            LOW    // default state of ENable pin for motor power on
#endif

// common axis driver settings, DEC/ALT
#ifndef AXIS2_SERVO_P
#define AXIS2_SERVO_P                 2.0
#endif
#ifndef AXIS2_SERVO_I
#define AXIS2_SERVO_I                 5.0
#endif
#ifndef AXIS2_SERVO_D
#define AXIS2_SERVO_D                 1.0
#endif
#ifndef AXIS2_SERVO_ENCODER
#define AXIS2_SERVO_ENCODER           ENC_AB
#endif
#ifndef AXIS2_SERVO_ENCODER_TRIGGER
#define AXIS2_SERVO_ENCODER_TRIGGER   CHANGE
#endif
#ifndef AXIS2_SERVO_PH1_STATE
#define AXIS2_SERVO_PH1_STATE         LOW
#endif
#ifndef AXIS2_SERVO_PH2_STATE
#define AXIS2_SERVO_PH2_STATE         LOW
#endif
#ifndef AXIS2_DRIVER_STATUS
#define AXIS2_DRIVER_STATUS           OFF
#endif
#ifndef AXIS2_PARAMETER1      
#define AXIS2_PARAMETER1              1
#endif
#ifndef AXIS2_PARAMETER2 
#define AXIS2_PARAMETER2              0
#endif
#ifndef AXIS2_ENABLE_STATE
#define AXIS2_ENABLE_STATE            LOW
#endif

// common axis driver settings, ROTATOR
#ifndef AXIS3_SERVO_P
#define AXIS3_SERVO_P                 2.0
#endif
#ifndef AXIS3_SERVO_I
#define AXIS3_SERVO_I                 5.0
#endif
#ifndef AXIS3_SERVO_D
#define AXIS3_SERVO_D                 1.0
#endif
#ifndef AXIS3_SERVO_ENCODER
#define AXIS3_SERVO_ENCODER           ENC_AB
#endif
#ifndef AXIS3_SERVO_ENCODER_TRIGGER
#define AXIS3_SERVO_ENCODER_TRIGGER   CHANGE
#endif
#ifndef AXIS3_SERVO_PH1_STATE
#define AXIS3_SERVO_PH1_STATE         LOW
#endif
#ifndef AXIS3_SERVO_PH2_STATE
#define AXIS3_SERVO_PH2_STATE         LOW
#endif
#ifndef AXIS3_DRIVER_STATUS
#define AXIS3_DRIVER_STATUS           OFF
#endif
#ifndef AXIS3_PARAMETER1      
#define AXIS3_PARAMETER1              1
#endif
#ifndef AXIS3_PARAMETER2 
#define AXIS3_PARAMETER2              0
#endif
#ifndef AXIS3_ENABLE_STATE
#define AXIS3_ENABLE_STATE            LOW
#endif

// common axis driver settings, FOCUSER1
#ifndef AXIS4_SERVO_P
#define AXIS4_SERVO_P                 2.0
#endif
#ifndef AXIS4_SERVO_I
#define AXIS4_SERVO_I                 5.0
#endif
#ifndef AXIS4_SERVO_D
#define AXIS4_SERVO_D                 1.0
#endif
#ifndef AXIS4_SERVO_ENCODER
#define AXIS4_SERVO_ENCODER           ENC_AB
#endif
#ifndef AXIS4_SERVO_ENCODER_TRIGGER
#define AXIS4_SERVO_ENCODER_TRIGGER   CHANGE
#endif
#ifndef AXIS4_SERVO_PH1_STATE
#define AXIS4_SERVO_PH1_STATE         LOW
#endif
#ifndef AXIS4_SERVO_PH2_STATE
#define AXIS4_SERVO_PH2_STATE         LOW
#endif
#ifndef AXIS4_DRIVER_STATUS
#define AXIS4_DRIVER_STATUS           OFF
#endif
#ifndef AXIS4_PARAMETER1      
#define AXIS4_PARAMETER1              1
#endif
#ifndef AXIS4_PARAMETER2 
#define AXIS4_PARAMETER2              0
#endif
#ifndef AXIS4_ENABLE_STATE
#define AXIS4_ENABLE_STATE            LOW
#endif

// common axis driver settings, FOCUSER2
#ifndef AXIS5_SERVO_P
#define AXIS5_SERVO_P                 2.0
#endif
#ifndef AXIS5_SERVO_I
#define AXIS5_SERVO_I                 5.0
#endif
#ifndef AXIS5_SERVO_D
#define AXIS5_SERVO_D                 1.0
#endif
#ifndef AXIS5_SERVO_ENCODER
#define AXIS5_SERVO_ENCODER           ENC_AB
#endif
#ifndef AXIS5_SERVO_ENCODER_TRIGGER
#define AXIS5_SERVO_ENCODER_TRIGGER   CHANGE
#endif
#ifndef AXIS5_SERVO_PH1_STATE
#define AXIS5_SERVO_PH1_STATE         LOW
#endif
#ifndef AXIS5_SERVO_PH2_STATE
#define AXIS5_SERVO_PH2_STATE         LOW
#endif
#ifndef AXIS5_DRIVER_STATUS
#define AXIS5_DRIVER_STATUS           OFF
#endif
#ifndef AXIS5_PARAMETER1      
#define AXIS5_PARAMETER1              1
#endif
#ifndef AXIS5_PARAMETER2 
#define AXIS5_PARAMETER2              0
#endif
#ifndef AXIS5_ENABLE_STATE
#define AXIS5_ENABLE_STATE            LOW
#endif

// common axis driver settings, FOCUSER3
#ifndef AXIS6_SERVO_P
#define AXIS6_SERVO_P                 2.0
#endif
#ifndef AXIS6_SERVO_I
#define AXIS6_SERVO_I                 5.0
#endif
#ifndef AXIS6_SERVO_D
#define AXIS6_SERVO_D                 1.0
#endif
#ifndef AXIS6_SERVO_ENCODER
#define AXIS6_SERVO_ENCODER           ENC_AB
#endif
#ifndef AXIS6_SERVO_ENCODER_TRIGGER
#define AXIS6_SERVO_ENCODER_TRIGGER   CHANGE
#endif
#ifndef AXIS6_SERVO_PH1_STATE
#define AXIS6_SERVO_PH1_STATE         LOW
#endif
#ifndef AXIS6_SERVO_PH2_STATE
#define AXIS6_SERVO_PH2_STATE         LOW
#endif
#ifndef AXIS6_DRIVER_STATUS
#define AXIS6_DRIVER_STATUS           OFF
#endif
#ifndef AXIS6_PARAMETER1      
#define AXIS6_PARAMETER1              1
#endif
#ifndef AXIS6_PARAMETER2 
#define AXIS6_PARAMETER2              0
#endif
#ifndef AXIS6_ENABLE_STATE
#define AXIS6_ENABLE_STATE            LOW
#endif

// common axis driver settings, FOCUSER4
#ifndef AXIS7_SERVO_P
#define AXIS7_SERVO_P                 2.0
#endif
#ifndef AXIS7_SERVO_I
#define AXIS7_SERVO_I                 5.0
#endif
#ifndef AXIS7_SERVO_D
#define AXIS7_SERVO_D                 1.0
#endif
#ifndef AXIS7_SERVO_ENCODER
#define AXIS7_SERVO_ENCODER           ENC_AB
#endif
#ifndef AXIS7_SERVO_ENCODER_TRIGGER
#define AXIS7_SERVO_ENCODER_TRIGGER   CHANGE
#endif
#ifndef AXIS7_SERVO_PH1_STATE
#define AXIS7_SERVO_PH1_STATE         LOW
#endif
#ifndef AXIS7_SERVO_PH2_STATE
#define AXIS7_SERVO_PH2_STATE         LOW
#endif
#ifndef AXIS7_DRIVER_STATUS
#define AXIS7_DRIVER_STATUS           OFF
#endif
#ifndef AXIS7_PARAMETER1      
#define AXIS7_PARAMETER1              1
#endif
#ifndef AXIS7_PARAMETER2 
#define AXIS7_PARAMETER2              0
#endif
#ifndef AXIS7_ENABLE_STATE
#define AXIS7_ENABLE_STATE            LOW
#endif

// common axis driver settings, FOCUSER5
#ifndef AXIS8_SERVO_P
#define AXIS8_SERVO_P                 2.0
#endif
#ifndef AXIS8_SERVO_I
#define AXIS8_SERVO_I                 5.0
#endif
#ifndef AXIS8_SERVO_D
#define AXIS8_SERVO_D                 1.0
#endif
#ifndef AXIS8_SERVO_ENCODER
#define AXIS8_SERVO_ENCODER           ENC_AB
#endif
#ifndef AXIS8_SERVO_ENCODER_TRIGGER
#define AXIS8_SERVO_ENCODER_TRIGGER   CHANGE
#endif
#ifndef AXIS8_SERVO_PH1_STATE
#define AXIS8_SERVO_PH1_STATE         LOW
#endif
#ifndef AXIS8_SERVO_PH2_STATE
#define AXIS8_SERVO_PH2_STATE         LOW
#endif
#ifndef AXIS8_DRIVER_STATUS
#define AXIS8_DRIVER_STATUS           OFF
#endif
#ifndef AXIS8_PARAMETER1      
#define AXIS8_PARAMETER1              1
#endif
#ifndef AXIS8_PARAMETER2 
#define AXIS8_PARAMETER2              0
#endif
#ifndef AXIS8_ENABLE_STATE
#define AXIS8_ENABLE_STATE            LOW
#endif

// common axis driver settings, FOCUSER6
#ifndef AXIS9_SERVO_P
#define AXIS9_SERVO_P                 2.0
#endif
#ifndef AXIS9_SERVO_I
#define AXIS9_SERVO_I                 5.0
#endif
#ifndef AXIS9_SERVO_D
#define AXIS9_SERVO_D                 1.0
#endif
#ifndef AXIS9_SERVO_ENCODER
#define AXIS9_SERVO_ENCODER           ENC_AB
#endif
#ifndef AXIS9_SERVO_ENCODER_TRIGGER
#define AXIS9_SERVO_ENCODER_TRIGGER   CHANGE
#endif
#ifndef AXIS9_SERVO_PH1_STATE
#define AXIS9_SERVO_PH1_STATE         LOW
#endif
#ifndef AXIS9_SERVO_PH2_STATE
#define AXIS9_SERVO_PH2_STATE         LOW
#endif
#ifndef AXIS9_DRIVER_STATUS
#define AXIS9_DRIVER_STATUS           OFF
#endif
#ifndef AXIS9_PARAMETER1      
#define AXIS9_PARAMETER1              1
#endif
#ifndef AXIS9_PARAMETER2 
#define AXIS9_PARAMETER2              0
#endif
#ifndef AXIS9_ENABLE_STATE
#define AXIS9_ENABLE_STATE            LOW
#endif
