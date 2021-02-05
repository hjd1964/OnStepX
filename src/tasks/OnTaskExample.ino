/*
 * Title       OnTask
 * by          Howard Dutton
 *
 * Copyright (C) 2021 Howard Dutton
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
 * Author: Howard Dutton
 *   http://www.stellarjourney.com
 *   hjd1964@gmail.com
 *
 */

// -----------------------------------------------------------------------------------
// some constants
#define OFF 0
#define ON  1

// -----------------------------------------------------------------------------------
// configure settings in this section to experiment, enable/disable the various tasks

#define SERIAL_BAUD                     115200

//      feature name              ON/OFF state
#define CLOCK_TICK                          ON
#define CLOCK_EXTRA_NOTHING                 ON
#define SHOW_CLOCK_EVERY_SEC                OFF
#define SHOW_MESSAGE_FOUR_TIMES             ON
#define SHOW_MESSAGE_10SEC                  ON
#define SHOW_MESSAGE_HALFSEC_FOR_4SEC       ON
#define PROFILER_VT100                      ON
#define SHOW_TASKS_PROFILER_EVERY_SEC       ON

// uncomment the line below to enable using an hardware timer for the CLOCK_TICK 
#define TASKS_HWTIMER1_ENABLE

// -----------------------------------------------------------------------------------

#if SHOW_TASKS_PROFILER_EVERY_SEC == ON
  #define TASKS_PROFILER_ENABLE
#endif

#include "OnTask.h"

#if SHOW_TASKS_PROFILER_EVERY_SEC == ON
  #include "profiler.h"
#endif

// handles to keep track of tasks
int clockCountHandle=0;
int fourTimesHandle =0;
int profilerHandle  =0;

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial1.begin(SERIAL_BAUD);

  Serial.println();
  Serial.println("Creating tasks...");
  Serial.println();

  // this helps show what the parameters are:
  // handle  = tasks.add(period_ms, duration_ms, repeat_true_or_false, priority_0to7, callback_function);
  // success = tasks.requestHardwareTimer(handle, hardware_timer_number_1to4, hardware_timer_priority_0to255);

  // create an task, the callback function "clockCount" is the process for this handler
#if CLOCK_TICK == ON
  clockCountHandle = tasks.add(1, 0, true, 0, clockCount, "ClkCnt");
  Serial.print("ClockCount running every millisecond");
  // move this task to a hardware timer
  bool success = tasks.requestHardwareTimer(clockCountHandle,1);
  if (success) Serial.println(" on HardwareTimer1"); else Serial.println();
#endif

  // create/start another task to throw some extra work at the task manager
#if CLOCK_EXTRA_NOTHING == ON
  tasks.add(1, 0, true, 0, clockExtraNothing, "EtraNth");
#endif

  // create/start another task to show the clock at 1 second intervals
#if SHOW_CLOCK_EVERY_SEC == ON
  tasks.add(1000, 0, true, 2, showClock, "ShowClk");
  Serial.println("ShowClock: running every second");
#endif

  // start another task with a period of 1 second which it then adds 2 seconds to each time it runs (before it stops itself)
#if SHOW_MESSAGE_FOUR_TIMES == ON
  fourTimesHandle=tasks.add(1000, 0, true, 2, fourTimes, "FourTms");
  Serial.println("FourTimes: running every 1,2,4,6 seconds");
#endif

  // create/start another task with a period of 10 seconds
#if SHOW_MESSAGE_10SEC == ON
  tasks.add(10000, 0, true, 4, longCalc, "LngCalc");
  Serial.println("TenSec:    running every 10 seconds");
#endif

  // create/start another task with a period of 0.5 seconds and a duration of 4 seconds
#if SHOW_MESSAGE_HALFSEC_FOR_4SEC == ON
  tasks.add(2000, 4000, true, 3, twoSec, "TwoSecs");
  Serial.println("TwoSec:    running every 2 seconds, for 4 seconds");
  Serial.println();
#endif

  // create/start a task to show the profiler at work
#if SHOW_TASKS_PROFILER_EVERY_SEC == ON
  profilerHandle = tasks.add(250, 0, true, 2, profiler, "Profilr");
  Serial.println("Profiler:  running every second");
  Serial.println();
#endif
}

void loop() {
  tasks.yield();
}

// -----------------------------------------------------------------------------------
// task processes

// runs 1000 times a second in a hardware timer, if available, for a millisecond clock
volatile long cs=0;
void clockCount() {
  cs++;
}

// runs 1000 times a second in the task scheduler, to waste some time
void clockExtraNothing() {
  noInterrupts();
  cs++;
  cs--;
  interrupts();
}

// show the clock time
void showClock() {
  // Y macros allow any higher priority processes to run
  Serial.print("[ShowClock ] timer vs. millis() = "); Y;
  noInterrupts();
  long centiSec = cs;
  interrupts();
  Serial.print((long)(millis()-centiSec)); Y;
  Serial.println(" ms");
}

// show a long process working in the background
void longCalc() {
  Serial.println("[LongCalc  ] started (sending progress to Serial1)");
  for (long j=0; j<100; j++) {
    for (long i=0; i<300; i++) {
      delayMicroseconds(50); Y; // burn some time
    }
    Serial1.print(".");
  }
  Serial1.println(); Y;
  Serial.println("[LongCalc  ] done");
}

// run four times then stop, first at 1 seconds, then 2 seconds, etc.
void fourTimes() {
  static int period = 0;
  if (period <= 4000) {
    period += 2000;
    tasks.setPeriod(fourTimesHandle,period);
    Serial.print("[FourTimes ] next pass in "); Y;
    Serial.print(period); Y;
    Serial.println(" ms");
  } else {
    tasks.setPeriod(fourTimesHandle,0);
    Serial.println("[FourTimes ] done");
  }
}

// run four times then stop, first at 1 seconds, then 2 seconds, etc.
void twoSec() {
  Serial.println("[TwoSec    ] ran"); Y;
}
