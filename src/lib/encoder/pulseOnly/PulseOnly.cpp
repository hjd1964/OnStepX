// Pulse only encoders

#include "PulseOnly.h"

#if AXIS1_ENCODER == PULSE_ONLY || AXIS2_ENCODER == PULSE_ONLY || AXIS3_ENCODER == PULSE_ONLY || \
    AXIS4_ENCODER == PULSE_ONLY || AXIS5_ENCODER == PULSE_ONLY || AXIS6_ENCODER == PULSE_ONLY || \
    AXIS7_ENCODER == PULSE_ONLY || AXIS8_ENCODER == PULSE_ONLY || AXIS9_ENCODER == PULSE_ONLY

PulseOnly *pulseOnlyInstance[9];

#if AXIS1_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis1() { pulseOnlyInstance[0]->pulse();  }
#endif

#if AXIS2_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis2() { pulseOnlyInstance[1]->pulse();  }
#endif

#if AXIS3_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis3() { pulseOnlyInstance[2]->pulse();  }
#endif

#if AXIS4_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis4() { pulseOnlyInstance[3]->pulse();  }
#endif

#if AXIS5_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis5() { pulseOnlyInstance[4]->pulse(); }
#endif

#if AXIS6_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis6() { pulseOnlyInstance[5]->pulse(); }
#endif

#if AXIS7_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis7() { pulseOnlyInstance[6]->pulse(); }
#endif

#if AXIS8_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis8() { pulseOnlyInstance[7]->pulse(); }
#endif

#if AXIS9_ENCODER == PULSE_ONLY
  IRAM_ATTR void pulse_A_Axis9() { pulseOnlyInstance[8]->pulse(); }
#endif

PulseOnly::PulseOnly(int16_t pulsePin, volatile int8_t *direction, int16_t axis) {
  if (axis < 1 || axis > 9) return;

  this->axis = axis;
  axis_index = axis - 1;

  PulsePin = pulsePin;
  pulseDirection = direction;

  pulseOnlyInstance[axis_index] = this;
}

bool PulseOnly::init() {
  if (ready) return true;
  if (!Encoder::init()) return false;

  pinMode(pulsePin, INPUT_PULLUP);

  int pulsePin = digitalPinToInterrupt(PulsePin);

  if (pulsePin < 0) {
    DF("ERR: Encoder PulseOnly"); D(axis); DLF(" init(), couldn't attach interrupt!"); 
    return false;
  }

  switch (axis) {
    #if AXIS1_ENCODER == PULSE_ONLY
      case 1:
        attachInterrupt(pulsePin, pulse_A_Axis1, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS2_ENCODER == PULSE_ONLY
      case 2:
        attachInterrupt(pulsePin, pulse_A_Axis2, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS3_ENCODER == PULSE_ONLY
      case 3:
        attachInterrupt(pulsePin, pulse_A_Axis3, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS4_ENCODER == PULSE_ONLY
      case 4:
        attachInterrupt(pulsePin, pulse_A_Axis4, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS5_ENCODER == PULSE_ONLY
      case 5:
        attachInterrupt(pulsePin, pulse_A_Axis5, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS6_ENCODER == PULSE_ONLY
      case 6:
        attachInterrupt(pulsePin, pulse_A_Axis6, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS7_ENCODER == PULSE_ONLY
      case 7:
        attachInterrupt(pulsePin, pulse_A_Axis7, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS8_ENCODER == PULSE_ONLY
      case 8:
        attachInterrupt(pulsePin, pulse_A_Axis8, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS9_ENCODER == PULSE_ONLY
      case 9:
        attachInterrupt(pulsePin, pulse_A_Axis9, ENCODER_SIGNAL_MODE);
      break;
    #endif
  }

  ready = true;
}

int32_t PulseOnly::read() {
  if (!ready) return 0;

  noInterrupts();
  count = pulseCount;
  interrupts();

  return count + index;
}

void PulseOnly::write(int32_t position) {
  if (!ready) return;

  noInterrupts();
  index = position - pulseCount;
  interrupts();
}

void PulseOnly::setDirection(volatile int8_t *direction) {
  if (!ready) return;

  pulseDirection = direction;
}

IRAM_ATTR void PulseOnly::pulse() {
  #if ENCODER_FILTER > 0
    ENCODER_FILTER_UNTIL(ENCODER_FILTER);
  #endif
  pulseCount += *pulseDirection; 
}

#endif
