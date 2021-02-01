//--------------------------------------------------------------------------------------------------
// Empty hardware timers

// minimum structure

// start hw timers
bool HAL_HWTIMER1_INIT(uint8_t priority) { return false; }
bool HAL_HWTIMER2_INIT(uint8_t priority) { return false; }
bool HAL_HWTIMER3_INIT(uint8_t priority) { return false; }
bool HAL_HWTIMER4_INIT(uint8_t priority) { return false; }

// pointers to event callbacks
void (*HAL_HWTIMER1_FUN)() = NULL;
void (*HAL_HWTIMER2_FUN)() = NULL;
void (*HAL_HWTIMER3_FUN)() = NULL;
void (*HAL_HWTIMER4_FUN)() = NULL;

// stop hw timers
void HAL_HWTIMER1_DONE() { }
void HAL_HWTIMER2_DONE() { }
void HAL_HWTIMER3_DONE() { }
void HAL_HWTIMER4_DONE() { }

// prepare hw timer for interval in sub-microseconds (1/16us)
void HAL_HWTIMER_PREPARE_PERIOD(uint8_t num, unsigned long period) { }

// fast routines to adopt new rates, must work from within the timer ISR
#define HAL_HWTIMER1_SET_PERIOD()
#define HAL_HWTIMER2_SET_PERIOD()
#define HAL_HWTIMER3_SET_PERIOD()
#define HAL_HWTIMER4_SET_PERIOD()

// interrupt service routine wrappers
// (none)
