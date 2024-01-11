// -----------------------------------------------------------------------------------
// axis odrive servo motor

#include "ODrive.h"

#ifdef ODRIVE_MOTOR_PRESENT

#include "ODriveEnums.h"

#include "../../../tasks/OnTask.h"

ODriveMotor *odriveMotorInstance[2];
IRAM_ATTR void moveODriveMotorAxis1() { odriveMotorInstance[0]->move(); }
IRAM_ATTR void moveODriveMotorAxis2() { odriveMotorInstance[1]->move(); }

// ODrive servo motor driver object pointer
#if ODRIVE_COMM_MODE == OD_UART
  ODriveArduino *_oDriveDriver;
#elif ODRIVE_COMM_MODE == OD_CAN
  ODriveTeensyCAN *_oDriveDriver;
#endif

// constructor
ODriveMotor::ODriveMotor(uint8_t axisNumber, const ODriveDriverSettings *Settings, bool useFastHardwareTimers) {
  if (axisNumber < 1 || axisNumber > 2) return;

  driverType = ODRIVER;
  strcpy(axisPrefix, "MSG: ODrive_, ");
  axisPrefix[11] = '0' + this->axisNumber;
  #if ODRIVE_SWAP_AXES == ON
    this->axisNumber = 3 - axisNumber;
  #else
    this->axisNumber = axisNumber;
  #endif


  if (axisNumber > 2) useFastHardwareTimers = false;
  this->useFastHardwareTimers = useFastHardwareTimers;

  if (axisNumber == 1) { // only do once since motor 2 object creation could
    #if ODRIVE_COMM_MODE == OD_UART
      _oDriveDriver = new ODriveArduino(ODRIVE_SERIAL);
    #elif ODRIVE_COMM_MODE == OD_CAN
      _oDriveDriver = new ODriveTeensyCAN(250000);
    #endif
  }

  // attach the function pointers to the callbacks
  odriveMotorInstance[this->axisNumber - 1] = this;
  switch (this->axisNumber) {
    case 1: callback = moveODriveMotorAxis1; break;
    case 2: callback = moveODriveMotorAxis2; break;
  }
}

bool ODriveMotor::init() {
  if (axisNumber < 1 || axisNumber > 2) return false;

  if (axisNumber == 1) {
    pinModeEx(ODRIVE_RST_PIN, OUTPUT);
    digitalWriteEx(ODRIVE_RST_PIN, HIGH); // bring ODrive out of Reset
    delay(1000);  // allow time for ODrive to boot
    
    #if ODRIVE_COMM_MODE == OD_UART
      ODRIVE_SERIAL.begin(ODRIVE_SERIAL_BAUD);
      VLF("MSG: ODrive, SERIAL channel init");
    #elif ODRIVE_COMM_MODE == OD_CAN
      // .begin is done by the constructor
      VLF("MSG: ODrive, CAN channel init");
    #endif
  }

  enable(false);

  // start the motor timer
  V(axisPrefix);
  VF("start task to move motor... ");
  char timerName[] = "Target_";
  timerName[6] = '0' + axisNumber;
  taskHandle = tasks.add(0, 0, true, 0, callback, timerName);
  if (taskHandle) {
    V("success");
    if (useFastHardwareTimers && !tasks.requestHardwareTimer(taskHandle, 0)) { VLF(" (no hardware timer!)"); } else { VLF(""); }
  } else {
    VLF("FAILED!");
    return false;
  }

  return true;
}

// set driver reverse state
void ODriveMotor::setReverse(int8_t state) {
  if (state == ON) {
    VF("WRN: ODrive"); V(axisNumber); VF(", ");
    VLF("axis reversal must be accomplished with hardware or ODrive setup!");
  }
}

// set driver parameters
void ODriveMotor::setParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  UNUSED(param1); // general purpose settings defined in Extended.config.h and stored in NV, they can be modified at runtime
  UNUSED(param2);
  UNUSED(param3);
  UNUSED(param4);
  UNUSED(param5);
  stepsPerMeasure = param6;
  setSlewing(isSlewing);
}

// validate driver parameters
bool ODriveMotor::validateParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  UNUSED(param1);
  UNUSED(param2);
  UNUSED(param3);
  UNUSED(param4);
  UNUSED(param5);
  UNUSED(param6);
  return true;
}

// sets motor enable on/off (if possible)
void ODriveMotor::enable(bool state) {
  V(axisPrefix); VF("driver powered ");
  if (state) { VLF("up"); } else { VLF("down"); } 
  
  int requestedState = AXIS_STATE_IDLE;
  if (state) requestedState = AXIS_STATE_CLOSED_LOOP_CONTROL;
  
  #if ODRIVE_COMM_MODE == OD_UART 
  float timeout = 0.5;                        
    if(!_oDriveDriver->run_state(axisNumber - 1, requestedState, false, timeout)) {
      VF("WRN: ODrive"); V(axisNumber); VF(", ");
      VLF(" Power, closed loop control - command timeout!");
      return;
    }
  #elif ODRIVE_COMM_MODE == OD_CAN
    if(!_oDriveDriver->RunState(axisNumber - 1, requestedState)) { //currently, always returns true...need to add timeout
      VF("WRN: ODrive"); V(axisNumber); VF(", ");
      VLF(" Power, closed loop control - command timeout!");
      return;
    }
  #endif

  V(axisPrefix); VF("closed loop control - ");
  if (state) { VLF("Active"); } else { VLF("Idle"); }

  enabled = state;
}

