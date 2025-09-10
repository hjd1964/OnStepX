// -----------------------------------------------------------------------------------
// axis servo TMC2209 stepper motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../../Common.h"

#ifdef SERVO_TMC2209_PRESENT

#include "../TmcServoDriver.h"

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

class ServoTmc2209 : public TmcServoDriver {
  public:
    // constructor
    ServoTmc2209(uint8_t axisNumber, const ServoPins *Pins, const ServoSettings *Settings, float countsToStepsRatio, int16_t microsteps, int16_t current, int8_t decay, int8_t decaySlewing);

    // decodes driver model and sets up the pin modes
    bool init(bool reverse);

    // enable or disable the driver using the enable pin or other method
    void enable(bool state);

    // set motor velocity
    // \param velocity as needed to reach the target position, in encoder counts per second
    // \returns velocity in effect, in encoder counts per second
    float setMotorVelocity(float velocity);

    // calibrate the motor if required
    void calibrateDriver();

    // get the driver name
    const char* name() { return "TMC2209 (UART)"; }

  private:
    // read status from driver
    void readStatus();

    bool stealthChop() { if (lround(decay.value) == STEALTHCHOP) return true; else return false; }

    #if SERIAL_TMC == SoftSerial
      SoftwareSerial *SerialTMC;
    #endif

    TMC2209Stepper *driver;

    bool sdMode = false;

    // for switching to/from spreadCycle
    int16_t velocityThrs;
};

#endif
