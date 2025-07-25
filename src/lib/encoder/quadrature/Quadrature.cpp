// A/B Quadrature encoders

#include "Quadrature.h"

#if AXIS1_ENCODER == AB || AXIS2_ENCODER == AB || AXIS3_ENCODER == AB || \
    AXIS4_ENCODER == AB || AXIS5_ENCODER == AB || AXIS6_ENCODER == AB || \
    AXIS7_ENCODER == AB || AXIS8_ENCODER == AB || AXIS9_ENCODER == AB

Quadrature *quadratureInstance[9];

#if AXIS1_ENCODER == AB
  IRAM_ATTR void quadrature_A_Axis1() { quadratureInstance[0]->A(AXIS1_ENCODER_A_PIN); }
  IRAM_ATTR void quadrature_B_Axis1() { quadratureInstance[0]->B(AXIS1_ENCODER_B_PIN); }
#endif

#if AXIS2_ENCODER == AB
  IRAM_ATTR void quadrature_A_Axis2() { quadratureInstance[1]->A(AXIS2_ENCODER_A_PIN); }
  IRAM_ATTR void quadrature_B_Axis2() { quadratureInstance[1]->B(AXIS2_ENCODER_B_PIN); }
#endif

#if AXIS3_ENCODER == AB
  IRAM_ATTR void quadrature_A_Axis3() { quadratureInstance[2]->A(AXIS3_ENCODER_A_PIN); }
  IRAM_ATTR void quadrature_B_Axis3() { quadratureInstance[2]->B(AXIS3_ENCODER_B_PIN); }
#endif

#if AXIS4_ENCODER == AB
  IRAM_ATTR void quadrature_A_Axis4() { quadratureInstance[3]->A(AXIS4_ENCODER_A_PIN); }
  IRAM_ATTR void quadrature_B_Axis4() { quadratureInstance[3]->B(AXIS4_ENCODER_B_PIN); }
#endif

#if AXIS5_ENCODER == AB
  IRAM_ATTR void quadrature_A_Axis5() { quadratureInstance[4]->A(AXIS5_ENCODER_A_PIN); }
  IRAM_ATTR void quadrature_B_Axis5() { quadratureInstance[4]->B(AXIS5_ENCODER_B_PIN); }
#endif

#if AXIS6_ENCODER == AB
  IRAM_ATTR void quadrature_A_Axis6() { quadratureInstance[5]->A(AXIS6_ENCODER_A_PIN); }
  IRAM_ATTR void quadrature_B_Axis6() { quadratureInstance[5]->B(AXIS6_ENCODER_B_PIN); }
#endif

#if AXIS7_ENCODER == AB
  IRAM_ATTR void quadrature_A_Axis7() { quadratureInstance[6]->A(AXIS7_ENCODER_A_PIN); }
  IRAM_ATTR void quadrature_B_Axis7() { quadratureInstance[6]->B(AXIS7_ENCODER_B_PIN); }
#endif

#if AXIS8_ENCODER == AB
  IRAM_ATTR void quadrature_A_Axis8() { quadratureInstance[7]->A(AXIS8_ENCODER_A_PIN); }
  IRAM_ATTR void quadrature_B_Axis8() { quadratureInstance[7]->B(AXIS8_ENCODER_B_PIN); }
#endif

#if AXIS9_ENCODER == AB
  IRAM_ATTR void quadrature_A_Axis9() { quadratureInstance[8]->A(AXIS9_ENCODER_A_PIN); }
  IRAM_ATTR void quadrature_B_Axis9() { quadratureInstance[8]->B(AXIS9_ENCODER_B_PIN); }
#endif

Quadrature::Quadrature(int16_t APin, int16_t BPin, int16_t axis) {
  if (axis < 1 || axis > 9) return;

  this->axis = axis;
  axis_index = axis - 1;

  this->APin = APin;
  this->BPin = BPin;
  quadratureInstance[axis_index] = this;
}

