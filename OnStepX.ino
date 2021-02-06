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

// firmware info, these are returned by the ":GV?#" commands
#define FirmwareDate          __DATE__
#define FirmwareVersionMajor  00
#define FirmwareVersionMinor  02       // minor version 00 to 99
#define FirmwareVersionPatch  "a"      // for example major.minor patch: 1.03c
#define FirmwareVersionConfig 01       // internal, for tracking configuration file changes
#define FirmwareName          "OnStepX"
#define FirmwareTime          __TIME__

// Enable additional debugging and/or status messages on the specified DebugSer port
// Note that the DebugSer port cannot be used for normal communication with OnStep
#define DEBUG OFF                      // default OFF, use "ON" for background errors only, use "VERBOSE" for all errors and status messages,
                                       // use "CONSOLE" for VT100 debug console, use "PROFILER" for VT100 task profiler
#define SERIAL_DEBUG          SERIAL_A // default SERIAL_A... or use Serial4, for example (always 9600 baud)
#define SERIAL_DEBUG_BAUD     115200

#include "Constants.h"
#include "Config.h"
#include "src/pinmaps/Models.h"
#include "src/HAL/HAL.h"
#include "src/debug/Debug.h"

#define  TASKS_SKIP_MISSED
#define  TASKS_HWTIMER1_ENABLE         // only the Mega2560 hardware timers are tested and seem to work
#define  TASKS_HWTIMER2_ENABLE         // if the Teensy, etc. don't work comment these out to use the
#define  TASKS_HWTIMER3_ENABLE         // software task scheduler instead
#include "src/tasks/OnTask.h"
#ifdef TASKS_PROFILER_ENABLE
  #include "src/tasks/Profiler.h"
#endif

#include "Observatory.h"
#include "Telescope.h"
#include "Axis.h"
#include "Transform.h"
#include "src/lib/BufferCmds.h"
#include "ProcessCommands.h"
#include "src/debug/Console.h"

void setup() {
  uint8_t handle;

  #if DEBUG != OFF
    SERIAL_DEBUG.begin(SERIAL_DEBUG_BAUD);
  #endif

  observatory.init();

  // ------------------------------------------------------------------------------------------------
  // add an event to process commands
  // period ms (0=idle), duration ms (0=forever), repeat, priority (highest 0..7 lowest), task_handle
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

  // ------------------------------------------------------------------------------------------------

  // setup axis1
#if AXIS1_DRIVER_MODEL != OFF
  handle = tasks.add(0, 0, true, 0, moveAxis1, "MoveAx1"); tasks.requestHardwareTimer(handle,1,0);
  axis1.init(handle);
  axis1.setStepsPerMeasure(radToDeg(AXIS1_STEPS_PER_DEGREE));
  axis1.setMinCoordinate(degToRad(-180.0));
  axis1.setMaxCoordinate(degToRad(180.0));
  axis1.setInstrumentCoordinate(degToRad(90.0));
  axis1.enable(true);
#endif

  // setup axis2
#if AXIS2_DRIVER_MODEL != OFF
  handle = tasks.add(0, 0, true , 0, moveAxis2, "MoveAx2"); tasks.requestHardwareTimer(handle,2,0);
  axis2.init(handle);
  axis2.setStepsPerMeasure(radToDeg(AXIS2_STEPS_PER_DEGREE));
  axis2.setMinCoordinate(degToRad(-90.0));
  axis2.setMaxCoordinate(degToRad(90.0));
  axis2.setInstrumentCoordinate(degToRad(90.0));
  axis2.enable(true);
#endif

  // ------------------------------------------------------------------------------------------------
  // move in measures (radians) per second, tracking_enabled
  axis1.setFrequencyMax(degToRad(4.0));
  axis1.setFrequency(arcsecToRad(15.0*SIDEREAL_RATIO));
  axis1.setTracking(true);

  // ------------------------------------------------------------------------------------------------
  // task manager debug events
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
