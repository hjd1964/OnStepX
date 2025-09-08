// Pulse/Dir encoders

#include "PulseDir.h"

#if AXIS1_ENCODER == PULSE_DIR || AXIS2_ENCODER == PULSE_DIR || AXIS3_ENCODER == PULSE_DIR || \
    AXIS4_ENCODER == PULSE_DIR || AXIS5_ENCODER == PULSE_DIR || AXIS6_ENCODER == PULSE_DIR || \
    AXIS7_ENCODER == PULSE_DIR || AXIS8_ENCODER == PULSE_DIR || AXIS9_ENCODER == PULSE_DIR

PulseDir *pulseDirInstance[9];

#if AXIS1_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis1() { pulseDirInstance[0]->pulse(AXIS1_ENCODER_B_PIN); }
#endif

#if AXIS2_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis2() { pulseDirInstance[1]->pulse(AXIS2_ENCODER_B_PIN); }
#endif

#if AXIS3_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis3() { pulseDirInstance[2]->pulse(AXIS3_ENCODER_B_PIN); }
#endif

#if AXIS4_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis4() { pulseDirInstance[3]->pulse(AXIS4_ENCODER_B_PIN); }
#endif

#if AXIS5_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis5() { pulseDirInstance[4]->pulse(AXIS5_ENCODER_B_PIN); }
#endif

#if AXIS6_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis6() { pulseDirInstance[5]->pulse(AXIS6_ENCODER_B_PIN); }
#endif

#if AXIS7_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis7() { pulseDirInstance[6]->pulse(AXIS7_ENCODER_B_PIN); }
#endif

#if AXIS8_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis8() { pulseDirInstance[7]->pulse(AXIS8_ENCODER_B_PIN); }
#endif

#if AXIS9_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis9() { pulseDirInstance[8]->pulse(AXIS9_ENCODER_B_PIN); }
#endif

PulseDir::PulseDir(int16_t axis, int16_t pulsePin, int16_t dirPin) {
  if (axis < 1 || axis > 9) return;

  this->axis = axis;
  axis_index = axis - 1;

  PulsePin = pulsePin;
  DirPin = dirPin;

  pulseDirInstance[axis_index] = this;
}

bool PulseDir::init() {
  if (ready) return true;
  if (!Encoder::init()) return false;

  pinMode(PulsePin, INPUT_PULLUP);
  pinMode(DirPin, INPUT_PULLUP);

  int pulsePin = digitalPinToInterrupt(PulsePin);

  if (pulsePin < 0) {
    DF("ERR: Encoder PulseDir"); D(axis); DLF(" init(), couldn't attach interrupt!"); 
    return false;
  }

  switch (axis) {
    #if AXIS1_ENCODER == PULSE_DIR
      case 1:
        attachInterrupt(pulsePin, pulseDir_A_Axis1, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS2_ENCODER == PULSE_DIR
      case 2:
        attachInterrupt(pulsePin, pulseDir_A_Axis2, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS3_ENCODER == PULSE_DIR
      case 3:
        attachInterrupt(pulsePin, pulseDir_A_Axis3, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS4_ENCODER == PULSE_DIR
      case 4:
        attachInterrupt(pulsePin, pulseDir_A_Axis4, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS5_ENCODER == PULSE_DIR
      case 5:
        attachInterrupt(pulsePin, pulseDir_A_Axis5, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS6_ENCODER == PULSE_DIR
      case 6:
        attachInterrupt(pulsePin, pulseDir_A_Axis6, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS7_ENCODER == PULSE_DIR
      case 7:
        attachInterrupt(pulsePin, pulseDir_A_Axis7, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS8_ENCODER == PULSE_DIR
      case 8:
        attachInterrupt(pulsePin, pulseDir_A_Axis8, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS9_ENCODER == PULSE_DIR
      case 9:
        attachInterrupt(pulsePin, pulseDir_A_Axis9, ENCODER_SIGNAL_MODE);
      break;
    #endif
  }

  ready = true;
  return true;
}

int32_t PulseDir::read() {
  if (!ready) return 0;

  noInterrupts();
  count = pulseDirCount;
  interrupts();

  return count + index;
}

void PulseDir::write(int32_t position) {
  if (!ready) return;

  noInterrupts();
  index = position - pulseDirCount;
  interrupts();
}

IRAM_ATTR PulseDir::pulse(const int16_t dirPin) {
  #if ENCODER_FILTER > 0
    ENCODER_FILTER_UNTIL(ENCODER_FILTER);
  #endif
  if (digitalReadF(dirPin)) pulseDirCount--; else pulseDirCount++;
}

#endif
