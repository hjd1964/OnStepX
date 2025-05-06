// -----------------------------------------------------------------------------------
// axis servo motor driver

#include "ServoDriver.h"

#ifdef SERVO_MOTOR_PRESENT

// the various servo driver models
#define SERVO_DRIVER_MODEL_COUNT (SERVO_DRIVER_LAST - SERVO_DRIVER_FIRST + 1)

#include "../../../gpioEx/GpioEx.h"

#include "../Motor.h"

#if DEBUG != OFF
  const char* SERVO_DRIVER_NAME[SERVO_DRIVER_MODEL_COUNT] =
  {
    "SERVO_PE",         // Phase (Direction)/Enable
    "SERVO_EE",         // Enable/Enable
    "SERVO_TMC2130_DC", // TMC2130 w/ DC motor
    "SERVO_TMC5160_DC", // TMC5160 w/ DC motor
    "SERVO_TMC2209",    // TMC2209 w/ stepper motor
    "SERVO_TMC5160",    // TMC5160 w/ stepper motor
    "SERVO_KTECH"       // KTech servo motor using velocity control
  };
#endif

bool ServoDriver::init() {
  #if DEBUG == VERBOSE
    VF("MSG:"); V(axisPrefix); VF("init model "); VL(SERVO_DRIVER_NAME[model - SERVO_DRIVER_FIRST]);
    VF("MSG:"); V(axisPrefix); VF("en=");
    if (enablePin == OFF) { VLF("OFF"); } else
    if (enablePin == SHARED) { VLF("SHARED"); } else { VL(enablePin); }
  #endif

  // get the maximum current and Rsense for this axis
  user_currentMax = 0;
  switch (axisNumber) {
    case 1:
      #ifdef AXIS1_DRIVER_RSENSE
        user_rSense = AXIS1_DRIVER_RSENSE;
      #endif
      #ifdef AXIS1_DRIVER_MAX_CURRENT
        user_currentMax = AXIS1_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 2:
      #ifdef AXIS2_DRIVER_RSENSE
        user_rSense = AXIS2_DRIVER_RSENSE;
      #endif
      #ifdef AXIS2_DRIVER_MAX_CURRENT
        user_currentMax = AXIS2_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 3:
      #ifdef AXIS3_DRIVER_RSENSE
        user_rSense = AXIS3_DRIVER_RSENSE;
      #endif
      #ifdef AXIS3_DRIVER_MAX_CURRENT
        user_currentMax = AXIS3_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 4:
      #ifdef AXIS4_DRIVER_RSENSE
        user_rSense = AXIS4_DRIVER_RSENSE;
      #endif
      #ifdef AXIS4_DRIVER_MAX_CURRENT
        user_currentMax = AXIS4_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 5:
      #ifdef AXIS5_DRIVER_RSENSE
        user_rSense = AXIS5_DRIVER_RSENSE;
      #endif
      #ifdef AXIS5_DRIVER_MAX_CURRENT
        user_currentMax = AXIS5_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 6:
      #ifdef AXIS6_DRIVER_RSENSE
        user_rSense = AXIS6_DRIVER_RSENSE;
      #endif
      #ifdef AXIS6_DRIVER_MAX_CURRENT
        user_currentMax = AXIS6_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 7:
      #ifdef AXIS7_DRIVER_RSENSE
        user_rSense = AXIS7_DRIVER_RSENSE;
      #endif
      #ifdef AXIS7_DRIVER_MAX_CURRENT
        user_currentMax = AXIS7_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 8:
      #ifdef AXIS8_DRIVER_RSENSE
        user_rSense = AXIS8_DRIVER_RSENSE;
      #endif
      #ifdef AXIS8_DRIVER_MAX_CURRENT
        user_currentMax = AXIS8_DRIVER_MAX_CURRENT;
      #endif
    break;
    case 9:
      #ifdef AXIS9_DRIVER_RSENSE
        user_rSense = AXIS9_DRIVER_RSENSE;
      #endif
      #ifdef AXIS9_DRIVER_MAX_CURRENT
        user_currentMax = AXIS9_DRIVER_MAX_CURRENT;
      #endif
    break;
    default:
      user_rSense = 0;
      user_currentMax = 0;
    break;
  }

  // init default driver control pins
  if (enablePin != SHARED) {
    pinModeEx(enablePin, OUTPUT);
    digitalWriteEx(enablePin, !enabledState);
  }

  return true;
}

// update status info. for driver
void ServoDriver::updateStatus() {
  if (statusMode == ON) {
    if ((long)(millis() - timeLastStatusUpdate) > 200) {
      readStatus();

      // open load indication is not reliable in standstill
      if (status.outputA.shortToGround ||
          status.outputB.shortToGround ||
          status.overTemperatureWarning ||
          status.overTemperature) status.fault = true; else status.fault = false;

      timeLastStatusUpdate = millis();
    }
  }

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
      VF("MSG:"); V(axisPrefix); VF("status change ");
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
