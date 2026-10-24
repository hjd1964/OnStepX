//--------------------------------------------------------------------------------------------------
// Renesas RA4M1 (Arduino UNO R4 Minima/WiFi) hardware timers

// uses up to four GPT timers through the Arduino core's FspTimer class
// timers count at 3MHz (48MHz PCLKD / 16), granularity of timer is 0.333uS
// counts are kept within 16 bits (21.8ms) so the 16 bit GPT channels work too, longer periods use
// the software pre-scaler (reps); this also keeps the delay before a new period takes effect short
// once the free GPT channels are used, timers take channels the core reserves for analogWrite() PWM
// (D11, D10, D9 first), so PWM is lost on the pins sharing that channel

#include <FspTimer.h>

#define TIMER_RATE_MHZ 3L                            // GPT timers run at 3MHz
#define TIMER_RATE_16MHZ_TICKS 5.333333F             // 16MHz/TIMER_RATE_MHZ, for the default 16MHz "sub-micros"

#if defined(TASKS_HWTIMER1_ENABLE) || defined(TASKS_HWTIMER2_ENABLE) || defined(TASKS_HWTIMER3_ENABLE) || defined(TASKS_HWTIMER4_ENABLE)
  // prepare hw timer for interval in sub-microseconds (1/16us)
  volatile uint32_t _nextPeriod1 = 3000, _nextPeriod2 = 3000, _nextPeriod3 = 3000, _nextPeriod4 = 3000;
  volatile uint16_t _nextRep1 = 0, _nextRep2 = 0, _nextRep3 = 0, _nextRep4 = 0;
  void HAL_HWTIMER_PREPARE_PERIOD(uint8_t num, unsigned long period) {
    // maximum time is about 134 seconds for this design
    uint32_t counts, reps = 0;
    if (period != 0 && period <= 2144000000) {
      if (period < 16) period = 16;   // minimum time is 1us
      uint32_t ticks = (period/16)*3 + ((period%16)*3)/16;
      reps    = ticks/65536 + 1;
      counts  = ticks/reps;
    } else counts = 3000;             // set for a 1ms period, stopped

    noInterrupts();
    switch (num) {
      case 1: _nextPeriod1 = counts; _nextRep1 = reps; break;
      case 2: _nextPeriod2 = counts; _nextRep2 = reps; break;
      case 3: _nextPeriod3 = counts; _nextRep3 = reps; break;
      case 4: _nextPeriod4 = counts; _nextRep4 = reps; break;
    }
    interrupts();
  }

  // claim a GPT channel: a free one if possible, otherwise one the core reserved for PWM on D11, D10, D9..
  // (only two of the eight are free on the UNO R4, get_available_timer() offers an AGT timer after that)
  bool HAL_HWTIMER_BEGIN(FspTimer &timer, GPTimerCbk_f isr) {
    uint8_t type = GPT_TIMER;
    int8_t channel = FspTimer::get_available_timer(type, false);
    if (channel >= 0 && type == GPT_TIMER) {
      return timer.begin(TIMER_MODE_PERIODIC, GPT_TIMER, channel, 3000, 0, TIMER_SOURCE_DIV_16, isr);
    }
    const pin_size_t pwmPins[] = { 11, 10, 9, 6, 5, 3 };
    for (pin_size_t pin : pwmPins) {
      channel = GET_CHANNEL(getPinCfgs(pin, PIN_CFG_REQ_PWM)[0]);
      FspTimer::force_use_of_pwm_reserved_timer();
      if (timer.begin(TIMER_MODE_PERIODIC, GPT_TIMER, channel, 3000, 0, TIMER_SOURCE_DIV_16, isr)) return true;
    }
    return false;
  }

  // start a periodic GPT timer at 1ms that calls isr on each overflow
  bool HAL_HWTIMER_INIT(uint8_t priority, FspTimer &timer, GPTimerCbk_f isr) {
    // the 3MHz count rate assumes the GPT clock (PCLKD) is 48MHz
    if (R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKD) != 48000000) return false;
    if (!HAL_HWTIMER_BEGIN(timer, isr)) return false;
    // OnTask priority 0 is the highest, keep these above the core's default IRQ priority (12)
    if (!timer.setup_overflow_irq(8 + priority)) { timer.end(); return false; }
    if (!timer.open() || !timer.start()) { timer.end(); return false; }
    return true;
  }
#else
  void HAL_HWTIMER_PREPARE_PERIOD(uint8_t num, unsigned long period) { (void)(num); (void)(period); }
#endif

#ifdef TASKS_HWTIMER1_ENABLE
  FspTimer itimer1;

  void (*HAL_HWTIMER1_FUN)() = NULL; // points to task/process callback function
  void HAL_HWTIMER1_WRAPPER(timer_callback_args_t *args);

  bool HAL_HWTIMER1_INIT(uint8_t priority) {
    return HAL_HWTIMER_INIT(priority, itimer1, HAL_HWTIMER1_WRAPPER);
  }

  void HAL_HWTIMER1_DONE() {
    HAL_HWTIMER1_FUN = NULL;
    itimer1.end();
  }

  // the period register is buffered, so the new period starts after the current one ends
  #define HAL_HWTIMER1_SET_PERIOD() itimer1.set_period(_nextPeriod1)
  void HAL_HWTIMER1_WRAPPER(timer_callback_args_t *args) {
    (void)(args);
    TASKS_HWTIMER1_PROFILER_PREFIX;
    static uint16_t count = 0;
    if (_nextRep1 > 1) { count++; if (count % _nextRep1 != 0) goto done; }
    if (_nextRep1) HAL_HWTIMER1_FUN();
    HAL_HWTIMER1_SET_PERIOD();
    done: {}
    TASKS_HWTIMER1_PROFILER_SUFFIX;
  }
