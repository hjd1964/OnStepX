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
 */

// DESCRIPTION:
// 
// Default provision is for 8 tasks, up to 255 are allowed, to change use:
// #define TASKS_MAX 200 (for example) before the #include for this library.
//
// To enable the option to use a given hardware timer (1..4), if available depending on the HAL) add:
// #define TASKS_HWTIMER1_ENABLE (for example) before the #include for this library.
//
// Normally tasks are skipped if they become too late, to instead queue them for later processing add:
// #define TASKS_QUEUE_MISSED
//
// Enabling the profiler inserts time logging code into the process calls.  This provides the average
// and largest start time delta and run time for each process.  To enable the profiler use:
// #define TASKS_PROFILER
//
// Software timer based processes must use "tasks.yield();" (or the "Y;" macro, which is the same exact thing)
// during any processing that requires a significant amount of time. This allows for the processing of other tasks
// while it is working.  What a "significant amount of time" is depends on the program's requirements.  It might
// be measured in micro-seconds, or milli-seconds, or seconds.  Yield should not be used for hardware timers based 
// processes
//
// Up to 64 mutexes (0 to 63) are supported with the following macros, any
// code between these two statements will be limited to running on one process
// at a time even if "tasks.yield();" is called within that code:
// tasks_mutex_enter(mutex_number);
// tasks_mutex_exit(mutex_number);
//

#pragma once

#include "HAL_PROFILER.h"
#include "HAL_HWTIMERS.h"

// up to 8 tasks default
#ifndef TASKS_MAX
  #define TASKS_MAX 8
#endif

// mutex macros, do not run these in hardware timers (not ISR safe)!
#define tasks_mutex_enter(m) while (bitRead(__task_mutex[m/8],m%8)) tasks.yield(); bitSet(__task_mutex[m/8],m%8,1));
#define tasks_mutex_exit(m)  bitClear(__task_mutex[m/8],m%8,0));
static uint8_t __task_mutex[8] = {0, 0, 0, 0, 0, 0, 0, 0};

enum PeriodUnits {PU_NONE, PU_MILLIS, PU_MICROS, PU_SUB_MICROS};

class Task {
  public:
    Task(uint32_t period, uint32_t duration, bool repeat, uint8_t priority, void (*callback)()) {
      this->period   = period;
      period_units   = PU_MILLIS;
      this->duration = duration;
      this->repeat   = repeat;
      this->priority = priority;
      this->callback = callback;
      start_time     = millis();
      next_task_time = start_time + period;
    }

    ~Task() {
      switch (hardwareTimer) {
        case 1: HAL_HWTIMER1_DONE(); break;
        case 2: HAL_HWTIMER2_DONE(); break;
        case 3: HAL_HWTIMER3_DONE(); break;
        case 4: HAL_HWTIMER4_DONE(); break;
      }
    }

    bool requestHardwareTimer(uint8_t num, uint8_t hwPriority) {
      if (num < 1 || num > 4) return false;
      if (repeat != true) return false;
      if (priority != 0) return false;

      unsigned long hwPeriod = period;
      if (period_units == PU_NONE) hwPeriod = 0; else if (period_units == PU_MILLIS) hwPeriod *= 16000UL; else if (period_units == PU_MICROS) hwPeriod *= 16UL;
      HAL_HWTIMER_PREPARE_PERIOD(num, hwPeriod);

      switch (num) {
        case 1:
          if (HAL_HWTIMER1_FUN != NULL) return false;
          HAL_HWTIMER1_SET_PERIOD();
          HAL_HWTIMER1_FUN = callback;
          if (!HAL_HWTIMER1_INIT(hwPriority)) return false;
        break;
        case 2:
          if (HAL_HWTIMER2_FUN != NULL) return false;
          HAL_HWTIMER2_SET_PERIOD();
          HAL_HWTIMER2_FUN = callback;
          if (!HAL_HWTIMER2_INIT(hwPriority)) return false;
        break;
        case 3:
          if (HAL_HWTIMER3_FUN != NULL) return false;
          HAL_HWTIMER3_SET_PERIOD();
          HAL_HWTIMER3_FUN = callback;
          if (!HAL_HWTIMER3_INIT(hwPriority)) return false;
        break;
        case 4:
          if (HAL_HWTIMER4_FUN != NULL) return false;
          HAL_HWTIMER4_SET_PERIOD();
          HAL_HWTIMER4_FUN = callback;
          if (HAL_HWTIMER4_INIT(hwPriority)) return false;
        break;
      }
      hardwareTimer = num;
      period = hwPeriod;
      period_units = PU_SUB_MICROS;
      return true;
    }

