/*
 * Title       OnTask
 * by          Howard Dutton
 *
 * Copyright (C) 2021 Howard Dutton
 * www.stellarjourney.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * DESCRIPTION:
 * 
 * Software timer based processes must use "tasks.yield();" (or the "Y;" macro, which is the same exact thing)
 * during any processing that requires a significant amount of time. This allows for the processing of other tasks
 * while it is working.  What a "significant amount of time" is depends on the program's requirements.  It might
 * be measured in micro-seconds, or milli-seconds, or seconds.  Yield should not be used for hardware timer based 
 * processes
 *
*/

#pragma once
#include <Arduino.h>

#ifndef IRAM_ATTR
  #define IRAM_ATTR
#endif

// To enable the option to use a given hardware timer (1..4), if available, uncomment that line:
#ifndef TASKS_HWTIMERS
  #define TASKS_HWTIMERS -1
#endif
#if TASKS_HWTIMERS < -1 || TASKS_HWTIMERS > 4
    #error "OnTask: TASKS_HWTIMERS must be OFF, or 1 to 4."
#endif
#if TASKS_HWTIMERS >= 1
  #define TASKS_HWTIMER1_ENABLE
#endif
#if TASKS_HWTIMERS >= 2
  #define TASKS_HWTIMER2_ENABLE
#endif
#if TASKS_HWTIMERS >= 3
  #define TASKS_HWTIMER3_ENABLE
#endif
#if TASKS_HWTIMERS == 4
  #define TASKS_HWTIMER4_ENABLE
#endif

// default provision is for 8 tasks, up to 255 are allowed, to change use:
// #define TASKS_MAX 200 (for example)
#ifndef TASKS_MAX
  #define TASKS_MAX 8
#endif

// default is to allow only higher priority tasks to run during a yield()
// comment out and any task can run except the task that yields
#define TASKS_HIGHER_PRIORITY_ONLY

// ESP32 override cli/sei and use muxes to block the h/w timer ISR's instead
#ifdef ESP32
  // on the ESP32 noInterrupts()/interrupts() are #defined to be cli()/sei()
  // cli()/sei() are empty and do nothing, below we use them for our purposes
  // to the block execution of all four timer interrupts
  extern portMUX_TYPE timerMux;
  #undef cli
  // disable hardware timer based tasks by FreeRTOS mux
  #define cli() portENTER_CRITICAL(&timerMux)
  #undef sei
  // enable hardware timer based tasks by FreeRTOS mux
  #define sei() portEXIT_CRITICAL(&timerMux)
  // disable hardware timer based tasks by clearing interrupt flag(s)
  extern void timerAlarmsDisable();
  // enable hardware timer based tasks by setting interrupt flag(s)
  extern void timerAlarmsEnable();
#endif

// short Y macro to embed yield()
#define Y tasks.yield()

// short macro to allow momentary postponement of the current task
extern unsigned char __task_postpone;
// postpone currently executing task (scheduler picks task up ASAP again on exit)
#define task_postpone() __task_postpone = true;

enum PeriodUnits: uint8_t {PU_NONE, PU_MILLIS, PU_MICROS, PU_SUB_MICROS};

// Timing modes
// TM_BALANCED (default) to maintain the specified frequency/period where a task that runs late is next run early to compensate
// TM_MINIMUM to run the task at an interval not less than the specified frequency/period from task start to next start
// TM_GAP to run the task at an interval not less than the specified frequency/period from task exit to next start
enum TimingMode: uint8_t {TM_BALANCED, TM_MINIMUM, TM_GAP};

class Task {
  public:
    Task(uint32_t period, uint32_t duration, bool repeat, uint8_t priority, void (*callback)());
    ~Task();

    bool requestHardwareTimer(uint8_t num, uint8_t hwPriority);
    uint8_t hardware_timer = 0;

    void setCallback(void (*callback)());

    void setTimingMode(TimingMode mode);

    // run task at the prescribed interval
    // note: tasks are timed in such a way as to achieve an accurate average frequency, if
    //       the task occurs late the next call is scheduled earlier to make up the difference
    bool poll();

    void refreshPeriod();
    void setPeriod(unsigned long period, PeriodUnits units = PU_MILLIS);
    void setFrequency(float freq);

    void setDuration(unsigned long duration);
    bool isDurationComplete();
    void setDurationComplete();

    void setRepeat(bool repeat);

