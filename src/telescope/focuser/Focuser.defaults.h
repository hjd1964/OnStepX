//--------------------------------------------------------------------------------------------------
// telescope focuser control, configuration defaults
#pragma once

// common axis driver settings, FOCUSER1
#if AXIS4_DRIVER_MODEL >= DRIVER_FIRST && AXIS4_DRIVER_MODEL <= DRIVER_LAST
#define AXIS4_DRIVER_SD
#endif
#if AXIS4_DRIVER_MODEL == TMC2130 || AXIS4_DRIVER_MODEL == TMC5160
#define AXIS4_DRIVER_TMC_SPI
#endif
#if AXIS3_DRIVER_MODEL >= SERVO_DRIVER_FIRST
#define AXIS3_SERVO
#endif
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
#ifndef AXIS4_PARAMETER1      
#define AXIS4_PARAMETER1              AXIS4_DRIVER_MICROSTEPS
#endif
#ifndef AXIS4_PARAMETER2 
#define AXIS4_PARAMETER2              AXIS4_DRIVER_IRUN
#endif
#ifndef AXIS4_ENABLE_STATE
#define AXIS4_ENABLE_STATE            LOW
#endif
#ifndef AXIS4_STEP_STATE
#define AXIS4_STEP_STATE              HIGH
#endif
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
#ifndef AXIS4_SERVO_PARAMETER1      
#define AXIS4_SERVO_PARAMETER1        1
#endif
#ifndef AXIS4_SERVO_PARAMETER2 
#define AXIS4_SERVO_PARAMETER2        0
#endif

// common axis driver settings, FOCUSER2
#if AXIS5_DRIVER_MODEL >= DRIVER_FIRST && AXIS5_DRIVER_MODEL <= DRIVER_LAST
#define AXIS5_DRIVER_SD
#endif
#if AXIS5_DRIVER_MODEL == TMC2130 || AXIS5_DRIVER_MODEL == TMC5160
#define AXIS5_DRIVER_TMC_SPI
#endif
#if AXIS5_DRIVER_MODEL >= SERVO_DRIVER_FIRST
#define AXIS5_SERVO
#endif
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
#ifndef AXIS5_PARAMETER1      
#define AXIS5_PARAMETER1              AXIS5_DRIVER_MICROSTEPS
#endif
#ifndef AXIS5_PARAMETER2 
#define AXIS5_PARAMETER2              AXIS5_DRIVER_IRUN
#endif
#ifndef AXIS5_ENABLE_STATE
#define AXIS5_ENABLE_STATE            LOW
#endif
#ifndef AXIS5_STEP_STATE
#define AXIS5_STEP_STATE              HIGH
#endif
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
#ifndef AXIS5_SERVO_PARAMETER1      
#define AXIS5_SERVO_PARAMETER1        1
#endif
#ifndef AXIS5_SERVO_PARAMETER2 
#define AXIS5_SERVO_PARAMETER2        0
#endif

// common axis driver settings, FOCUSER3
#if AXIS6_DRIVER_MODEL >= DRIVER_FIRST && AXIS6_DRIVER_MODEL <= DRIVER_LAST
#define AXIS6_DRIVER_SD
#endif
#if AXIS6_DRIVER_MODEL == TMC2130 || AXIS6_DRIVER_MODEL == TMC5160
#define AXIS6_DRIVER_TMC_SPI
#endif
#if AXIS6_DRIVER_MODEL >= SERVO_DRIVER_FIRST
#define AXIS6_SERVO
#endif
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
#ifndef AXIS6_PARAMETER1      
#define AXIS6_PARAMETER1              AXIS6_DRIVER_MICROSTEPS
#endif
#ifndef AXIS6_PARAMETER2 
#define AXIS6_PARAMETER2              AXIS6_DRIVER_IRUN
#endif
#ifndef AXIS6_ENABLE_STATE
#define AXIS6_ENABLE_STATE            LOW
#endif
#ifndef AXIS6_STEP_STATE
#define AXIS6_STEP_STATE              HIGH
#endif
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
#ifndef AXIS6_SERVO_PARAMETER1      
#define AXIS6_SERVO_PARAMETER1        1
#endif
#ifndef AXIS6_SERVO_PARAMETER2 
#define AXIS6_SERVO_PARAMETER2        0
#endif

