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
#define FirmwareVersionMinor  02      // minor version 0 to 99
#define FirmwareVersionPatch  "a"     // for example major.minor patch: 1.3c
#define FirmwareVersionConfig 01      // internal, for tracking configuration file changes
#define FirmwareName          "OnStepX"
#define FirmwareTime          __TIME__

// Enable additional debugging and/or status messages on the specified DebugSer port
// Note that the DebugSer port cannot be used for normal communication with OnStep
#define DEBUG OFF             // default=OFF, use "DEBUG ON" for background errors only, use "DEBUG VERBOSE" for all errors and status messages
#define DebugSer SerialA      // default=SerialA, or Serial4 for example (always 9600 baud)

#include "Constants.h"
#include "Config.h"
#include "src/pinmaps/Models.h"
#include "src/HAL/HAL.h"

// Helper macros for debugging, with less typing
#if DEBUG != OFF
  #define D(x)       DebugSer.print(x)
  #define DF(x)      DebugSer.print(F(x))
  #define DL(x)      DebugSer.println(x)
  #define DLF(x)     DebugSer.println(F(x))
#else
  #define D(x)
  #define DF(x)
  #define DL(x)
  #define DLF(x)
#endif
#if DEBUG == VERBOSE
  #define V(x)       DebugSer.print(x)
  #define VF(x)      DebugSer.print(F(x))
  #define VL(x)      DebugSer.println(x)
  #define VLF(x)     DebugSer.println(F(x))
#else
  #define V(x)
  #define VF(x)
  #define VL(x)
  #define VLF(x)
#endif

#define  TASKS_SKIP_MISSED
#define  TASKS_HWTIMER1_ENABLE     // only the Mega2560 hardware timers are tested and seem to work
#define  TASKS_HWTIMER2_ENABLE     // if the Teensy or UNO don't work comment these out to use the
#define  TASKS_HWTIMER3_ENABLE     // software task scheduler instead
#include "src/tasks/OnTask.h"
#ifdef TASKS_PROFILER_ENABLE
  #include "src/tasks/Profiler.h"
#endif

#include "Axis.h"
#include "Observatory.h"
#include "Transform.h"
#include "src/lib/BufferCmds.h"
#include "ProcessCommands.h"

void setup() {
  uint8_t handle;

  // ------------------------------------------------------------------------------------------------
  // setup date/time/location for time keeping and coordinate converson
  TI ut1;
  ut1.year=2020; ut1.month=1;   ut1.day=20;
  ut1.hour=12;   ut1.minute=11; ut1.second=9; ut1.centisecond=252;
  LI site;
  site.latitude.value=degToRad(40.1); site.longitude=degToRad(76.0); site.timezone=5;

  // ------------------------------------------------------------------------------------------------
  // add an event to tick the centisecond sidereal clock
  // period ms (0=idle), duration ms (0=forever), repeat, priority (highest 0..7 lowest), task_handle
  handle = tasks.add(0, 0, true, 0, clockTick, "ClkTick");
  tasks.requestHardwareTimer(handle, 3, 1);
  tasks.setPeriodSubMicros(handle, lround(160000.0/SIDEREAL_RATIO));

  observatory.init(site, ut1, handle);

  // ------------------------------------------------------------------------------------------------
  // add an event to process commands
  // period ms (0=idle), duration ms (0=forever), repeat, priority (highest 0..7 lowest), task_handle
#ifdef SERIAL_A
  handle = tasks.add(2, 0, true, 7, processCmdsA, "PrcCmdA");
#endif
#ifdef SERIAL_B
  handle = tasks.add(2, 0, true, 7, processCmdsB, "PrcCmdB");
#endif
#ifdef SERIAL_C
  handle = tasks.add(2, 0, true, 7, processCmdsC, "PrcCmdC");
#endif
#ifdef SERIAL_D
  handle = tasks.add(2, 0, true, 7, processCmdsD, "PrcCmdD");
#endif
#ifdef SERIAL_ST4
  handle = tasks.add(2, 0, true, 7, processCmdsST4, "PrcCmdS");
#endif

  // ------------------------------------------------------------------------------------------------

  // setup axis1
#if AXIS1_DRIVER_MODEL != OFF
  handle = tasks.add(0, 0, true, 0, moveAxis1, "MoveAx1"); tasks.requestHardwareTimer(handle,1,0);
  axis1.init(false, false, true, handle);
  axis1.setStepsPerMeasure(radToDeg(AXIS1_STEPS_PER_DEGREE));
  axis1.setMinCoordinate(degToRad(-180.0));
  axis1.setMaxCoordinate(degToRad(180.0));
  axis1.setInstrumentCoordinate(degToRad(90.0));
  axis1.enable(true);
#endif

  // setup axis2
#if AXIS2_DRIVER_MODEL != OFF
  handle = tasks.add(0, 0, true , 0, moveAxis2, "MoveAx2"); tasks.requestHardwareTimer(handle,2,0);
  axis2.init(false, false, true, handle);
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
  // add an event to show the Task Profiler
#ifdef TASKS_PROFILER_ENABLE
  tasks.add(142, 0, true, 7, profiler, "Profilr");
#else
  // add an event to show the telescope coordinates
  #ifdef TASKS_SHOW_COORDS
    tasks.add(1000, 0, true, 7, showEquatorialCoordinates);
  #endif
#endif
}

void loop() {
  tasks.yield();
}

#ifdef TASKS_SHOW_COORDS
void showEquatorialCoordinates() {
  EquCoordinate instrument, mount, observed, topocentric;
  HorCoordinate horizon;

  instrument.h=axis1.getInstrumentCoordinate();
  instrument.d=axis2.getInstrumentCoordinate();

  mount = transform.equInstrumentToMount(instrument);

  Serial.println("");

  observed = transform.equMountToObservedPlace(mount);

  transform.hourAngleToRightAscension(&observed);
  Serial.print("LST = "); Serial.println((observatory.getLAST()/SIDEREAL_RATIO)*3600, 1);
  Serial.print("RA  = "); Serial.println(radToDeg(observed.r), 4);
  Serial.print("HA  = "); Serial.println(radToDeg(observed.h), 4);
  Serial.print("Dec = "); Serial.println(radToDeg(observed.d), 4);

  horizon = transform.equToHor(instrument);
  Serial.print("Alt = "); Serial.println(radToDeg(horizon.a), 4);
  Serial.print("Azm = "); Serial.println(radToDeg(horizon.z), 4);
}
#endif
