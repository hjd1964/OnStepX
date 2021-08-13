#pragma once

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
#ifndef AXIS1_DRIVER_STATUS
#define AXIS1_DRIVER_STATUS    OFF  // driver status reporting (ON for TMC SPI or HIGH/LOW for fault pin)
#endif

// common axis driver settings, DEC/ALT
#ifndef AXIS2_DRIVER_STATUS
#define AXIS2_DRIVER_STATUS    OFF
#endif

// common axis driver settings, ROTATOR
#ifndef AXIS3_DRIVER_STATUS
#define AXIS3_DRIVER_STATUS    OFF
#endif

// common axis driver settings, FOCUSER1
#ifndef AXIS4_DRIVER_STATUS
#define AXIS4_DRIVER_STATUS    OFF
#endif

// common axis driver settings, FOCUSER2
#ifndef AXIS5_DRIVER_STATUS
#define AXIS5_DRIVER_STATUS    OFF
#endif

// common axis driver settings, FOCUSER3
#ifndef AXIS6_DRIVER_STATUS
#define AXIS6_DRIVER_STATUS    OFF
#endif

// common axis driver settings, FOCUSER4
#ifndef AXIS7_DRIVER_STATUS
#define AXIS7_DRIVER_STATUS    OFF
#endif

// common axis driver settings, FOCUSER5
#ifndef AXIS8_DRIVER_STATUS
#define AXIS8_DRIVER_STATUS    OFF
#endif

// common axis driver settings, FOCUSER6
#ifndef AXIS9_DRIVER_STATUS
#define AXIS9_DRIVER_STATUS    OFF
#endif

// state of ENable pin for motor power on
#ifndef AXIS1_ENABLE_STATE
#define AXIS1_ENABLE_STATE     LOW
#endif
#ifndef AXIS2_ENABLE_STATE
#define AXIS2_ENABLE_STATE     LOW
#endif
#ifndef AXIS3_ENABLE_STATE
#define AXIS3_ENABLE_STATE     LOW
#endif
#ifndef AXIS4_ENABLE_STATE
#define AXIS4_ENABLE_STATE     LOW
#endif
#ifndef AXIS5_ENABLE_STATE
#define AXIS5_ENABLE_STATE     LOW
#endif
#ifndef AXIS6_ENABLE_STATE
#define AXIS6_ENABLE_STATE     LOW
#endif
#ifndef AXIS7_ENABLE_STATE
#define AXIS7_ENABLE_STATE     LOW
#endif
#ifndef AXIS8_ENABLE_STATE
#define AXIS8_ENABLE_STATE     LOW
#endif
#ifndef AXIS9_ENABLE_STATE
#define AXIS9_ENABLE_STATE     LOW
#endif

// PHASE1 also doubles for DIR
#ifndef AXIS1_DC_IN1_STATE
#define AXIS1_DC_IN1_STATE     HIGH
#endif
#ifndef AXIS2_DC_IN1_STATE
#define AXIS2_DC_IN1_STATE     HIGH
#endif
#ifndef AXIS3_DC_IN1_STATE
#define AXIS3_DC_IN1_STATE     HIGH
#endif
#ifndef AXIS4_DC_IN1_STATE
#define AXIS4_DC_IN1_STATE     HIGH
#endif
#ifndef AXIS5_DC_IN1_STATE
#define AXIS5_DC_IN1_STATE     HIGH
#endif
#ifndef AXIS6_DC_IN1_STATE
#define AXIS6_DC_IN1_STATE     HIGH
#endif
#ifndef AXIS7_DC_IN1_STATE
#define AXIS7_DC_IN1_STATE     HIGH
#endif
#ifndef AXIS8_DC_IN1_STATE
#define AXIS8_DC_IN1_STATE     HIGH
#endif
#ifndef AXIS9_DC_IN1_STATE
#define AXIS9_DC_IN1_STATE     HIGH
#endif

// PHASE2 is the primary phase (pwm) pin for DIR/PHASE drivers
#ifndef AXIS1_DC_IN2_STATE
#define AXIS1_DC_IN2_STATE     HIGH
#endif
#ifndef AXIS2_DC_IN2_STATE
#define AXIS2_DC_IN2_STATE     HIGH
#endif
#ifndef AXIS3_DC_IN2_STATE
#define AXIS3_DC_IN2_STATE     HIGH
#endif
#ifndef AXIS4_DC_IN2_STATE
#define AXIS4_DC_IN2_STATE     HIGH
#endif
#ifndef AXIS5_DC_IN2_STATE
#define AXIS5_DC_IN2_STATE     HIGH
#endif
#ifndef AXIS6_DC_IN2_STATE
#define AXIS6_DC_IN2_STATE     HIGH
#endif
#ifndef AXIS7_DC_IN2_STATE
#define AXIS7_DC_IN2_STATE     HIGH
#endif
#ifndef AXIS8_DC_IN2_STATE
#define AXIS8_DC_IN2_STATE     HIGH
#endif
#ifndef AXIS9_DC_IN2_STATE
#define AXIS9_DC_IN2_STATE     HIGH
#endif

#ifndef AXIS1_DRIVER_SERVO_P
#define AXIS1_DRIVER_SERVO_P      1   // 1% response
#endif
#ifndef AXIS1_DRIVER_SERVO_I
#define AXIS1_DRIVER_SERVO_I      1   // integrate time in ms
#endif
#ifndef AXIS1_DRIVER_SERVO_D
#define AXIS1_DRIVER_SERVO_D      1   //
#endif

#ifndef AXIS3_DRIVER_SERVO_P
#define AXIS3_DRIVER_SERVO_P      1
#endif
#ifndef AXIS3_DRIVER_SERVO_I
#define AXIS3_DRIVER_SERVO_I      1
#endif
#ifndef AXIS3_DRIVER_SERVO_D
#define AXIS3_DRIVER_SERVO_D      1
#endif

#ifndef AXIS3_DRIVER_SERVO_P
#define AXIS3_DRIVER_SERVO_P      1
#endif
#ifndef AXIS3_DRIVER_SERVO_I
#define AXIS3_DRIVER_SERVO_I      1
#endif
#ifndef AXIS3_DRIVER_SERVO_D
#define AXIS3_DRIVER_SERVO_D      1
#endif

