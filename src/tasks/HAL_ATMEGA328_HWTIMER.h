//--------------------------------------------------------------------------------------------------
// ATMEGA328 hardware timers

// provides one 16 bit timer (Timer1) with 16 bit software pre-scaler, running at 2MHz
// timer configured as ~0 to 0.032 seconds (31 calls per second minimum, granularity of timer is 0.5uS) /8  pre-scaler

#define TIMER_RATE_MHZ 2L           // AtMega328 motor timers run at 2 MHz
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
bool HAL_HWTIMER2_INIT(uint8_t priority) { return false; }
bool HAL_HWTIMER3_INIT(uint8_t priority) { return false; }
bool HAL_HWTIMER4_INIT(uint8_t priority) { return false; }

// pointers to event callbacks
void (*HAL_HWTIMER1_FUN)() = NULL;
void (*HAL_HWTIMER2_FUN)() = NULL;
void (*HAL_HWTIMER3_FUN)() = NULL;
void (*HAL_HWTIMER4_FUN)() = NULL;

// stop hw timers
void HAL_HWTIMER1_DONE() {
#ifdef TASKS_HWTIMER1_ENABLE
  HAL_HWTIMER1_FUN = NULL;
  TIMSK1 = 0;
#endif
}
void HAL_HWTIMER2_DONE() { }
void HAL_HWTIMER3_DONE() { }
void HAL_HWTIMER4_DONE() { }

// prepare hw timer for interval in sub-microseconds (1/16us)
volatile uint32_t _nextPeriod1;
volatile uint16_t _nextRep1;
void HAL_HWTIMER_PREPARE_PERIOD(uint8_t num, unsigned long period) {
#ifdef TASKS_HWTIMER1_ENABLE
  // maximum time is about 134 seconds for this design
  uint32_t i, reps=0;
  if (period != 0 && period <= 2144000000) {
    if (period<16) period=16;      // minimum time is 1us
    period/=TIMER_RATE_16MHZ_TICKS;
    reps = (period/65536)+1;
    i = period/reps-1;             // has -1 since this is dropped right into a timer register
  } else i=20000;                  // set for a 10ms period, stopped

  noInterrupts();
  _nextPeriod1=i; _nextRep1=reps;
  interrupts();
#endif
}

// fast routines to adopt new rates, must work from within the timer ISR
#ifdef TASKS_HWTIMER1_ENABLE
  #define HAL_HWTIMER1_SET_PERIOD() (OCR1A=_nextPeriod1)
#else
  #define HAL_HWTIMER1_SET_PERIOD()
#endif
#define HAL_HWTIMER2_SET_PERIOD()
#define HAL_HWTIMER3_SET_PERIOD()
#define HAL_HWTIMER4_SET_PERIOD()

// interrupt service routine wrappers
#ifdef TASKS_HWTIMER1_ENABLE
ISR(TIMER1_COMPA_vect) {
  TASKS_HWTIMER1_PROFILER_PREFIX;
  static uint16_t count = 0;
  if (_nextRep1 > 1) { count++; if (count%_nextRep1 != 0) goto done; }
  if (_nextRep1) HAL_HWTIMER1_FUN();
  HAL_HWTIMER1_SET_PERIOD();
  done: {}
  TASKS_HWTIMER1_PROFILER_SUFFIX;
}
#endif
