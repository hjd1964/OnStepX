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
#define FirmwareVersionMinor  01      // minor version 0 to 99
#define FirmwareVersionPatch  "a"     // for example major.minor patch: 1.3c
#define FirmwareVersionConfig 01      // internal, for tracking configuration file changes
#define FirmwareName          "OnStepX"
#define FirmwareTime          __TIME__

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
#define  TASKS_HWTIMER1_ENABLE
#define  TASKS_HWTIMER2_ENABLE
#define  TASKS_HWTIMER3_ENABLE
#include "OnTask.h"

#include "Constants.h"
#include "Config.h"
#include "Axis.h"
#include "Transform.h"
#include "StepDriver.h"
#include "Globals.h"
#include "Astro.h"
#include "BufferCmds.h"
#include "ProcessCmds.h"

// -------------------------------
// various tasks

void clockTick() { transform.clockTick(); }

#if AXIS1_DRIVER != OFF
void moveAxis1() { axis1.move(AXIS1_STEP_PIN, AXIS1_DIR_PIN); }
#endif
#if AXIS2_DRIVER != OFF
void moveAxis2() { axis2.move(AXIS2_STEP_PIN, AXIS2_DIR_PIN); }
#endif
#if AXIS3_DRIVER != OFF
void moveAxis3() { axis3.move(AXIS3_STEP_PIN, AXIS3_DIR_PIN); }
#endif
#if AXIS4_DRIVER != OFF
void moveAxis4() { axis4.move(AXIS4_STEP_PIN, AXIS4_DIR_PIN); }
#endif
#if AXIS5_DRIVER != OFF
void moveAxis5() { axis5.move(AXIS5_STEP_PIN, AXIS5_DIR_PIN); }
#endif
#if AXIS6_DRIVER != OFF
void moveAxis6() { axis6.move(AXIS6_STEP_PIN, AXIS6_DIR_PIN); }
#endif

// -------------------------------

void setup() {
  uint8_t handle;

  Serial.begin(9600);

  // ------------------------------------------------------------------------------------------------
  // setup date/time/location for time keeping and coordinate converson
  TI ut1;
  ut1.year=2020; ut1.month=1;   ut1.day=20;
  ut1.hour=12;   ut1.minute=11; ut1.second=9; ut1.centisecond=252;
  LI site;
  site.latitude.value=degToRad(40.1); site.longitude=degToRad(76.0); site.timezone=5;

  // ------------------------------------------------------------------------------------------------
  // add an event to tick the centisecond sidereal clock
  // period ms (0=idle), duration ms (0=forever), repeat, priority (highest 0..7 lowest), event_handle
  handle=tasks.add(0, 0, true, 0, clockTick);
  tasks.requestHardwareTimer(handle,3,1);
  tasks.setPeriodSubMicros(handle, lround(160000.0/SIDEREAL_RATIO));
  transform.init(site, ut1, handle);

  // ------------------------------------------------------------------------------------------------
  // add an event to process commands
  // period ms (0=idle), duration ms (0=forever), repeat, priority (highest 0..7 lowest), event_handle
  handle=tasks.add(0, 0, true, 7, processCommands);
  tasks.setPeriodMicros(handle, 200);

  // ------------------------------------------------------------------------------------------------

  // setup axis1
#if AXIS1_DRIVER != OFF
  axis1Driver.init(axis1DriverSettings);
  handle = tasks.add(0, 0, true, 0, moveAxis1); tasks.requestHardwareTimer(handle,1,0);
  axis1.init(false, false, true, handle);
  axis1.setStepsPerMeasure(axis1_steps_per_radian);
  axis1.setMinCoordinate(degToRad(-180.0));
  axis1.setMaxCoordinate(degToRad(180.0));
  axis1.setInstrumentCoordinate(degToRad(90.0));
  axis1.enable(true);
#endif

  // setup axis2
#if AXIS2_DRIVER != OFF
  axis2Driver.init(axis2DriverSettings);
  handle = tasks.add(0, 0, true , 0, moveAxis2); tasks.requestHardwareTimer(handle,2,0);
  axis2.init(false, false, true, handle);
  axis2.setStepsPerMeasure(axis2_steps_per_radian);
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
  // add an event to show RA and Dec
  tasks.add(1000, 0, true, 7, showEquatorialCoordinates);
}

void loop() {
  tasks.yield();
}

void showEquatorialCoordinates() {
  EquCoordinate instrument, mount, observed, topocentric;
  HorCoordinate horizon;

  instrument.h=axis1.getInstrumentCoordinate();
  instrument.d=axis2.getInstrumentCoordinate();

  mount = transform.equInstrumentToMount(instrument);

  Serial.println("");

  observed = transform.equMountToObservedPlace(mount);

  transform.hourAngleToRightAscension(&observed);
  Serial.print("LST = "); Serial.println((transform.getLAST()/SIDEREAL_RATIO)*3600,1);
  Serial.print("RA  = "); Serial.println(radToDeg(observed.r),4);
  Serial.print("HA  = "); Serial.println(radToDeg(observed.h),4);
  Serial.print("Dec = "); Serial.println(radToDeg(observed.d),4);

  horizon = transform.equToHor(instrument);
  Serial.print("Alt = "); Serial.println(radToDeg(horizon.a),4);
  Serial.print("Azm = "); Serial.println(radToDeg(horizon.z),4);
}
