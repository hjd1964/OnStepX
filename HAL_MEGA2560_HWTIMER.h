//--------------------------------------------------------------------------------------------------
// MEGA2560 hardware timers

// provides four 16 bit timers (Timer1, Timer3, Timer4, Timer5) with 16 bit software pre-scalers, running at 2MHz
// each timer configured as ~0 to 0.032 seconds (31 calls per second minimum, granularity of timer is 0.5uS) /8  pre-scaler

#define TIMER_RATE_MHZ 2L           // Mega2560 motor timers run at 2 MHz
#define TIMER_RATE_16MHZ_TICKS 8L   // 16L/TIMER_RATE_MHZ, 8x slower than the default 16MHz

// start hw timers
bool HAL_HWTIMER1_INIT(uint8_t priority) {
#ifdef TASKS_HWTIMER1_ENABLE
  noInterrupts();
  TCCR1B = (1 << WGM12) | (1 << CS11);
  TCCR1A = 0;
  TIMSK1 = (1 << OCIE1A);
  interrupts();
  return true;
#else
  return false;
#endif
}
bool HAL_HWTIMER2_INIT(uint8_t priority) {
#ifdef TASKS_HWTIMER2_ENABLE
  noInterrupts();
  TCCR3B = (1 << WGM12) | (1 << CS11);
  TCCR3A = 0;
  TIMSK3 = (1 << OCIE3A);
  interrupts();
  return true;
#else
  return false;
#endif
}
bool HAL_HWTIMER3_INIT(uint8_t priority) {
#ifdef TASKS_HWTIMER3_ENABLE
  noInterrupts();
  TCCR4B = (1 << WGM12) | (1 << CS11);
  TCCR4A = 0;
  TIMSK4 = (1 << OCIE4A);
  interrupts();
  return true;
#else
  return false;
#endif
}
bool HAL_HWTIMER4_INIT(uint8_t priority) {
#ifdef TASKS_HWTIMER4_ENABLE
  noInterrupts();
  TCCR5B = (1 << WGM12) | (1 << CS11);
  TCCR5A = 0;
  TIMSK5 = (1 << OCIE5A);
  interrupts();
  return true;
#else
  return false;
#endif
}

// pointers to event callbacks
void (*HAL_HWTIMER1_FUN)() = NULL;
void (*HAL_HWTIMER2_FUN)() = NULL;
void (*HAL_HWTIMER3_FUN)() = NULL;
void (*HAL_HWTIMER4_FUN)() = NULL;

// stop hw timers (clear register interrupt flag)
void HAL_HWTIMER1_DONE() {
#ifdef TASKS_HWTIMER1_ENABLE
  HAL_HWTIMER1_FUN = NULL;
  TIMSK1 = 0;
#endif
}
void HAL_HWTIMER2_DONE() {
#ifdef TASKS_HWTIMER2_ENABLE
  HAL_HWTIMER2_FUN = NULL;
  TIMSK3 = 0;
#endif
}
void HAL_HWTIMER3_DONE() {
#ifdef TASKS_HWTIMER3_ENABLE
  HAL_HWTIMER3_FUN = NULL;
  TIMSK4 = 0;
#endif
}
void HAL_HWTIMER4_DONE() {
#ifdef TASKS_HWTIMER4_ENABLE
  HAL_HWTIMER4_FUN = NULL;
  TIMSK5 = 0;
#endif
}

// prepare hw timer for interval in sub-microseconds (1/16us)
volatile uint32_t _nextPeriod1, _nextPeriod2, _nextPeriod3, _nextPeriod4;
volatile uint16_t _nextRep1, _nextRep2, _nextRep3, _nextRep4;
void HAL_HWTIMER_PREPARE_PERIOD(uint8_t num, unsigned long period) {
//  Serial.print("Sidereal -> "); Serial.println(period);
  // maximum time is about 134 seconds for this design
  uint32_t i, reps=0;
  if (period != 0 && period <= 2144000000) {
    if (period<16) period=16;      // minimum time is 1us
    period/=TIMER_RATE_16MHZ_TICKS;
    reps = (period/65536)+1;
    i = period/reps-1;             // has -1 since this is dropped right into a timer register
  } else i=20000;                  // set for a 10ms period, stopped

  noInterrupts();
  switch (num) {
    case 1: _nextPeriod1=i; _nextRep1=reps; break;
    case 2: _nextPeriod2=i; _nextRep2=reps; break;
    case 3: _nextPeriod3=i; _nextRep3=reps; break;
    case 4: _nextPeriod4=i; _nextRep4=reps; break;
  }
  interrupts();
}

// fast routines to adopt new rates, must work from within the timer ISR
#ifdef TASKS_HWTIMER1_ENABLE
  #define HAL_HWTIMER1_SET_PERIOD() (OCR1A=_nextPeriod1)
#else
  #define HAL_HWTIMER1_SET_PERIOD()
#endif
#ifdef TASKS_HWTIMER2_ENABLE
  #define HAL_HWTIMER2_SET_PERIOD() (OCR3A=_nextPeriod2)
#else
  #define HAL_HWTIMER2_SET_PERIOD()
#endif
#ifdef TASKS_HWTIMER3_ENABLE
  #define HAL_HWTIMER3_SET_PERIOD() (OCR4A=_nextPeriod3)
#else
  #define HAL_HWTIMER3_SET_PERIOD()
#endif
#ifdef TASKS_HWTIMER4_ENABLE
  #define HAL_HWTIMER4_SET_PERIOD() (OCR5A=_nextPeriod4)
#else
  #define HAL_HWTIMER4_SET_PERIOD()
#endif

// interrupt service routine wrappers
#ifdef TASKS_HWTIMER1_ENABLE
ISR(TIMER1_COMPA_vect) {
  static uint16_t count = 0;
  if (_nextRep1 > 1) { count++; if (count%_nextRep1 != 0) goto done; }
  if (_nextRep1) HAL_HWTIMER1_FUN();
  HAL_HWTIMER1_SET_PERIOD();
  done: {}
}
#endif
#ifdef TASKS_HWTIMER2_ENABLE
ISR(TIMER3_COMPA_vect) {
  static uint16_t count = 0;
  if (_nextRep2 > 1) { count++; if (count%_nextRep2 != 0) goto done; }
  if (_nextRep2) HAL_HWTIMER2_FUN();
  HAL_HWTIMER2_SET_PERIOD();
  done: {}
}
#endif
#ifdef TASKS_HWTIMER3_ENABLE
ISR(TIMER4_COMPA_vect) {
  static uint16_t count = 0;
  if (_nextRep3 > 1) { count++; if (count%_nextRep3 != 0) goto done; }
  if (_nextRep3) HAL_HWTIMER3_FUN();
  HAL_HWTIMER3_SET_PERIOD();
  done: {}
}
#endif
#ifdef TASKS_HWTIMER4_ENABLE
ISR(TIMER5_COMPA_vect) {
  static uint16_t count = 0;
  if (_nextRep4 > 1) { count++; if (count%_nextRep4 != 0) goto done; }
  if (_nextRep4) HAL_HWTIMER4_FUN();
  HAL_HWTIMER4_SET_PERIOD();
  done: {}
}
#endif
