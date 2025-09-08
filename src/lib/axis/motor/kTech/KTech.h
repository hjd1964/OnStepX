// -----------------------------------------------------------------------------------
// axis ktech servo motor
#pragma once
#include "../../../../Common.h"

#ifdef KTECH_MOTOR_PRESENT

#if !defined(CAN_PLUS) || CAN_PLUS == OFF
  #error "No KTECH motor CAN interface!"
#endif

#include "../Motor.h"
#include "../../../convert/Convert.h"

// KTECH update rate default 10Hz
#ifndef CAN_SEND_RATE_MS
  #define CAN_SEND_RATE_MS 100
#endif

// KTECH status rate default 1Hz
#ifndef KTECH_STATUS_MS
  #define KTECH_STATUS_MS 1000
#endif

// KTECH direct slewing ON or OFF (KTech handles acceleration)
#ifndef KTECH_SLEW_DIRECT
  #define KTECH_SLEW_DIRECT OFF
#endif

typedef struct KTechDriverSettings {
  int16_t model;
  int8_t  status;
} KTechDriverSettings;

class KTechMotor : public Motor {
  public:
    // constructor
    KTechMotor(uint8_t axisNumber, int8_t reverse, const KTechDriverSettings *Settings, bool useFastHardwareTimers = true);

    // sets up the ktech motor
    bool init();

    // low level reversal of axis directions
    // \param state: OFF normal or ON to reverse
    void setReverse(int8_t state);

    // sets motor enable on/off (if possible)
    void enable(bool value);

    // set instrument coordinate, in steps
    void setInstrumentCoordinateSteps(long value);

    // get the associated driver status
    DriverStatus getDriverStatus() { if (ready) updateStatus(); else status = errorStatus; return status; }

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

    // update the associated driver status
    void updateStatus();

    // update the associated driver status from CAN
    void requestStatusCallback(uint8_t data[8]);

    // get the motor name
    const char* name() { return "KTECH"; }

  private:
    int canID;

    unsigned long lastSetPositionTime = 0;
    unsigned long lastStatusRequestTime = 0;
    unsigned long lastStatusUpdateTime = 0;

    uint8_t taskHandle = 0;

    int  stepSize = 1;                  // step size
    volatile int  homeSteps = 1;        // step count for microstep sequence between home positions (driver indexer)
    volatile bool takeStep = false;     // should we take a step
    long lastTarget = 0;

    float currentFrequency = 0.0F;      // last frequency set 
    float lastFrequency = 0.0F;         // last frequency requested
    unsigned long lastPeriod = 0;       // last timer period (in sub-micros)
    float maxFrequency = HAL_FRACTIONAL_SEC; // fastest timer rate

    volatile int absStep = 1;           // absolute step size (unsigned)

    void (*callback)() = NULL;

    bool useFastHardwareTimers = true;

    bool isSlewing = false;

    int16_t statusMode = OFF;
    DriverStatus status = { false, {false, false}, {false, false}, false, false, false, false };
    bool statusRequestSent = false;

    float stepsPerMeasure = 0.0F;
};

#endif