//--------------------------------------------------------------------------------------------------
// Empty hardware timers

// minimum structure

                                      // a useful example for 16bit counter/timer hardware to
#define TIMER_RATE_MHZ          4L    // allow sufficient resolution (0.25us) and range (134 seconds)
#define TIMER_RATE_16MHZ_TICKS  4L    // timer hardware 4x slower than the default 16MHz "sub-micros" (4MHz)

#if defined(TASKS_HWTIMER1_ENABLE) || defined(TASKS_HWTIMER2_ENABLE) || defined(TASKS_HWTIMER3_ENABLE) || defined(TASKS_HWTIMER4_ENABLE)
  // prepare hw timer for interval in sub-microseconds (1/16us)
  volatile uint16_t _nextPeriod1, _nextPeriod2, _nextPeriod3, _nextPeriod4;
  volatile uint16_t _nextRep1, _nextRep2, _nextRep3, _nextRep4;
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

#ifdef TASKS_HWTIMER1_ENABLE
  void (*HAL_HWTIMER1_FUN)() = NULL; // points to task/process callback function
  void HAL_HWTIMER1_WRAPPER(void);   // forward definition of the timer ISR

  bool HAL_HWTIMER1_INIT(uint8_t priority) {
    // <--- code to init/start timer goes here
    return true;
  }
  
  void HAL_HWTIMER1_DONE() {
    // <--- code to stop timer goes here
    HAL_HWTIMER1_FUN = NULL;
  }

  #define HAL_HWTIMER1_SET_PERIOD() {} // <--- code to set timer period goes here
  void HAL_HWTIMER1_WRAPPER() {
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
  void HAL_HWTIMER2_WRAPPER(void);   // forward definition of the timer ISR
  
  bool HAL_HWTIMER2_INIT(uint8_t priority) {
    // <--- code to init/start timer goes here
    return true;
  }

  void HAL_HWTIMER2_DONE() {
    // <--- code to stop timer goes here
    HAL_HWTIMER2_FUN = NULL;
  }

  #define HAL_HWTIMER2_SET_PERIOD() {} // <--- code to set timer period goes here
  void HAL_HWTIMER2_WRAPPER() {
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
  void HAL_HWTIMER3_WRAPPER(void);   // forward definition of the timer ISR

  bool HAL_HWTIMER3_INIT(uint8_t priority) {
    // <--- code to init/start timer goes here
    return true;
  }
  
  void HAL_HWTIMER3_DONE() {
    // <--- code to stop timer goes here
    HAL_HWTIMER3_FUN = NULL;
  }

  #define HAL_HWTIMER3_SET_PERIOD() {} // <--- code to set timer period goes here
  void HAL_HWTIMER3_WRAPPER() {
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
  void HAL_HWTIMER4_WRAPPER(void);   // forward definition of the timer ISR

  bool HAL_HWTIMER4_INIT(uint8_t priority) {
    // init code goes here
    return false;
  }

  void HAL_HWTIMER4_DONE() {
    // <--- code to stop timer goes here
    HAL_HWTIMER4_FUN = NULL;
  }

  #define HAL_HWTIMER4_SET_PERIOD() {} // <--- code to set timer period goes here
  void HAL_HWTIMER4_WRAPPER() {
    TASKS_HWTIMER4_PROFILER_PREFIX;
    static uint16_t count = 0;
    if (_nextRep4 > 1) { count++; if (count%_nextRep4 != 0) goto done; }
    if (_nextRep4) HAL_HWTIMER4_FUN();
    HAL_HWTIMER4_SET_PERIOD();
    done: {}
    TASKS_HWTIMER4_PROFILER_SUFFIX;
  }
#endif
