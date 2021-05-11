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

// this line allows configuration of OnTask from OnStepX, if this library is used outside of OnStepX remove it
#include "../OnStepX.h"

// To enable the option to use a given hardware timer (1..4), if available, uncomment that line:
//#define TASKS_HWTIMER1_ENABLE
//#define TASKS_HWTIMER2_ENABLE
//#define TASKS_HWTIMER3_ENABLE
//#define TASKS_HWTIMER4_ENABLE

// Enabling the profiler inserts time logging code into the process calls.  This provides the average
// and largest start time delta and run time for each process.  To enable the profiler uncomment:
//#define TASKS_PROFILER_ENABLE

// Normally tasks are skipped if they become too late, to instead delay them for later processing uncomment:
//#define TASKS_QUEUE_MISSED

#include "HAL_PROFILER.h"
#include "HAL_HWTIMERS.h"
#include "OnTask.h"

unsigned char __task_mutex[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char __task_postpone = false;

// Task object
Task::Task(uint32_t period, uint32_t duration, bool repeat, uint8_t priority, void (*volatile callback)()) {
  this->period   = period;
  period_units   = PU_MILLIS;
  this->duration = duration;
  this->repeat   = repeat;
  this->priority = priority;
  this->callback = callback;
  start_time     = millis();
  next_task_time = start_time + period;
}

Task::~Task() {
  switch (hardwareTimer) {
    case 1: HAL_HWTIMER1_DONE(); break;
    case 2: HAL_HWTIMER2_DONE(); break;
    case 3: HAL_HWTIMER3_DONE(); break;
    case 4: HAL_HWTIMER4_DONE(); break;
  }
}

bool Task::requestHardwareTimer(uint8_t num, uint8_t hwPriority) {
  if (num < 1 || num > 4) { DL("ERR: Task::requestHardwareTimer(), timer number out of range"); return false; }
  if (repeat != true) { DL("ERR: Task::requestHardwareTimer(), repeat must be true"); return false; }
  if (priority != 0) { DL("ERR: Task::requestHardwareTimer(), s/w priority must be 0 (highest)"); return false; }

  unsigned long hwPeriod = period;
  if (period_units == PU_NONE) hwPeriod = 0; else if (period_units == PU_MILLIS) hwPeriod *= 16000UL; else if (period_units == PU_MICROS) hwPeriod *= 16UL;
  HAL_HWTIMER_PREPARE_PERIOD(num, hwPeriod);

  switch (num) {
    case 1:
      if (HAL_HWTIMER1_FUN != NULL) { DL("ERR: Task::requestHardwareTimer(), HAL_HWTIMER1_FUN not NULL"); return false; }
      HAL_HWTIMER1_FUN = callback;
      if (!HAL_HWTIMER1_INIT(hwPriority)) { DL("ERR: Task::requestHardwareTimer(), HAL_HWTIMER1_INIT() failed"); return false; }
    break;
    case 2:
      if (HAL_HWTIMER2_FUN != NULL) { DL("ERR: Task::requestHardwareTimer(), HAL_HWTIMER2_FUN not NULL"); return false; }
      HAL_HWTIMER2_FUN = callback;
      if (!HAL_HWTIMER2_INIT(hwPriority)) { DL("ERR: Task::requestHardwareTimer(), HAL_HWTIMER2_INIT() failed"); return false; }
    break;
    case 3:
      if (HAL_HWTIMER3_FUN != NULL) { DL("ERR: Task::requestHardwareTimer(), HAL_HWTIMER3_FUN not NULL"); return false; }
      HAL_HWTIMER3_FUN = callback;
      if (!HAL_HWTIMER3_INIT(hwPriority)) { DL("ERR: Task::requestHardwareTimer(), HAL_HWTIMER3_INIT() failed"); return false; }
    break;
    case 4:
      if (HAL_HWTIMER4_FUN != NULL) { DL("ERR: Task::requestHardwareTimer(), HAL_HWTIMER4_FUN not NULL"); return false; }
      HAL_HWTIMER4_FUN = callback;
      if (HAL_HWTIMER4_INIT(hwPriority)) { DL("ERR: Task::requestHardwareTimer(), HAL_HWTIMER4_INIT() failed"); return false; }
    break;
  }
  hardwareTimer = num;
  period = hwPeriod;
  period_units = PU_SUB_MICROS;
  return true;
}

void Task::setCallback(void (*volatile callback)()) {
  this->callback = callback;
  noInterrupts();
  switch (hardwareTimer) {
    case 1:
      HAL_HWTIMER1_FUN = callback;
    break;
    case 2:
      HAL_HWTIMER2_FUN = callback;
    break;
    case 3:
      HAL_HWTIMER3_FUN = callback;
    break;
    case 4:
      HAL_HWTIMER4_FUN = callback;
    break;
  }
  interrupts();
}

bool Task::poll() {
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
    
      running = false;

      if (__task_postpone) { __task_postpone = false; return false; }

      // adopt next period
      if (next_period_units != PU_NONE) {
        time_to_next_task = 0;
        period = next_period;
        period_units = next_period_units;
        next_period_units = PU_NONE;
      }

      // set adjusted period
      #ifndef TASKS_QUEUE_MISSED
        if ((long)(period + time_to_next_task) < 0) time_to_next_task = -period;
//      if (-time_to_next_task > period) time_to_next_task = period;
      #endif
      next_task_time = t + (long)(period + time_to_next_task);
      if (!repeat) period = 0;
    }
  }
  return false;
}

