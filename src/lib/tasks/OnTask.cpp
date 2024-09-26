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

// this line allows configuration of OnTask from the a given project, if this library is used stand-alone remove it
#include "../../Common.h"

// Enabling the profiler inserts time logging code into the process calls.  This provides the average
// and largest start time delta and run time for each process.  To enable the profiler uncomment:
//#define TASKS_PROFILER_ENABLE

// Normally tasks are skipped if they become too late, to instead delay them for later processing uncomment:
//#define TASKS_QUEUE_MISSED

#include "OnTask.h"

#include "HAL_PROFILER.h"
#include "HAL_HWTIMERS.h"

#define roundPeriod(x) ((unsigned long)((x)+(double)0.5L))

unsigned char _task_postpone = false;
volatile unsigned long _task_ppsAverageSubMicros = 16000000UL;
double _task_masterFrequencyRatio = 1.0L;

// Task object
Task::Task(uint32_t period, uint32_t duration, bool repeat, uint8_t priority, void (*callback)()) {
  idle = period == 0;
  this->period   = period;
  period_units   = PU_MILLIS;
  this->duration = duration;
  this->repeat   = repeat;
  if (!repeat) immediate = false;
  this->priority = priority;
  this->callback = callback;
  start_time     = millis();
  next_task_time = start_time + period;
  strcpy(processName, "");
}

Task::~Task() {
  switch (hardware_timer) {
    case 1: HAL_HWTIMER1_DONE(); break;
    case 2: HAL_HWTIMER2_DONE(); break;
    case 3: HAL_HWTIMER3_DONE(); break;
    case 4: HAL_HWTIMER4_DONE(); break;
  }
}

bool Task::requestHardwareTimer(uint8_t num, uint8_t hwPriority) {
  if (num < 1 || num > 4) { DLF("ERR: Task::requestHardwareTimer(), timer number out of range"); return false; }
  if (repeat != true) { DLF("ERR: Task::requestHardwareTimer(), repeat must be true"); return false; }
  if (priority != 0) { DLF("ERR: Task::requestHardwareTimer(), s/w priority must be 0 (highest)"); return false; }
  switch (num) {
    case 1:
      if (HAL_HWTIMER1_FUN != NULL) { DLF("ERR: Task::requestHardwareTimer(), channel1 already in use"); return false; }
    break;
    case 2:
      if (HAL_HWTIMER2_FUN != NULL) { DLF("ERR: Task::requestHardwareTimer(), channel2 already in use"); return false; }
    break;
    case 3:
      if (HAL_HWTIMER3_FUN != NULL) { DLF("ERR: Task::requestHardwareTimer(), channel3 already in use"); return false; }
    break;
    case 4:
      if (HAL_HWTIMER4_FUN != NULL) { DLF("ERR: Task::requestHardwareTimer(), channel4 already in use"); return false; }
    break;
  }

  unsigned long hardware_timer_period = period;
  if (period_units == PU_NONE) hardware_timer_period = 0; else
  if (period_units == PU_MILLIS) hardware_timer_period *= 16000UL; else
  if (period_units == PU_MICROS) hardware_timer_period *= 16UL;

  HAL_HWTIMER_PREPARE_PERIOD(num, roundPeriod((double)hardware_timer_period*_task_masterFrequencyRatio));

  bool success = true;
  switch (num) {
    case 1:
      HAL_HWTIMER1_FUN = callback;
      if (!HAL_HWTIMER1_INIT(hwPriority)) { success = false; HAL_HWTIMER1_FUN = NULL; }
    break;
    case 2:
      HAL_HWTIMER2_FUN = callback;
      if (!HAL_HWTIMER2_INIT(hwPriority)) { success = false; HAL_HWTIMER2_FUN = NULL; }
    break;
    case 3:
      HAL_HWTIMER3_FUN = callback;
      if (!HAL_HWTIMER3_INIT(hwPriority)) { success = false; HAL_HWTIMER3_FUN = NULL; }
    break;
    case 4:
      HAL_HWTIMER4_FUN = callback;
      if (!HAL_HWTIMER4_INIT(hwPriority)) { success = false; HAL_HWTIMER4_FUN = NULL; }
    break;
  }
  if (!success) { DF("ERR: Task::requestHardwareTimer(), HAL_HWTIMER"); D(num); DLF("_INIT() failed"); return false; }
  hardware_timer = num;

  period = hardware_timer_period;
  period_units = PU_SUB_MICROS;
  next_period = period;
  next_period_units = PU_NONE;
  return true;
}

