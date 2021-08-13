// -----------------------------------------------------------------------------------
// stepper driver control
#pragma once

#include <Arduino.h>
#include "../../../Common.h"

// the various microsteps for different driver models, with the bit modes for each
#define DRIVER_SERVO_MODEL_COUNT 2

#include "DcDrivers.defaults.h"

#ifdef SERVO_DRIVER_PRESENT

#pragma pack(1)
#define StepDriverSettingsSize 15
typedef struct DriverSettings {
  int16_t model;
  int16_t p;
  int16_t i;
  int16_t d;
  int8_t  status;
} DriverSettings;
#pragma pack()

typedef struct DriverPins {
  uint8_t axis;
  int16_t in1;
  uint8_t inState1;
  int16_t in2;
  uint8_t inState2;
  int16_t enable;
  uint8_t enabledState;
  int16_t fault;
} DriverPins;

typedef struct DriverOutputStatus {
  bool shortToGround;
  bool openLoad;
} DriverOutputStatus;

typedef struct DriverStatus {
  DriverOutputStatus outputA;
  DriverOutputStatus outputB;
  bool overTemperaturePreWarning;
  bool overTemperature;
  bool standstill;
  bool fault;
} DriverStatus;

class DcDriver {
  public:
    // decodes driver model/microstep mode into microstep codes (bit patterns or SPI)
    // and sets up the pin modes
    void init(uint8_t axisNumber);

    // update status info. for driver
    // this is a required method for the Axis class
    void updateStatus();

    // get status info.
    // this is a required method for the Axis class
    DriverStatus getStatus();

    // secondary way to power down not using the enable pin
    // this is a required method for the Axis class, even if it does nothing
    void power(bool state);

    // get the microsteps goto
    // this is a required method for the Axis class, even if it only ever returns 1
    inline int getSubdivisionsGoto() { return 1; }

    // power level to the motor (0 to 100%)
    void DcDriver::motorPower(float percent);

    // motor direction (DIR_FORMWARD or DIR_REVERSE)
    void DcDriver::motorDirection(Direction dir);

    DriverSettings settings;

  private:
    void DcDriver::update();

    int axisNumber;
    int index;
    bool powered = false;
    float dcPower = 0.0F;
    Direction dcDirection = DIR_FORWARD;

    DriverStatus status = {{false, false}, {false, false}, false, false, false, false};

    DriverPins pins;
};

#endif