void Task::setPeriod(unsigned long period) {
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

void Task::setPeriodMicros(unsigned long period) {
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

void Task::setPeriodSubMicros(unsigned long period) {
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

void Task::setFrequency(double freq) {
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

void Task::setDuration(unsigned long duration) {
  this->duration = duration;
}

bool Task::isDurationComplete() {
  return (duration > 0 && ((long)(millis() - (start_time + duration)) >= 0));
}

void Task::setDurationComplete() {
  duration = 1;
  start_time--;
}

void Task::setRepeat(bool repeat) {
  if (hardwareTimer) return;
  this->repeat = repeat;
}

void Task::setPriority(bool priority) {
  if (hardwareTimer) return;
  this->priority = priority;
}

uint8_t Task::getPriority() {
  return priority;
}

void Task::setNameStr(const char name[]) {
  strncpy(processName, name, 7);
}

char* Task::getNameStr() {
  return processName;
}

#ifdef TASKS_PROFILER_ENABLE
double Task::getArrivalAvg() {
  if (hardwareTimer) return 0;
  if (average_arrival_time_count == 0) return 0;
  double value = -average_arrival_time/average_arrival_time_count;
  average_arrival_time = 0;
  average_arrival_time_count = 0;
  return value;
}
double Task::getArrivalMax() {
  if (hardwareTimer) return 0;
  double value = max_arrival_time;
  max_arrival_time = 0;
  return value;
}
double Task::getRuntimeTotal() {
  if (hardwareTimer) { noInterrupts(); total_runtime = _task_total_runtime[hardwareTimer-1]; _task_total_runtime[hardwareTimer-1] = 0; total_runtime_count=_task_total_runtime_count[hardwareTimer-1]; interrupts(); };
  double value = total_runtime;
  total_runtime = 0;
  return value;
}
long Task::getRuntimeTotalCount() {
  if (hardwareTimer) { noInterrupts(); total_runtime_count = _task_total_runtime_count[hardwareTimer-1]; _task_total_runtime_count[hardwareTimer-1] = 0; interrupts(); };
  long value = total_runtime_count;
  total_runtime_count = 0;
  return value;
}
double Task::getRuntimeMax() {
  if (hardwareTimer) { noInterrupts(); max_runtime = _task_max_runtime[hardwareTimer-1]; _task_max_runtime[hardwareTimer-1] = 0; interrupts(); };
  double value = max_runtime;
  max_runtime = 0;
  return value;
}
#endif

void Task::setHardwareTimerPeriod() {
  // adopt next period
  if (next_period_units != PU_NONE) {
    if (next_period_units == PU_MILLIS) next_period *= 16000UL; else if (next_period_units == PU_MICROS) next_period *= 16UL;
    next_period_units = PU_NONE;
    period_units = PU_SUB_MICROS;
    period = next_period;
    HAL_HWTIMER_PREPARE_PERIOD(hardwareTimer, period);
  }
}

Tasks::Tasks() {
  // clear tasks
  for (uint8_t c = 0; c < TASKS_MAX; c++) {
    task[c] = NULL;
    allocated[c] = false;
  }
  
  for (uint8_t c = 0; c < 8; c++) __task_mutex[c] = 0;
}

Tasks::~Tasks() {
  // find active tasks and remove
  for (uint8_t c = 0; c < TASKS_MAX; c++) {
    if (allocated[c]) delete task[c];
  }
}

uint8_t Tasks::add(uint32_t period, uint32_t duration, bool repeat, uint8_t priority, void (*volatile callback)()) {
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

uint8_t Tasks::add(uint32_t period, uint32_t duration, bool repeat, uint8_t priority, void (*volatile callback)(), const char name[]) {
  uint8_t handle = add(period, duration, repeat, priority, callback);
  setNameStr(handle, name);
  return handle;
}

bool Tasks::requestHardwareTimer(uint8_t handle, uint8_t num) {
  return requestHardwareTimer(handle, num, 128);
}

bool Tasks::requestHardwareTimer(uint8_t handle, uint8_t num, uint8_t hwPriority) {
  if (handle != 0 && allocated[handle - 1]) {
    return task[handle - 1]->requestHardwareTimer(num, hwPriority);
  } else return false;
}

bool Tasks::setCallback(uint8_t handle, void (*volatile callback)()) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setCallback(callback);
    return true;
  } else return false;
}

void Tasks::remove(uint8_t handle) {
  if (handle != 0 && allocated[handle - 1]) {
    delete task[handle - 1];
    allocated[handle - 1] = false;
    updateEventRange();
    updatePriorityRange();
  }
}

void Tasks::setPeriod(uint8_t handle, unsigned long period) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setPeriod(period);
  }
}

void Tasks::setPeriodMicros(uint8_t handle, unsigned long period) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setPeriodMicros(period);
  }
}

