//--------------------------------------------------------------------------------------------------
// MEGA2560 hardware timers

// provides four 16 bit timers (Timer1, Timer3, Timer4, Timer5) with 16 bit software pre-scalers, running at 2MHz
// each timer configured as ~0 to 0.032 seconds (granularity of timer is 0.5uS) /8  pre-scaler
// note that timer use my collide with PWM or tone() depending on which pin is being controlled

#define TIMER_RATE_MHZ          2L    // Mega2560 motor timers run at 2 MHz
#define TIMER_RATE_16MHZ_TICKS  8L    // 16L/TIMER_RATE_MHZ, 8x slower than the default 16MHz "sub-micros"

#if defined(TASKS_HWTIMER1_ENABLE) || defined(TASKS_HWTIMER2_ENABLE) || defined(TASKS_HWTIMER3_ENABLE) || defined(TASKS_HWTIMER4_ENABLE)
  // prepare hw timer for interval in sub-microseconds (1/16us)
  volatile uint16_t _nextPeriod1 = 2000, _nextPeriod2 = 2000, _nextPeriod3 = 2000, _nextPeriod4 = 2000;
  volatile uint16_t _nextRep1 = 0, _nextRep2 = 0, _nextRep3 = 0, _nextRep4 = 0;
  void HAL_HWTIMER_PREPARE_PERIOD(uint8_t num, unsigned long period) {
    // maximum time is about 134 seconds for this design
    uint32_t counts, reps=0;
    if (period != 0 && period <= 2144000000) {
      if (period < 16) period = 16;   // minimum time is 1us
      period /= TIMER_RATE_16MHZ_TICKS;
      reps    = period/65536 + 1;
      counts  = period/reps - 1;      // has -1 since this is dropped right into a timer register
    } else counts = 2000;             // set for a 1ms period, stopped
  
    noInterrupts();
    switch (num) {
      case 1: _nextPeriod1 = counts; _nextRep1 = reps; break;
      case 2: _nextPeriod2 = counts; _nextRep2 = reps; break;
      case 3: _nextPeriod3 = counts; _nextRep3 = reps; break;
      case 4: _nextPeriod4 = counts; _nextRep4 = reps; break;
    }
    interrupts();
  }
#else
  void HAL_HWTIMER_PREPARE_PERIOD(uint8_t num, unsigned long period) { (void)(num); (void)(period); }
#endif

// start hw timers
#ifdef TASKS_HWTIMER1_ENABLE
  void (*HAL_HWTIMER1_FUN)() = NULL; // points to task/process callback function

  bool HAL_HWTIMER1_INIT(uint8_t priority) {
    // period 0.5... us per count 32.767 ms max
    (void)priority;
    noInterrupts();
    OCR1A  = 2000; // startup one millisecond
    TCCR1B = (1 << WGM12) | (1 << CS11);
    TCCR1A = 0;
    TIMSK1 = (1 << OCIE1A);
    interrupts();
    return true;
  }
  
  void HAL_HWTIMER1_DONE() {
    TIMSK1 = 0; // stop hw timer (clear register interrupt flag)
    HAL_HWTIMER1_FUN = NULL;
  }
  
  #define HAL_HWTIMER1_SET_PERIOD() OCR1A = _nextPeriod1
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
  void (*HAL_HWTIMER2_FUN)() = NULL; // points to task/process callback function

  bool HAL_HWTIMER2_INIT(uint8_t priority) {
    (void)priority;
    noInterrupts();
    OCR3A  = 2000; // startup one millisecond
    TCCR3B = (1 << WGM12) | (1 << CS11);
    TCCR3A = 0;
    TIMSK3 = (1 << OCIE3A);
    interrupts();
    return true;
  }

  void HAL_HWTIMER2_DONE() {
    TIMSK3 = 0; // stop hw timer (clear register interrupt flag)
    HAL_HWTIMER2_FUN = NULL;
  }

  #define HAL_HWTIMER2_SET_PERIOD() OCR3A = _nextPeriod2
  ISR(TIMER3_COMPA_vect) {
    TASKS_HWTIMER2_PROFILER_PREFIX;
    static uint16_t count = 0;
    if (_nextRep2 > 1) { count++; if (count%_nextRep2 != 0) goto done; }
    if (_nextRep2) HAL_HWTIMER2_FUN();
    HAL_HWTIMER2_SET_PERIOD();
    done: {}
    TASKS_HWTIMER2_PROFILER_SUFFIX;
  }
#endif

#ifdef TASKS_HWTIMER3_ENABLE
  void (*HAL_HWTIMER3_FUN)() = NULL; // points to task/process callback function

  bool HAL_HWTIMER3_INIT(uint8_t priority) {
    (void)priority;
    noInterrupts();
    OCR4A  = 2000; // startup one millisecond
    TCCR4B = (1 << WGM12) | (1 << CS11);
    TCCR4A = 0;
    TIMSK4 = (1 << OCIE4A);
    interrupts();
    return true;
  }
  
  void HAL_HWTIMER3_DONE() {
    TIMSK4 = 0; // stop hw timer (clear register interrupt flag)
    HAL_HWTIMER3_FUN = NULL;
  }

  #define HAL_HWTIMER3_SET_PERIOD() OCR4A = _nextPeriod3
  ISR(TIMER4_COMPA_vect) {
    TASKS_HWTIMER3_PROFILER_PREFIX;
    static uint16_t count = 0;
    if (_nextRep3 > 1) { count++; if (count%_nextRep3 != 0) goto done; }
    if (_nextRep3) HAL_HWTIMER3_FUN();
    HAL_HWTIMER3_SET_PERIOD();
    done: {}
    TASKS_HWTIMER3_PROFILER_SUFFIX;
  }
#endif

#ifdef TASKS_HWTIMER4_ENABLE
  void (*HAL_HWTIMER4_FUN)() = NULL; // points to task/process callback function

  bool HAL_HWTIMER4_INIT(uint8_t priority) {
    (void)priority;
    noInterrupts();
    OCR5A  = 2000; // startup one millisecond
    TCCR5B = (1 << WGM12) | (1 << CS11);
    TCCR5A = 0;
    TIMSK5 = (1 << OCIE5A);
    interrupts();
    return true;
  }

  void HAL_HWTIMER4_DONE() {
    TIMSK5 = 0; // stop hw timer (clear register interrupt flag)
    HAL_HWTIMER4_FUN = NULL;
  }

  #define HAL_HWTIMER4_SET_PERIOD() OCR5A = _nextPeriod4
  ISR(TIMER5_COMPA_vect) {
    TASKS_HWTIMER4_PROFILER_PREFIX;
    static uint16_t count = 0;
    if (_nextRep4 > 1) { count++; if (count%_nextRep4 != 0) goto done; }
    if (_nextRep4) HAL_HWTIMER4_FUN();
    HAL_HWTIMER4_SET_PERIOD();
    done: {}
    TASKS_HWTIMER4_PROFILER_SUFFIX;
  }
#endif
