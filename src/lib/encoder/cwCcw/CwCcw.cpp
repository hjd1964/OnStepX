// CW/CCW encoders

#include "CwCcw.h"

#if AXIS1_ENCODER == CW_CCW || AXIS2_ENCODER == CW_CCW || AXIS3_ENCODER == CW_CCW || \
    AXIS4_ENCODER == CW_CCW || AXIS5_ENCODER == CW_CCW || AXIS6_ENCODER == CW_CCW || \
    AXIS7_ENCODER == CW_CCW || AXIS8_ENCODER == CW_CCW || AXIS9_ENCODER == CW_CCW

volatile int32_t _cw_ccw_count[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

#if AXIS1_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis1() { _cw_ccw_count[0]++; }
  IRAM_ATTR void cwCcw_B_Axis1() { _cw_ccw_count[0]--; }
#endif

#if AXIS2_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis2() { _cw_ccw_count[1]++; }
  IRAM_ATTR void cwCcw_B_Axis2() { _cw_ccw_count[1]--; }
#endif

#if AXIS3_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis3() { _cw_ccw_count[2]++; }
  IRAM_ATTR void cwCcw_B_Axis3() { _cw_ccw_count[2]--;}
#endif

#if AXIS4_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis4() { _cw_ccw_count[3]++; }
  IRAM_ATTR void cwCcw_B_Axis4() { _cw_ccw_count[3]--; }
#endif

#if AXIS5_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis5() { _cw_ccw_count[4]++; }
  IRAM_ATTR void cwCcw_B_Axis5() { _cw_ccw_count[4]--; }
#endif

#if AXIS6_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis6() { _cw_ccw_count[5]++; }
  IRAM_ATTR void cwCcw_B_Axis6() { _cw_ccw_count[5]--; }
#endif

#if AXIS7_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis7() { _cw_ccw_count[6]++; }
  IRAM_ATTR void cwCcw_B_Axis7() { _cw_ccw_count[6]--; }
#endif

#if AXIS8_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis8() { _cw_ccw_count[7]++; }
  IRAM_ATTR void cwCcw_B_Axis8() { _cw_ccw_count[7]--; }
#endif

#if AXIS9_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis9() { _cw_ccw_count[8]++; }
  IRAM_ATTR void cwCcw_B_Axis9() { _cw_ccw_count[8]--; }
#endif

CwCcw::CwCcw(int16_t cwPin, int16_t ccwPin, int16_t axis) {
  if (axis < 1 || axis > 9) return;

  this->axis = axis;
  axis_index = axis - 1;

  CwPin = cwPin;
  CcwPin = ccwPin;
}

bool CwCcw::init() {
  if (ready) return true;

  pinMode(CwPin, INPUT_PULLUP);
  pinMode(CcwPin, INPUT_PULLUP);

  int cwPin = digitalPinToInterrupt(CwPin);
  int ccwPin = digitalPinToInterrupt(CcwPin);

  if (cwPin < 0 || ccwPin < 0) {
    DF("ERR: Encoder CwCcw"); D(axis); DLF(" init(), couldn't attach interrupt!"); 
    return false;
  }

  switch (axis) {
    #if AXIS1_ENCODER == CW_CCW
      case 1:
        attachInterrupt(cwPin, cwCcw_A_Axis1, CHANGE);
        attachInterrupt(ccwPin, cwCcw_B_Axis1, CHANGE);
      break;
    #endif
    #if AXIS2_ENCODER == CW_CCW
      case 2:
        attachInterrupt(cwPin, cwCcw_A_Axis2, CHANGE);
        attachInterrupt(ccwPin, cwCcw_B_Axis2, CHANGE);
      break;
    #endif
    #if AXIS3_ENCODER == CW_CCW
      case 3:
        attachInterrupt(cwPin, cwCcw_A_Axis3, CHANGE);
        attachInterrupt(ccwPin, cwCcw_B_Axis3, CHANGE);
      break;
    #endif
    #if AXIS4_ENCODER == CW_CCW
      case 4:
        attachInterrupt(cwPin, cwCcw_A_Axis4, CHANGE);
        attachInterrupt(ccwPin, cwCcw_B_Axis4, CHANGE);
      break;
    #endif
    #if AXIS5_ENCODER == CW_CCW
      case 5:
        attachInterrupt(cwPin, cwCcw_A_Axis5, CHANGE);
        attachInterrupt(ccwPin, cwCcw_B_Axis5, CHANGE);
      break;
    #endif
    #if AXIS6_ENCODER == CW_CCW
      case 6:
        attachInterrupt(cwPin, cwCcw_A_Axis6, CHANGE);
        attachInterrupt(ccwPin, cwCcw_B_Axis6, CHANGE);
      break;
    #endif
    #if AXIS7_ENCODER == CW_CCW
      case 7:
        attachInterrupt(cwPin, cwCcw_A_Axis7, CHANGE);
        attachInterrupt(ccwPin, cwCcw_B_Axis7, CHANGE);
      break;
    #endif
    #if AXIS8_ENCODER == CW_CCW
      case 8:
        attachInterrupt(cwPin, cwCcw_A_Axis8, CHANGE);
        attachInterrupt(ccwPin, cwCcw_B_Axis8, CHANGE);
      break;
    #endif
    #if AXIS9_ENCODER == CW_CCW
      case 9:
        attachInterrupt(cwPin, cwCcw_A_Axis9, CHANGE);
        attachInterrupt(ccwPin, cwCcw_B_Axis9, CHANGE);
      break;
    #endif
  }

  ready = true;
  return true;
}

int32_t CwCcw::read() {
  if (!ready) return 0;
  
  noInterrupts();
  count = _cw_ccw_count[axis_index];
  interrupts();

  return count + index;
}

void CwCcw::write(int32_t position) {
  if (!ready) return;

  noInterrupts();
  index = position - _cw_ccw_count[axis_index];
  interrupts();
}

#endif