void Task::setCallback(void (*callback)()) {
  this->callback = callback;
  noInterrupts();
  switch (hardware_timer) {
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

void Task::setTimingMode(TimingMode mode) {
  timingMode = mode;
}

bool Task::poll() {
  if (hardware_timer || running) return false;

  if (period != 0) {
    unsigned long t, time_to_next_task;

    if (period_units == PU_MICROS) t = micros(); else t = millis();
    if (immediate) { immediate = false; next_task_time = t; }
    time_to_next_task = next_task_time - t;

    if ((long)time_to_next_task < 0) {
      running = true;

      TASKS_PROFILER_PREFIX;
      callback();
      TASKS_PROFILER_SUFFIX;
    
      running = false;

      if (_task_postpone) { _task_postpone = false; return false; }

      // set timing for guaranteed minimum (or gap) period
      if (timingMode != TM_BALANCED) {
        if (timingMode == TM_GAP) {
          if (period_units == PU_MICROS) t = micros(); else t = millis();
        }
        time_to_next_task = 0;
      }

      // adopt next period
      if (next_period_units != PU_NONE) {
        time_to_next_task = 0;
        period = roundPeriod((double)next_period*_task_masterFrequencyRatio);
        period_units = next_period_units;
        next_period_units = PU_NONE;
      }

      // set adjusted period
      #ifndef TASKS_QUEUE_MISSED
        if ((long)(period + time_to_next_task) < 0) time_to_next_task = -period;
      #endif
      next_task_time = t + (long)(period + time_to_next_task);
      if (!repeat) period = 0;
    }
  } else immediate = true;

  return false;
}

void Task::refreshPeriod() {
  if (hardware_timer) {
    next_period_units = PU_SUB_MICROS;
    setHardwareTimerPeriod();
  }
}

void Task::setPeriod(unsigned long period, PeriodUnits units) {
  if (hardware_timer) {
    next_period = period;
    next_period_units = units;
    setHardwareTimerPeriod();
  } else {
    if (units == PU_SUB_MICROS) { period = lroundf(period/16.0F); units = PU_MICROS; }
    if (units == PU_MILLIS && period < 1000) { period *= 1000; units = PU_MICROS; }
    if (this->period == 0) {
      idle = true;
      this->period = period;
      period_units = units;
      next_period_units = PU_NONE;
    } else {
      idle = false;
      next_period = period;
      next_period_units = units;
    }
  }
}

void Task::setFrequency(float freq) {
  if (freq > 0.0F) {
    freq = 1.0F / freq;            // seconds per call
    float f = freq * 16000000.0F;  // sub-micros per call
    if (f <= 4294967295.0F) {
      setPeriod(lroundf(f), PU_SUB_MICROS);
      return;
    }
    f = freq * 1000000.0F;         // micros per call
    if (f <= 4294967295.0F) {
      setPeriod(lroundf(f), PU_MICROS);
      return;
    }
    f = freq * 1000.0F;            // millis per call
    if (f <= 4294967295.0F) {
      setPeriod(lroundf(f));
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
  if (hardware_timer) return;
  this->repeat = repeat;
}

void Task::setPriority(bool priority) {
  if (hardware_timer) return;
  this->priority = priority;
}

uint8_t Task::getPriority() {
  return priority;
}

void Task::setNameStr(const char name[]) {
  strncpy(processName, name, 7);
  processName[7] = 0;
}

char* Task::getNameStr() {
  return processName;
}

#ifdef TASKS_PROFILER_ENABLE
float Task::getArrivalAvg() {
  if (hardware_timer) return 0;
  if (average_arrival_time_count == 0) return 0;
  float value = -average_arrival_time/average_arrival_time_count;
  average_arrival_time = 0;
  average_arrival_time_count = 0;
  return value;
}
float Task::getArrivalMax() {
  if (hardware_timer) return 0;
  float value = max_arrival_time;
  max_arrival_time = 0;
  return value;
}
float Task::getRuntimeTotal() {
  if (hardware_timer) { noInterrupts(); total_runtime = _task_total_runtime[hardware_timer-1]; _task_total_runtime[hardware_timer-1] = 0; total_runtime_count=_task_total_runtime_count[hardware_timer-1]; interrupts(); };
  float value = total_runtime;
  if (value > 2147483647) value = 0.0;
  total_runtime = 0;
  return value;
}
long Task::getRuntimeTotalCount() {
  if (hardware_timer) { noInterrupts(); total_runtime_count = _task_total_runtime_count[hardware_timer-1]; _task_total_runtime_count[hardware_timer-1] = 0; interrupts(); };
  long value = total_runtime_count;
  total_runtime_count = 0;
  return value;
}
float Task::getRuntimeMax() {
  if (hardware_timer) { noInterrupts(); max_runtime = _task_max_runtime[hardware_timer-1]; _task_max_runtime[hardware_timer-1] = 0; interrupts(); };
  float value = max_runtime;
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

    unsigned long lastPeriod = period;

    period = roundPeriod((double)next_period*_task_masterFrequencyRatio);
    HAL_HWTIMER_PREPARE_PERIOD(hardware_timer, period);

    // if the current period is > 0.1 seconds and the new period < lastPeriod adopt the new rate immediately
    if (lastPeriod > 1600000UL && period < lastPeriod) {
      switch (hardware_timer) {
        case 1: HAL_HWTIMER1_SET_PERIOD(); break;
        case 2: HAL_HWTIMER2_SET_PERIOD(); break;
        case 3: HAL_HWTIMER3_SET_PERIOD(); break;
        case 4: HAL_HWTIMER4_SET_PERIOD(); break;
      }
    }
  }
}

void tasksMonitor() {
  static unsigned long _lastPpsAverageSubMicros = 16000000UL;
  if (_lastPpsAverageSubMicros != _task_ppsAverageSubMicros) {
    _task_masterFrequencyRatio = (double)_task_ppsAverageSubMicros/16000000.0L;

    uint8_t handle = tasks.getFirstHandle();
    for (int i = 0; i < TASKS_MAX; i++) {
      if (handle == 0) break;
      tasks.refreshPeriod(handle);
      handle = tasks.getNextHandle(handle);
    }

    _lastPpsAverageSubMicros = _task_ppsAverageSubMicros;
  }
}

Tasks::Tasks() {
  // clear tasks
  for (uint8_t c = 0; c < TASKS_MAX; c++) {
    task[c] = NULL;
    allocated[c] = false;
  }

  // start the task monitor
  tasks.add(1000, 0, true, 7, tasksMonitor, "TaskMtr");
}

Tasks::~Tasks() {
  // find active tasks and remove
  for (uint8_t c = 0; c < TASKS_MAX; c++) {
    if (allocated[c]) delete task[c];
  }
}

uint8_t Tasks::add(uint32_t period, uint32_t duration, bool repeat, uint8_t priority, void (*callback)()) {
  // check priority
  if (priority > 7) return false;
  if (priority > highest_priority) highest_priority = priority;

  // find the next free task
  int8_t e = -1;
  for (uint8_t c = 0; c < TASKS_MAX; c++) {
    if (!allocated[c]) { e = c; break; }
  }
  // no tasks available
  if (e == -1) return false;

  // create the task handler
  task[e] = new Task(period, duration, repeat, priority, callback);
  if (task[e] != NULL) allocated[e] = true; else return false;

  updateEventRange();
  return e + 1;
}

uint8_t Tasks::add(uint32_t period, uint32_t duration, bool repeat, uint8_t priority, void (*callback)(), const char name[]) {
  uint8_t handle = add(period, duration, repeat, priority, callback);
  setNameStr(handle, name);
  return handle;
}

bool Tasks::requestHardwareTimer(uint8_t handle) {
  return requestHardwareTimer(handle, 128);
}

bool Tasks::requestHardwareTimer(uint8_t handle, uint8_t hwPriority) {
  if (handle != 0 && allocated[handle - 1] && TASKS_HWTIMERS > 0) {
    for (int num = 0; num < TASKS_HWTIMERS; num++) {
      if (!hardware_timer_allocated[num]) {
        hardware_timer_allocated[num] = task[handle - 1]->requestHardwareTimer(num + 1, hwPriority);
        return hardware_timer_allocated[num];
      }
    }
  }
  return false;
}

bool Tasks::setCallback(uint8_t handle, void (*callback)()) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setCallback(callback);
    return true;
  } else return false;
}

bool Tasks::setTimingMode(uint8_t handle, TimingMode mode) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setTimingMode(mode);
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

void Tasks::refreshPeriod(uint8_t handle) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->refreshPeriod();
  }
}

