// -----------------------------------------------------------------------------------
// axis step/dir motor driver

#include "Generic.h"

#ifdef STEP_DIR_LEGACY_PRESENT

#include "../../../../gpioEx/GpioEx.h"

// constructor
StepDirGeneric::StepDirGeneric(uint8_t axisNumber, const StepDirDriverPins *Pins, const StepDirDriverSettings *Settings) : StepDirDriver(axisNumber, Pins, Settings) {
  strcpy(axisPrefix, " Axis_StepDirGeneric, ");
  axisPrefix[5] = '0' + axisNumber;
}

// setup driver
bool StepDirGeneric::init() {
  if (!StepDirDriver::init()) return false;

  m0Pin = Pins->m0;
  m1Pin = Pins->m1;
  if (isDecayOnM2()) { decayPin = Pins->m2; m2Pin = OFF; } else { decayPin = Pins->decay; m2Pin = Pins->m2; }
  pinModeEx(decayPin, OUTPUT);
  digitalWriteEx(decayPin, getDecayPinState(normalizedDecay));

  #if DEBUG == VERBOSE
    VF("MSG: StepDirDriver"); V(axisNumber);
    V(", pins m0="); if (Pins->m0 == OFF) V("OFF"); else V(Pins->m0);
    V(", m1="); if (Pins->m1 == OFF) VF("OFF"); else V(Pins->m1);
    V(", m2="); if (m2Pin == OFF) VF("OFF"); else V(m2Pin);
    V(", decay="); if (decayPin == OFF) VF("OFF"); else V(decayPin);
    if (statusMode == ON) {
      V(", fault="); if (Pins->fault == OFF) VF("OFF"); else V(Pins->fault);
    }
    VL("");
  #endif

  // toggle the M2 pin states if needed, this allows extra flexibility for stand alone stepper drivers
  if (Pins->m2State == LOW) {
    if (bitRead(microstepCode, 2)) bitClear(microstepCode, 2); else bitSet(microstepCode, 2);
    if (bitRead(microstepCodeSlewing, 2)) bitClear(microstepCodeSlewing, 2); else bitSet(microstepCodeSlewing, 2);
  }

  microstepBitCode = microstepCode;
  microstepBitCodeM0 = bitRead(microstepBitCode, 0);
  microstepBitCodeM1 = bitRead(microstepBitCode, 1);
  microstepBitCodeM2 = bitRead(microstepBitCode, 2);

  microstepBitCodeGoto = microstepCodeSlewing;
  microstepBitCodeGotoM0 = bitRead(microstepBitCodeGoto, 0);
  microstepBitCodeGotoM1 = bitRead(microstepBitCodeGoto, 1);
  microstepBitCodeGotoM2 = bitRead(microstepBitCodeGoto, 2);

  pinModeEx(m0Pin, OUTPUT);
  digitalWriteEx(m0Pin, microstepBitCodeM0);
  pinModeEx(m1Pin, OUTPUT);
  digitalWriteEx(m1Pin, microstepBitCodeM1);
  pinModeEx(m2Pin, OUTPUT);
  digitalWriteEx(m2Pin, microstepBitCodeM2);

  // set mode switching support flags
  // use high speed mode
  modeSwitchAllowed = false;
  modeSwitchFastAllowed = microstepRatio != 1;

  return true;
}

IRAM_ATTR void StepDirGeneric::modeMicrostepTracking() {
  if (m0Pin != OFF) digitalWriteF(m0Pin, microstepBitCodeM0);
  if (m1Pin != OFF) digitalWriteF(m1Pin, microstepBitCodeM1);
  digitalWriteEx(m2Pin, microstepBitCodeM2);
}

IRAM_ATTR int StepDirGeneric::modeMicrostepSlewing() {
  if (microstepRatio > 1) {
    if (m0Pin != OFF) digitalWriteF(m0Pin, microstepBitCodeGotoM0);
    if (m1Pin != OFF) digitalWriteF(m1Pin, microstepBitCodeGotoM1);
    digitalWriteEx(m2Pin, microstepBitCodeGotoM2);
  }
  return microstepRatio;
}

void StepDirGeneric::modeDecayTracking() {
  if (normalizedDecay == OFF) return;
  int8_t state = getDecayPinState(normalizedDecay);
  if (state != OFF) digitalWriteEx(decayPin, state);
}

void StepDirGeneric::modeDecaySlewing() {
  if (normalizedDecaySlewing == OFF) return;
  int8_t state = getDecayPinState(normalizedDecaySlewing);
  if (state != OFF) digitalWriteEx(decayPin, state);
}

int8_t StepDirGeneric::getDecayPinState(int8_t decay) {
  uint8_t state = OFF;
  if (decay == SPREADCYCLE) state = HIGH; else
  if (decay == STEALTHCHOP) state = LOW;  else
  if (decay == MIXED)       state = LOW;  else
  if (decay == FAST)        state = HIGH;
  return state;
}

bool StepDirGeneric::isDecayOnM2() {
  if (driverModel == TMC2209S || driverModel == TMC2225S) return true; else return false;
}

#endif