    // run task at the prescribed interval
    // note: tasks are timed in such a way as to achieve an accurate average frequency, if
    //       the task occurs late the next call is scheduled earlier to make up the difference
    bool poll() {
      if (hardwareTimer) return false;

      if (period > 0 && !running) {
        unsigned long t;
        if (period_units == PU_MICROS) t = micros(); else if (period_units == PU_SUB_MICROS) t = micros() * 16; else t = millis();
        unsigned long time_to_next_task = next_task_time - t;
        if ((long)time_to_next_task < 0) {
          running = true;
          TASKS_PROFILER_PREFIX;
          callback();
          TASKS_PROFILER_SUFFIX;

          // adopt next period
          if (next_period_units != PU_NONE) {
            time_to_next_task = 0;
            period = next_period;
            period_units = next_period_units;
            next_period_units = PU_NONE;
          }

          // set adjusted period
          #ifndef TASKS_QUEUE_MISSED
            if (-time_to_next_task > period) time_to_next_task = period;
          #endif
          next_task_time = t + (long)(period + time_to_next_task);
          if (!repeat) period = 0;

          running = false;
          return true;
        }
      }
      return false;
    }

    void setPeriod(unsigned long period) {
      if (hardwareTimer) {
        next_period = period;
        next_period_units = PU_MILLIS;
        setHardwareTimerPeriod();
      } else {
        if (this->period == 0) {
          this->period = period;
          period_units = PU_MILLIS;
          next_period_units = PU_NONE;
        } else {
          next_period = period;
          next_period_units = PU_MILLIS;
        }
      }
    }

    void setPeriodMicros(unsigned long period) {
      if (hardwareTimer) {
        next_period = period;
        next_period_units = PU_MICROS;
        setHardwareTimerPeriod();
      } else {
        if (this->period == 0) {
          this->period = period;
          period_units = PU_MICROS;
          next_period_units = PU_NONE;
        } else {
          next_period = period;
          next_period_units = PU_MICROS;
        }
      }
    }

    void setPeriodSubMicros(unsigned long period) {
      if (hardwareTimer) {
        next_period = period;
        next_period_units = PU_SUB_MICROS;
        setHardwareTimerPeriod();
      } else {
        if (this->period == 0) {
          this->period = period;
          period_units = PU_SUB_MICROS;
          next_period_units = PU_NONE;
        } else {
          next_period = period;
          next_period_units = PU_SUB_MICROS;
        }
      }
    }

    void setFrequency(double freq) {
      if (freq > 0.0) {
        freq = 1.0 / freq;            // seconds per call
        double x = freq * 16000000.0; // sub-micros per call
        if (x <= 4294967295.0) {
          setPeriodSubMicros(lround(x));
          return;
        }
        x = freq * 1000000.0;         // micros per call
        if (x <= 4294967295.0) {
          setPeriodMicros(lround(x));
          return;
        }
        x = freq * 1000.0;            // millis per call
        if (x <= 4294967295.0) {
          setPeriod(lround(x));
          return;
        }
      }
      setPeriod(0);
    }

    void setDuration(unsigned long duration) {
      this->duration = duration;
    }

    bool isDurationComplete() {
      return (duration > 0 && ((long)(millis() - (start_time + duration)) >= 0));
    }

    void setRepeat(bool repeat) {
      if (hardwareTimer) return;
      this->repeat = repeat;
    }

