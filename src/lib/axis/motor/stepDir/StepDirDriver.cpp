// -----------------------------------------------------------------------------------
// axis step/dir motor driver

#include "StepDirDriver.h"

#ifdef STEP_DIR_MOTOR_PRESENT

// the various microsteps for different driver models, with the bit modes for each
#define DRIVER_MODEL_COUNT 17 

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
  {OFF,OFF,OFF,  0,  3,  1,  2,OFF,OFF},   // TMC2209S/TMC2226S

  {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC2130
  {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC5160
  {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC5161

  {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC2208/TMC2225
  {  8,  7,  6,  5,  4,  3,  2,  1,  0},   // TMC2209/TMC2226
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
  103,   // TMC2209S/TMC2226S

  103,   // TMC2130
  103,   // TMC5160
  103,   // TMC5161

  103,   // TMC2208/TMC2225
  103,   // TMC2209/TMC2226
};

#if DEBUG != OFF
  const char* DRIVER_NAME[DRIVER_MODEL_COUNT] =
  { 
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
    "TMC2209/TMC2226",

    "TMC2130 (SPI)",
    "TMC5160 (SPI)",
    "TMC5161 (SPI)",

    "TMC2208/TMC2225 (UART)",
    "TMC2209/TMC2226 (UART)",
  };
#endif

// set up driver and parameters: microsteps, microsteps goto, hold current, run current, goto current, unused
void StepDirDriver::init(float param1, float param2, float param3, float param4, float param5, float param6) {
  settings.microsteps = round(param1);
  settings.microstepsSlewing = round(param2);
  settings.currentHold = round(param3);
  settings.currentRun  = round(param4);
  settings.currentGoto = round(param5);
  UNUSED(param6);

  if (settings.intpol == ON) settings.intpol = true; else settings.intpol = false;
  if (settings.decay == OFF) settings.decay = STEALTHCHOP;
  if (settings.decaySlewing == OFF) settings.decaySlewing = SPREADCYCLE;

  VF("MSG: StepDirDriver"); V(axisNumber); VF(", init model "); V(DRIVER_NAME[settings.model]);
  VF(" u-step mode "); if (settings.microsteps == OFF) { VF("OFF (assuming 1X)"); settings.microsteps = 1; } else { V(settings.microsteps); VF("X"); }
  VF(" (goto mode "); if (settings.microstepsSlewing == OFF) { VLF("OFF)"); } else { V(settings.microstepsSlewing); VL("X)"); }

  if (settings.microstepsSlewing == OFF) settings.microstepsSlewing = settings.microsteps;
  microstepCode = subdivisionsToCode(settings.microsteps);
  microstepCodeSlewing = subdivisionsToCode(settings.microstepsSlewing);
  microstepRatio = settings.microsteps/settings.microstepsSlewing;
}

// validate driver parameters
bool StepDirDriver::validateParameters(float param1, float param2, float param3, float param4, float param5, float param6) {
  int index = axisNumber - 1;
  if (index > 3) index = 3;

  #if STEP_WAVE_FORM == PULSE
    // check if platform pulse width (ns) is ok for this stepper driver timing in PULSE mode
    long pulseWidth = HAL_PULSE_WIDTH;
    if (axisNumber > 2) pulseWidth = 2000;

    if (DriverPulseWidth[settings.model] == OFF) {
      VF("WRN: StepDirDriver::validateParameters(), Axis"); V(axisNumber); VF(" ");
      V(DRIVER_NAME[settings.model]); VF(" min. pulse width unknown!");
    }

    if (DriverPulseWidth[settings.model] > pulseWidth) {
      DF("ERR: StepDirDriver::validateParameters(), Axis"); D(axisNumber); DF(" "); 
      D(DRIVER_NAME[settings.model]); DF(" min. pulse width "); D(DriverPulseWidth[settings.model]); DF("ns > platform at ");
      D(pulseWidth); DLF("ns");
      return false;
    }
  #endif

  long subdivisions = round(param1);
  long subdivisionsGoto = round(param2);
  UNUSED(param3);
  UNUSED(param4);
  UNUSED(param5);
  UNUSED(param6);

  if (subdivisions == OFF) {
    VF("WRN: StepDirDriver::validateParameters(), Axis"); V(axisNumber); VLF(" subdivisions OFF (assuming 1X)");
    subdivisions = 1;
  }

  if (subdivisions <= subdivisionsGoto) {
    DF("ERR: StepDirDriver::validateParameters(), Axis"); D(axisNumber); DLF(" subdivisions must be > subdivisionsGoto");
    return false;
  }

  if (subdivisions != OFF && (subdivisionsToCode(subdivisions) == OFF)) {
    DF("ERR: StepDirDriver::validateParameters(), Axis"); D(axisNumber); DF(" bad subdivisions="); DL(subdivisions);
    return false;
  }

  if (subdivisionsGoto != OFF && (subdivisionsToCode(subdivisionsGoto) == OFF)) {
    DF("ERR: StepDirDriver::validateParameters(), Axis"); D(axisNumber); DF(" bad subdivisionsGoto="); DL(subdivisionsGoto);
    return false;
  }

  return true;
}

// get the pulse width in nanoseconds, if unknown (-1) returns 2000 nanoseconds
long StepDirDriver::getPulseWidth() {
  long ns = DriverPulseWidth[settings.model];
  if (ns < 0) ns = 2000;
  return ns;
}

// different models of stepper drivers have different bit settings for microsteps
// translate the human readable microsteps in the configuration to mode bit settings
// returns bit code (0 to 7) or OFF if microsteps is not supported or unknown
int StepDirDriver::subdivisionsToCode(long microsteps) {
  int allowed[9] = {1,2,4,8,16,32,64,128,256};
  if (settings.model >= DRIVER_MODEL_COUNT) return OFF;
  for (int i = 0; i < 9; i++) {
    if (microsteps == allowed[i]) return steps[settings.model][i];
  }
  return OFF;
}

// update status info. for driver
void StepDirDriver::updateStatus() {
  #if DEBUG == VERBOSE
    if ((status.outputA.shortToGround     != lastStatus.outputA.shortToGround) ||
//      (status.outputA.openLoad          != lastStatus.outputA.openLoad) ||
        (status.outputB.shortToGround     != lastStatus.outputB.shortToGround) ||
//      (status.outputB.openLoad          != lastStatus.outputB.openLoad) ||
        (status.overTemperatureWarning    != lastStatus.overTemperatureWarning) ||
        (status.overTemperature           != lastStatus.overTemperature) ||
//      (status.standstill                != lastStatus.standstill) ||
        (status.fault                     != lastStatus.fault)) {
      VF("MSG: StepDirDriver"); V(axisNumber); VF(", status change ");
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
