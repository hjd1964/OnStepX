// -----------------------------------------------------------------------------------
// axis servo motor
#pragma once
#include "../../../../Common.h"

#ifdef SERVO_MOTOR_PRESENT

#include "../../../encoder/Encoder.h"
#include "filter/Filter.h"
#include "feedback/Feedback.h"
#include "calibration/TrackingVelocity.h"

#include "dc/eE/EE.h"
#include "dc/pE/PE.h"
#include "dc/tmc/tmc2130/Tmc2130.h"
#include "dc/tmc/tmc5160/Tmc5160.h"
#include "kTech/KTech.h"
#include "tmc/tmc2209/Tmc2209.h"
#include "tmc/tmc5160/Tmc5160.h"

#ifndef SERVO_SLEW_DIRECT
  #define SERVO_SLEW_DIRECT OFF
#endif

#ifndef SERVO_SLEWING_TO_TRACKING_DELAY
  #define SERVO_SLEWING_TO_TRACKING_DELAY 3000 // in milliseconds
#endif

#ifndef SERVO_SAFETY_STALL_POWER
  #define SERVO_SAFETY_STALL_POWER 33 // in percent
#endif

#ifndef AXIS1_SERVO_VELOCITY_CALIBRATION
  #define AXIS1_SERVO_VELOCITY_CALIBRATION 1
#endif

class ServoMotor : public Motor {
  public:
    // constructor
    ServoMotor(uint8_t axisNumber, int8_t reverse, ServoDriver *Driver, Filter *filter, Encoder *encoder, uint32_t encoderOrigin, bool encoderReverse, Feedback *feedback, ServoControl *control, long syncThreshold, bool useFastHardwareTimers = true);

    // sets up the servo motor
    bool init();

    // returns the number of parameters from the motor/driver
    uint8_t getParameterCount() { return Motor::getParameterCount() + driver->getParameterCount() + feedback->getParameterCount(); }

    // returns the specified axis parameter
    AxisParameter* getParameter(uint8_t number) {
      if (number > Motor::getParameterCount() + driver->getParameterCount()) return feedback->getParameter(number - (Motor::getParameterCount() + driver->getParameterCount())); else
      if (number > Motor::getParameterCount()) return driver->getParameter(number - Motor::getParameterCount()); else
      if (number >= 1 && number <= Motor::getParameterCount()) return Motor::getParameter(number); else
      return &invalid;
    }

    // check if parameter is valid
    bool parameterIsValid(AxisParameter* parameter, bool next = false) { if (!Motor::parameterIsValid(parameter, next)) return false; else return driver->parameterIsValid(parameter, next); }
    
    // sets reversal of axis directions
    // \param state: true reverses the direction behavior specified in settings
    void setReverse(int8_t state);

    // sets motor enable on/off (if possible)
    void enable(bool value);

    // get the associated motor driver status
    DriverStatus getDriverStatus();

    // resets motor and target angular position in steps, also zeros backlash and index 
    void resetPositionSteps(long value);

    // get instrument coordinate, in steps
    long getInstrumentCoordinateSteps();

    // set instrument coordinate, in steps
    void setInstrumentCoordinateSteps(long value);

    // distance to target in steps (+/-)
    long getTargetDistanceSteps();

    // get tracking mode steps per slewing mode step
    inline int getStepsPerStepSlewing() { return 64; }

    // get movement frequency in steps per second
    float getFrequencySteps();

    // set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
    void setFrequencySteps(float frequency);

    // set slewing state (hint that we are about to slew or are done slewing)
    void setSlewing(bool state);

    // calibrate the motor driver
    void calibrateDriver() { if (ready) driver->calibrateDriver(); }

    // get encoder count
    int32_t getEncoderCount() { if (ready) return encoder->count; else return 0; }

    // set zero of absolute encoders
    uint32_t encoderZero();

    // set origin of absolute encoders
    void encoderSetOrigin(uint32_t origin) { if (ready) encoder->setOrigin(origin); }

    // read encoder
    int32_t encoderRead();

    // get the motor name
    const char* name() { strcpy(nameStr, "Servo, "); strcat(nameStr, driver->name()); return nameStr; }

    // updates PID and sets servo motor power/direction
    void poll();

    // sets dir as required and moves coord toward target at setFrequencySteps() rate
    void move();
    
    // servo motor driver
    ServoDriver *driver;

    // servo encoder
    Encoder *encoder;

    float velocityPercent = 0.0F;
    long delta = 0;

  private:
    Filter *filter;

    float velocityEstimate = 0.0F;
    float velocityOverride = 0.0F;

    uint8_t servoMonitorHandle = 0;
    uint8_t taskHandle = 0;
    float maxFrequency = HAL_FRACTIONAL_SEC; // fastest timer rate

    int  stepSize = 1;                  // step size
    volatile int  homeSteps = 1;        // step count for microstep sequence between home positions (driver indexer)
    volatile bool takeStep = false;     // should we take a step
    float trackingFrequency = 0;        // help figure out if equatorial mount is tracking

    float currentDirection = 0.0F;      // last direction
    float currentFrequency = 0.0F;      // last frequency set in encoder counts per second
    float lastFrequency = 0.0F;         // last frequency requested
    unsigned long lastPeriod = 0;       // last timer period (in sub-micros)
    long syncThreshold = OFF;           // sync threshold in counts (for absolute encoders) or OFF

    long lastRawEncoderCounts = 0;      // the last encoder position for velocity calculation
    unsigned long lastEncoderReadUs = 0;
    unsigned long lastEncoderReadMs = 0;
    
    long lastEncoderCounts = 0;         // the last encoder position for stall check
    long lastDelta = 0;                 // the last distance from target for runaway check
    long movingAwaySeconds = 0;         // amount of time for runaway check
    unsigned long lastCheckTime = 0;    // time since the last encoder position was checked
    unsigned long startTime = 0;        // time at start of servo polling
    unsigned long lastSlewingTime = 0;  // time when last slewing

    volatile int absStep = 1;           // absolute step size (unsigned)
    volatile long originIndexSteps = 0; // for absolute motor position to axis position at coordinate origin

    void (*callback)() = NULL;

    Feedback *feedback;
    ServoControl *control;
    #ifdef CALIBRATE_SERVO_DC
      ServoCalibrateTrackingVelocity *calibrateVelocity;
    #endif

    bool useFastHardwareTimers = true;
    bool slewing = false;
    bool motorStepsInitDone = false;
    bool homeSet = false;
    uint32_t encoderOrigin = 0;
    bool encoderReverse = false;
    bool encoderReverseDefault = false;
    bool wasAbove33 = false;
    bool wasBelow33 = false;
    bool safetyShutdown = false;
    long lastTargetDistance = 0;
};

#endif