    void setPriority(bool priority) {
      if (hardwareTimer) return;
      this->priority = priority;
    }

    uint8_t getPriority() {
      return priority;
    }

    void setNameStr(const char name[]) {
      strncpy(processName,name,7);
    }

    char *getNameStr() {
      return processName;
    }

#ifdef TASKS_PROFILER_ENABLE
    double getArrivalAvg() {
      if (hardwareTimer) return 0;
      if (average_arrival_time_count == 0) return 0;
      double value = -average_arrival_time/average_arrival_time_count;
      average_arrival_time = 0;
      average_arrival_time_count = 0;
      return value;
    }
    double getArrivalMax() {
      if (hardwareTimer) return 0;
      double value = max_arrival_time;
      max_arrival_time = 0;
      return value;
    }
    double getRuntimeTotal() {
      if (hardwareTimer) { noInterrupts(); total_runtime = _task_total_runtime[hardwareTimer-1]; _task_total_runtime[hardwareTimer-1] = 0; total_runtime_count=_task_total_runtime_count[hardwareTimer-1]; interrupts(); };
      double value = total_runtime;
      total_runtime = 0;
      return value;
    }
    long getRuntimeTotalCount() {
      if (hardwareTimer) { noInterrupts(); total_runtime_count = _task_total_runtime_count[hardwareTimer-1]; _task_total_runtime_count[hardwareTimer-1] = 0; interrupts(); };
      long value = total_runtime_count;
      total_runtime_count = 0;
      return value;
    }
    double getRuntimeMax() {
      if (hardwareTimer) { noInterrupts(); max_runtime = _task_max_runtime[hardwareTimer-1]; _task_max_runtime[hardwareTimer-1] = 0; interrupts(); };
      double value = max_runtime;
      max_runtime = 0;
      return value;
    }
#endif

  private:

    void setHardwareTimerPeriod() {
      // adopt next period
      if (next_period_units != PU_NONE) {
        if (next_period_units == PU_MILLIS) next_period *= 16000UL; else if (next_period_units == PU_MICROS) next_period *= 16UL;
        next_period_units = PU_NONE;
        period_units = PU_SUB_MICROS;
        period = next_period;
        HAL_HWTIMER_PREPARE_PERIOD(hardwareTimer, period);
      }
    }

    char                   processName[8]             = "";
    unsigned long          period                     = 0;
    unsigned long          next_period                = 0;
    unsigned long          duration                   = 0;
    bool                   repeat                     = false;
    uint8_t                priority                   = 0;
    bool                   running                    = false;
    PeriodUnits            period_units               = PU_MILLIS;
    PeriodUnits            next_period_units          = PU_NONE;
    unsigned long          start_time                 = 0;
    unsigned long          next_task_time             = 0;
    uint8_t                hardwareTimer              = 0;
    void (*callback)() = NULL;
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
    Tasks() {
      // clear tasks
      for (uint8_t c = 0; c < TASKS_MAX; c++) {
        task[c] = NULL;
        allocated[c] = false;
      }
      
      for (uint8_t c = 0; c < 8; c++) __task_mutex[c] = 0;
    }

    ~Tasks() {
      // find active tasks and remove
      for (uint8_t c = 0; c < TASKS_MAX; c++) {
        if (allocated[c]) delete task[c];
      }
    }

