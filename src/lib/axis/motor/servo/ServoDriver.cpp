// -----------------------------------------------------------------------------------
// axis servo motor driver

#include "ServoDriver.h"

#ifdef SERVO_MOTOR_PRESENT

// the various servo driver models
#define SERVO_DRIVER_MODEL_COUNT (SERVO_DRIVER_LAST - SERVO_DRIVER_FIRST + 1)

#include "../Motor.h"

#if DEBUG != OFF
  const char* SERVO_DRIVER_NAME[SERVO_DRIVER_MODEL_COUNT] =
  {
    "SERVO_PE",         // Phase (Direction)/Enable
    "SERVO_EE",         // Enable/Enable
    "SERVO_TMC2130_DC", // TMC2130 w/ DC motor
    "SERVO_TMC5160_DC", // TMC5160 w/ DC motor
    "SERVO_TMC2209",    // TMC2209 w/ stepper motor
    "SERVO_TMC5160"     // TMC5160 w/ stepper motor
  };
#endif

void ServoDriver::init() {
  #if DEBUG == VERBOSE
    VF("MSG: ServoDriver"); V(axisNumber); VF(", init model "); VL(SERVO_DRIVER_NAME[model - SERVO_DRIVER_FIRST]);
    VF("MSG: ServoDriver"); V(axisNumber); VF(", en=");
    if (enablePin == OFF) { VLF("OFF"); } else
    if (enablePin == SHARED) { VLF("SHARED"); } else { VL(enablePin); }
  #endif

  // init default driver control pins
  if (enablePin != SHARED) {
    pinModeEx(enablePin, OUTPUT);
    digitalWriteEx(enablePin, !enabledState);
  }
}

// update status info. for driver
void ServoDriver::updateStatus() {
  #if DEBUG == VERBOSE
    if (status.standstill) { status.outputA.openLoad = false; status.outputB.openLoad = false; status.standstill = false; }
    if ((status.outputA.shortToGround     != lastStatus.outputA.shortToGround) ||
        (status.outputA.openLoad          != lastStatus.outputA.openLoad) ||
        (status.outputB.shortToGround     != lastStatus.outputB.shortToGround) ||
        (status.outputB.openLoad          != lastStatus.outputB.openLoad) ||
        (status.overTemperatureWarning    != lastStatus.overTemperatureWarning) ||
        (status.overTemperature           != lastStatus.overTemperature) ||
        (status.standstill                != lastStatus.standstill) ||
        (status.fault                     != lastStatus.fault)) {
      VF("MSG: ServoDriver"); V(axisNumber); VF(", status change ");
      VF("SGA"); if (status.outputA.shortToGround) VF("< "); else VF(". "); 
      VF("OLA"); if (status.outputA.openLoad) VF("< "); else VF(". "); 
      VF("SGB"); if (status.outputB.shortToGround) VF("< "); else VF(". "); 
      VF("OLB"); if (status.outputB.openLoad) VF("< "); else VF(". "); 
      VF("OTP"); if (status.overTemperatureWarning) VF("< "); else VF(". "); 
      VF("OTE"); if (status.overTemperature) VF("< "); else VF(". "); 
      VF("SST"); if (status.standstill) VF("< "); else VF(". "); 
      VF("FLT"); if (status.fault) VLF("<"); else VLF("."); 
    }
    lastStatus = status;
  #endif
};

#endif
