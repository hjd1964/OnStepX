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

#if DEBUG == PROFILER
  extern void profiler();
#endif

#if DEBUG == CONSOLE
  extern void debugConsole();
#endif

void setup() {

  #if DEBUG != OFF
    SERIAL_DEBUG.begin(SERIAL_DEBUG_BAUD);
    delay(2000);
  #endif

  DL("MSG: setup, HAL initalize");
  HAL_INIT;
  
  // Command processing
  // add tasks to process commands
  // period ms (0=idle), duration ms (0=forever), repeat, priority (highest 0..7 lowest), task_handle
  DL("MSG: setup, starting command channel tasks");
  #ifdef SERIAL_A
    tasks.add(2, 0, true, 7, processCmdsA, "PrcCmdA");
  #endif
  #ifdef SERIAL_B
    tasks.add(2, 0, true, 7, processCmdsB, "PrcCmdB");
  #endif
  #ifdef SERIAL_C
    tasks.add(2, 0, true, 7, processCmdsC, "PrcCmdC");
  #endif
  #ifdef SERIAL_D
    tasks.add(2, 0, true, 7, processCmdsD, "PrcCmdD");
  #endif
  #ifdef SERIAL_ST4
    tasks.add(2, 0, true, 7, processCmdsST4, "PrcCmdS");
  #endif

  DL("MSG: setup, telescope initalize");
  telescope.init();

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
