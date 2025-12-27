// -----------------------------------------------------------------------------------
// oDrive servo motor
// EXPERIMENTAL!!!
#pragma once
#include "../../../../Common.h"

#ifdef ODRIVE_NEW_MOTOR_PRESENT
#ifdef ODRIVE_MOTOR_PRESENT

#include "../Motor.h"
#include "../../../convert/Convert.h"

#ifndef ODRIVE_RST_PIN
  #define ODRIVE_RST_PIN     3
#endif

// odrive update rate default 40Hz
#ifndef ODRIVE_UPDATE_MS
  #define ODRIVE_UPDATE_MS   CAN_SEND_RATE_MS // normally > about 5ms and < 100ms
#endif

// odrive direct slewing ON or OFF (ODrive handles acceleration)
// ON to use ODrive trapezoidal move profile, OFF for OnStep move profile
#ifndef ODRIVE_SLEW_DIRECT
  #define ODRIVE_SLEW_DIRECT OFF
#endif

// odrive if using absolute encoders set to ON
#ifndef ODRIVE_ABSOLUTE
  #define ODRIVE_ABSOLUTE    ON       // was OFF
#endif

// odrive sync limit (for absolute encoders) OFF or specify the sync limit in arc-seconds
// this should, I hope, allow you to limit just how far from the encoders you can sync. (+/-) to fine-tune the origin
// with absolute encoders this protects you from exceeding the software min/max limits by > the amount specified
// in arc seconds..one encoder tick encoder resolution = 2^14 = 16384
// 16384/360=45.5 ticks/deg; 45.5/60=0.7583 ticks/min; 0.7583/60 = .00126 ticks/sec
// or 1/0.7583 = 1.32 arc-min/tick; 1.32*60 sec = 79.2 arc sec per encoder tick
#ifndef ODRIVE_SYNC_LIMIT
  #define ODRIVE_SYNC_LIMIT  80       // was OFF
#endif

// Use OD_UART or OD_CAN...I2C may be added later
#ifndef ODRIVE_COMM_MODE
  #define ODRIVE_COMM_MODE   OD_CAN
#endif

// ODrive axis 0 = OnStep Axis2 = DEC or ALT
// ODrive axis 1 = OnStep Axis1 = RA or AZM
#ifndef ODRIVE_SWAP_AXES
  #define ODRIVE_SWAP_AXES   ON
#endif

typedef struct ODriveDriverSettings {
  int16_t model;
  int8_t  status;
} ODriveDriverSettings;

class ODriveMotor : public Motor {
  public:
    // constructor
    ODriveMotor(uint8_t axisNumber, int8_t reverse, const ODriveDriverSettings *Settings, float countsPerRad, bool useFastHardwareTimers = true);

    // sets up the odrive motor
    bool init();

    // returns the number of parameters from the motor
    uint8_t getParameterCount() { return Motor::getParameterCount() + numParameters; }

    // returns the specified axis parameter
    AxisParameter* getParameter(uint8_t number) {
      if (number == Motor::getParameterCount() + 1) return &countsPerRad; else
      if (number >= 1 && number <= Motor::getParameterCount()) return Motor::getParameter(number); else
      return &invalid;
    }

    // set motor reverse state
    void setReverse(bool state);

    // sets motor enable on/off (if possible)
    void enable(bool value);

    // set instrument coordinate, in steps
    void setInstrumentCoordinateSteps(long value);

    // get the associated driver status
    inline DriverStatus getDriverStatus() { if (ready) return status; else return errorStatus; }

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

    // get the motor name
    const char* name() { return "ODRIVE"; }

    // updates PID and sets servo motor power/direction
    void poll();

    // sets dir as required and moves coord toward target at setFrequencySteps() rate
    void move();

  private:
    void stopSyntheticMotion();
    void resetToTrackingBaseline();

    float feedForwardVelocity = 0.0F;
    unsigned long lastSetPositionTime = 0;
    unsigned long lastEncoderUpdateRequestTime = 0;
    uint8_t oDriveNode = 0;             // ODrive node for this mount axis (swapped if ODRIVE_SWAP_AXES==ON)
    uint8_t taskHandle = 0;

    int  stepSize = 1;                  // step size

    unsigned long lastPeriod = 0;       // last timer period (in sub-micros)
    float maxFrequency = HAL_FRACTIONAL_SEC; // fastest timer rate

    volatile int absStep = 0;           // absolute step size (unsigned)

    void (*callback)() = NULL;

    bool useFastHardwareTimers = true;

    bool enablePending = false;
    bool enableTarget = false;
    uint32_t enableDeadlineMs = 0;
    bool isSlewing = false;

    DriverStatus status = { false, {false, false}, {false, false}, false, false, false, false };

    // runtime adjustable settings
    AxisParameter countsPerRad = {NAN, NAN, NAN, 300.0*RAD_DEG_RATIO_F, 360000.0*RAD_DEG_RATIO_F, AXP_FLOAT, AXPN_COUNTS_PER_RAD};

    const int numParameters = 1;
    AxisParameter* parameter[2] = {&invalid, &countsPerRad};
};

#endif
#endif