    void setPriority(bool priority);
    uint8_t getPriority();

    void setNameStr(const char name[]);
    char *getNameStr();

    #ifdef TASKS_PROFILER_ENABLE
      float getArrivalAvg();
      float getArrivalMax();
      float getRuntimeTotal();
      long getRuntimeTotalCount();
      float getRuntimeMax();
    #endif

    volatile bool immediate = true;

  private:

    void setHardwareTimerPeriod();

    char                   processName[8];
    unsigned long          period            = 0;
    unsigned long          next_period       = 0;
    unsigned long          duration          = 0;
    bool                   repeat            = false;
    uint8_t                priority          = 0;
    bool                   idle              = true;
    bool                   running           = false;
    PeriodUnits            period_units      = PU_MILLIS;
    PeriodUnits            next_period_units = PU_NONE;
    unsigned long          start_time        = 0;
    unsigned long          last_task_time    = 0;
    unsigned long          next_task_time    = 0;
    TimingMode             timingMode        = TM_BALANCED;
    void (*volatile callback)() = NULL;

    #ifdef TASKS_PROFILER_ENABLE
      volatile double        average_arrival_time       = 0;
      volatile unsigned long average_arrival_time_count = 0;
      volatile long          max_arrival_time           = 0;
      volatile double        total_runtime              = 0;
      volatile unsigned long total_runtime_count        = 0;
      volatile long          max_runtime                = 0;
    #endif
};

class Tasks {
  public:
    Tasks();
    ~Tasks();

    // add process task
    // \param period    between process calls in milliseconds, use 0 to disable
    //                  software timers are limited to <= 49 days, hardware timers are limited to <= 139 seconds
    // \param duration  in milliseconds the task is valid for, use 0 for unlimited (deletes the task on completion)
    // \param repeat    true if the task is allowed to repeat, false to run once (sets period to 0 on completion)
    // \param priority  software level highest 0 to 7 lowest.  hardware tasks are always higher priority than software tasks
    //                  for software timed tasks level 0 must be complete before level 1 are serviced, etc.
    //                  tasks run round robin in ea priority level, the most recently serviced task will be the last visited again
    //                  only higher priority tasks are allowed to run when a lower priority task yields
    // \param callback  function to handle this tasks processing
    // \return          handle to the task on success, or 0 on failure
    uint8_t add(uint32_t period, uint32_t duration, bool repeat, uint8_t priority, void (*callback)());

    // add process task
    // \param period    between process calls in milliseconds, use 0 to disable
    //                  software timers are limited to <= 49 days, hardware timers are limited to <= 139 seconds
    // \param duration  in milliseconds the task is valid for, use 0 for unlimited (deletes the task on completion)
    // \param repeat    true if the task is allowed to repeat, false to run once (sets period to 0 on completion)
    // \param priority  software level highest 0 to 7 lowest.  hardware tasks are always higher priority than software tasks
    //                  for software timed tasks level 0 must be complete before level 1 are serviced, etc.
    //                  tasks run round robin in ea priority level, the most recently serviced task will be the last visited again
    //                  only higher priority tasks are allowed to run when a lower priority task yields
    // \param callback  function to handle this tasks processing
    // \param name      an optional short (max length 7) char str describing the task 
    // \return          handle to the task on success, or 0 on failure
    uint8_t add(uint32_t period, uint32_t duration, bool repeat, uint8_t priority, void (*callback)(), const char name[]);

    // allocates a hardware timer, if available, for this task. Note: for the associated task: *repeat* must be true,
    // *priority* must be 0 (all are higher than s/w task priority 0.)
    // \param handle        task handle
    // \param hwPriority    optional hardware interrupt priority, default is 128 if not specified
    // \return              true if successful, or false (in which case the standard polling timer will still be available)
    bool requestHardwareTimer(uint8_t handle, uint8_t hwPriority);
    bool requestHardwareTimer(uint8_t handle);

    // change task callback
    // \param handle        task handle
    // \param callback      function to handle this tasks processing
    // \return              true if successful, or false if unable to find the associated task
    bool setCallback(uint8_t handle, void (*callback)());

    // change task timing mode
    // \param handle        task handle
    // \param mode          either TM_BALANCED (default) or TM_MINIMUM
    // \return              true if successful, or false if unable to find the associated task
    bool setTimingMode(uint8_t handle, TimingMode mode);

    // remove process task. Note: do not remove a task if the task process is running
    // \param handle    task handle
    // \return          nothing
    void remove(uint8_t handle);

