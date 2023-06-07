//--------------------------------------------------------------------------------------------------
// Configures the profiler according to platform

#ifdef TASKS_PROFILER_ENABLE
  volatile unsigned long _task_max_runtime[4] = {0, 0, 0, 0};
  volatile unsigned long _task_total_runtime[4] = {0, 0, 0, 0};
  volatile unsigned long _task_total_runtime_count[4] = {0, 0, 0, 0};
  #ifdef TASKS_HWTIMER1_ENABLE
    #define TASKS_HWTIMER1_PROFILER_PREFIX unsigned long runtime_t0 = micros()
    #define TASKS_HWTIMER1_PROFILER_SUFFIX { long at = micros()-runtime_t0; _task_total_runtime[0] += at; _task_total_runtime_count[0]++; if (labs(at) > _task_max_runtime[0]) _task_max_runtime[0] = labs(at); }
  #endif
  #ifdef TASKS_HWTIMER2_ENABLE
    #define TASKS_HWTIMER2_PROFILER_PREFIX unsigned long runtime_t0 = micros()
    #define TASKS_HWTIMER2_PROFILER_SUFFIX { long at = micros()-runtime_t0; _task_total_runtime[1] += at; _task_total_runtime_count[1]++; if (labs(at) > _task_max_runtime[1]) _task_max_runtime[1] = labs(at); }
  #endif
  #ifdef TASKS_HWTIMER3_ENABLE
    #define TASKS_HWTIMER3_PROFILER_PREFIX unsigned long runtime_t0 = micros()
    #define TASKS_HWTIMER3_PROFILER_SUFFIX { long at = micros()-runtime_t0; _task_total_runtime[2] += at; _task_total_runtime_count[2]++; if (labs(at) > _task_max_runtime[2]) _task_max_runtime[2] = labs(at); }
  #endif
  #ifdef TASKS_HWTIMER4_ENABLE
    #define TASKS_HWTIMER4_PROFILER_PREFIX unsigned long runtime_t0 = micros()
    #define TASKS_HWTIMER4_PROFILER_SUFFIX { long at = micros()-runtime_t0; _task_total_runtime[3] += at; _task_total_runtime_count[3]++; if (labs(at) > _task_max_runtime[3]) _task_max_runtime[3] = labs(at); }
  #endif

  #define TASKS_PROFILER_PREFIX \
    long at = time_to_next_task - period; \
    average_arrival_time += at; \
    average_arrival_time_count++; \
    if (labs(at + period) > max_arrival_time) max_arrival_time = labs(at + period); \
    unsigned long runtime_t0 = micros(); 
  #define TASKS_PROFILER_SUFFIX \
    at = micros()-runtime_t0; \
    total_runtime += at; \
    total_runtime_count++; \
    if (labs(at) > max_runtime) max_runtime = labs(at);

#else
  #define TASKS_HWTIMER1_PROFILER_PREFIX
  #define TASKS_HWTIMER1_PROFILER_SUFFIX
  #define TASKS_HWTIMER2_PROFILER_PREFIX
  #define TASKS_HWTIMER2_PROFILER_SUFFIX
  #define TASKS_HWTIMER3_PROFILER_PREFIX
  #define TASKS_HWTIMER3_PROFILER_SUFFIX
  #define TASKS_HWTIMER4_PROFILER_PREFIX
  #define TASKS_HWTIMER4_PROFILER_SUFFIX
  #define TASKS_PROFILER_PREFIX
  #define TASKS_PROFILER_SUFFIX
#endif
