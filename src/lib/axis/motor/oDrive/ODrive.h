// -----------------------------------------------------------------------------------
// axis servo motor
#pragma once
#include "../../../../Common.h"

// SERVO DRIVER (usually for DC motors equipped with encoders)
#define ODRIVE_MODEL_COUNT 1

#ifndef ODRIVE_DRIVER_FIRST
  #define ODRIVE_DRIVER_FIRST       200
  #define ODRIVE                    200
  #define ODRIVE_DRIVER_LAST        200
#endif

#ifdef ODRIVE_MOTOR_PRESENT

#include "../Motor.h"

// the following would be in the pinmap normally and should trigger #error on compile here when not present
// for now, they fit your hardware as best as I can tell...
#ifndef ODRIVE_SERIAL
  #define ODRIVE_SERIAL Serial3
#endif
#ifndef ODRIVE_RST_PIN
  #define ODRIVE_RST_PIN 3
#endif

#ifndef ODRIVE_UPDATE_MS
  #define ODRIVE_UPDATE_MS 100 // for a 10Hz update rate
#endif

#include <ODriveArduino.h> // https://github.com/odriverobotics/ODrive/tree/master/Arduino/ODriveArduino

typedef struct ODriveDriverSettings {
  int16_t model;
  int8_t  status;
} ODriveDriverSettings;

class ODriveMotor : public Motor {
  public:
    // constructor
    ODriveMotor(uint8_t axisNumber, const ODriveDriverSettings *Settings, bool useFastHardwareTimers = true);

    // sets up the odrive motor
    bool init();

    // set driver reverse state
    void setReverse(int8_t state);

    // get driver type code
    inline char getParameterTypeCode() { return 'O'; }  // codes used so far are S(tep/dir), T(mc), P(id), and O(Drive)

    // set driver parameters
    void setParameters(float param1, float param2, float param3, float param4, float param5, float param6);

    // validate driver parameters
    bool validateParameters(float param1, float param2, float param3, float param4, float param5, float param6);

    // sets motor power on/off (if possible)
    void power(bool value);

    // get the associated driver status
    DriverStatus getDriverStatus();

    // resets motor and target angular position in steps, also zeros backlash and index 
    void resetPositionSteps(long value);

    // get tracking mode steps per slewing mode step
    inline int getStepsPerStepSlewing() { return 64; }

    // get movement frequency in steps per second
    float getFrequencySteps();

    // set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
    void setFrequencySteps(float frequency);

    // set slewing state (hint that we are about to slew or are done slewing)
    void setSlewing(bool state);

    // updates PID and sets servo motor power/direction
    void poll();

    // sets dir as required and moves coord toward target at setFrequencySteps() rate
    void move();

  private:
    unsigned long lastSetPositionTime = 0;
    uint8_t oDriveMonitorHandle = 0;
    uint8_t taskHandle = 0;

    int  stepSize = 1;                  // step size
    volatile int  homeSteps = 1;        // step count for microstep sequence between home positions (driver indexer)
    volatile bool takeStep = false;     // should we take a step

    float currentFrequency = 0.0F;      // last frequency set 
    float lastFrequency = 0.0F;         // last frequency requested
    unsigned long lastPeriod = 0;       // last timer period (in sub-micros)

    volatile int absStep = 1;           // absolute step size (unsigned)

    void (*callback)() = NULL;

    bool useFastHardwareTimers = true;

    bool isSlewing = false;

    DriverStatus status = { false, {false, false}, {false, false}, false, false, false, false };
    float stepsPerMeasure = 0.0F;
};

#endif