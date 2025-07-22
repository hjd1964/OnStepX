// A/B Quadrature encoders

#include "Quadrature.h"

#if AXIS1_ENCODER == AB || AXIS2_ENCODER == AB || AXIS3_ENCODER == AB || \
    AXIS4_ENCODER == AB || AXIS5_ENCODER == AB || AXIS6_ENCODER == AB || \
    AXIS7_ENCODER == AB || AXIS8_ENCODER == AB || AXIS9_ENCODER == AB

Quadrature *quadratureInstance[9];

// Macro to generate interrupt handlers for each axis
#define GENERATE_INTERRUPT_HANDLERS(axis_num) \
  IRAM_ATTR void quadrature_A_Axis##axis_num() { \
    if (quadratureInstance[axis_num-1]) quadratureInstance[axis_num-1]->handleInterrupt(); \
  } \
  IRAM_ATTR void quadrature_B_Axis##axis_num() { \
    if (quadratureInstance[axis_num-1]) quadratureInstance[axis_num-1]->handleInterrupt(); \
  }

#if AXIS1_ENCODER == AB
  GENERATE_INTERRUPT_HANDLERS(1)
#endif
#if AXIS2_ENCODER == AB
  GENERATE_INTERRUPT_HANDLERS(2)
#endif
#if AXIS3_ENCODER == AB
  GENERATE_INTERRUPT_HANDLERS(3)
#endif
#if AXIS4_ENCODER == AB
  GENERATE_INTERRUPT_HANDLERS(4)
#endif
#if AXIS5_ENCODER == AB
  GENERATE_INTERRUPT_HANDLERS(5)
#endif
#if AXIS6_ENCODER == AB
  GENERATE_INTERRUPT_HANDLERS(6)
#endif
#if AXIS7_ENCODER == AB
  GENERATE_INTERRUPT_HANDLERS(7)
#endif
#if AXIS8_ENCODER == AB
  GENERATE_INTERRUPT_HANDLERS(8)
#endif
#if AXIS9_ENCODER == AB
  GENERATE_INTERRUPT_HANDLERS(9)
#endif

Quadrature::Quadrature(int16_t APin, int16_t BPin, int16_t axis) {
  if (axis < 1 || axis > 9) return;

  // Check if axis is already assigned
  if (quadratureInstance[axis - 1] != nullptr) return;

  this->APin = APin;
  this->BPin = BPin;
  this->axis = axis;
  this->count = 0;
  this->origin = 0;
  this->error = 0;
  this->warn = 0;
  this->ready = false;

  quadratureInstance[this->axis - 1] = this;
}

bool Quadrature::init() {
  if (ready) return true;

  pinMode(APin, INPUT_PULLUP);
  pinMode(BPin, INPUT_PULLUP);

  // Read initial states
  stateA = digitalRead(APin);
  stateB = digitalRead(BPin);

  // Attach interrupts based on axis
  bool success = false;
  switch (axis) {
    #if AXIS1_ENCODER == AB
      case 1:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis1, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis1, CHANGE);
        success = true;
      break;
    #endif
    #if AXIS2_ENCODER == AB
      case 2:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis2, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis2, CHANGE);
        success = true;
      break;
    #endif
    #if AXIS3_ENCODER == AB
      case 3:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis3, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis3, CHANGE);
        success = true;
      break;
    #endif
    #if AXIS4_ENCODER == AB
      case 4:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis4, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis4, CHANGE);
        success = true;
      break;
    #endif
    #if AXIS5_ENCODER == AB
      case 5:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis5, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis5, CHANGE);
        success = true;
      break;
    #endif
    #if AXIS6_ENCODER == AB
      case 6:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis6, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis6, CHANGE);
        success = true;
      break;
    #endif
    #if AXIS7_ENCODER == AB
      case 7:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis7, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis7, CHANGE);
        success = true;
      break;
    #endif
    #if AXIS8_ENCODER == AB
      case 8:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis8, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis8, CHANGE);
        success = true;
      break;
    #endif
    #if AXIS9_ENCODER == AB
      case 9:
        attachInterrupt(digitalPinToInterrupt(APin), quadrature_A_Axis9, CHANGE);
        attachInterrupt(digitalPinToInterrupt(BPin), quadrature_B_Axis9, CHANGE);
        success = true;
      break;
    #endif
  }

  ready = success;
  return success;
}

int32_t Quadrature::read() {
  if (!ready) return 0;

  int32_t count = 0;
  noInterrupts();
  count = this->count;
  interrupts();

  return count + origin;
}

void Quadrature::write(int32_t count) {
  if (!ready) return;

  count -= origin;

  noInterrupts();
  this->count = count;
  interrupts();
}

// Reset error and warning counters
void Quadrature::clearErrors() {
  noInterrupts();
  this->error = 0;
  this->warn = 0;
  interrupts();
}

// Get error counts
uint32_t Quadrature::getErrors() {
  uint32_t errors = 0;
  noInterrupts();
  errors = this->error;
  interrupts();
  return errors;
}

uint32_t Quadrature::getWarnings() {
  uint32_t warnings = 0;
  noInterrupts();
  warnings = this->warn;
  interrupts();
  return warnings;
}

// Quadrature state transition table
// Phase A: LLHH LLHH LLHH ...
// Phase B: LHHL LHHL LHHL ...
// States:  00 01 11 10 00 01 11 10 ... (forward)
//         00 10 11 01 00 10 11 01 ... (reverse)

static const int8_t QUADRATURE_DECODE_TABLE[16] = {
  // Bits: [newA][newB][oldA][oldB]
   0, // 0000 - no change, error
  -1, // 0001 - 00->01, reverse
   1, // 0010 - 00->10, forward
   0, // 0011 - 00->11, invalid transition (warn)
   1, // 0100 - 01->00, forward
   0, // 0101 - no change, error
   0, // 0110 - 01->10, invalid transition (warn)
  -1, // 0111 - 01->11, reverse
  -1, // 1000 - 10->00, reverse
   0, // 1001 - 10->01, invalid transition (warn)
   0, // 1010 - no change, error
   1, // 1011 - 10->11, forward
   0, // 1100 - 11->00, invalid transition (warn)
   1, // 1101 - 11->01, forward
  -1, // 1110 - 11->10, reverse
   0  // 1111 - no change, error
};

ICACHE_RAM_ATTR void Quadrature::handleInterrupt() {
  // Read current pin states
  bool newA = digitalReadF(APin);
  bool newB = digitalReadF(BPin);

  // Only process if there's actually a state change
  if (newA == stateA && newB == stateB) {
    return; // No change, ignore spurious interrupt
  }

  // Create lookup index: [newA][newB][oldA][oldB]
  uint8_t index = (newA << 3) | (newB << 2) | (stateA << 1) | stateB;

  int8_t direction = QUADRATURE_DECODE_TABLE[index];

  if (direction == 0) {
    // Check if this is an invalid transition or no-change
    if (newA != stateA || newB != stateB) {
      // Invalid transition - likely noise or missed pulses
      if (index == 0b0011 || index == 0b0110 || index == 0b1001 || index == 0b1100) {
        warn++; // Two-step transition, recoverable
      } else {
        error++; // Impossible transition
      }
    } else {
      error++; // No change but interrupt fired
    }
  } else {
    // Valid transition, update count
    count += direction;
  }

  // Update state for next comparison
  stateA = newA;
  stateB = newB;

}

#endif