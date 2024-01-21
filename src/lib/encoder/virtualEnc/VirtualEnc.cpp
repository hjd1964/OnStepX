// Pulse only encoders

#include "VirtualEnc.h"

#if AXIS1_ENCODER == VIRTUAL || AXIS2_ENCODER == VIRTUAL || AXIS3_ENCODER == VIRTUAL || \
    AXIS4_ENCODER == VIRTUAL || AXIS5_ENCODER == VIRTUAL || AXIS6_ENCODER == VIRTUAL || \
    AXIS7_ENCODER == VIRTUAL || AXIS8_ENCODER == VIRTUAL || AXIS9_ENCODER == VIRTUAL

#include "..\..\tasks\OnTask.h"

float _pulse_count[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
float _direction[9];
float _increment[9];

#if AXIS1_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis1() { _pulse_count[0] += _direction[0] * _increment[0]; }
#endif

#if AXIS2_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis2() { _pulse_count[1] += _direction[1] * _increment[1]; }
#endif

#if AXIS3_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis3() { _pulse_count[2] += _direction[2] * _increment[2]; }
#endif

#if AXIS4_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis4() { _pulse_count[3] += _direction[3] * _increment[3]; }
#endif

#if AXIS5_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis5() { _pulse_count[4] += _direction[4] * _increment[4]; }
#endif

#if AXIS6_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis6() { _pulse_count[5] += _direction[5] * _increment[5]; }
#endif

#if AXIS7_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis7() { _pulse_count[6] += _direction[6] * _increment[6]; }
#endif

#if AXIS8_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis8() { _pulse_count[7] += _direction[7] * _increment[7]; }
#endif

#if AXIS9_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis9() { _pulse_count[8] += _direction[8] * _increment[8]; }
#endif

VirtualEnc::VirtualEnc(int16_t axis) {
  if (axis < 1 || axis > 9) return;

  this->axis = axis - 1;

  this->countsPerSec = countsPerSec;

  _direction[this->axis] = 1; 
  _increment[this->axis] = 0.0F;

  isVirtual = true;
}

void VirtualEnc::init() {
  if (initialized) { VF("WRN: Encoder Virtual"); V(axis); VLF(" init(), already initialized!"); return; }

  #if AXIS1_ENCODER == VIRTUAL
    if (axis == 0) {
      virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis1, "venc1");
    }
  #endif
  #if AXIS2_ENCODER == VIRTUAL
    if (axis == 1) {
      virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis2, "venc2");
    }
  #endif
  #if AXIS3_ENCODER == VIRTUAL
    if (axis == 2) {
      virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis3, "venc3");
    }
  #endif
  #if AXIS4_ENCODER == VIRTUAL
    if (axis == 3) {
      virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis4, "venc4");
    }
  #endif
  #if AXIS5_ENCODER == VIRTUAL
    if (axis == 4) {
      virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis5, "venc5");
    }
  #endif
  #if AXIS6_ENCODER == VIRTUAL
    if (axis == 5) {
      virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis6, "venc6");
    }
  #endif
  #if AXIS7_ENCODER == VIRTUAL
    if (axis == 6) {
      virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis7, "venc7");
    }
  #endif
  #if AXIS8_ENCODER == VIRTUAL
    if (axis == 7) {
      virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis8, "venc8");
    }
  #endif
  #if AXIS9_ENCODER == VIRTUAL
    if (axis == 8) {
      virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis9, "venc9");
    }
  #endif

  initialized = true;
}

int32_t VirtualEnc::read() {
  if (!initialized) { VF("WRN: Encoder Virtual"); V(axis); VLF(" read(), not initialized!"); return 0; }

  noInterrupts();
  int32_t count = _pulse_count[axis];
  interrupts();

  return count + origin;
}

void VirtualEnc::write(int32_t count) {
  if (!initialized) { VF("WRN: Encoder Virtual"); V(axis); VLF(" write(), not initialized!"); return; }

  count -= origin;

  noInterrupts();
  _pulse_count[axis] = count;
  interrupts();
}

void VirtualEnc::setVelocity(float countsPerSec) {
  if (!initialized) { VF("WRN: Encoder Virtual"); V(axis); VLF(" setVelocity(), not initialized!"); return; }
  _increment[axis] = (countsPerSec)*(timerRateMs/1000.0F);
}

void VirtualEnc::setDirection(volatile int8_t *direction) {
  _direction[axis] = *direction;
}

#endif
