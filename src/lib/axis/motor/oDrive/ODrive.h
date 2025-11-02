// -----------------------------------------------------------------------------------
// axis odrive servo motor
#pragma once
#include "../../../../Common.h"

#ifdef ODRIVE_MOTOR_PRESENT

#include "../Motor.h"
#include "../../../convert/Convert.h"

// the following would be in the pinmap normally and should trigger #error on compile here when not present
// for now, they fit your hardware as best as I can tell...
#ifndef ODRIVE_SERIAL
  #define ODRIVE_SERIAL      Serial3  // Teensy HW Serial3 (if used,) for example
#endif
#ifndef ODRIVE_SERIAL_BAUD
  #define ODRIVE_SERIAL_BAUD 115200   // was 19200
#endif

#ifndef ODRIVE_RST_PIN
  #define ODRIVE_RST_PIN     3
#endif

// odrive update rate default 10Hz
#ifndef ODRIVE_UPDATE_MS
  #define ODRIVE_UPDATE_MS   100      // was 3000
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
// in arc seconds..one encoder tick encoder resolution = 2^14 = 16380
// 16380/360=45.5 ticks/deg; 45.5/60=0.7583 ticks/min; 0.7583/60 = .00126 ticks/sec
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
    ODriveMotor(uint8_t axisNumber, int8_t reverse, const ODriveDriverSettings *Settings, float radsPerCount, bool useFastHardwareTimers = true);

    // sets up the odrive motor
    bool init();

    // returns the number of parameters from the motor
    uint8_t getParameterCount() { return Motor::getParameterCount() + numParameters; }

    // returns the specified axis parameter
    AxisParameter* getParameter(uint8_t number) {
      if (number == Motor::getParameterCount() + 1) return &radsPerCount; else
      if (number >= 1 && number <= Motor::getParameterCount()) return Motor::getParameter(number); else
      return &invalid;
    }

    // set motor reverse state
    void setReverse(int8_t state);

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

    // runtime adjustable settings
    AxisParameter radsPerCount = {NAN, NAN, NAN, degToRadF(300.0), degToRadF(360000.0), AXP_FLOAT, AXPN_RADS_PER_COUNT};

    const int numParameters = 1;
    AxisParameter* parameter[2] = {&invalid, &radsPerCount};
};

#endif