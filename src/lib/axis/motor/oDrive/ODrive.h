// -----------------------------------------------------------------------------------
// axis odrive servo motor
#pragma once
#include "../../../../Common.h"

// ODRIVE DRIVER MODEL
#ifndef ODRIVE_DRIVER_FIRST
  #define ODRIVE_DRIVER_FIRST       200
  #define ODRIVE                    200
  #define ODRIVE_DRIVER_LAST        200
#endif

#ifdef ODRIVE_MOTOR_PRESENT

#include "../Motor.h"
#include "../../../convert/Convert.h"

// the following would be in the pinmap normally and should trigger #error on compile here when not present
// for now, they fit your hardware as best as I can tell...
#ifndef ODRIVE_SERIAL
  #define ODRIVE_SERIAL      Serial3 
#endif
#ifndef ODRIVE_SERIAL_BAUD
  #define ODRIVE_SERIAL_BAUD 19200
#endif
#ifndef ODRIVE_RST_PIN
  #define ODRIVE_RST_PIN     3
#endif

// odrive update rate default 10Hz
#ifndef ODRIVE_UPDATE_MS
  #define ODRIVE_UPDATE_MS   3000
#endif

// odrive direct slewing ON or OFF (ODrive handles acceleration)
#ifndef ODRIVE_SLEW_DIRECT
  #define ODRIVE_SLEW_DIRECT OFF
#endif

// odrive if using absolute encoders set to ON
#ifndef ODRIVE_ABSOLUTE
  #define ODRIVE_ABSOLUTE    OFF
#endif

// odrive sync limit (for absolute encoders) OFF or specify the sync limit in arc-seconds
// this should, I hope, allow you to limit just how far from the encoders you can sync. (+/-) to fine-tune the origin
// with absolute encoders this protects you from exceeding the software min/max limits by > the amount specified
#ifndef ODRIVE_SYNC_LIMIT
  #define ODRIVE_SYNC_LIMIT  OFF
#endif

#if ODRIVE_COMM_MODE == OD_UART
  #include <ODriveArduino.h> // https://github.com/odriverobotics/ODrive/tree/master/Arduino/ODriveArduino 
  // ODrive servo motor serial driver
  extern ODriveArduino *_oDriveDriver;
#elif ODRIVE_COMM_MODE == OD_CAN
  #include <FlexCAN_T4.h> // https://github.com/tonton81/FlexCAN_T4.git
  // changes were required to this CAN library so it is local now
  #include "src/plugins/DDScope/ODriveTeensyCAN/ODriveTeensyCAN.h" //https://github.com/Malaphor/ODriveTeensyCAN.git
  extern ODriveTeensyCAN *_oDriveDriver;
#endif

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

    // sets motor enable on/off (if possible)
    void enable(bool value);

    // set instrument coordinate, in steps
    void setInstrumentCoordinateSteps(long value);

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

//  float o_position0 = 0;
//  float o_position1 = 0;

    #if ODRIVE_COMM_MODE == OD_UART
    // special command to send high resolution position to odrive
    void setPosition(int motor_number, float position) {
//    if (motor_number == 0) o_position0 = position;
//    if (motor_number == 1) o_position1 = position;
      char command[32];
      sprintF(command, "p n %1.8f\n", position);
      command[2] = '0' + motor_number;
      ODRIVE_SERIAL.print(command);
    }
    #endif

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