    // add process task
    // period:   between process calls in milliseconds, use 0 to disable
    //           the possible period in milliseconds is about 49 days
    // duration: in milliseconds the task is valid for, use 0 for unlimited (deletes the task on completion)
    // repeat:   true if the task is allowed to repeat, false to run once (sets period to 0 on completion)
    // priority: level highest 0 to 7 lowest, all tasks of priority 0 must be complete before priority 1 tasks are serviced, etc.
    // callback: function to handle this tasks processing
    // returns:  handle to the task on success, or 0 on failure
    uint8_t add(uint32_t period, uint32_t duration, bool repeat, uint8_t priority, void (*callback)()) {

      // check priority
      if (priority > 7) return false;
      if (priority > highest_priority) highest_priority = priority;

      // find the next free task
      int8_t e = -1;
      for (uint8_t c = 0; c < TASKS_MAX; c++) {
        if (!allocated[c]) {
          e = c;
          break;
        }
      }
      // no tasks available
      if (e == -1) return false;

      // create the task handler
      task[e] = new Task(period, duration, repeat, priority, callback);
      allocated[e] = true;

      updateEventRange();

      return e + 1;
    }
    // as above, and adds a process name
    uint8_t add(uint32_t period, uint32_t duration, bool repeat, uint8_t priority, void (*callback)(), const char name[]) {
      uint8_t handle = add(period, duration, repeat, priority, callback);
      setNameStr(handle, name);
      return handle;
    }

    // allocates a hardware timer, if available, for this task
    // handle:       task handle
    // num:          the hardware timer number, there are up to four (1 to 4) depending on the platform
    // hwPriority:   hardware interrupt priority, default is 128
    // returns:      true if successful, or false (in which case the standard polling timer will still be available)
    // notes:
    //   "repeat"   must be true
    //   "priority" must be 0 (all are higher than task priority 0)
    //   generally it's a good idea to NOT poll tasks (P macro) from within the process associated with this task
    bool requestHardwareTimer(uint8_t handle, uint8_t num) {
      return requestHardwareTimer(handle, num, 128);
    }
    bool requestHardwareTimer(uint8_t handle, uint8_t num, uint8_t hwPriority) {
      if (handle != 0 && allocated[handle - 1]) {
        return task[handle - 1]->requestHardwareTimer(num, hwPriority);
      } else return false;
    }

    // remove process task
    // handle: task handle
    // note:   do not remove an task if the task process is running
    void remove(uint8_t handle) {
      if (handle != 0 && allocated[handle - 1]) {
        delete task[handle - 1];
        allocated[handle - 1] = false;
        updateEventRange();
        updatePriorityRange();
      }
    }

    // set process period ms
    // handle: task handle
    // period: in milliseconds, use 0 for disabled
    // notes:
    //   the period/frequency change takes effect on the next task cycle
    //   if the period is > the hardware timers maximum period the task is disabled
    void setPeriod(uint8_t handle, unsigned long period) {
      if (handle != 0 && allocated[handle - 1]) {
        task[handle - 1]->setPeriod(period);
      }
    }

    // set process period us
    // handle: task handle
    // period: in milliseconds, use 0 for disabled
    // notes:
    //   the period/frequency change takes effect on the next task cycle
    //   if the period is > the hardware timers maximum period the task is disabled
    void setPeriodMicros(uint8_t handle, unsigned long period) {
      if (handle != 0 && allocated[handle - 1]) {
        task[handle - 1]->setPeriodMicros(period);
      }
    }

    // set process period sub-us
    // handle: task handle
    // period: in sub-microseconds (1/16 microsecond units), use 0 for disabled
    // notes:
    //   the period/frequency change takes effect on the next task cycle
    //   if the period is > the hardware timers maximum period the task is disabled
    void setPeriodSubMicros(uint8_t handle, unsigned long period) {
      if (handle != 0 && allocated[handle - 1]) {
        task[handle - 1]->setPeriodSubMicros(period);
      }
    }

    // change process period Hz
    // handle: task handle
    // freq:  in Hertz, use 0 for disabled
    // notes:
    //   the period/frequency change takes effect on the next task cycle
    //   when setting a frequency the most appropriate setPeriod is used automatically
    //   if the period is > ~49 days (or > the hardware timers maximum period) the task is disabled
    void setFrequency(uint8_t handle, double freq) {
      if (handle != 0 && allocated[handle - 1]) {
        task[handle - 1]->setFrequency(freq);
      }
    }

    // change process duration (milliseconds,) use 0 for disabled
    void setDuration(uint8_t handle, unsigned long duration) {
      if (handle != 0 && allocated[handle - 1]) {
        task[handle - 1]->setDuration(duration);
      }
    }