bool Quadrature::init() {
  if (ready) return true;
  if (!Encoder::init()) return false;

  pinMode(APin, INPUT_PULLUP);
  pinMode(BPin, INPUT_PULLUP);

  stateA = digitalRead(APin);
  lastA = stateA;
  stateB = digitalRead(BPin);
  lastB = stateB;

  int aPin = digitalPinToInterrupt(APin);
  int bPin = digitalPinToInterrupt(BPin);

  if (aPin < 0 || bPin < 0) {
    DF("ERR: Encoder Quadrature"); D(axis); DLF(" init(), couldn't attach interrupt!"); 
    return false;
  }

  switch (axis) {
    #if AXIS1_ENCODER == AB
      case 1:
        attachInterrupt(aPin, quadrature_A_Axis1, CHANGE);
        attachInterrupt(bPin, quadrature_B_Axis1, CHANGE);
      break;
    #endif
    #if AXIS2_ENCODER == AB
      case 2:
        attachInterrupt(aPin, quadrature_A_Axis2, CHANGE);
        attachInterrupt(bPin, quadrature_B_Axis2, CHANGE);
      break;
    #endif
    #if AXIS3_ENCODER == AB
      case 3:
        attachInterrupt(aPin, quadrature_A_Axis3, CHANGE);
        attachInterrupt(bPin, quadrature_B_Axis3, CHANGE);
      break;
    #endif
    #if AXIS4_ENCODER == AB
      case 4:
        attachInterrupt(aPin, quadrature_A_Axis4, CHANGE);
        attachInterrupt(bPin, quadrature_B_Axis4, CHANGE);
      break;
    #endif
    #if AXIS5_ENCODER == AB
      case 5:
        attachInterrupt(aPin, quadrature_A_Axis5, CHANGE);
        attachInterrupt(bPin, quadrature_B_Axis5, CHANGE);
      break;
    #endif
    #if AXIS6_ENCODER == AB
      case 6:
        attachInterrupt(aPin, quadrature_A_Axis6, CHANGE);
        attachInterrupt(bPin, quadrature_B_Axis6, CHANGE);
      break;
    #endif
    #if AXIS7_ENCODER == AB
      case 7:
        attachInterrupt(aPin, quadrature_A_Axis7, CHANGE);
        attachInterrupt(bPin, quadrature_B_Axis7, CHANGE);
      break;
    #endif
    #if AXIS8_ENCODER == AB
      case 8:
        attachInterrupt(aPin, quadrature_A_Axis8, CHANGE);
        attachInterrupt(bPin, quadrature_B_Axis8, CHANGE);
      break;
    #endif
    #if AXIS9_ENCODER == AB
      case 9:
        attachInterrupt(aPin, quadrature_A_Axis9, CHANGE);
        attachInterrupt(bPin, quadrature_B_Axis9, CHANGE);
      break;
    #endif
  }

  ready = true;
  return true;
}

int32_t Quadrature::read() {
  if (!ready) return 0;

  noInterrupts();
  count = quadratureCount;
  interrupts();

  return count + index;
}

void Quadrature::write(int32_t position) {
  if (!ready) return;

  noInterrupts();
  index = position - quadratureCount;
  interrupts();
}

// Phase 1: LLHH LLHH
// Phase 2: LHHL LHHL
// ...00 01 11 10 00 01 11 10 00 01 11 10...

ICACHE_RAM_ATTR void Quadrature::A(const int16_t pin) {
  #if ENCODER_FILTER > 0
    ENCODER_FILTER_UNTIL(ENCODER_FILTER);
  #endif
  stateA = digitalReadF(pin);

  uint8_t v = (stateA<<3) + (stateB<<2) + (lastA<<1) + lastB;
  switch (v) {
    case 0b0000: QUAD_F2; break; // skipped pulse (invalid A state, valid B state)
    case 0b0001: dir = -1; break;
    case 0b0010: dir = 1; break;
    case 0b0011: QUAD_F1; break; // skipped pulse (valid A state, impossible B state)
    case 0b0100: dir = 1; break;
    case 0b0101: QUAD_F2; break; // skipped pulse (invalid A state, valid B state)
    case 0b0110: QUAD_F1; break; // skipped pulse (valid A state, impossible B state)
    case 0b0111: dir = -1; break;
    case 0b1000: dir = -1; break;
    case 0b1001: QUAD_F1; break; // skipped pulse (valid A state, impossible B state)
    case 0b1010: QUAD_F2; break; // skipped pulse (invalid A state, valid B state)
    case 0b1011: dir = 1; break;
    case 0b1100: QUAD_F1; break; // skipped pulse (valid A state, impossible B state)
    case 0b1101: dir = 1; break;
    case 0b1110: dir = -1; break;
    case 0b1111: QUAD_F2; break; // skipped pulse (invalid A state, valid B state)
  }
  quadratureCount += dir;
  
  lastA = stateA;
}

ICACHE_RAM_ATTR void Quadrature::B(const int16_t pin) {
  #if ENCODER_FILTER > 0
    ENCODER_FILTER_UNTIL(ENCODER_FILTER);
  #endif
  stateB = digitalReadF(pin);

  uint8_t v = (stateA<<3) + (stateB<<2) + (lastA<<1) + lastB;
  switch (v) {
    case 0b0000: QUAD_F2; break;  // skipped pulse (valid A state, invalid B state)
    case 0b0001: dir = -1; break;
    case 0b0010: dir = 1; break;
    case 0b0011: QUAD_F1; break;  // skipped pulse (impossible A state, valid B state)
    case 0b0100: dir = 1; break;
    case 0b0101: QUAD_F2; break;  // skipped pulse (valid A state, invalid B state)
    case 0b0110: QUAD_F1; break;  // skipped pulse (impossible A state, valid B state)
    case 0b0111: dir = -1; break;
    case 0b1000: dir = -1; break;
    case 0b1001: QUAD_F1; break;  // skipped pulse (impossible A state, valid B state)
    case 0b1010: QUAD_F2; break;  // skipped pulse (valid A state, invalid B state)
    case 0b1011: dir = 1; break;
    case 0b1100: QUAD_F1; break;  // skipped pulse (impossible A state, valid B state)
    case 0b1101: dir = 1; break;
    case 0b1110: dir = -1; break;
    case 0b1111: QUAD_F2; break;  // skipped pulse (valid A state, invalid B state)
  }
  quadratureCount += dir;

  lastB = stateB;
}

#endif