#endif

#ifdef TASKS_HWTIMER2_ENABLE
  FspTimer itimer2;

  void (*HAL_HWTIMER2_FUN)() = NULL; // points to task/process callback function
  void HAL_HWTIMER2_WRAPPER(timer_callback_args_t *args);

  bool HAL_HWTIMER2_INIT(uint8_t priority) {
    return HAL_HWTIMER_INIT(priority, itimer2, HAL_HWTIMER2_WRAPPER);
  }

  void HAL_HWTIMER2_DONE() {
    HAL_HWTIMER2_FUN = NULL;
    itimer2.end();
  }

  #define HAL_HWTIMER2_SET_PERIOD() itimer2.set_period(_nextPeriod2)
  void HAL_HWTIMER2_WRAPPER(timer_callback_args_t *args) {
    (void)(args);
    TASKS_HWTIMER2_PROFILER_PREFIX;
    static uint16_t count = 0;
    if (_nextRep2 > 1) { count++; if (count % _nextRep2 != 0) goto done; }
    if (_nextRep2) HAL_HWTIMER2_FUN();
    HAL_HWTIMER2_SET_PERIOD();
    done: {}
    TASKS_HWTIMER2_PROFILER_SUFFIX;
  }
#endif

#ifdef TASKS_HWTIMER3_ENABLE
  FspTimer itimer3;

  void (*HAL_HWTIMER3_FUN)() = NULL; // points to task/process callback function
  void HAL_HWTIMER3_WRAPPER(timer_callback_args_t *args);

  bool HAL_HWTIMER3_INIT(uint8_t priority) {
    return HAL_HWTIMER_INIT(priority, itimer3, HAL_HWTIMER3_WRAPPER);
  }

  void HAL_HWTIMER3_DONE() {
    HAL_HWTIMER3_FUN = NULL;
    itimer3.end();
  }

  #define HAL_HWTIMER3_SET_PERIOD() itimer3.set_period(_nextPeriod3)
  void HAL_HWTIMER3_WRAPPER(timer_callback_args_t *args) {
    (void)(args);
    TASKS_HWTIMER3_PROFILER_PREFIX;
    static uint16_t count = 0;
    if (_nextRep3 > 1) { count++; if (count % _nextRep3 != 0) goto done; }
    if (_nextRep3) HAL_HWTIMER3_FUN();
    HAL_HWTIMER3_SET_PERIOD();
    done: {}
    TASKS_HWTIMER3_PROFILER_SUFFIX;
  }
#endif

#ifdef TASKS_HWTIMER4_ENABLE
  FspTimer itimer4;

  void (*HAL_HWTIMER4_FUN)() = NULL; // points to task/process callback function
  void HAL_HWTIMER4_WRAPPER(timer_callback_args_t *args);

  bool HAL_HWTIMER4_INIT(uint8_t priority) {
    return HAL_HWTIMER_INIT(priority, itimer4, HAL_HWTIMER4_WRAPPER);
  }

  void HAL_HWTIMER4_DONE() {
    HAL_HWTIMER4_FUN = NULL;
    itimer4.end();
  }

  #define HAL_HWTIMER4_SET_PERIOD() itimer4.set_period(_nextPeriod4)
  void HAL_HWTIMER4_WRAPPER(timer_callback_args_t *args) {
    (void)(args);
    TASKS_HWTIMER4_PROFILER_PREFIX;
    static uint16_t count = 0;
    if (_nextRep4 > 1) { count++; if (count % _nextRep4 != 0) goto done; }
    if (_nextRep4) HAL_HWTIMER4_FUN();
    HAL_HWTIMER4_SET_PERIOD();
    done: {}
    TASKS_HWTIMER4_PROFILER_SUFFIX;
  }
#endif

// disable hardware timer based tasks by stopping the timer(s)
void timerAlarmsDisable() {
#ifdef TASKS_HWTIMER1_ENABLE
  if (itimer1.is_opened()) itimer1.stop();
#endif
#ifdef TASKS_HWTIMER2_ENABLE
  if (itimer2.is_opened()) itimer2.stop();
#endif
#ifdef TASKS_HWTIMER3_ENABLE
  if (itimer3.is_opened()) itimer3.stop();
#endif
#ifdef TASKS_HWTIMER4_ENABLE
  if (itimer4.is_opened()) itimer4.stop();
#endif
}

// enable hardware timer based tasks by starting the timer(s)
void timerAlarmsEnable() {
#ifdef TASKS_HWTIMER1_ENABLE
  if (itimer1.is_opened()) itimer1.start();
#endif
#ifdef TASKS_HWTIMER2_ENABLE
  if (itimer2.is_opened()) itimer2.start();
#endif
#ifdef TASKS_HWTIMER3_ENABLE
  if (itimer3.is_opened()) itimer3.start();
#endif
#ifdef TASKS_HWTIMER4_ENABLE
  if (itimer4.is_opened()) itimer4.start();
#endif
}
