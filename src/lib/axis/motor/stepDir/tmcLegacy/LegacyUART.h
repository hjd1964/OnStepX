// -----------------------------------------------------------------------------------
// axis step/dir motor driver
#pragma once

#include <Arduino.h>
#include "../../../../../Common.h"

#if !defined(DRIVER_TMC_STEPPER) && defined(STEP_DIR_TMC_UART_PRESENT)

#include "../StepDirDriver.h"

// default settings for any TMC UART drivers that may be present
#ifndef SERIAL_TMC
  #define SERIAL_TMC                  SoftSerial     // Use software serial with TX on M3 (CS) of each axis
#endif
#ifndef SERIAL_TMC_RX_DISABLE
  #define SERIAL_TMC_RX_DISABLE       true           // Recieving data often doesn't work with software serial
#endif
#ifndef SERIAL_TMC_BAUD
  #define SERIAL_TMC_BAUD             115200         // Baud rate
#endif
#ifndef SERIAL_TMC_ADDRESS_MAP
  #define SERIAL_TMC_ADDRESS_MAP(x)   (0)            // driver addresses are 0 for all axes
#endif
#ifndef SERIAL_TMC_INVERT
  #define SERIAL_TMC_INVERT           OFF            // invert the serial interface signal logic
#endif

#ifndef SERIAL_TMC_HARDWARE_UART
  #include <SoftwareSerial.h>                        // source as appropriate for your platform (if not built-in)
  #define TMC2209_SOFTWARE_SERIAL
#endif

#define TMC2209_DEBUG false
#include <TMC2209.h> // https://github.com/hjd1964/TMC2209

class StepDirTmcUART : public StepDirDriver {
  public:
    // constructor
    StepDirTmcUART(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings);

    // get driver type code
    inline char getParameterTypeCode() { return 'T'; }

    // set up driver and parameters: microsteps, microsteps goto, hold current, run current, goto current, unused
    void init(float param1, float param2, float param3, float param4, float param5, float param6);

    // validate driver parameters
    bool validateParameters(float param1, float param2, float param3, float param4, float param5, float param6);

    // set microstep mode for tracking
    void modeMicrostepTracking();

    // set microstep mode for slewing
    int modeMicrostepSlewing();

    // set decay mode for tracking
    void modeDecayTracking();

    // set decay mode for slewing
    void modeDecaySlewing();

    // update status info. for driver
    void updateStatus();

    // secondary way to power down not using the enable pin
    bool enable(bool state);

    // calibrate the motor driver if required
    void calibrateDriver();

    TMC2209Stepper *driver;

  private:
    // checks if decay pin should be HIGH/LOW for a given decay setting
    int8_t getDecayPinState(int8_t decay);

    const int MicroStepCodeToMode[9] = {256, 128, 64, 32, 16, 8, 4, 2, 1};

    // TMC2209/TMC5160 specific
    uint32_t pc_pwm_grad  = 0x04; // default=4,   range 0 to 14  (PWM gradient scale using automatic current control)
    uint32_t pc_pwm_auto  = 0x01; // default=1,   range 0 to 1   (PWM automatic current control 0: off, 1: on)
    uint32_t pc_pwm_ofs   = 0x1e; // default=30,  range 0 to 255 (PWM user defined amplitude offset related to full motor current)
};

#endif
