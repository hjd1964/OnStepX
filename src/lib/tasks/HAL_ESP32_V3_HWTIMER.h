//--------------------------------------------------------------------------------------------------
// ESP32 hardware timers

// mux to block code from running simultaniously
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// provides two 32 bit interval timers with 16 bit software pre-scalers, running at 16MHz
// each timer configured as ~0 to x seconds (granularity of timer is 0.062uS)
// timer use doesn't collide with PWM or tone() on these platforms

// notes for future improvements:
// the ESP32 timers flexible enough that we can get rid of the software pre-scalers
// also, we can more directly access the timer objects for example:
// #define HAL_HWTIMER1_SET_PERIOD() itimer1->dev->alarm_low = _nextPeriod1

#define TIMER_RATE_MHZ 16L                           // ESP32 motor timers run at 16MHz so use full resolution
#define TIMER_RATE_16MHZ_TICKS 1L                    // 16L/TIMER_RATE_MHZ, for the default 16MHz "sub-micros" (16MHz)

#if defined(TASKS_HWTIMER1_ENABLE) || defined(TASKS_HWTIMER2_ENABLE) || defined(TASKS_HWTIMER3_ENABLE) || defined(TASKS_HWTIMER4_ENABLE)
  // prepare hw timer for interval in sub-microseconds (1/16us)
  volatile uint32_t _nextPeriod1 = 16000, _nextPeriod2 = 16000, _nextPeriod3 = 16000, _nextPeriod4 = 16000;
  volatile uint16_t _nextRep1 = 0, _nextRep2 = 0, _nextRep3 = 0, _nextRep4 = 0;
  void HAL_HWTIMER_PREPARE_PERIOD(uint8_t num, unsigned long period) {
    // maximum time is about 134 seconds for this design
    uint32_t counts, reps = 0;
    if (period != 0 && period <= 2144000000) {
      if (period < 16) period = 16;   // minimum time is 1us
      period /= TIMER_RATE_16MHZ_TICKS;
      reps    = period/4194304UL + 1;
      counts  = period/reps;
    } else counts = 16000;            // set for a 1ms period, stopped
  
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
  hw_timer_t *itimer1 = NULL;

  void (*HAL_HWTIMER1_FUN)() = NULL; // points to task/process callback function
  void HAL_HWTIMER1_WRAPPER();

  bool HAL_HWTIMER1_INIT(uint8_t priority) {
    itimer1 = timerBegin(16000000); // ESP32 timer frequency is 16MHz
    timerAttachInterrupt(itimer1, &HAL_HWTIMER1_WRAPPER);
    timerAlarm(itimer1, 1000*16, true, 0); // startup one millisecond
    return true;
  }

  void HAL_HWTIMER1_DONE() {
    HAL_HWTIMER1_FUN = NULL;
    timerEnd(itimer1);
  }

  #define HAL_HWTIMER1_SET_PERIOD() timerAlarm(itimer1, _nextPeriod1, true, 0)
  IRAM_ATTR void HAL_HWTIMER1_WRAPPER() {
    portENTER_CRITICAL_ISR(&timerMux);
    TASKS_HWTIMER1_PROFILER_PREFIX;
    static uint16_t count = 0;
    if (_nextRep1 > 1) { count++; if (count % _nextRep1 != 0) goto done; }
    if (_nextRep1) HAL_HWTIMER1_FUN();
    HAL_HWTIMER1_SET_PERIOD();
    done: {}
    TASKS_HWTIMER1_PROFILER_SUFFIX;
    portEXIT_CRITICAL_ISR ( &timerMux );
  }
#endif

#ifdef TASKS_HWTIMER2_ENABLE
  hw_timer_t *itimer2 = NULL;

  void (*HAL_HWTIMER2_FUN)() = NULL; // points to task/process callback function
  void HAL_HWTIMER2_WRAPPER();

  bool HAL_HWTIMER2_INIT(uint8_t priority) {
    itimer2 = timerBegin(16000000); // ESP32 timer frequency is 16MHz
    timerAttachInterrupt(itimer2, &HAL_HWTIMER2_WRAPPER);
    timerAlarm(itimer2, 1000*16, true, 0); // startup one millisecond
    return true;
  }

  void HAL_HWTIMER2_DONE() {
    HAL_HWTIMER2_FUN = NULL;
    timerEnd(itimer2);
  }
  
  #define HAL_HWTIMER2_SET_PERIOD() timerAlarm(itimer2, _nextPeriod2, true, 0)
  IRAM_ATTR void HAL_HWTIMER2_WRAPPER() {
    portENTER_CRITICAL_ISR(&timerMux);
    TASKS_HWTIMER2_PROFILER_PREFIX;
    static uint16_t count = 0;
    if (_nextRep2 > 1) { count++; if (count % _nextRep2 != 0) goto done; }
    if (_nextRep2) HAL_HWTIMER2_FUN();
    HAL_HWTIMER2_SET_PERIOD();
    done: {}
    TASKS_HWTIMER2_PROFILER_SUFFIX;
    portEXIT_CRITICAL_ISR ( &timerMux );
  }
#endif

#ifdef TASKS_HWTIMER3_ENABLE
  hw_timer_t *itimer3 = NULL;

  void (*HAL_HWTIMER3_FUN)() = NULL; // points to task/process callback function
  void HAL_HWTIMER3_WRAPPER();

  bool HAL_HWTIMER3_INIT(uint8_t priority) {
    itimer3 = timerBegin(16000000); // ESP32 timer frequency is 16MHz
    timerAttachInterrupt(itimer3, &HAL_HWTIMER3_WRAPPER);
    timerAlarm(itimer3, 1000*16, true, 0); // startup one millisecond
    return true;
  }

  void HAL_HWTIMER3_DONE() {
    HAL_HWTIMER3_FUN = NULL;
    timerEnd(itimer3);
  }
  
  #define HAL_HWTIMER3_SET_PERIOD() timerAlarm(itimer3, _nextPeriod3, true, 0)
  IRAM_ATTR void HAL_HWTIMER3_WRAPPER() {
    portENTER_CRITICAL_ISR(&timerMux);
    TASKS_HWTIMER3_PROFILER_PREFIX;
    static uint16_t count = 0;
    if (_nextRep3 > 1) { count++; if (count % _nextRep3 != 0) goto done; }
    if (_nextRep3) HAL_HWTIMER3_FUN();
    HAL_HWTIMER3_SET_PERIOD();
    done: {}
    TASKS_HWTIMER3_PROFILER_SUFFIX;
    portEXIT_CRITICAL_ISR ( &timerMux );
  }
#endif

#ifdef TASKS_HWTIMER4_ENABLE
  hw_timer_t *itimer4 = NULL;

  void (*HAL_HWTIMER4_FUN)() = NULL; // points to task/process callback function
  void HAL_HWTIMER4_WRAPPER();

  bool HAL_HWTIMER4_INIT(uint8_t priority) {
    itimer4 = timerBegin(16000000); // ESP32 timer frequency is 16MHz
    timerAttachInterrupt(itimer4, &HAL_HWTIMER4_WRAPPER);
    timerAlarm(itimer4, 1000*16, true, 0); // startup one millisecond
    return true;
  }

  void HAL_HWTIMER4_DONE() {
    HAL_HWTIMER4_FUN = NULL;
    timerEnd(itimer4);
  }

  #define HAL_HWTIMER4_SET_PERIOD() timerAlarm(itimer4, _nextPeriod4, true, 0)
  IRAM_ATTR void HAL_HWTIMER4_WRAPPER() {
    portENTER_CRITICAL_ISR(&timerMux);
    TASKS_HWTIMER4_PROFILER_PREFIX;
    static uint16_t count = 0;
    if (_nextRep4 > 1) { count++; if (count % _nextRep4 != 0) goto done; }
    if (_nextRep4) HAL_HWTIMER4_FUN();
    HAL_HWTIMER4_SET_PERIOD();
    done: {}
    TASKS_HWTIMER4_PROFILER_SUFFIX;
    portEXIT_CRITICAL_ISR ( &timerMux );
  }
#endif

// disable hardware timer based tasks by clearing interrupt flag(s)
void timerAlarmsDisable() {
#ifdef TASKS_HWTIMER1_ENABLE
  timerStop(itimer1);
#endif
#ifdef TASKS_HWTIMER2_ENABLE
  timerStop(itimer2);
#endif
#ifdef TASKS_HWTIMER3_ENABLE
  timerStop(itimer3);
#endif
#ifdef TASKS_HWTIMER4_ENABLE
  timerStop(itimer4);
#endif
}

// enable hardware timer based tasks by setting interrupt flag(s)
void timerAlarmsEnable()  {
#ifdef TASKS_HWTIMER1_ENABLE
  timerStart(itimer1);
#endif
#ifdef TASKS_HWTIMER2_ENABLE
  timerStart(itimer2);
#endif
#ifdef TASKS_HWTIMER3_ENABLE
  timerStart(itimer3);
#endif
#ifdef TASKS_HWTIMER4_ENABLE
  timerStart(itimer4);
#endif
}
