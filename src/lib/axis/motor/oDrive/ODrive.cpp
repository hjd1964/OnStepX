// -----------------------------------------------------------------------------------
// axis servo motor

#include "ODrive.h"

#ifdef ODRIVE_MOTOR_PRESENT

#include "../../../tasks/OnTask.h"

ODriveMotor *odriveMotorInstance[2];
IRAM_ATTR void moveServoMotorAxis1() { odriveMotorInstance[0]->move(); }
IRAM_ATTR void moveServoMotorAxis2() { odriveMotorInstance[1]->move(); }

// ODrive servo motor driver
ODriveArduino *_oDriveDriver;

// constructor
ODriveMotor::ODriveMotor(uint8_t axisNumber, const ODriveDriverSettings *Settings, bool useFastHardwareTimers) {
  if (axisNumber < 1 || axisNumber > 2) return;

  strcpy(axisPrefix, "MSG: ODrive_, ");
  axisPrefix[11] = '0' + axisNumber;
  this->axisNumber = axisNumber;
  this->useFastHardwareTimers = useFastHardwareTimers;
  driverType = ODRIVER;

  _oDriveDriver = new ODriveArduino(ODRIVE_SERIAL);

  // attach the function pointers to the callbacks
  odriveMotorInstance[axisNumber - 1] = this;
  switch (axisNumber) {
    case 1: callback = moveServoMotorAxis1; break;
    case 2: callback = moveServoMotorAxis2; break;
  }
}

bool ODriveMotor::init() {
  if (axisNumber < 1 || axisNumber > 2) return false;

  if (axisNumber == 1) {
    pinModeEx(ODRIVE_RST_PIN, OUTPUT);
    digitalWriteEx(ODRIVE_RST_PIN, HIGH); // bring ODrive out of Reset
    delay(1000);                          // allow time for ODrive to boot
    ODRIVE_SERIAL.begin(19200);
    VLF("MSG: ODrive channel Init");
  }

  power(false);

  // start the motor timer
  V(axisPrefix);
  VF("start task to move motor... ");
  char timerName[] = "Target_";
  timerName[6] = '0' + axisNumber;
  taskHandle = tasks.add(0, 0, true, 0, callback, timerName);
  if (taskHandle) {
    V("success");
    if (useFastHardwareTimers) {
      if (!tasks.requestHardwareTimer(taskHandle, axisNumber, 0)) {
        VF(" (no hardware timer!)");
      }
    }
    VL("");
  } else {
    VLF("FAILED!");
    return false;
  }

  return true;
}

// set driver reverse state
void ODriveMotor::setReverse(int8_t state) {
  UNUSED(state);  // not sure how one would do this, not required strictly speaking though
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

// sets motor power on/off (if possible)
void ODriveMotor::power(bool state) {
  int requestedState = AXIS_STATE_IDLE;
  int timeout = 0.01;
  if (state) {
    requestedState = AXIS_STATE_CLOSED_LOOP_CONTROL;
    timeout = 0.5;
  }
  if(!_oDriveDriver->run_state(axisNumber - 1, requestedState, false, timeout)) {
    VF("WRN: ODrive"); V(axisNumber); VF(", ");
    VLF("closed loop - command timeout!");
    return;
  }
  V(axisPrefix); VLF("closed loop control - ");
  if (state) { VLF("Active"); } else { VLF("Idle"); }
}

// get the associated stepper drivers status
DriverStatus ODriveMotor::getDriverStatus() {
  return status;
}

// resets motor and target angular position in steps, also zeros backlash and index
void ODriveMotor::resetPositionSteps(long value) {
  // this is where the initial odrive position in "steps" is brought into agreement with the motor position in steps
  // not sure on this... but code below ignores (value,) gets the odrive position convert to steps and resets the motor
  // there (as the odrive encoders are absolute.)

  long oPosition = _oDriveDriver->GetPosition(axisNumber - 1) * TWO_PI * stepsPerMeasure; // axis1/2 are in steps per radian
  Motor::resetPositionSteps(oPosition);

  // but what if the odrive encoders are incremental?  how to tell the odrive what its angular position is?
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
  if (lastPeriod == 0)
    return 0;
  return (16000000.0F / lastPeriod) * absStep;
}

// set slewing state (hint that we are about to slew or are done slewing)
void ODriveMotor::setSlewing(bool state) {
  isSlewing = state;
}

// updates PID and sets odrive position
void ODriveMotor::poll() {
  noInterrupts();
  long target = motorSteps + backlashSteps;
  interrupts();

  _oDriveDriver->SetPosition(axisNumber - 1, target/(TWO_PI * stepsPerMeasure));
}

// sets dir as required and moves coord toward target at setFrequencySteps() rate
IRAM_ATTR void ODriveMotor::move() {
  if (synchronized && !inBacklash) targetSteps += step;

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
