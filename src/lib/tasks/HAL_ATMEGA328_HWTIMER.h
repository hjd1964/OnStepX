//--------------------------------------------------------------------------------------------------
// ATMEGA328 hardware timers

// provides one 16 bit timer (Timer1) with 16 bit software pre-scaler, running at 2MHz
// timer configured as ~0 to 0.032 seconds (granularity of timer is 0.5uS) /8  pre-scaler

#define TIMER_RATE_MHZ          2L    // AtMega328 motor timers run at 2 MHz
#define TIMER_RATE_16MHZ_TICKS  8L    // 16L/TIMER_RATE_MHZ, 8x slower than the default 16MHz "sub-micros"

#if defined(TASKS_HWTIMER1_ENABLE) || defined(TASKS_HWTIMER2_ENABLE) || defined(TASKS_HWTIMER3_ENABLE) || defined(TASKS_HWTIMER4_ENABLE)
  // prepare hw timer for interval in sub-microseconds (1/16us)
  volatile uint16_t _nextPeriod1 = 2000, _nextPeriod2 = 2000, _nextPeriod3 = 2000, _nextPeriod4 = 2000;
  volatile uint16_t _nextRep1 = 0, _nextRep2 = 0, _nextRep3 = 0, _nextRep4 = 0;
  void HAL_HWTIMER_PREPARE_PERIOD(uint8_t num, unsigned long period) {
    // maximum time is about 134 seconds for this design
    uint32_t counts, reps = 0;
    if (period != 0 && period <= 2144000000) {
      if (period < 16) period = 16;   // minimum time is 1us
      period /= TIMER_RATE_16MHZ_TICKS;
      reps    = period/65536 + 1;
      counts  = period/reps - 1;      // has -1 since this is dropped right into a timer register
    } else counts = 2000;             // set for a 1ms period, stopped
  
    noInterrupts();
    _nextPeriod1 = counts; _nextRep1 = reps;
    interrupts();
  }
#else
  void HAL_HWTIMER_PREPARE_PERIOD(uint8_t num, unsigned long period) { (void)(num); (void)(period); }
#endif

#ifdef TASKS_HWTIMER1_ENABLE
  void (*HAL_HWTIMER1_FUN)() = NULL; // points to task/process callback function

  bool HAL_HWTIMER1_INIT(uint8_t priority) {
    noInterrupts();
    OCR1A  = 2000; // startup one millisecond
    TCCR1B = (1 << WGM12) | (1 << CS11);
    TCCR1A = 0;
    TIMSK1 = (1 << OCIE1A);
    interrupts();
    return true;
  }

  void HAL_HWTIMER1_DONE() {
    HAL_HWTIMER1_FUN = NULL;
    TIMSK1 = 0;
  }

  #define HAL_HWTIMER1_SET_PERIOD() (OCR1A = _nextPeriod1)
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

#ifdef TASKS_HWTIMER2_ENABLE
  void (*HAL_HWTIMER2_FUN)() = NULL;
  #define HAL_HWTIMER2_SET_PERIOD()
  bool HAL_HWTIMER2_INIT(uint8_t priority) { return false; }
  void HAL_HWTIMER2_DONE() { }
#endif

#ifdef TASKS_HWTIMER3_ENABLE
  void (*HAL_HWTIMER3_FUN)() = NULL;
  #define HAL_HWTIMER3_SET_PERIOD()
  bool HAL_HWTIMER3_INIT(uint8_t priority) { return false;}
  void HAL_HWTIMER3_DONE() { }
#endif

#ifdef TASKS_HWTIMER4_ENABLE
  void (*HAL_HWTIMER4_FUN)() = NULL;
  #define HAL_HWTIMER4_SET_PERIOD()
  bool HAL_HWTIMER4_INIT(uint8_t priority) { return false; }
  void HAL_HWTIMER4_DONE() { }
#endif