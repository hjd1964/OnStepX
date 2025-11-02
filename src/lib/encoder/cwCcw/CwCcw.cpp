// CW/CCW encoders

#include "CwCcw.h"

#if AXIS1_ENCODER == CW_CCW || AXIS2_ENCODER == CW_CCW || AXIS3_ENCODER == CW_CCW || \
    AXIS4_ENCODER == CW_CCW || AXIS5_ENCODER == CW_CCW || AXIS6_ENCODER == CW_CCW || \
    AXIS7_ENCODER == CW_CCW || AXIS8_ENCODER == CW_CCW || AXIS9_ENCODER == CW_CCW

CwCcw *cwCcwInstance[9];

#if AXIS1_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis1() { cwCcwInstance[0]->cw(); }
  IRAM_ATTR void cwCcw_B_Axis1() { cwCcwInstance[0]->ccw(); }
#endif

#if AXIS2_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis2() { cwCcwInstance[1]->cw();}
  IRAM_ATTR void cwCcw_B_Axis2() { cwCcwInstance[1]->ccw(); }
#endif

#if AXIS3_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis3() { cwCcwInstance[2]->cw(); }
  IRAM_ATTR void cwCcw_B_Axis3() { cwCcwInstance[2]->ccw(); }
#endif

#if AXIS4_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis4() { cwCcwInstance[3]->cw(); }
  IRAM_ATTR void cwCcw_B_Axis4() { cwCcwInstance[3]->ccw(); }
#endif

#if AXIS5_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis5() { cwCcwInstance[4]->cw(); }
  IRAM_ATTR void cwCcw_B_Axis5() { cwCcwInstance[4]->ccw(); }
#endif

#if AXIS6_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis6() { cwCcwInstance[5]->cw(); }
  IRAM_ATTR void cwCcw_B_Axis6() { cwCcwInstance[5]->ccw(); }
#endif

#if AXIS7_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis7() { cwCcwInstance[6]->cw(); }
  IRAM_ATTR void cwCcw_B_Axis7() { cwCcwInstance[6]->ccw(); }
#endif

#if AXIS8_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis8() { cwCcwInstance[7]->cw(); }
  IRAM_ATTR void cwCcw_B_Axis8() { cwCcwInstance[7]->ccw(); }
#endif

#if AXIS9_ENCODER == CW_CCW
  IRAM_ATTR void cwCcw_A_Axis9() { cwCcwInstance[8]->cw(); }
  IRAM_ATTR void cwCcw_B_Axis9() { cwCcwInstance[8]->ccw(); }
#endif

CwCcw::CwCcw(int16_t axis, int16_t cwPin, int16_t ccwPin) {
  if (axis < 1 || axis > 9) return;

  this->axis = axis;
  axis_index = axis - 1;

  CwPin = cwPin;
  CcwPin = ccwPin;

  cwCcwInstance[axis_index] = this;
}

bool CwCcw::init() {
  if (ready) return true;
  if (!Encoder::init()) return false;

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
        attachInterrupt(cwPin, cwCcw_A_Axis1, ENCODER_SIGNAL_MODE);
        attachInterrupt(ccwPin, cwCcw_B_Axis1, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS2_ENCODER == CW_CCW
      case 2:
        attachInterrupt(cwPin, cwCcw_A_Axis2, ENCODER_SIGNAL_MODE);
        attachInterrupt(ccwPin, cwCcw_B_Axis2, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS3_ENCODER == CW_CCW
      case 3:
        attachInterrupt(cwPin, cwCcw_A_Axis3, ENCODER_SIGNAL_MODE);
        attachInterrupt(ccwPin, cwCcw_B_Axis3, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS4_ENCODER == CW_CCW
      case 4:
        attachInterrupt(cwPin, cwCcw_A_Axis4, ENCODER_SIGNAL_MODE);
        attachInterrupt(ccwPin, cwCcw_B_Axis4, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS5_ENCODER == CW_CCW
      case 5:
        attachInterrupt(cwPin, cwCcw_A_Axis5, ENCODER_SIGNAL_MODE);
        attachInterrupt(ccwPin, cwCcw_B_Axis5, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS6_ENCODER == CW_CCW
      case 6:
        attachInterrupt(cwPin, cwCcw_A_Axis6, ENCODER_SIGNAL_MODE);
        attachInterrupt(ccwPin, cwCcw_B_Axis6, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS7_ENCODER == CW_CCW
      case 7:
        attachInterrupt(cwPin, cwCcw_A_Axis7, ENCODER_SIGNAL_MODE);
        attachInterrupt(ccwPin, cwCcw_B_Axis7, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS8_ENCODER == CW_CCW
      case 8:
        attachInterrupt(cwPin, cwCcw_A_Axis8, ENCODER_SIGNAL_MODE);
        attachInterrupt(ccwPin, cwCcw_B_Axis8, ENCODER_SIGNAL_MODE);
      break;
    #endif
    #if AXIS9_ENCODER == CW_CCW
      case 9:
        attachInterrupt(cwPin, cwCcw_A_Axis9, ENCODER_SIGNAL_MODE);
        attachInterrupt(ccwPin, cwCcw_B_Axis9, ENCODER_SIGNAL_MODE);
      break;
    #endif
  }

  ready = true;
  return true;
}

int32_t CwCcw::read() {
  if (!ready) return 0;
  
  noInterrupts();
  count = cwCcwCount;
  interrupts();

  return count + index;
}

void CwCcw::write(int32_t position) {
  if (!ready) return;

  noInterrupts();
  index = position - cwCcwCount;
  interrupts();
}

IRAM_ATTR void CwCcw::cw() {
  #if ENCODER_FILTER > 0
    ENCODER_FILTER_UNTIL(ENCODER_FILTER);
  #endif
  cwCcwCount++;
}

IRAM_ATTR void CwCcw::ccw() {
  #if ENCODER_FILTER > 0
    ENCODER_FILTER_UNTIL(ENCODER_FILTER);
  #endif
  cwCcwCount--;
}

#endif
