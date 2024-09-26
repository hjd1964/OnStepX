// CW/CCW encoders

#include "CwCcw.h"

#if AXIS1_ENCODER == CW_CCW || AXIS2_ENCODER == CW_CCW || AXIS3_ENCODER == CW_CCW || \
    AXIS4_ENCODER == CW_CCW || AXIS5_ENCODER == CW_CCW || AXIS6_ENCODER == CW_CCW || \
    AXIS7_ENCODER == CW_CCW || AXIS8_ENCODER == CW_CCW || AXIS9_ENCODER == CW_CCW

volatile int32_t _cw_ccw_count[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

#if AXIS1_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis1() { _cw_ccw_count[0] = _cw_ccw_count[0] + 1; }
  IRAM_ATTR void cwCcw_B_Axis1() { _cw_ccw_count[0] = _cw_ccw_count[0] - 1; }
#endif

#if AXIS2_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis2() { _cw_ccw_count[1] = _cw_ccw_count[1] + 1; }
  IRAM_ATTR void cwCcw_B_Axis2() { _cw_ccw_count[1] = _cw_ccw_count[1] - 1; }
#endif

#if AXIS3_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis3() { _cw_ccw_count[2] = _cw_ccw_count[2] + 1; }
  IRAM_ATTR void cwCcw_B_Axis3() { _cw_ccw_count[2] = _cw_ccw_count[2] - 1;}
#endif

#if AXIS4_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis4() { _cw_ccw_count[3] = _cw_ccw_count[3] + 1; }
  IRAM_ATTR void cwCcw_B_Axis4() { _cw_ccw_count[3] = _cw_ccw_count[3] - 1; }
#endif

#if AXIS5_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis5() { _cw_ccw_count[4] = _cw_ccw_count[4] + 1; }
  IRAM_ATTR void cwCcw_B_Axis5() { _cw_ccw_count[4] = _cw_ccw_count[4] - 1; }
#endif

#if AXIS6_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis6() { _cw_ccw_count[5] = _cw_ccw_count[5] + 1; }
  IRAM_ATTR void cwCcw_B_Axis6() { _cw_ccw_count[5] = _cw_ccw_count[5] - 1; }
#endif

#if AXIS7_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis7() { _cw_ccw_count[6] = _cw_ccw_count[6] + 1; }
  IRAM_ATTR void cwCcw_B_Axis7() { _cw_ccw_count[6] = _cw_ccw_count[6] - 1; }
#endif

#if AXIS8_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis8() { _cw_ccw_count[7] = _cw_ccw_count[7] + 1; }
  IRAM_ATTR void cwCcw_B_Axis8() { _cw_ccw_count[7] = _cw_ccw_count[7] - 1; }
#endif

#if AXIS9_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis9() { _cw_ccw_count[8] = _cw_ccw_count[8] + 1; }
  IRAM_ATTR void cwCcw_B_Axis9() { _cw_ccw_count[8] = _cw_ccw_count[8] - 1; }
#endif

CwCcw::CwCcw(int16_t cwPin, int16_t ccwPin, int16_t axis) {
  if (axis < 1 || axis > 9) return;

  this->cwPin = cwPin;
  this->ccwPin = ccwPin;
  this->axis = axis - 1;
}

void CwCcw::init() {
  if (initialized) { VF("WRN: Encoder CwCcw"); V(axis); VLF(" init(), already initialized!"); return; }

  pinMode(cwPin, INPUT_PULLUP);
  pinMode(ccwPin, INPUT_PULLUP);
  #if AXIS1_ENCODER == CW_CCW
    if (axis == 0) {
      attachInterrupt(digitalPinToInterrupt(cwPin), cwCcw_A_Axis1, CHANGE);
      attachInterrupt(digitalPinToInterrupt(ccwPin), cwCcw_B_Axis1, CHANGE);
    }
  #endif
  #if AXIS2_ENCODER == CW_CCW
    if (axis == 1) {
      attachInterrupt(digitalPinToInterrupt(cwPin), cwCcw_A_Axis2, CHANGE);
      attachInterrupt(digitalPinToInterrupt(ccwPin), cwCcw_B_Axis2, CHANGE);
    }
  #endif
  #if AXIS3_ENCODER == CW_CCW
    if (axis == 2) {
      attachInterrupt(digitalPinToInterrupt(cwPin), cwCcw_A_Axis3, CHANGE);
      attachInterrupt(digitalPinToInterrupt(ccwPin), cwCcw_B_Axis3, CHANGE);
    }
  #endif
  #if AXIS4_ENCODER == CW_CCW
    if (axis == 3) {
      attachInterrupt(digitalPinToInterrupt(cwPin), cwCcw_A_Axis4, CHANGE);
      attachInterrupt(digitalPinToInterrupt(ccwPin), cwCcw_B_Axis4, CHANGE);
    }
  #endif
  #if AXIS5_ENCODER == CW_CCW
    if (axis == 4) {
      attachInterrupt(digitalPinToInterrupt(cwPin), cwCcw_A_Axis5, CHANGE);
      attachInterrupt(digitalPinToInterrupt(ccwPin), cwCcw_B_Axis5, CHANGE);
    }
  #endif
  #if AXIS6_ENCODER == CW_CCW
    if (axis == 5) {
      attachInterrupt(digitalPinToInterrupt(cwPin), cwCcw_A_Axis6, CHANGE);
      attachInterrupt(digitalPinToInterrupt(ccwPin), cwCcw_B_Axis6, CHANGE);
    }
  #endif
  #if AXIS7_ENCODER == CW_CCW
    if (axis == 6) {
      attachInterrupt(digitalPinToInterrupt(cwPin), cwCcw_A_Axis7, CHANGE);
      attachInterrupt(digitalPinToInterrupt(ccwPin), cwCcw_B_Axis7, CHANGE);
    }
  #endif
  #if AXIS8_ENCODER == CW_CCW
    if (axis == 7) {
      attachInterrupt(digitalPinToInterrupt(cwPin), cwCcw_A_Axis8, CHANGE);
      attachInterrupt(digitalPinToInterrupt(ccwPin), cwCcw_B_Axis8, CHANGE);
    }
  #endif
  #if AXIS9_ENCODER == CW_CCW
    if (axis == 8) {
      attachInterrupt(digitalPinToInterrupt(cwPin), cwCcw_A_Axis9, CHANGE);
      attachInterrupt(digitalPinToInterrupt(ccwPin), cwCcw_B_Axis9, CHANGE);
    }
  #endif

  initialized = true;
}

int32_t CwCcw::read() {
  if (!initialized) { VF("WRN: Encoder CwCcw"); V(axis); VLF(" read(), not initialized!"); return 0; }
  
  int32_t count = 0;
  noInterrupts();
  count = _cw_ccw_count[axis];
  interrupts();

  return count + origin;
}

void CwCcw::write(int32_t count) {
  if (!initialized) { VF("WRN: Encoder CwCcw"); V(axis); VLF(" write(), not initialized!"); return; }

  count -= origin;

  noInterrupts();
  _cw_ccw_count[axis] = count;
  interrupts();
}

#endif