    // refresh process period, causes setPeriodRatioSubMicros() ratio to be recognized by hardware timers
    // handle: task handle
    void refreshPeriod(uint8_t handle);

    // set process period ms. Note: the period/frequency change takes effect on the next task cycle, 
    // if the period is > the hardware timers maximum period the task is disabled
    // \param handle    task handle
    // \param period    in milliseconds, use 0 for disabled
    // \return          nothing
    void setPeriod(uint8_t handle, unsigned long period);

    // set process period us
    // handle: task handle
    // period: in milliseconds, use 0 for disabled
    // notes:
    //   the period/frequency change takes effect on the next task cycle
    //   if the period is > the hardware timers maximum period the task is disabled
    void setPeriodMicros(uint8_t handle, unsigned long period);

    // set process period sub-us
    // handle: task handle
    // period: in sub-microseconds (1/16 microsecond units), use 0 for disabled
    // notes:
    //   the period/frequency change takes effect on the next task cycle
    //   if the period is > the hardware timers maximum period the task is disabled
    void setPeriodSubMicros(uint8_t handle, unsigned long period);

    // change process period Hz
    // handle: task handle
    // freq:  in Hertz, use 0 for disabled
    // notes:
    //   the period/frequency change takes effect on the next task cycle
    //   when setting a frequency the most appropriate setPeriod is used automatically
    //   if the period is > ~49 days (or > the hardware timers maximum period) the task is disabled
    void setFrequency(uint8_t handle, double freq);

    // set the period ratio to compensate for MCU clock inaccuracy
    // in sub-microseconds per second, this is safe to call from within an ISR
    // values above 16M cause the timers to compensate by running slower
    // values below 16M cause the timers to compensate by running faster
    void setPeriodRatioSubMicros(unsigned long value);

    // set process to run immediately on the next pass (within its priority level)
    IRAM_ATTR inline void immediate(uint8_t handle) { if (handle != 0 && allocated[handle - 1]) { task[handle - 1]->immediate = true; } }

    // change process duration (milliseconds,) use 0 for disabled
    void setDuration(uint8_t handle, unsigned long duration);

    // late removal of process task
    // handle: task handle
    // note:   removes a task process, even if it is running
    void setDurationComplete(uint8_t handle);

    // change process repeat (true/false)
    void setRepeat(uint8_t handle, bool repeat);

    // change process priority level (highest 0 to 7 lowest)
    void setPriority(uint8_t handle, uint8_t priority);

    // set the process name
    void setNameStr(uint8_t handle, const char name[]);

    // get the process name
    char *getNameStr(uint8_t handle);

    // search for tasks, returns 0 if no tasks are found
    uint8_t getFirstHandle();

    // return next task (after task handle) or 0 if no tasks are found
    uint8_t getNextHandle(uint8_t handle);

    // search for task by name, returns 0 if not found
    uint8_t getHandleByName(const char name[]);

    #ifdef TASKS_PROFILER_ENABLE
      double getArrivalAvg(uint8_t handle);
      double getArrivalMax(uint8_t handle);
      double getRuntimeTotal(uint8_t handle);
      long getRuntimeTotalCount(uint8_t handle);
      double getRuntimeMax(uint8_t handle);
    #endif

    // runs tasks at their prescribed interval, each call can trigger at most a single process
    // processes that are already running are ignored so it's ok to poll() within a process
    void yield();
    void yield(unsigned long milliseconds);
    void yieldMicros(unsigned long microseconds);

  private:
    // keep track of the range of priorities so we don't waste cycles looking at empty ones
    void updatePriorityRange();
    // keep track of the range of tasks so we don't waste cycles looking at empty ones
    void updateEventRange();

    uint8_t highest_task     = 0; // the highest task# assigned
    uint8_t highest_priority = 0; // the highest task priority
    #ifdef TASKS_HIGHER_PRIORITY_ONLY
      uint8_t highest_active_priority = 8;
    #endif
    uint8_t count            = 0;
    uint8_t num_tasks        = 0; // the total number of tasks
    uint8_t number[8]        = {255, 255, 255, 255, 255, 255, 255, 255}; // the task# we are servicing at this priority level
    bool    allocated[TASKS_MAX];
    bool    hardware_timer_allocated[4] = {false, false, false, false};
    Task    *task[TASKS_MAX];
};

extern Tasks tasks;
