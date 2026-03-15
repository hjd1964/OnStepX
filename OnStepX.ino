/*
 * Title       OnStepX
 * by          Howard Dutton
 *
 * Copyright (C) 2021-2026 Howard Dutton
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
 *   Full featured telescope control for
 *   Equatorial and Alt-Az mounts
 *   Rotator
 *   Focusers
 *   Accessories (automatic covers, dew heaters, etc.)
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

// Use tabs "Config..." to configure OnStep to your requirements

// Firmware version ----------------------------------------------------------------------------------------------------------------
#define FirmwareName                "On-Step"
#define FirmwareVersionMajor        10
#define FirmwareVersionMinor        28     // minor version 00 to 99
#define FirmwareVersionPatch        "e"    // for example major.minor patch: 10.03c
#define FirmwareVersionConfig       6      // internal, for tracking configuration file changes

#include "src/Common.h"
#include "src/Validate.h"
#include "src/lib/nv/Nv.h"
#include "src/lib/analog/Analog.h"
#include "src/lib/sense/Sense.h"
#include "src/lib/tasks/OnTask.h"

#include "src/telescope/Telescope.h"
extern Telescope telescope;

#include "src/plugins/Plugins.config.h"

#if DEBUG == PROFILER
  extern void profiler();
#endif

void sensesPoll() {
  sense.poll();
}

void setup() {
  #if ADDON_SELECT_PIN != OFF
    pinMode(ADDON_SELECT_PIN, OUTPUT);
    digitalWrite(ADDON_SELECT_PIN, HIGH);
  #endif

  #if DEBUG != OFF
    SERIAL_DEBUG.begin(SERIAL_DEBUG_BAUD);
    delay(2000);
  #endif

  // let any special processing the pinmap needs happen
  #ifdef PIN_INIT
    PIN_INIT();
  #endif

  // say hello
  VLF("");
  VF("MSG: OnStepX, version "); V(FirmwareVersionMajor); V("."); V(FirmwareVersionMinor); VL(FirmwareVersionPatch);
  VF("MSG: OnStepX, MCU "); VLF(MCU_STR);
  VF("MSG: OnStepX, pinmap "); VLF(PINMAP_STR);

  // start low level hardware
  VLF("MSG: System, HAL initialize");
  HAL_INIT();
  WIRE_INIT();

  analog.begin();

  nv().setGate(&xBusy);
  if (!nv().init()) {
    DLF("ERR: Setup, NV (EEPROM/FRAM/Flash/etc.) device not found!");
  }
  delay(2000);

  #if defined(NV_WIPE) && NV_WIPE == ON
    nv().wipe();
  #endif

  // start input sense polling task
  int pollingRate = round((1000.0F/HAL_FRACTIONAL_SEC)/2.0F);
  if (pollingRate < 1) pollingRate = 1;
  VF("MSG: System, start input sense service task (rate "); V(pollingRate); VF("ms priority 7)... ");
  if (tasks.add(pollingRate, 0, true, 7, sensesPoll, "SysSens")) { VLF("success"); } else { VLF("FAILED!"); }

  // start telescope object
  telescope.init(FirmwareName, FirmwareVersionMajor, FirmwareVersionMinor, FirmwareVersionPatch, FirmwareVersionConfig);

  // start command channel tasks
  commandChannelInit();

  tasks.yield(2000);

  // start any plugins
  #if PLUGIN1 != OFF
    PLUGIN1.init();
  #endif
  #if PLUGIN2 != OFF
    PLUGIN2.init();
  #endif
  #if PLUGIN3 != OFF
    PLUGIN3.init();
  #endif
  #if PLUGIN4 != OFF
    PLUGIN4.init();
  #endif
  #if PLUGIN5 != OFF
    PLUGIN5.init();
  #endif
  #if PLUGIN6 != OFF
    PLUGIN6.init();
  #endif
  #if PLUGIN7 != OFF
    PLUGIN7.init();
  #endif
  #if PLUGIN8 != OFF
    PLUGIN8.init();
  #endif

  // start task manager debug events
  #if DEBUG == PROFILER
    tasks.add(142, 0, true, 7, profiler, "Profilr");
  #endif

  sense.poll();

  telescope.ready = true;
}

void loop() {
  tasks.yield();
}
