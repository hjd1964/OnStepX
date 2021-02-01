//--------------------------------------------------------------------------------------------------
// Teensyduino hardware timers

// provides four 32 bit pit timers with 16 bit software pre-scalers, running at 16MHz

#define TIMER_RATE_MHZ (F_BUS/1000000.0)                 // Teensy motor timers run at F_BUS Hz so use full resolution
#define TIMER_RATE_16MHZ_TICKS (16.0/TIMER_RATE_MHZ)     // 16.0/TIMER_RATE_MHZ

// start hw timers
#ifdef TASKS_HWTIMER1_ENABLE
  IntervalTimer itimer1;
  void HAL_HWTIMER1_WRAPPER();
  bool HAL_HWTIMER1_INIT(uint8_t priority) {
    itimer1.priority(priority);
    return itimer1.begin(HAL_HWTIMER1_WRAPPER, (float)128 * 0.0625);
  }
#else
  bool HAL_HWTIMER1_INIT(uint8_t priority) { return false; }
#endif
#ifdef TASKS_HWTIMER2_ENABLE
  IntervalTimer itimer2;
  void HAL_HWTIMER2_WRAPPER();
  bool HAL_HWTIMER2_INIT(uint8_t priority) {
    itimer2.priority(priority);
    return itimer2.begin(HAL_HWTIMER2_WRAPPER, (float)128 * 0.0625);
  }
#else
  bool HAL_HWTIMER1_INIT(uint8_t priority) { return false; }
#endif
#ifdef TASKS_HWTIMER3_ENABLE
  IntervalTimer itimer3;
  void HAL_HWTIMER3_WRAPPER();
  bool HAL_HWTIMER3_INIT(uint8_t priority) {
    itimer3.priority(priority);
    return itimer3.begin(HAL_HWTIMER3_WRAPPER, (float)128 * 0.0625);
  }
#else
  bool HAL_HWTIMER1_INIT(uint8_t priority) { return false; }
#endif
#ifdef TASKS_HWTIMER4_ENABLE
  IntervalTimer itimer4;
  void HAL_HWTIMER4_WRAPPER();
  bool HAL_HWTIMER4_INIT(uint8_t priority) {
    itimer4.priority(priority);
    return itimer4.begin(HAL_HWTIMER4_WRAPPER, (float)128 * 0.0625);
  }
#else
  bool HAL_HWTIMER1_INIT(uint8_t priority) { return false; }
#endif

// pointers to event callbacks
void (*HAL_HWTIMER1_FUN)() = NULL;
void (*HAL_HWTIMER2_FUN)() = NULL;
void (*HAL_HWTIMER3_FUN)() = NULL;
void (*HAL_HWTIMER4_FUN)() = NULL;

// stop hw timers
void HAL_HWTIMER1_DONE() {
#ifdef TASKS_HWTIMER1_ENABLE
  HAL_HWTIMER1_FUN = NULL;
  itimer1.done();
#endif
}
void HAL_HWTIMER2_DONE() {
#ifdef TASKS_HWTIMER2_ENABLE
  HAL_HWTIMER2_FUN = NULL;
  itimer2.done();
#endif
}
void HAL_HWTIMER3_DONE() {
#ifdef TASKS_HWTIMER3_ENABLE
  HAL_HWTIMER3_FUN = NULL;
  itimer3.done();
#endif
}
void HAL_HWTIMER4_DONE() {
#ifdef TASKS_HWTIMER4_ENABLE
  HAL_HWTIMER4_FUN = NULL;
  itimer4.done();
#endif
}

// prepare hw timer for interval in sub-microseconds (1/16us)
volatile uint32_t _nextPeriod1, _nextPeriod2, _nextPeriod3, _nextPeriod4;
volatile uint16_t _nextRep1, _nextRep2, _nextRep3, _nextRep4;
const double timerRate16MHzTicks TIMER_RATE_16MHZ_TICKS;
void HAL_HWTIMER_PREPARE_PERIOD(uint8_t num, unsigned long period) {
  uint32_t i, reps=0;
  if (period != 0 && period <= 2144000000) {
    if (period < 16) period=16;    // minimum time is 1us
    double fperiod = period/timerRate16MHzTicks;
    uint32_t reps = (fperiod/4194304.0)+1.0;
    uint32_t i = fperiod/reps-1.0; // has -1 since this is dropped right into a timer register
  } else i=160000;                 // set for a 10ms period, stopped

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
#define HAL_HWTIMER1_SET_PERIOD(r) (PIT_LDVAL1=r)
#define HAL_HWTIMER2_SET_PERIOD(r) (PIT_LDVAL2=r)
#define HAL_HWTIMER3_SET_PERIOD(r) (PIT_LDVAL3=r)
#define HAL_HWTIMER4_SET_PERIOD(r) (PIT_LDVAL4=r)

// interrupt service routine wrappers
#ifdef TASKS_HWTIMER1_ENABLE
void HAL_HWTIMER1_WRAPPER() {
  static uint16_t count = 0;
  if (_nextRep1 > 1) { count++; if (count%_nextRep1 != 0) goto done; }
  if (_nextRep1) HAL_HWTIMER1_FUN();
  HAL_HWTIMER1_SET_PERIOD();
  done: {}
}
#endif
#ifdef TASKS_HWTIMER2_ENABLE
void HAL_HWTIMER2_WRAPPER() {
  static uint16_t count = 0;
  if (_nextRep2 > 1) { count++; if (count%_nextRep2 != 0) goto done; }
  if (_nextRep2) HAL_HWTIMER2_FUN();
  HAL_HWTIMER2_SET_PERIOD();
  done: {}
}
#endif
#ifdef TASKS_HWTIMER3_ENABLE
void HAL_HWTIMER3_WRAPPER() {
  static uint16_t count = 0;
  if (_nextRep3 > 1) { count++; if (count%_nextRep3 != 0) goto done; }
  if (_nextRep3) HAL_HWTIMER3_FUN();
  HAL_HWTIMER3_SET_PERIOD();
  done: {}
}
#endif
#ifdef TASKS_HWTIMER4_ENABLE
void HAL_HWTIMER4_WRAPPER() {
  static uint16_t count = 0;
  if (_nextRep4 > 1) { count++; if (count%_nextRep4 != 0) goto done; }
  if (_nextRep4) HAL_HWTIMER4_FUN();
  HAL_HWTIMER4_SET_PERIOD();
  done: {}
}
#endif
