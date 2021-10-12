//--------------------------------------------------------------------------------------------------
// telescope rotator control, configuration defaults
#pragma once

// common axis driver settings, ROTATOR
#if AXIS3_DRIVER_MODEL >= DRIVER_FIRST && AXIS3_DRIVER_MODEL <= DRIVER_LAST
  #define AXIS3_DRIVER_SD
#endif
#if AXIS3_DRIVER_MODEL == TMC2130 || AXIS3_DRIVER_MODEL == TMC5160
  #define AXIS3_DRIVER_TMC_SPI
#endif
#if AXIS3_DRIVER_MODEL >= SERVO_DRIVER_FIRST
#define AXIS3_SERVO
#endif
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
#ifndef AXIS3_PARAMETER1      
#define AXIS3_PARAMETER1              AXIS3_DRIVER_MICROSTEPS
#endif
#ifndef AXIS3_PARAMETER2 
#define AXIS3_PARAMETER2              AXIS3_DRIVER_IRUN
#endif
#ifndef AXIS3_ENABLE_STATE
#define AXIS3_ENABLE_STATE            LOW
#endif
#ifndef AXIS3_STEP_STATE
#define AXIS3_STEP_STATE              HIGH
#endif
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
#ifndef AXIS3_SERVO_PARAMETER1      
#define AXIS3_SERVO_PARAMETER1        1
#endif
#ifndef AXIS3_SERVO_PARAMETER2 
#define AXIS3_SERVO_PARAMETER2        0
#endif
