// Pulse only encoders

#include "PulseOnly.h"

#if AXIS1_ENCODER == PULSE_ONLY || AXIS2_ENCODER == PULSE_ONLY || AXIS3_ENCODER == PULSE_ONLY || \
    AXIS4_ENCODER == PULSE_ONLY || AXIS5_ENCODER == PULSE_ONLY || AXIS6_ENCODER == PULSE_ONLY || \
    AXIS7_ENCODER == PULSE_ONLY || AXIS8_ENCODER == PULSE_ONLY || AXIS9_ENCODER == PULSE_ONLY

volatile int32_t _pulse_count[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
volatile int16_t *_direction[9];

#if AXIS1_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis1() { if (_direction) _pulse_count[0]--; else _pulse_count[0]++; }
#endif

#if AXIS2_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis2() { _pulse_count[1] += *_direction[1]; }
#endif

#if AXIS3_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis3() { _pulse_count[2] += *_direction[2]; }
#endif

#if AXIS4_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis4() { _pulse_count[3] += *_direction[3]; }
#endif

#if AXIS5_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis5() { _pulse_count[4] += *_direction[4]; }
#endif

#if AXIS6_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis6() { _pulse_count[5] += *_direction[5]; }
#endif

#if AXIS7_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis7() { _pulse_count[6] += *_direction[6]; }
#endif

#if AXIS8_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis8() { _pulse_count[7] += *_direction[7]; }
#endif

#if AXIS9_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis9() { _pulse_count[8] += *_direction[8]; }
#endif

PulseOnly::PulseOnly(int16_t pulsePin, int16_t *direction, int16_t axis) {
  if (axis < 1 || axis > 9) return;

  this->pulsePin = pulsePin;
  this->axis = axis - 1;
  _direction[this->axis] = direction;
}

void PulseOnly::init() {
  if (initialized) { VF("WRN: Encoder PulseOnly"); V(axis); VLF(" init(), already initialized!"); return; }

  pinMode(pulsePin, INPUT_PULLUP);
  #if AXIS1_ENCODER == PULSE_ONLY
    if (axis == 0) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulse_A_Axis1, CHANGE);
    }
  #endif
  #if AXIS2_ENCODER == PULSE_ONLY
    if (axis == 1) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulse_A_Axis2, CHANGE);
    }
  #endif
  #if AXIS3_ENCODER == PULSE_ONLY
    if (axis == 2) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulse_A_Axis3, CHANGE);
    }
  #endif
  #if AXIS4_ENCODER == PULSE_ONLY
    if (axis == 3) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulse_A_Axis4, CHANGE);
    }
  #endif
  #if AXIS5_ENCODER == PULSE_ONLY
    if (axis == 4) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulse_A_Axis5, CHANGE);
    }
  #endif
  #if AXIS6_ENCODER == PULSE_ONLY
    if (axis == 5) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulse_A_Axis6, CHANGE);
    }
  #endif
  #if AXIS7_ENCODER == PULSE_ONLY
    if (axis == 6) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulse_A_Axis7, CHANGE);
    }
  #endif
  #if AXIS8_ENCODER == PULSE_ONLY
    if (axis == 7) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulse_A_Axis8, CHANGE);
    }
  #endif
  #if AXIS9_ENCODER == PULSE_ONLY
    if (axis == 8) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulse_A_Axis9, CHANGE);
    }
  #endif

  initialized = true;
}

int32_t PulseOnly::read() {
  if (!initialized) { VF("WRN: Encoder PulseOnly"); V(axis); VLF(" read(), not initialized!"); return 0; }

  noInterrupts();
  int32 count = _pulse_count[axis];
  interrupts();

  return count + origin;
}

void PulseOnly::write(int32_t count) {
  if (!initialized) { VF("WRN: Encoder PulseOnly"); V(axis); VLF(" write(), not initialized!"); return; }

  count -= origin;

  noInterrupts();
  _pulse_count[axis] = count;
  interrupts();
}

void PulseOnly::setDirection(int16_t direction) {
  if (!initialized) { VF("WRN: Encoder PulseOnly"); V(axis); VLF(" setDirection(), not initialized!"); return; }

  _direction[axis] = direction;
}

#endif