void Tasks::setPeriod(uint8_t handle, unsigned long period) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setPeriod(period);
  }
}

void Tasks::setPeriodMicros(uint8_t handle, unsigned long period) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setPeriod(period, PU_MICROS);
  }
}

void Tasks::setPeriodSubMicros(uint8_t handle, unsigned long period) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setPeriod(period, PU_SUB_MICROS);
  }
}

void Tasks::setFrequency(uint8_t handle, double freq) {
  if (handle != 0 && allocated[handle - 1]) {
    task[handle - 1]->setFrequency(freq);
  }
}

IRAM_ATTR void Tasks::setPeriodRatioSubMicros(unsigned long value) {
  _task_ppsAverageSubMicros = value;
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
  static char empty[1];
  strcpy(empty, "");
  if (handle != 0 && allocated[handle - 1]) {
    return task[handle - 1]->getNameStr();
  } else return empty;
}

uint8_t Tasks::getFirstHandle() {
  return getNextHandle(0);
}

uint8_t Tasks::getNextHandle(uint8_t handle) {
  do {
    if (allocated[handle]) {
      return handle + 1;
    }
  } while (++handle < highest_task);
  return false;
}

uint8_t Tasks::getHandleByName(const char name[]) {
  char *candidate;
  uint8_t handle = getFirstHandle();
  while (handle) {
    candidate = getNameStr(handle);
    if (strstr(candidate, name)) {
      if (strlen(candidate) == strlen(name)) break;
    }
    handle = getNextHandle(handle);
  }

  return handle;
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
    ::yield();
    for (uint8_t priority = 0; priority <= highest_priority; priority++) {
      uint8_t last_priority = highest_active_priority;
      if (priority < highest_active_priority) {
        highest_active_priority = priority;
        for (uint8_t i = 0; i <= highest_task; i++) {
          if (++number[priority] > highest_task) number[priority] = 0;
          if (allocated[number[priority]]) {
            if (task[number[priority]]->getPriority() == priority) {
              if (task[number[priority]]->isDurationComplete()) { remove(number[priority] + 1); highest_active_priority = last_priority; return; }
              if (task[number[priority]]->poll()) { highest_active_priority = last_priority; return; }
            }
          }
        }
        highest_active_priority = last_priority;
      }
    }
  }
#else
  void Tasks::yield() {
    for (uint8_t priority = 0; priority <= highest_priority; priority++) {
      for (uint8_t i = 0; i <= highest_task; i++) {
        if (++number[priority] > highest_task) number[priority] = 0;
        if (allocated[number[priority]]) {
          if (task[number[priority]]->getPriority() == priority) {
            if (task[number[priority]]->isDurationComplete()) { remove(number[priority] + 1); return; }
            if (task[number[priority]]->poll()) return;
          }
        }
      }
    }
  }
#endif

void Tasks::yield(unsigned long milliseconds) {
  unsigned long endTime = millis() + milliseconds;
  while ((long)(millis() - endTime) < 0) this->yield();
}

void Tasks::yieldMicros(unsigned long microseconds) {
  unsigned long endTime = micros() + microseconds;
  while ((long)(micros() - endTime) < 0) this->yield();
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

Tasks tasks;
