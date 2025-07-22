// Pulse/Dir encoders

#include "PulseDir.h"

#if AXIS1_ENCODER == PULSE_DIR || AXIS2_ENCODER == PULSE_DIR || AXIS3_ENCODER == PULSE_DIR || \
    AXIS4_ENCODER == PULSE_DIR || AXIS5_ENCODER == PULSE_DIR || AXIS6_ENCODER == PULSE_DIR || \
    AXIS7_ENCODER == PULSE_DIR || AXIS8_ENCODER == PULSE_DIR || AXIS9_ENCODER == PULSE_DIR

volatile int32_t _pulse_dir_count[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
volatile int16_t _pulse_dir_dirPin[9];

#if AXIS1_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis1() { if (digitalReadF(_pulse_dir_dirPin[0])) _pulse_dir_count[0]--; else _pulse_dir_count[0]++; }
#endif

#if AXIS2_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis2() { if (digitalReadF(_pulse_dir_dirPin[1])) _pulse_dir_count[1]--; else _pulse_dir_count[1]++; }
#endif

#if AXIS3_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis3() { if (digitalReadF(_pulse_dir_dirPin[2])) _pulse_dir_count[2]--; else _pulse_dir_count[2]++; }
#endif

#if AXIS4_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis4() { if (digitalReadF(_pulse_dir_dirPin[3])) _pulse_dir_count[3]--; else _pulse_dir_count[3]++; }
#endif

#if AXIS5_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis5() { if (digitalReadF(_pulse_dir_dirPin[4])) _pulse_dir_count[4]--; else _pulse_dir_count[4]++; }
#endif

#if AXIS6_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis6() { if (digitalReadF(_pulse_dir_dirPin[5])) _pulse_dir_count[5]--; else _pulse_dir_count[5]++; }
#endif

#if AXIS7_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis7() { if (digitalReadF(_pulse_dir_dirPin[6])) _pulse_dir_count[6]--; else _pulse_dir_count[6]++; }
#endif

#if AXIS8_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis8() { if (digitalReadF(_pulse_dir_dirPin[7])) _pulse_dir_count[7]--; else _pulse_dir_count[7]++; }
#endif

#if AXIS9_ENCODER == PULSE_DIR
  IRAM_ATTR void pulseDir_A_Axis9() { if (digitalReadF(_pulse_dir_dirPin[8])) _pulse_dir_count[8]--; else _pulse_dir_count[8]++; }
#endif

PulseDir::PulseDir(int16_t pulsePin, int16_t dirPin, int16_t axis) {
  if (axis < 1 || axis > 9) return;

  this->axis = axis;
  axis_index = axis - 1;

  PulsePin = pulsePin;
  DirPin = dirPin;
  _pulse_dir_dirPin[axis_index] = DirPin;
}

bool PulseDir::init() {
  if (ready) return true;

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
        attachInterrupt(pulsePin, pulseDir_A_Axis1, CHANGE);
      break;
    #endif
    #if AXIS2_ENCODER == PULSE_DIR
      case 2:
        attachInterrupt(pulsePin, pulseDir_A_Axis2, CHANGE);
      break;
    #endif
    #if AXIS3_ENCODER == PULSE_DIR
      case 3:
        attachInterrupt(pulsePin, pulseDir_A_Axis3, CHANGE);
      break;
    #endif
    #if AXIS4_ENCODER == PULSE_DIR
      case 4:
        attachInterrupt(pulsePin, pulseDir_A_Axis4, CHANGE);
      break;
    #endif
    #if AXIS5_ENCODER == PULSE_DIR
      case 5:
        attachInterrupt(pulsePin, pulseDir_A_Axis5, CHANGE);
      break;
    #endif
    #if AXIS6_ENCODER == PULSE_DIR
      case 6:
        attachInterrupt(pulsePin, pulseDir_A_Axis6, CHANGE);
      break;
    #endif
    #if AXIS7_ENCODER == PULSE_DIR
      case 7:
        attachInterrupt(pulsePin, pulseDir_A_Axis7, CHANGE);
      break;
    #endif
    #if AXIS8_ENCODER == PULSE_DIR
      case 8:
        attachInterrupt(pulsePin, pulseDir_A_Axis8, CHANGE);
      break;
    #endif
    #if AXIS9_ENCODER == PULSE_DIR
      case 9:
        attachInterrupt(pulsePin, pulseDir_A_Axis9, CHANGE);
      break;
    #endif
  }

  ready = true;
  return true;
}

int32_t PulseDir::read() {
  if (!ready) return 0;

  noInterrupts();
  count = _pulse_dir_count[axis_index];
  interrupts();

  return count + index;
}

void PulseDir::write(int32_t position) {
  if (!ready) return;

  noInterrupts();
  index = position - _pulse_dir_count[axis_index];
  interrupts();
}

#endif