    // change process repeat (true/false)
    void setRepeat(uint8_t handle, bool repeat) {
      if (handle != 0 && allocated[handle - 1]) {
        task[handle - 1]->setRepeat(repeat);
      }
    }

    // change process priority level (highest 0 to 7 lowest)
    void setPriority(uint8_t handle, uint8_t priority) {
      if (handle != 0 && allocated[handle - 1]) {
        if (priority > 7) return;
        task[handle - 1]->setPriority(priority);
        updateEventRange();
        updatePriorityRange();
      }
    }

    // set the process name
    void setNameStr(uint8_t handle, const char name[]) {
      if (handle != 0 && allocated[handle - 1]) {
        task[handle - 1]->setNameStr(name);
      }
    }

    // set the process name
    char *getNameStr(uint8_t handle) {
      if (handle != 0 && allocated[handle - 1]) {
        return task[handle - 1]->getNameStr();
      }
    }

    // allow search for tasks, returns 0 if no handles are found
    uint8_t getFirstHandle() {
      handleSearch = 255;
      return getNextHandle();
    }

    uint8_t getNextHandle() {
      do {
        handleSearch++;
        if (allocated[handleSearch]) return handleSearch + 1;
      } while (handleSearch < highest_task);
      return false;
    }

#ifdef TASKS_PROFILER_ENABLE
    double getArrivalAvg(uint8_t handle) {
      if (handle != 0 && allocated[handle - 1]) {
        return task[handle - 1]->getArrivalAvg();
      } else return 0;
    }
    double getArrivalMax(uint8_t handle) {
      if (handle != 0 && allocated[handle - 1]) {
        return task[handle - 1]->getArrivalMax();
      } else return 0;
    }
    double getRuntimeTotal(uint8_t handle) {
      if (handle != 0 && allocated[handle - 1]) {
        return task[handle - 1]->getRuntimeTotal();
      } else return 0;
    }
    long getRuntimeTotalCount(uint8_t handle) {
      if (handle != 0 && allocated[handle - 1]) {
        return task[handle - 1]->getRuntimeTotalCount();
      } else return 0;
    }
    double getRuntimeMax(uint8_t handle) {
      if (handle != 0 && allocated[handle - 1]) {
        return task[handle - 1]->getRuntimeMax();
      } else return 0;
    }
#endif

    // runs tasks at the prescribed interval, each call can trigger at most a single process
    // processes that are already running are ignored so it's ok to poll() within a process
    void yield() {
      for (uint8_t priority = 0; priority <= highest_priority; priority++) {
        for (uint8_t i = 0; i <= highest_task; i++) {
          number[priority]++; if (number[priority] > highest_task) number[priority] = 0;
          if (allocated[number[priority]]) {
            if (!task[number[priority]]->isDurationComplete()) {
              if (task[number[priority]]->poll()) return;
            } else remove(number[priority] + 1);
          }
        }
      }
    }

  private:
    // keep track of the range of priorities so we don't waste cycles looking at empty ones
    void updatePriorityRange() {
      highest_priority = 0;
      for (uint8_t e = 0; e <= highest_task; e++) {
        uint8_t p = task[e]->getPriority();
        if (p > highest_priority) highest_priority = p;
      }
    }
    // keep track of the range of tasks so we don't waste cycles looking at empty ones
    void updateEventRange() {
      // scan for highest task
      highest_task = 0;
      for (uint8_t e = TASKS_MAX - 1; e >= 0 ; e--) {
        if (allocated[e]) {
          highest_task = e;
          break;
        }
      }
    }

    uint8_t highest_task     = 0;
    uint8_t highest_priority = 0;
    uint8_t count            = 0;
    uint8_t num_tasks        = 0;
    uint8_t number[8]        = {255, 255, 255, 255, 255, 255, 255, 255};
    bool    allocated[TASKS_MAX];
    uint8_t handleSearch     = 255;
    Task    *task[TASKS_MAX];
};

Tasks tasks;

// short P macro to embed polling
#define Y tasks.yield()
