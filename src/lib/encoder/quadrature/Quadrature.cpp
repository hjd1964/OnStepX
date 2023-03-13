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

// for example:
// Quadrature encoder1(AXIS1_ENCODER_A_PIN, AXIS1_ENCODER_B_PIN, 1);

Quadrature::Quadrature(int16_t APin, int16_t BPin, int16_t axis) {
  if (axis < 1 || axis > 9) return;

  this->APin = APin;
  this->BPin = BPin;
  this->axis = axis;
  quadratureInstance[this->axis - 1] = this;
}

void Quadrature::init() {
  if (initialized) { VF("WRN: Encoder Quadrature"); V(axis); VLF(" init(), already initialized!"); return; }

  pinMode(APin, INPUT_PULLUP);
  pinMode(BPin, INPUT_PULLUP);

  stateA = digitalRead(APin);
  lastA = stateA;
  stateB = digitalRead(BPin);
  lastB = stateB;

  switch (axis) {
    #if AXIS1_ENCODER == AB
      case 1:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis1, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis1, CHANGE);
      break;
    #endif
    #if AXIS2_ENCODER == AB
      case 2:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis2, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis2, CHANGE);
      break;
    #endif
    #if AXIS3_ENCODER == AB
      case 3:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis3, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis3, CHANGE);
      break;
    #endif
    #if AXIS4_ENCODER == AB
      case 4:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis4, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis4, CHANGE);
      break;
    #endif
    #if AXIS5_ENCODER == AB
      case 5:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis5, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis5, CHANGE);
      break;
    #endif
    #if AXIS6_ENCODER == AB
      case 6:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis6, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis6, CHANGE);
      break;
    #endif
    #if AXIS7_ENCODER == AB
      case 7:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis7, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis7, CHANGE);
      break;
    #endif
    #if AXIS8_ENCODER == AB
      case 8:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis8, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis8, CHANGE);
      break;
    #endif
    #if AXIS9_ENCODER == AB
      case 9:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis9, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis9, CHANGE);
      break;
    #endif
  }

  initialized = true;
}

int32_t Quadrature::read() {
  if (!initialized) { VF("WRN: Encoder Quadrature"); V(axis); VLF(" read(), not initialized!"); return 0; }

  int32_t count = 0;
  noInterrupts();
  count = this->count;
  interrupts();

  return count + origin;
}

void Quadrature::write(int32_t count) {
  if (!initialized) { VF("WRN: Encoder Quadrature"); V(axis); VLF(" write(), not initialized!"); return; }

  count -= origin;

  noInterrupts();
  this->count = count;
  interrupts();
}

// Phase 1: LLHH LLHH
// Phase 2: LHHL LHHL
// ...00 01 11 10 00 01 11 10 00 01 11 10...

ICACHE_RAM_ATTR void Quadrature::A(const int16_t pin) {
  stateA = digitalReadF(pin);

  uint8_t v = stateA*8 + stateB*4 + lastA*2 + lastB;
  static int16_t dir;
  switch (v) {
    case 0b0000: dir = 0; error = true; break; // skipped pulse use last dir (way too fast if this is happening)
    case 0b0001: dir = -1; break;
    case 0b0010: dir = 1; break;
    case 0b0011: warn = true; break;           // skipped pulse use last dir
    case 0b0100: dir = 1; break;
    case 0b0101: dir = 0; error = true; break; // skipped pulse use last dir (way too fast if this is happening)
    case 0b0110: warn = true; break;           // skipped pulse use last dir
    case 0b0111: dir = -1; break;
    case 0b1000: dir = -1; break;
    case 0b1001: warn = true; break;           // skipped pulse use last dir
    case 0b1010: dir = 0; error = true; break; // skipped pulse use last dir (way too fast if this is happening)
    case 0b1011: dir = 1; break;
    case 0b1100: warn = true; break;           // skipped pulse use last dir
    case 0b1101: dir = 1; break;
    case 0b1110: dir = -1; break;
    case 0b1111: dir = 0; error = true; break; // skipped pulse use last dir (way too fast if this is happening)
  }
  count += dir;
  
  lastA = stateA;
  lastB = stateB;
}

ICACHE_RAM_ATTR void Quadrature::B(const int16_t pin) {
  stateB = digitalReadF(pin);

  uint8_t v = stateA*8 + stateB*4 + lastA*2 + lastB;
  static int16_t dir;
  switch (v) {
    case 0b0000: dir = 0; error = true; break;
    case 0b0001: dir = -1; break;
    case 0b0010: dir = 1; break;
    case 0b0011: warn = true; break;
    case 0b0100: dir = 1; break;
    case 0b0101: dir = 0; error = true; break;
    case 0b0110: warn = true; break;
    case 0b0111: dir = -1; break;
    case 0b1000: dir = -1; break;
    case 0b1001: warn = true; break;
    case 0b1010: dir = 0; error = true; break;
    case 0b1011: dir = 1; break;
    case 0b1100: warn = true; break;
    case 0b1101: dir = 1; break;
    case 0b1110: dir = -1; break;
    case 0b1111: dir = 0; error = true; break;
  }
  count += dir;
  
  lastA = stateA;
  lastB = stateB;
}

#endif
