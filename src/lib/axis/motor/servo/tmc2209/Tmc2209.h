// -----------------------------------------------------------------------------------
// axis servo TMC2209 stepper motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#ifdef SERVO_TMC2209_PRESENT

#include "../ServoDriver.h"

#ifndef DRIVER_TMC_STEPPER_AUTOGRAD
  #define DRIVER_TMC_STEPPER_AUTOGRAD true
#endif

// default settings for any TMC UART drivers that may be present
#ifndef SERIAL_TMC
  #define SERIAL_TMC                  SoftSerial     // Use software serial w/ TX on M3 (CS) and RX on M4 (MISO) of each axis
#endif
#ifndef SERIAL_TMC_BAUD
  #define SERIAL_TMC_BAUD             115200         // Baud rate
#endif
#ifndef SERIAL_TMC_ADDRESS_MAP
  #define SERIAL_TMC_ADDRESS_MAP(x)   (0)            // driver addresses are 0 for all axes
#endif

#if SERIAL_TMC == SoftSerial
  #include <SoftwareSerial.h> // must be built into the board libraries
#endif

#include <TMCStepper.h> // https://github.com/teemuatlut/TMCStepper

typedef struct ServoTmcPins {
  int16_t step;
  int16_t dir;
  int16_t enable;
  uint8_t enabledState;
  int16_t m0;
  int16_t m1;
  int16_t fault;
} ServoTmcPins;

typedef struct ServoTmcSettings {
  int16_t model;
  int8_t  status;
  int32_t velocityMax;   // maximum velocity in steps/second
  int32_t velocityThrs;  // velocity for stealthChop to spreadCycle switch in steps/second
  int32_t acceleration;  // acceleration steps/second/second
  int16_t microsteps;
  int16_t current;       // peak current in mA
  int8_t  decay;
  int8_t  decaySlewing;
} ServoTmcSettings;

class ServoTmc2209 : public ServoDriver {
  public:
    // constructor
    ServoTmc2209(uint8_t axisNumber, const ServoTmcPins *Pins, const ServoTmcSettings *TmcSettings);

    // decodes driver model and sets up the pin modes
    void init();

    // move using step/dir signals
    void alternateMode(bool state);

    // enable or disable the driver using the enable pin or other method
    void enable(bool state);

    // power level to the motor
    float setMotorVelocity(float power);

    // update status info. for driver
    void updateStatus();

    // calibrate the motor if required
    void calibrateDriver();

    const ServoTmcSettings *Settings;

  private:
    float rSense = 0.11F;

    bool stealthChop() { if (decay == STEALTHCHOP) return true; else return false; }

    #if SERIAL_TMC == SoftSerial
      SoftwareSerial SerialTMC;
    #endif

    TMC2209Stepper *driver;

    int16_t currentRms;
    int16_t decay;
    int16_t decaySlewing;
    int16_t velocityThrs;
    bool powered = false;
    bool sdMode = false;
    float currentVelocity = 0.0F;
    float acceleration;
    float accelerationFs;
    const ServoTmcPins *Pins;
};

#endif
