//--------------------------------------------------------------------------------------------------
// telescope mount control, configuration defaults
#pragma once

// common axis driver settings, RA/AZM
#if AXIS1_DRIVER_MODEL >= DRIVER_FIRST && AXIS1_DRIVER_MODEL <= DRIVER_LAST
#define AXIS1_DRIVER_SD
#endif
#if AXIS1_DRIVER_MODEL == TMC2130 || AXIS1_DRIVER_MODEL == TMC5160
#define AXIS1_DRIVER_TMC_SPI
#endif
#if AXIS1_DRIVER_MODEL >= SERVO_DRIVER_FIRST
#define AXIS1_SERVO
#endif
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
#ifndef AXIS1_PARAMETER1      
#define AXIS1_PARAMETER1              AXIS1_DRIVER_MICROSTEPS
#endif
#ifndef AXIS1_PARAMETER2 
#define AXIS1_PARAMETER2              AXIS1_DRIVER_IRUN
#endif
#ifndef AXIS1_ENABLE_STATE
#define AXIS1_ENABLE_STATE            LOW  // default state of ENable pin for motor power on
#endif
#ifndef AXIS1_STEP_STATE
#define AXIS1_STEP_STATE              HIGH // default signal transition state for a step
#endif
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
#ifndef AXIS1_SERVO_PARAMETER1      
#define AXIS1_SERVO_PARAMETER1        1      // subdivisions always 1
#endif
#ifndef AXIS1_SERVO_PARAMETER2 
#define AXIS1_SERVO_PARAMETER2        0      // not used
#endif

// common axis driver settings, DEC/ALT
#if AXIS2_DRIVER_MODEL >= DRIVER_FIRST && AXIS2_DRIVER_MODEL <= DRIVER_LAST
#define AXIS2_DRIVER_SD
#endif
#if AXIS2_DRIVER_MODEL == TMC2130 || AXIS2_DRIVER_MODEL == TMC5160
#define AXIS2_DRIVER_TMC_SPI
#endif
#if AXIS2_DRIVER_MODEL >= SERVO_DRIVER_FIRST
#define AXIS2_SERVO
#endif
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
#ifndef AXIS2_PARAMETER1      
#define AXIS2_PARAMETER1              AXIS2_DRIVER_MICROSTEPS
#endif
#ifndef AXIS2_PARAMETER2 
#define AXIS2_PARAMETER2              AXIS2_DRIVER_IRUN
#endif
#ifndef AXIS2_ENABLE_STATE
#define AXIS2_ENABLE_STATE            LOW
#endif
#ifndef AXIS2_STEP_STATE
#define AXIS2_STEP_STATE              HIGH
#endif
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
#ifndef AXIS2_SERVO_PARAMETER1      
#define AXIS2_SERVO_PARAMETER1        1
#endif
#ifndef AXIS2_SERVO_PARAMETER2 
#define AXIS2_SERVO_PARAMETER2        0
#endif
