//--------------------------------------------------------------------------------------------------
// OnStepX telescope control

#include "../Common.h"
#include "../lib/tasks/OnTask.h"

#include "../lib/convert/Convert.h"
#include "../libApp/commands/ProcessCmds.h"
#include "Telescope.h"
#include "../libApp/weather/Weather.h"

#include "addonFlasher/AddonFlasher.h"
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

bool xBusy = false;
InitError initError;

#if STATUS_LED != OFF && STATUS_LED_PIN != OFF
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

    int pin = STATUS_LED_PIN;
 
    // everything is ok, turn on LED and exit
    if (flashes == 0) { digitalWriteEx(pin, STATUS_LED_ON_STATE); return; }

    // flash the LED if there's an error
    if (cycle%2 == 0) { digitalWriteEx(pin, !STATUS_LED_ON_STATE); } else { if (cycle/2 < flashes) digitalWriteEx(pin, STATUS_LED_ON_STATE); }
  }
#endif

void Telescope::init(const char *fwName, int fwMajor, int fwMinor, const char *fwPatch, int fwConfig) {
  strcpy(firmware.name, fwName);
  firmware.version.major = fwMajor;
  firmware.version.minor = fwMinor;
  strcpy(firmware.version.patch, fwPatch);
  firmware.version.config = fwConfig;
  strcpy(firmware.date, __DATE__);
  strcpy(firmware.time, __TIME__);

  if (!nv.isKeyValid(INIT_NV_KEY)) {
    VF("MSG: NV, invalid key wipe "); V(nv.size); VLF(" bytes");
    if (nv.verify()) { VLF("MSG: NV, ready for reset to defaults"); }
  } else { VLF("MSG: NV, correct key found"); }

  gpio.init();

  #if SERIAL_B_ESP_FLASHING == ON
    addonFlasher.init();
  #endif

  weather.init();

  #ifdef MOUNT_PRESENT
    mount.init();
    status.init();
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

  // init is done, write the NV key if necessary
  if (!nv.isKeyValid()) {
    if (!nv.initError) {
      nv.writeKey((uint32_t)INIT_NV_KEY);
      nv.wait();
      if (!nv.isKeyValid(INIT_NV_KEY)) { DLF("ERR: NV, failed to read back key!"); } else { VLF("MSG: NV, reset complete"); }
    }
  }

  initError.nv = nv.initError;

  // write the default settings to NV
  if (!nv.isKeyValid()) {
    VLF("MSG: Telescope, writing defaults to NV");
    nv.write(NV_RETICLE_SETTINGS_BASE, reticleBrightness);
  }

  #if RETICLE_LED_DEFAULT != OFF && RETICLE_LED_PIN != OFF
    #if RETICLE_LED_MEMORY == ON
      reticleBrightness = nv.readI(NV_RETICLE_SETTINGS_BASE);
      if (reticleBrightness < 0) reticleBrightness = RETICLE_LED_DEFAULT;
    #endif

    pinMode(RETICLE_LED_PIN, OUTPUT);
    analogWrite(RETICLE_LED_PIN, analog8BitToAnalogRange(reticleBrightness));
  #endif

  // bring up status LED and flash error codes
  #if STATUS_LED != OFF && STATUS_LED_PIN != OFF
    int pin = STATUS_LED_PIN;
    pinModeEx(pin, OUTPUT);
    VF("MSG: Telescope, start status LED task (rate 500ms priority 4)... ");
    if (tasks.add(500, 0, true, 4, statusFlash, "StaLed")) { VLF("success"); } else { VLF("FAILED!"); }
  #endif
}

Telescope telescope;
