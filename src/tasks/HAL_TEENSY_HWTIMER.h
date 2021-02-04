//--------------------------------------------------------------------------------------------------
// Teensyduino hardware timers

// provides four 32 bit pit timers with 16 bit software pre-scalers, running at 16MHz
// each timer configured as ~0 to x seconds (granularity of timer is 0.062uS)
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)  
  #define F_BUS 16000000L                            // F_BUS isn't defined for the T4.0, we force the timers to run at 16MHZ
#endif
#define TIMER_RATE_MHZ (F_BUS/1000000.0)             // Teensy motor timers run at F_BUS Hz so use full resolution
#define TIMER_RATE_16MHZ_TICKS (16.0/TIMER_RATE_MHZ) // 16.0/TIMER_RATE_MHZ for the default 16MHz "sub micros"

#if defined(TASKS_HWTIMER1_ENABLE) || defined(TASKS_HWTIMER2_ENABLE) || defined(TASKS_HWTIMER3_ENABLE) || defined(TASKS_HWTIMER4_ENABLE)
  // prepare hw timer for interval in sub-microseconds (1/16us)
  volatile uint32_t _nextPeriod1, _nextPeriod2, _nextPeriod3, _nextPeriod4;
  volatile uint16_t _nextRep1, _nextRep2, _nextRep3, _nextRep4;
  const double timerRate16MHzTicks TIMER_RATE_16MHZ_TICKS;
  void HAL_HWTIMER_PREPARE_PERIOD(uint8_t num, unsigned long period) {
    uint32_t counts, reps=0;
    if (period != 0 && period <= 2144000000) {
      if (period < 16) period = 16;         // minimum time is 1us
      double fperiod = period/timerRate16MHzTicks;
      reps           = fperiod/4194304.0 + 1.0;
      counts         = fperiod/reps - 1.0; // has -1 since this is dropped right into a timer register
    } else counts = 160000;                 // set for a 10ms period, stopped
  
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
  void HAL_HWTIMER_PREPARE_PERIOD(uint8_t num, unsigned long period) {}
#endif

#ifdef TASKS_HWTIMER1_ENABLE
  IntervalTimer itimer1;

  void (*HAL_HWTIMER1_FUN)() = NULL; // points to task/process callback function
  void HAL_HWTIMER1_WRAPPER();       // forward definition of the timer ISR

  bool HAL_HWTIMER1_INIT(uint8_t priority) {
    itimer1.priority(priority);
    return itimer1.begin(HAL_HWTIMER1_WRAPPER, (float)128 * 0.0625);
  }

  void HAL_HWTIMER1_DONE() {
    HAL_HWTIMER1_FUN = NULL;
    itimer1.end();
  }

#if defined(__IMXRT1052__) || defined(__IMXRT1062__)
  #define HAL_HWTIMER1_SET_PERIOD() itimer1.update((double)_nextPeriod1 * 0.0625)
#else
  #define HAL_HWTIMER1_SET_PERIOD() (PIT_LDVAL1 = _nextPeriod1)
#endif
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
  IntervalTimer itimer2;

  void (*HAL_HWTIMER2_FUN)() = NULL; // points to task/process callback function
  void HAL_HWTIMER2_WRAPPER();       // forward definition of the timer ISR

  bool HAL_HWTIMER2_INIT(uint8_t priority) {
    itimer2.priority(priority);
    return itimer2.begin(HAL_HWTIMER2_WRAPPER, (float)128 * 0.0625);
  }

  void HAL_HWTIMER2_DONE() {
    HAL_HWTIMER2_FUN = NULL;
    itimer2.end();
  }
  
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)
  #define HAL_HWTIMER2_SET_PERIOD() itimer2.update((double)_nextPeriod2 * 0.0625)
#else
  #define HAL_HWTIMER2_SET_PERIOD() (PIT_LDVAL2 = _nextPeriod2)
#endif
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
  IntervalTimer itimer3;

  void (*HAL_HWTIMER3_FUN)() = NULL; // points to task/process callback function
  void HAL_HWTIMER3_WRAPPER();       // forward definition of the timer ISR

  bool HAL_HWTIMER3_INIT(uint8_t priority) {
    itimer3.priority(priority);
    return itimer3.begin(HAL_HWTIMER3_WRAPPER, (float)128 * 0.0625);
  }

  void HAL_HWTIMER3_DONE() {
    HAL_HWTIMER3_FUN = NULL;
    itimer3.end();
  }
  
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)
  #define HAL_HWTIMER3_SET_PERIOD() itimer3.update((double)_nextPeriod3 * 0.0625)
#else
  #define HAL_HWTIMER3_SET_PERIOD() (PIT_LDVAL3 = _nextPeriod3)
#endif
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
  IntervalTimer itimer4;

  void (*HAL_HWTIMER4_FUN)() = NULL; // points to task/process callback function
  void HAL_HWTIMER4_WRAPPER();       // forward definition of the timer ISR

  bool HAL_HWTIMER4_INIT(uint8_t priority) {
    itimer4.priority(priority);
    return itimer4.begin(HAL_HWTIMER4_WRAPPER, (float)128 * 0.0625);
  }

  void HAL_HWTIMER4_DONE() {
    HAL_HWTIMER4_FUN = NULL;
    itimer4.end();
  }

#if defined(__IMXRT1052__) || defined(__IMXRT1062__)
  #define HAL_HWTIMER4_SET_PERIOD() itimer4.update((double)_nextPeriod4 * 0.0625)
#else
  #define HAL_HWTIMER4_SET_PERIOD() (PIT_LDVAL4 = _nextPeriod4)
#endif
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