void ODriveMotor::setInstrumentCoordinateSteps(long value) {
  #if ODRIVE_ABSOLUTE == ON && ODRIVE_SYNC_LIMIT != OFF
    noInterrupts();
    long index = value - motorSteps;
    interrupts();
    float indexDeg = index/stepsPerMeasure;
    if (indexDeg >= -degToRadF(ODRIVE_SYNC_LIMIT/3600.0F) && indexDeg <= degToRadF(ODRIVE_SYNC_LIMIT/3600.0F))
  #endif
  Motor::setInstrumentCoordinateSteps(value);
}

// get the associated driver status
DriverStatus ODriveMotor::getDriverStatus() {
  return status;
}

// resets motor and target angular position in steps, also zeros backlash and index
void ODriveMotor::resetPositionSteps(long value) {
  // this is where the initial odrive position in "steps" is brought into agreement with the motor position in steps
  // not sure on this... but code below ignores (value,) gets the odrive position convert to steps and resets the motor
  // there (as the odrive encoders are absolute.)

  long oPosition;
  // if (axisNumber - 1 == 0) oPosition = o_position0;
  // if (axisNumber - 1 == 1) oPosition = o_position1;

  // get ODrive position in fractionial Turns
  #if ODRIVE_COMM_MODE == OD_UART
    oPosition = _oDriveDriver->GetPosition(axisNumber - 1)*TWO_PI*stepsPerMeasure; // axis1/2 are in steps per radian
  #elif ODRIVE_COMM_MODE == OD_CAN
    oPosition = _oDriveDriver->GetPosition(axisNumber - 1)*TWO_PI*stepsPerMeasure; // axis1/2 are in steps per radian
  #endif

  noInterrupts();
  motorSteps    = oPosition;
  targetSteps   = motorSteps;
  #if ODRIVE_ABSOLUTE == OFF
    // but what if the odrive encoders are incremental?  how to tell the odrive what its angular position is?
    // here thinking we'll ignore it... sync OnStepX there and let the offset handle it
    indexSteps  = value - motorSteps;
  #else
    UNUSED(value);
    indexSteps = 0;
  #endif
  backlashSteps = 0;
  interrupts();
}

// set frequency (+/-) in steps per second negative frequencies move reverse in direction (0 stops motion)
void ODriveMotor::setFrequencySteps(float frequency) {
  // negative frequency, convert to positive and reverse the direction
  int dir = 0;
  if (frequency > 0.0F) dir = 1; else if (frequency < 0.0F) { frequency = -frequency; dir = -1; }

  // if in backlash override the frequency
  if (inBacklash)
    frequency = backlashFrequency;

  if (frequency != currentFrequency) {
    lastFrequency = frequency;

    // if slewing has a larger step size divide the frequency to account for it
    if (lastFrequency <= backlashFrequency * 2.0F) stepSize = 1; else { if (!inBacklash) stepSize = 64; }
    frequency /= stepSize;

    // timer period in microseconds
    float period = 1000000.0F / frequency;

    // range is 0 to 134 seconds/step
    if (!isnan(period) && period <= 130000000.0F) {
      period *= 16.0F;
      lastPeriod = (unsigned long)lroundf(period);
    } else {
      lastPeriod = 0;
      frequency = 0.0F;
      dir = 0;
    }

    currentFrequency = frequency;

    // change the motor rate/direction
    noInterrupts();
    step = 0;
    interrupts();
    tasks.setPeriodSubMicros(taskHandle, lastPeriod);
  }

  noInterrupts();
  step = dir * stepSize;
  absStep = abs(step);
  interrupts();
}

float ODriveMotor::getFrequencySteps() {
  if (lastPeriod == 0) return 0;
  return (16000000.0F / lastPeriod) * absStep;
}

// set slewing state (hint that we are about to slew or are done slewing)
void ODriveMotor::setSlewing(bool state) {
  isSlewing = state;
}

// updates PID and sets odrive position
void ODriveMotor::poll() {
  if ((long)(millis() - lastSetPositionTime) < ODRIVE_UPDATE_MS) return;
  lastSetPositionTime = millis();

  noInterrupts();
  #if ODRIVE_SLEW_DIRECT == ON
    long target = targetSteps + backlashSteps;
  #else
    long target = motorSteps + backlashSteps;
  #endif
  interrupts();
  #if ODRIVE_COMM_MODE == OD_UART
    setPosition(axisNumber -1, target/(TWO_PI*stepsPerMeasure));
  #elif ODRIVE_COMM_MODE == OD_CAN
    _oDriveDriver->SetPosition(axisNumber -1, target/(TWO_PI*stepsPerMeasure));
  #endif
}

// sets dir as required and moves coord toward target at setFrequencySteps() rate
IRAM_ATTR void ODriveMotor::move() {
  if (sync && !inBacklash) targetSteps += step;

  if (motorSteps > targetSteps) {
    if (backlashSteps > 0) {
      backlashSteps -= absStep;
      inBacklash = backlashSteps > 0;
    } else {
      motorSteps -= absStep;
      inBacklash = false;
    }
  } else

  if (motorSteps < targetSteps || inBacklash) {
    if (backlashSteps < backlashAmountSteps) {
      backlashSteps += absStep;
      inBacklash = backlashSteps < backlashAmountSteps;
    } else {
      motorSteps += absStep;
      inBacklash = false;
    }
  }
}

#endif
