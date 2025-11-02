// -----------------------------------------------------------------------------------
// axis step/dir motor driver

#include "StepDirDriver.h"

#ifdef STEP_DIR_MOTOR_PRESENT

#include "../../../gpioEx/GpioEx.h"

// the various microsteps for different driver models, with the bit modes for each
#define DRIVER_MODEL_COUNT 22

const static int8_t steps[DRIVER_MODEL_COUNT][9] =
//  1   2   4   8  16  32  64 128 256x
{
  {  0,  1,  2,  3,  7,OFF,OFF,OFF,OFF},   // A4988
  {  0,  1,  2,  3,  4,  5,OFF,OFF,OFF},   // DRV8825
  {  0,  4,  4,  4,  4,  4,  4,  4,  4},   // GENERIC
  {  0,  1,  2,  3,  4,  5,  6,  7,OFF},   // LV8729
  {  4,  2,  6,  5,  3,  7,OFF,OFF,OFF},   // S109
  {  0,  1,  2,  3,  4,  5,OFF,  6,  7},   // ST820
  {  0,  1,  2,  3,  4,  5,  6,  7,OFF},   // RAPS128
  {  0,  1,  2,OFF,  3,OFF,OFF,OFF,OFF},   // TMC2100
  {  0,  1,  2,OFF,  3,OFF,OFF,OFF,OFF},   // TMC2130S
  {OFF,  1,  2,  0,  3,OFF,OFF,OFF,OFF},   // TMC2208S
  {OFF,OFF,  0,  1,  2,  3,OFF,OFF,OFF},   // TMC2225S
  {OFF,OFF,OFF,  0,  3,  1,  2,OFF,OFF},   // TMC2209S
  {OFF,OFF,OFF,  0,  3,  1,  2,OFF,OFF},   // TMC2226S

  {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC2130
  {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC2160
  {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC2660
  {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC5160
  {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC5161

  {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC2208
  {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC2225
  {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC2209
  {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC2226
};

const static int16_t DriverPulseWidth[DRIVER_MODEL_COUNT] =
// minimum step pulse width, in ns
{ 
  1000,  // A4988
  2000,  // DRV8825
  OFF,   // GENERIC
  500,   // LV8729
  7000,  // RAPS128
  300,   // S109
  20,    // ST820
  103,   // TMC2100
  103,   // TMC2130S
  103,   // TMC2208S
  103,   // TMC2225S
  103,   // TMC2209S
  103,   // TMC2226S

  103,   // TMC2130
  103,   // TMC2160
  103,   // TMC2660
  103,   // TMC5160
  103,   // TMC5161

  103,   // TMC2208
  103,   // TMC2225
  103,   // TMC2209
  103,   // TMC2226
};

const char* DRIVER_NAME[DRIVER_MODEL_COUNT] = { 
  "A4988",
  "DRV8825",
  "GENERIC",
  "LV8729",
  "RAPS128",
  "S109",
  "ST820",
  "TMC2100",
  "TMC2130",
  "TMC2208",
  "TMC2225",
  "TMC2209",
  "TMC2226",

  "TMC2130 (SPI)",
  "TMC2160 (SPI)",
  "TMC2660 (SPI)",
  "TMC5160 (SPI)",
  "TMC5161 (SPI)",

  "TMC2208 (UART)",
  "TMC2225 (UART)",
  "TMC2209 (UART)",
  "TMC2226 (UART)",
};

StepDirDriver::StepDirDriver(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings) {
  this->axisNumber = axisNumber;

  this->Pins = Pins;

  driverModel = Settings->model;
  statusMode = Settings->status;

  microsteps.valueDefault = Settings->microsteps;
  microstepsSlewing.valueDefault = Settings->microstepsSlewing;

  if (microsteps.valueDefault == OFF) microsteps.valueDefault = 1.0F;
  if (microstepsSlewing.valueDefault == OFF) microstepsSlewing.valueDefault = microsteps.valueDefault;

  decay.valueDefault = Settings->decay;
  decaySlewing.valueDefault = Settings->decaySlewing;
}

bool StepDirDriver::init() {
  VF("MSG:"); V(axisPrefix); VL(name());

  // check if platform pulse width (ns) is ok for this stepper driver timing in PULSE mode
  #if STEP_WAVE_FORM == PULSE
    long pulseWidth = HAL_PULSE_WIDTH;
    if (axisNumber > 2) pulseWidth = 2000;

    if (DriverPulseWidth[driverModel] == OFF) {
      VF("MSG:"); V(axisPrefix); V(DRIVER_NAME[driverModel]); VF(" min. pulse width unknown!");
    }

    if (DriverPulseWidth[driverModel] > pulseWidth) {
      DF("WRN:"); D(axisPrefix); D(DRIVER_NAME[driverModel]);
      DF(" min. pulse width "); D(DriverPulseWidth[driverModel]); DF("ns > platform at "); D(pulseWidth); DLF("ns");
      return false;
    }
  #endif

  normalizedMicrosteps = lround(microsteps.value);
  normalizedMicrostepsSlewing = lround(microstepsSlewing.value);
  VF("MSG:"); V(axisPrefix); VF("microstep mode "); V(microsteps.value); VF("X");
  VF(" (goto mode "); V(microstepsSlewing.value); VL("X)");

  // set parmeter min/max to cover range of available microsteps for this driver
  int microstep[9] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  int lowestMode = 256;
  int highestMode = 1;
  for (int i = 0; i < 9; i++) {
    if (subdivisionsToCode(microstep[i]) != OFF) {
      if (microstep[i] < lowestMode) lowestMode = microstep[i];
      if (microstep[i] > highestMode) highestMode = microstep[i];
    }
  }
  microsteps.min = lowestMode;
  microstepsSlewing.min = lowestMode;
  microsteps.max = highestMode;
  microstepsSlewing.max = highestMode;

  microstepCode = subdivisionsToCode(normalizedMicrosteps);
  microstepCodeSlewing = subdivisionsToCode(normalizedMicrostepsSlewing);

  if (microstepCode == OFF || microstepCodeSlewing == OFF) {
    DF("MSG:"); D(axisPrefix); DF("invalid microstep mode for this driver.");
    return false;
  }

  microstepRatio = normalizedMicrosteps/normalizedMicrostepsSlewing;

  normalizedDecay = lround(decay.value);
  normalizedDecaySlewing = lround(decaySlewing.value);

  // automatically set fault status for known drivers
  if (statusMode == ON) {
    switch (driverModel) {
      case DRV8825: statusMode = LOW; break;
      case ST820:   statusMode = LOW; break;
      default: break;
    }
  }

  // disable certain status modes if the fault pin isn't defined
  if ((statusMode == LOW || statusMode == HIGH) && Pins->fault == OFF) {
    statusMode = OFF;
  }

  // flag status as active or not
  status.active = statusMode != OFF;

  // set fault pin mode
  if (statusMode == LOW) pinModeEx(Pins->fault, INPUT_PULLUP);
  #ifdef PULLDOWN
    if (statusMode == HIGH) pinModeEx(Pins->fault, INPUT_PULLDOWN);
  #else
    if (statusMode == HIGH) pinModeEx(Pins->fault, INPUT);
  #endif

  // check to see if the driver is ok if we're using a fault pin
  if (statusMode == LOW || statusMode == HIGH) {
    status.fault = digitalReadEx(Pins->fault) == statusMode;
    if (status.fault) {
      DF("ERR:"); D(axisPrefix); DLF("motor driver device fault!");
      return false;
    } else {
      VF("MSG:"); V(axisPrefix); VLF("motor driver device ok");
    }
  }

  return true;
}

bool StepDirDriver::parameterIsValid(AxisParameter* parameter, bool next) {
  if (parameter == &microsteps || parameter == &microstepsSlewing) {

    int microstepsValue, microstepsSlewingValue;
    if (next) {
      microstepsValue = lround(microsteps.valueNv);
      microstepsSlewingValue = lround(microstepsSlewing.valueNv);
    } else {
      microstepsValue = lround(microsteps.value);
      microstepsSlewingValue = lround(microstepsSlewing.value);
    }

    if (subdivisionsToCode(microstepsValue) == OFF) {
      DF("WRN:"); D(axisPrefix); DLF("unsupported microsteps value");
      return false;
    }
    if (subdivisionsToCode(microstepsSlewingValue) == OFF) {
      DF("WRN:"); D(axisPrefix); DLF("unsupported microsteps slewing value");
      return false;
    }
    if (microstepsValue < microstepsSlewingValue) {
      DF("WRN:"); D(axisPrefix);
      DF("bad microsteps "); D(microstepsValue);
      DF(" must be >= microstepsGoto "); DL(microstepsSlewingValue);
      return false;
    }
  }
  return true;
}

// get the pulse width in nanoseconds, if unknown (-1) returns 2000 nanoseconds
long StepDirDriver::getPulseWidth() {
  long ns = DriverPulseWidth[driverModel];
  if (ns < 0) ns = 2000;
  return ns;
}

// different models of stepper drivers have different bit settings for microsteps
// translate the human readable microsteps in the configuration to mode bit settings
// returns bit code (0 to 7) or OFF if microsteps is not supported or unknown
int StepDirDriver::subdivisionsToCode(long microsteps) {
  int allowed[9] = {1,2,4,8,16,32,64,128,256};
  if (driverModel >= DRIVER_MODEL_COUNT) return OFF;
  for (int i = 0; i < 9; i++) {
    if (microsteps == allowed[i]) return steps[driverModel][i];
  }
  return OFF;
}

// get the driver name
const char* StepDirDriver::name() {
  return DRIVER_NAME[driverModel];
}

// update status info. for driver
void StepDirDriver::updateStatus() {
  if (statusMode == ON) {
    if ((long)(millis() - timeLastStatusUpdate) > 200) {
      readStatus();

      // open load indication is not reliable in standstill
      status.fault = status.outputA.shortToGround ||
                     status.outputB.shortToGround ||
                     status.overTemperatureWarning ||
                     status.overTemperature;

      timeLastStatusUpdate = millis();
    }
  } else
  if (statusMode == LOW || statusMode == HIGH) {
    status.fault = digitalReadEx(Pins->fault) == statusMode;
  }

  #if DEBUG == VERBOSE
    if ((status.outputA.shortToGround     != lastStatus.outputA.shortToGround) ||
//      (status.outputA.openLoad          != lastStatus.outputA.openLoad) ||
        (status.outputB.shortToGround     != lastStatus.outputB.shortToGround) ||
//      (status.outputB.openLoad          != lastStatus.outputB.openLoad) ||
        (status.overTemperatureWarning    != lastStatus.overTemperatureWarning) ||
        (status.overTemperature           != lastStatus.overTemperature) ||
//      (status.standstill                != lastStatus.standstill) ||
        (status.fault                     != lastStatus.fault)) {
      VF("MSG:"); V(axisPrefix);
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
