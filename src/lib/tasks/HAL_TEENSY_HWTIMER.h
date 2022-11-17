//--------------------------------------------------------------------------------------------------
// Teensyduino hardware timers

// provides four 32 bit pit timers with 16 bit software pre-scalers
// each timer configured as ~0 to x seconds (granularity of timer is 0.062uS)
// timer use doesn't collide with PWM or tone() on these platforms

#define F_BUS2 16000000L                              // Pretend the timers are running at 0.062us/tick
#define TIMER_RATE_MHZ (F_BUS2/1000000.0)             // Teensy PIT timers run at F_BUS Hz but we use the interval timer library
#define TIMER_RATE_16MHZ_TICKS (16.0/TIMER_RATE_MHZ)  // 16.0/TIMER_RATE_MHZ for the default 16MHz "sub micros"

#if defined(TASKS_HWTIMER1_ENABLE) || defined(TASKS_HWTIMER2_ENABLE) || defined(TASKS_HWTIMER3_ENABLE) || defined(TASKS_HWTIMER4_ENABLE)
  // prepare hw timer for interval in sub-microseconds (1/16us)
  volatile double _nextPeriod1 = 1000, _nextPeriod2 = 1000, _nextPeriod3 = 1000, _nextPeriod4 = 1000;
  volatile uint16_t _nextRep1 = 0, _nextRep2 = 0, _nextRep3 = 0, _nextRep4 = 0;
  const double timerRate16MHzTicks TIMER_RATE_16MHZ_TICKS;
  void HAL_HWTIMER_PREPARE_PERIOD(uint8_t num, unsigned long period) {
    double counts;
    uint16_t reps = 0;
    if (period != 0 && period <= 2144000000) {
      if (period < 16) period = 16;         // minimum time is 1us
      double fperiod = period/timerRate16MHzTicks;
      reps           = fperiod/4194304.0 + 1.0;
      counts         = round(fperiod/reps)/16.0;
    } else counts = 1000;                   // set for a 1ms period, stopped
  
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
  IntervalTimer itimer1;

  void (*HAL_HWTIMER1_FUN)() = NULL; // points to task/process callback function
  void HAL_HWTIMER1_WRAPPER();       // forward definition of the timer ISR

  bool HAL_HWTIMER1_INIT(uint8_t priority) {
    itimer1.priority(priority);
    return itimer1.begin(HAL_HWTIMER1_WRAPPER, 1000); // startup one millisecond
  }

  void HAL_HWTIMER1_DONE() {
    HAL_HWTIMER1_FUN = NULL;
    itimer1.end();
  }

  #define HAL_HWTIMER1_SET_PERIOD() itimer1.update(_nextPeriod1)
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
    return itimer2.begin(HAL_HWTIMER2_WRAPPER, 1000); // startup one millisecond
  }

  void HAL_HWTIMER2_DONE() {
    HAL_HWTIMER2_FUN = NULL;
    itimer2.end();
  }
  
  #define HAL_HWTIMER2_SET_PERIOD() itimer2.update(_nextPeriod2)
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
    return itimer3.begin(HAL_HWTIMER3_WRAPPER, 1000); // startup one millisecond
  }

  void HAL_HWTIMER3_DONE() {
    HAL_HWTIMER3_FUN = NULL;
    itimer3.end();
  }
  
  #define HAL_HWTIMER3_SET_PERIOD() itimer3.update(_nextPeriod3)
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
    return itimer4.begin(HAL_HWTIMER4_WRAPPER, 1000); // startup one millisecond
  }

  void HAL_HWTIMER4_DONE() {
    HAL_HWTIMER4_FUN = NULL;
    itimer4.end();
  }

  #define HAL_HWTIMER4_SET_PERIOD() itimer4.update(_nextPeriod4)
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