void Tasks::setPeriodSubMicros(uint8_t handle, unsigned long period) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setPeriodSubMicros(period);
  }
}

void Tasks::setFrequency(uint8_t handle, double freq) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setFrequency(freq);
  }
}

void Tasks::setDuration(uint8_t handle, unsigned long duration) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setDuration(duration);
  }
}

void Tasks::setDurationComplete(uint8_t handle) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setDurationComplete();
  }
}

void Tasks::setRepeat(uint8_t handle, bool repeat) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setRepeat(repeat);
  }
}

void Tasks::setPriority(uint8_t handle, uint8_t priority) {
  if (handle != 0 && allocated[handle - 1]) {
    if (priority > 7) return;
    task[handle - 1]->setPriority(priority);
    updateEventRange();
    updatePriorityRange();
  }
}

void Tasks::setNameStr(uint8_t handle, const char name[]) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setNameStr(name);
  }
}

char* Tasks::getNameStr(uint8_t handle) {
  static char empty[1]; strcpy(empty,"");
  if (handle != 0 && allocated[handle - 1]) {
    return task[handle - 1]->getNameStr();
  } else return empty;
}

uint8_t Tasks::getFirstHandle() {
  handleSearch = 255;
  return getNextHandle();
}

uint8_t Tasks::getNextHandle() {
  do {
    handleSearch++;
    if (allocated[handleSearch]) return handleSearch + 1;
  } while (handleSearch < highest_task);
  return false;
}

#ifdef TASKS_PROFILER_ENABLE
  double Tasks::getArrivalAvg(uint8_t handle) {
    if (handle != 0 && allocated[handle - 1]) {
      return task[handle - 1]->getArrivalAvg();
    } else return 0;
  }
  double Tasks::getArrivalMax(uint8_t handle) {
    if (handle != 0 && allocated[handle - 1]) {
      return task[handle - 1]->getArrivalMax();
    } else return 0;
  }
  double Tasks::getRuntimeTotal(uint8_t handle) {
    if (handle != 0 && allocated[handle - 1]) {
      return task[handle - 1]->getRuntimeTotal();
    } else return 0;
  }
  long Tasks::getRuntimeTotalCount(uint8_t handle) {
    if (handle != 0 && allocated[handle - 1]) {
      return task[handle - 1]->getRuntimeTotalCount();
    } else return 0;
  }
  double Tasks::getRuntimeMax(uint8_t handle) {
    if (handle != 0 && allocated[handle - 1]) {
      return task[handle - 1]->getRuntimeMax();
    } else return 0;
  }
#endif

#ifdef TASKS_HIGHER_PRIORITY_ONLY
  void Tasks::yield() {
    for (uint8_t priority = 0; priority <= highest_priority; priority++) {
      uint8_t last_priority = highest_active_priority;
      if (priority < highest_active_priority) {
        highest_active_priority = priority;
        for (uint8_t i = 0; i <= highest_task; i++) {
          number[priority]++; if (number[priority] > highest_task) number[priority] = 0;
          if (allocated[number[priority]]) {
            if (!task[number[priority]]->isDurationComplete()) {
              if (task[number[priority]]->poll()) {
                highest_active_priority = last_priority;
                return;
              }
            }
          } else remove(number[priority] + 1);
        }
        highest_active_priority = last_priority;
      }
    }
  }
#else
  void Tasks::yield() {
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
#endif

void Tasks::yield(unsigned long milliseconds) {
  unsigned long endTime = millis() + milliseconds;
  while ((long)(millis() - endTime) < 0) this->yield();
}

void Tasks::updatePriorityRange() {
  // scan for highest priority
  highest_priority = 0;
  for (uint8_t e = 0; e <= highest_task; e++) {
    uint8_t p = task[e]->getPriority();
    if (p > highest_priority) highest_priority = p;
  }
}

void Tasks::updateEventRange() {
  // scan for highest task handle
  highest_task = 0;
  for (int8_t e = TASKS_MAX - 1; e >= 0 ; e--) {
    if (allocated[e]) {
      highest_task = e;
      break;
    }
  }
}
