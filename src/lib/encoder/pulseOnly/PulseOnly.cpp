// Pulse only encoders

#include "PulseOnly.h"

#if AXIS1_ENCODER == PULSE_ONLY || AXIS2_ENCODER == PULSE_ONLY || AXIS3_ENCODER == PULSE_ONLY || \
    AXIS4_ENCODER == PULSE_ONLY || AXIS5_ENCODER == PULSE_ONLY || AXIS6_ENCODER == PULSE_ONLY || \
    AXIS7_ENCODER == PULSE_ONLY || AXIS8_ENCODER == PULSE_ONLY || AXIS9_ENCODER == PULSE_ONLY

volatile int32_t _pulse_count[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
volatile int8_t *_pulse_direction[9];

#if AXIS1_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis1() { _pulse_count[0] += *_pulse_direction[0]; }
#endif

#if AXIS2_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis2() { _pulse_count[1] += *_pulse_direction[1]; }
#endif

#if AXIS3_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis3() { _pulse_count[2] += *_pulse_direction[2]; }
#endif

#if AXIS4_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis4() { _pulse_count[3] += *_pulse_direction[3]; }
#endif

#if AXIS5_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis5() { _pulse_count[4] += *_pulse_direction[4]; }
#endif

#if AXIS6_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis6() { _pulse_count[5] += *_pulse_direction[5]; }
#endif

#if AXIS7_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis7() { _pulse_count[6] += *_pulse_direction[6]; }
#endif

#if AXIS8_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis8() { _pulse_count[7] += *_pulse_direction[7]; }
#endif

#if AXIS9_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis9() { _pulse_count[8] += *_pulse_direction[8]; }
#endif

PulseOnly::PulseOnly(int16_t pulsePin, volatile int8_t *direction, int16_t axis) {
  if (axis < 1 || axis > 9) return;

  this->axis = axis;
  axis_index = axis - 1;

  PulsePin = pulsePin;
  _pulse_direction[axis_index] = direction;
}

bool PulseOnly::init() {
  if (ready) return true;

  pinMode(pulsePin, INPUT_PULLUP);

  int pulsePin = digitalPinToInterrupt(PulsePin);

  if (pulsePin < 0) {
    DF("ERR: Encoder PulseOnly"); D(axis); DLF(" init(), couldn't attach interrupt!"); 
    return false;
  }

  switch (axis) {
    #if AXIS1_ENCODER == PULSE_ONLY
      case 1:
        attachInterrupt(pulsePin, pulse_A_Axis1, CHANGE);
      break;
    #endif
    #if AXIS2_ENCODER == PULSE_ONLY
      case 2:
        attachInterrupt(pulsePin, pulse_A_Axis2, CHANGE);
      break;
    #endif
    #if AXIS3_ENCODER == PULSE_ONLY
      case 3:
        attachInterrupt(pulsePin, pulse_A_Axis3, CHANGE);
      break;
    #endif
    #if AXIS4_ENCODER == PULSE_ONLY
      case 4:
        attachInterrupt(pulsePin, pulse_A_Axis4, CHANGE);
      break;
    #endif
    #if AXIS5_ENCODER == PULSE_ONLY
      case 5:
        attachInterrupt(pulsePin, pulse_A_Axis5, CHANGE);
      break;
    #endif
    #if AXIS6_ENCODER == PULSE_ONLY
      case 6:
        attachInterrupt(pulsePin, pulse_A_Axis6, CHANGE);
      break;
    #endif
    #if AXIS7_ENCODER == PULSE_ONLY
      case 7:
        attachInterrupt(pulsePin, pulse_A_Axis7, CHANGE);
      break;
    #endif
    #if AXIS8_ENCODER == PULSE_ONLY
      case 8:
        attachInterrupt(pulsePin, pulse_A_Axis8, CHANGE);
      break;
    #endif
    #if AXIS9_ENCODER == PULSE_ONLY
      case 9:
        attachInterrupt(pulsePin, pulse_A_Axis9, CHANGE);
      break;
    #endif
  }

  ready = true;
}

int32_t PulseOnly::read() {
  if (!ready) return 0;

  noInterrupts();
  count = _pulse_count[axis_index];
  interrupts();

  return count + index;
}

void PulseOnly::write(int32_t position) {
  if (!ready) return;

  noInterrupts();
  index = position - _pulse_count[axis_index];
  interrupts();
}

void PulseOnly::setDirection(volatile int8_t *direction) {
  if (!ready) return;

  _pulse_direction[axis_index] = direction;
}

#endif
