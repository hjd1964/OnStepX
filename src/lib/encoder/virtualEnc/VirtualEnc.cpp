// Pulse only encoders

#include "VirtualEnc.h"

#if AXIS1_ENCODER == VIRTUAL || AXIS2_ENCODER == VIRTUAL || AXIS3_ENCODER == VIRTUAL || \
    AXIS4_ENCODER == VIRTUAL || AXIS5_ENCODER == VIRTUAL || AXIS6_ENCODER == VIRTUAL || \
    AXIS7_ENCODER == VIRTUAL || AXIS8_ENCODER == VIRTUAL || AXIS9_ENCODER == VIRTUAL

#include "../../tasks/OnTask.h"

float _virtual_count[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
float _virtual_direction[9];
float _virtual_increment[9];

#if AXIS1_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis1() { _virtual_count[0] += _virtual_direction[0] * _virtual_increment[0]; }
#endif

#if AXIS2_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis2() { _virtual_count[1] += _virtual_direction[1] * _virtual_increment[1]; }
#endif

#if AXIS3_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis3() { _virtual_count[2] += _virtual_direction[2] * _virtual_increment[2]; }
#endif

#if AXIS4_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis4() { _virtual_count[3] += _virtual_direction[3] * _virtual_increment[3]; }
#endif

#if AXIS5_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis5() { _virtual_count[4] += _virtual_direction[4] * _virtual_increment[4]; }
#endif

#if AXIS6_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis6() { _virtual_count[5] += _virtual_direction[5] * _virtual_increment[5]; }
#endif

#if AXIS7_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis7() { _virtual_count[6] += _virtual_direction[6] * _virtual_increment[6]; }
#endif

#if AXIS8_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis8() { _virtual_count[7] += _virtual_direction[7] * _virtual_increment[7]; }
#endif

#if AXIS9_ENCODER == VIRTUAL
  IRAM_ATTR void pulse_A_Axis9() { _virtual_count[8] += _virtual_direction[8] * _virtual_increment[8]; }
#endif

VirtualEnc::VirtualEnc(int16_t axis) {
  if (axis < 1 || axis > 9) return;

  this->axis = axis;
  axis_index = axis - 1;

  this->countsPerSec = countsPerSec;

  _virtual_direction[axis_index] = 1; 
  _virtual_increment[axis_index] = 0.0F;

  isVirtual = true;
}

bool VirtualEnc::init() {
  if (ready) return true;
  Encoder::init();

  virtualEncoderHandle = 0;

  switch (axis) {
    #if AXIS1_ENCODER == VIRTUAL
      case 1:
        virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis1, "venc1");
      break;
    #endif
    #if AXIS2_ENCODER == VIRTUAL
      case 2:
        virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis2, "venc2");
      break;
    #endif
    #if AXIS3_ENCODER == VIRTUAL
      case 3:
        virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis3, "venc3");
      break;
    #endif
    #if AXIS4_ENCODER == VIRTUAL
      case 4:
        virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis4, "venc4");
      break;
    #endif
    #if AXIS5_ENCODER == VIRTUAL
      case 5:
        virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis5, "venc5");
      break;
    #endif
    #if AXIS6_ENCODER == VIRTUAL
      case 6:
        virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis6, "venc6");
      break;
    #endif
    #if AXIS7_ENCODER == VIRTUAL
      case 7:
        virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis7, "venc7");
      break;
    #endif
    #if AXIS8_ENCODER == VIRTUAL
      case 8:
        virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis8, "venc8");
      break;
    #endif
    #if AXIS9_ENCODER == VIRTUAL
      case 9:
        virtualEncoderHandle = tasks.add(timerRateMs, 0, true, 3, pulse_A_Axis9, "venc9");
      break;
    #endif
  }

  if (!virtualEncoderHandle) {
    DF("ERR: Encoder Virtual"); D(axis); DLF(" init(), couldn't get task!"); 
    return false;
  }

  ready = true;
  return true;
}

int32_t VirtualEnc::read() {
  if (!ready) return 0;

  noInterrupts();
  count = _virtual_count[axis_index];
  interrupts();

  return count + index;
}

void VirtualEnc::write(int32_t position) {
  if (!ready) return;
  
  noInterrupts();
  index = position - _virtual_count[axis_index];
  interrupts();
}

void VirtualEnc::setVelocity(float countsPerSec) {
  if (!ready) { DF("WRN: Encoder Virtual"); D(axis); DLF(" setVelocity(), not ready!"); return; }
  _virtual_increment[axis_index] = (countsPerSec)*(timerRateMs/1000.0F);
}

void VirtualEnc::setDirection(volatile int8_t *direction) {
  _virtual_direction[axis_index] = *direction;
}

#endif
