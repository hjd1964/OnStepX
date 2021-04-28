/*
 * Title       OnStepX
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
 * Description:
 *   Full featured stepper motor telescope microcontroller for Equatorial and
 *   Alt-Azimuth mounts, with the LX200 derived command set.
 *
 * Author: Howard Dutton
 *   http://www.stellarjourney.com
 *   hjd1964@gmail.com
 *
 * Revision history, and newer versions:
 *   See GitHub: https://github.com/hjd1964/OnStep
 *
 * Documentation:
 *   https://groups.io/g/onstep/wiki/home
 *
 * Discussion, Questions, ...etc
 *   https://groups.io/g/onstep
 */

// Use Config.h to configure OnStep to your requirements

// See Constants.h for version information

#include "Constants.h"
#include "Config.h"
#include "ConfigX.h"

#include "src/HAL/HAL.h"
NVS nv;

#include "src/debug/Debug.h"

#include "src/tasks/OnTask.h"
Tasks tasks;

#include "src/telescope/Telescope.h"
extern Telescope telescope;

#ifdef SERIAL_A
  extern void processCmdsA();
#endif
#ifdef SERIAL_B
  extern void processCmdsB();
#endif
#ifdef SERIAL_C
  extern void processCmdsC();
#endif
#ifdef SERIAL_D
  extern void processCmdsD();
#endif
#ifdef SERIAL_ST4
  extern void processCmdsST4();
#endif
#if SERIAL_BT_MODE == SLAVE
  extern void processCmdsBT();
#endif
#ifdef SERIAL_IP
  extern void processCmdsIP();
#endif

#if DEBUG == PROFILER
  extern void profiler();
#endif

#if DEBUG == CONSOLE
  extern void debugConsole();
#endif

void systemServices() {
  nv.poll();
}

void setup() {
  #if DEBUG != OFF
    SERIAL_DEBUG.begin(SERIAL_DEBUG_BAUD);
    delay(2000);
  #endif

  VLF("MSG: Setup, HAL initalize");
  HAL_INIT();

  telescope.init();

  // System services
  // add task for system services, runs at 10ms intervals so commiting 1KB of NV takes about 10 seconds
  VF("MSG: Setup, start system service task (rate 10ms priority 7)... ");
  if (tasks.add(10, 0, true, 7, systemServices, "SysSvcs")) { VL("success"); } else { VL("FAILED!"); }

  // Command processing
  // add tasks to process commands
  // period ms (0=idle), duration ms (0=forever), repeat, priority (highest 0..7 lowest), task_handle
  uint8_t handle;
  #ifdef HAL_SLOW_PROCESSOR
    long comPollRate = 2000;
  #else
    long comPollRate = 250;
  #endif
  #ifdef SERIAL_A
    VF("MSG: Setup, start command channel A task (priority 6)... ");
    handle = tasks.add(0, 0, true, 6, processCmdsA, "PrcCmdA");
    if (handle) { VL("success"); } else { VL("FAILED!"); }
    tasks.setPeriodMicros(handle, comPollRate);
  #endif
  #ifdef SERIAL_B
    VF("MSG: Setup, start command channel B task (priority 6)... ");
    handle = tasks.add(0, 0, true, 6, processCmdsB, "PrcCmdB");
    if (handle) { VL("success"); } else { VL("FAILED!"); }
    tasks.setPeriodMicros(handle, comPollRate);
  #endif
  #ifdef SERIAL_C
    VF("MSG: Setup, start command channel C task (priority 6)... ");
    handle = tasks.add(0, 0, true, 6, processCmdsC, "PrcCmdC");
    if (handle) { VL("success"); } else { VL("FAILED!"); }
    tasks.setPeriodMicros(handle, comPollRate);
  #endif
  #ifdef SERIAL_D
    VF("MSG: Setup, start command channel D task (priority 6)... ");
    handle = tasks.add(0, 0, true, 6, processCmdsD, "PrcCmdD")) { VL("success"); } else { VL("FAILED!"); }
    if (handle) { VL("success"); } else { VL("FAILED!"); }
    tasks.setPeriodMicros(handle, comPollRate);
  #endif
  #ifdef SERIAL_ST4
    VF("MSG: Setup, start command channel ST4 task (priority 6)... ");
    if (tasks.add(3, 0, true, 6, processCmdsST4, "PrcCmdS")) { VL("success"); } else { VL("FAILED!"); }
  #endif
  #if SERIAL_BT_MODE == SLAVE
    VF("MSG: Setup, start command channel BT task (priority 6)... ");
    handle = tasks.add(0, 0, true, 6, processCmdsBT, "PrcCmdT");
    if (handle) { VL("success"); } else { VL("FAILED!"); }
    tasks.setPeriodMicros(handle, comPollRate);
  #endif
  #ifdef SERIAL_IP
    VF("MSG: Setup, start command channel IP task (priority 6)... ");
    if (tasks.add(1, 0, true, 6, processCmdsIP, "PrcCmdI")) { VL("success"); } else { VL("FAILED!"); }
  #endif

  tasks.yield(5000);


  // ------------------------------------------------------------------------------------------------
  // add task manager debug events
  #if DEBUG == PROFILER
    tasks.add(142, 0, true, 7, profiler, "Profilr");
  #endif
  
  #if DEBUG == CONSOLE
    tasks.add(1000, 0, true, 7, debugConsole);
  #endif
}

void loop() {
  tasks.yield();
}