// common axis driver settings, FOCUSER4
#if AXIS7_DRIVER_MODEL >= DRIVER_FIRST && AXIS7_DRIVER_MODEL <= DRIVER_LAST
#define AXIS7_DRIVER_SD
#endif
#if AXIS7_DRIVER_MODEL == TMC2130 || AXIS7_DRIVER_MODEL == TMC5160
#define AXIS7_DRIVER_TMC_SPI
#endif
#if AXIS7_DRIVER_MODEL >= SERVO_DRIVER_FIRST
#define AXIS7_SERVO
#endif
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
#ifndef AXIS7_DRIVER_PARAMETER1      
#define AXIS7_DRIVER_PARAMETER1       AXIS7_DRIVER_MICROSTEPS
#endif
#ifndef AXIS7_DRIVER_PARAMETER2 
#define AXIS7_DRIVER_PARAMETER2       AXIS7_DRIVER_IRUN
#endif
#ifndef AXIS7_ENABLE_STATE
#define AXIS7_ENABLE_STATE            LOW
#endif
#ifndef AXIS7_STEP_STATE
#define AXIS7_STEP_STATE              HIGH
#endif
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
#ifndef AXIS7_SERVO_PARAMETER1      
#define AXIS7_SERVO_PARAMETER1        1
#endif
#ifndef AXIS7_SERVO_PARAMETER2 
#define AXIS7_SERVO_PARAMETER2        0
#endif

// common axis driver settings, FOCUSER5
#if AXIS8_DRIVER_MODEL >= DRIVER_FIRST && AXIS8_DRIVER_MODEL <= DRIVER_LAST
#define AXIS8_DRIVER_SD
#endif
#if AXIS8_DRIVER_MODEL == TMC2130 || AXIS8_DRIVER_MODEL == TMC5160
#define AXIS8_DRIVER_TMC_SPI
#endif
#if AXIS8_DRIVER_MODEL >= SERVO_DRIVER_FIRST
#define AXIS8_SERVO
#endif
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
#ifndef AXIS8_PARAMETER1      
#define AXIS8_PARAMETER1              AXIS8_DRIVER_MICROSTEPS
#endif
#ifndef AXIS8_PARAMETER2 
#define AXIS8_PARAMETER2              AXIS8_DRIVER_IRUN
#endif
#ifndef AXIS8_ENABLE_STATE
#define AXIS8_ENABLE_STATE            LOW
#endif
#ifndef AXIS8_STEP_STATE
#define AXIS8_STEP_STATE              HIGH
#endif
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
#ifndef AXIS8_SERVO_PARAMETER1      
#define AXIS8_SERVO_PARAMETER1        1
#endif
#ifndef AXIS8_SERVO_PARAMETER2 
#define AXIS8_SERVO_PARAMETER2        0
#endif

// common axis driver settings, FOCUSER6
#if AXIS9_DRIVER_MODEL >= DRIVER_FIRST && AXIS9_DRIVER_MODEL <= DRIVER_LAST
#define AXIS9_DRIVER_SD
#endif
#if AXIS9_DRIVER_MODEL == TMC2130 || AXIS9_DRIVER_MODEL == TMC5160
#define AXIS9_DRIVER_TMC_SPI
#endif
#if AXIS9_DRIVER_MODEL >= SERVO_DRIVER_FIRST
#define AXIS9_SERVO
#endif
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
#ifndef AXIS9_PARAMETER1      
#define AXIS9_PARAMETER1              AXIS9_DRIVER_MICROSTEPS
#endif
#ifndef AXIS9_PARAMETER2 
#define AXIS9_PARAMETER2              AXIS9_DRIVER_IRUN
#endif
#ifndef AXIS9_ENABLE_STATE
#define AXIS9_ENABLE_STATE            LOW
#endif
#ifndef AXIS9_STEP_STATE
#define AXIS9_STEP_STATE              HIGH
#endif
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
#ifndef AXIS9_SERVO_PARAMETER1      
#define AXIS9_SERVO_PARAMETER1        1
#endif
#ifndef AXIS9_SERVO_PARAMETER2 
#define AXIS9_SERVO_PARAMETER2        0
#endif
