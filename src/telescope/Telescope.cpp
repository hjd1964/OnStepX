//--------------------------------------------------------------------------------------------------
// OnStepX telescope control

#include "../Common.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;

#include "../lib/convert/Convert.h"
#include "../commands/ProcessCmds.h"
#include "Telescope.h"
#include "../lib/weather/Weather.h"

#include "mount/Mount.h"
#include "mount/goto/Goto.h"
#include "mount/guide/Guide.h"
#include "mount/home/Home.h"
#include "mount/library/Library.h"
#include "mount/limits/Limits.h"
#include "mount/park/Park.h"
#include "mount/pec/Pec.h"
#include "mount/site/Site.h"
#include "mount/status/Status.h"
#include "rotator/Rotator.h"
#include "focuser/Focuser.h"
#include "auxiliary/Features.h"

bool validKey = true;
bool xBusy = false;
InitError initError;

#if LED_STATUS != OFF && STATUS_LED_PIN != OFF
  void statusFlash() {
    static uint8_t cycle = 0;
    if (cycle++ > 16) cycle = 0;

    // show only the most severe error (in order)
    uint8_t flashes = 0;
    if (initError.nv)      flashes = 1; else
    if (initError.value)   flashes = 2; else
    if (initError.driver)  flashes = 3; else
    if (initError.tls)     flashes = 4; else
    if (initError.weather) flashes = 5;

    // everything is ok, turn on LED and exit
    if (flashes == 0) { digitalWriteEx(STATUS_LED_PIN, STATUS_LED_ON_STATE); return; }

    // flash the LED if there's an error
    if (cycle%2 == 0) { digitalWriteEx(STATUS_LED_PIN, !STATUS_LED_ON_STATE); } else { if (cycle/2 < flashes) digitalWriteEx(STATUS_LED_PIN, STATUS_LED_ON_STATE); }
  }
#endif

void Telescope::init(const char *fwName, int fwMajor, int fwMinor, const char *fwPatch, int fwConfig) {
  strcpy(telescope.firmware.name, fwName);
  telescope.firmware.version.major = fwMajor;
  telescope.firmware.version.minor = fwMinor;
  strcpy(telescope.firmware.version.patch, fwPatch);
  telescope.firmware.version.config = fwConfig;
  strcpy(telescope.firmware.date, __DATE__);
  strcpy(telescope.firmware.time, __TIME__);

  if (nv.readUL(NV_KEY) != INIT_NV_KEY) {
    validKey = false;

    VF("MSG: Telescope, Wipe NV "); V(nv.size); VLF(" Bytes");
    for (int i = 0; i < (int)nv.size; i++) nv.write(i, (char)0);
    VLF("MSG: Telescope, Wipe NV waiting for commit");
    while (!nv.committed()) { nv.poll(false); delay(10); }

    VLF("MSG: Telescope, NV reset to defaults");
  } else { VLF("MSG: Telescope, correct NV key found"); }

  gpio.init();

  weather.init();

  #ifdef MOUNT_PRESENT
    mount.init();
  #endif

  #ifdef ROTATOR_PRESENT
    rotator.init();
  #endif

  #ifdef FOCUSER_PRESENT
    focuser.init();
  #endif

  #ifdef FEATURES_PRESENT
    features.init();
  #endif

  if (!validKey) {
    while (!nv.committed()) nv.poll();
    nv.write(NV_KEY, (uint32_t)INIT_NV_KEY);
    while (!nv.committed()) { nv.poll(); delay(10); }
    nv.ignoreCache(true);
    uint32_t key = nv.readUL(NV_KEY);
    if (key != (uint32_t)INIT_NV_KEY) { DLF("ERR: Telescope, NV reset failed to read back key!"); } else { VLF("MSG: Telescope, NV reset complete"); }
    nv.ignoreCache(false);
  }

  // bring up status LED and flash error codes
  #if LED_STATUS != OFF && STATUS_LED_PIN != OFF
    pinModeEx(STATUS_LED_PIN, OUTPUT);
    VF("MSG: Telescope, start status LED task (rate 500ms priority 6)... ");
    if (tasks.add(500, 0, true, 6, statusFlash, "StaLed")) { VL("success"); } else { VL("FAILED!"); }
  #endif
}

Telescope telescope;
