// Pulse/Dir encoders

#include "PulseDir.h"

#if AXIS1_ENCODER == PULSE_DIR || AXIS2_ENCODER == PULSE_DIR || AXIS3_ENCODER == PULSE_DIR || \
    AXIS4_ENCODER == PULSE_DIR || AXIS5_ENCODER == PULSE_DIR || AXIS6_ENCODER == PULSE_DIR || \
    AXIS7_ENCODER == PULSE_DIR || AXIS8_ENCODER == PULSE_DIR || AXIS9_ENCODER == PULSE_DIR

volatile int32_t _pulse_dir_count[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

#if AXIS1_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis1() { if (digitalReadF(AXIS1_ENCODER_B_PIN)) _pulse_dir_count[0]--; else _pulse_dir_count[0]++; }
#endif

#if AXIS2_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis2() { if (digitalReadF(AXIS2_ENCODER_B_PIN)) _pulse_dir_count[1]--; else _pulse_dir_count[1]++; }
#endif

#if AXIS3_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis3() { if (digitalReadF(AXIS3_ENCODER_B_PIN)) _pulse_dir_count[2]--; else _pulse_dir_count[2]++; }
#endif

#if AXIS4_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis4() { if (digitalReadF(AXIS4_ENCODER_B_PIN)) _pulse_dir_count[3]--; else _pulse_dir_count[3]++; }
#endif

#if AXIS5_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis5() { if (digitalReadF(AXIS5_ENCODER_B_PIN)) _pulse_dir_count[4]--; else _pulse_dir_count[4]++; }
#endif

#if AXIS6_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis6() { if (digitalReadF(AXIS6_ENCODER_B_PIN)) _pulse_dir_count[5]--; else _pulse_dir_count[5]++; }
#endif

#if AXIS7_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis7() { if (digitalReadF(AXIS7_ENCODER_B_PIN)) _pulse_dir_count[6]--; else _pulse_dir_count[6]++; }
#endif

#if AXIS8_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis8() { if (digitalReadF(AXIS8_ENCODER_B_PIN)) _pulse_dir_count[7]--; else _pulse_dir_count[7]++; }
#endif

#if AXIS9_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis9() { if (digitalReadF(AXIS9_ENCODER_B_PIN)) _pulse_dir_count[8]--; else _pulse_dir_count[8]++; }
#endif

PulseDir::PulseDir(int16_t pulsePin, int16_t dirPin, int16_t axis) {
  if (axis < 1 || axis > 9) return;

  this->pulsePin = pulsePin;
  this->dirPin = dirPin;
  this->axis = axis - 1;
}

void PulseDir::init() {
  if (initialized) { VF("WRN: Encoder PulseDir"); V(axis); VLF(" init(), already initialized!"); return; }

  pinMode(pulsePin, INPUT_PULLUP);
  pinMode(dirPin, INPUT_PULLUP);
  #if AXIS1_ENCODER == PULSE_DIR
    if (axis == 0) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulseDir_A_Axis1, CHANGE);
    }
  #endif
  #if AXIS2_ENCODER == PULSE_DIR
    if (axis == 1) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulseDir_A_Axis2, CHANGE);
    }
  #endif
  #if AXIS3_ENCODER == PULSE_DIR
    if (axis == 2) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulseDir_A_Axis3, CHANGE);
    }
  #endif
  #if AXIS4_ENCODER == PULSE_DIR
    if (axis == 3) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulseDir_A_Axis4, CHANGE);
    }
  #endif
  #if AXIS5_ENCODER == PULSE_DIR
    if (axis == 4) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulseDir_A_Axis5, CHANGE);
    }
  #endif
  #if AXIS6_ENCODER == PULSE_DIR
    if (axis == 5) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulseDir_A_Axis6, CHANGE);
    }
  #endif
  #if AXIS7_ENCODER == PULSE_DIR
    if (axis == 6) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulseDir_A_Axis7, CHANGE);
    }
  #endif
  #if AXIS8_ENCODER == PULSE_DIR
    if (axis == 7) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulseDir_A_Axis8, CHANGE);
    }
  #endif
  #if AXIS9_ENCODER == PULSE_DIR
    if (axis == 8) {
      attachInterrupt(digitalPinToInterrupt(pulsePin), pulseDir_A_Axis9, CHANGE);
    }
  #endif

  initialized = true;
}

int32_t PulseDir::read() {
  if (!initialized) { VF("WRN: Encoder PulseDir"); V(axis); VLF(" read(), not initialized!"); return 0; }

  noInterrupts();
  int32 count = _pulse_dir_count[axis];
  interrupts();

  return count + origin;
}

void PulseDir::write(int32_t count) {
  if (!initialized) { VF("WRN: Encoder PulseDir"); V(axis); VLF(" write(), not initialized!"); return; }

  count -= origin;

  noInterrupts();
  _pulse_dir_count[axis] = count;
  interrupts();
}

#endif
