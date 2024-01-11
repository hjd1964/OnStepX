//--------------------------------------------------------------------------------------------------
// OnStepX telescope control

#include "../Common.h"
#include "../lib/tasks/OnTask.h"

#include "../lib/convert/Convert.h"
#include "../libApp/commands/ProcessCmds.h"
#include "../libApp/weather/Weather.h"
#include "../libApp/temperature/Temperature.h"

#include "Telescope.h"

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
    if (initError.gpio)    flashes = 3; else
    if (initError.driver)  flashes = 4; else
    if (initError.tls)     flashes = 5; else
    if (initError.weather) flashes = 6;

    int pin = STATUS_LED_PIN;
 
    // everything is ok, turn on LED and exit
    if (flashes == 0) { digitalWriteEx(pin, STATUS_LED_ON_STATE); return; }

    // flash the LED if there's an error
    if (cycle%2 == 0) { digitalWriteEx(pin, !STATUS_LED_ON_STATE); } else { if (cycle/2 < flashes) digitalWriteEx(pin, STATUS_LED_ON_STATE); }
  }
#endif

Telescope::Telescope() {
  // early init for stepper drivers
  #ifdef AXIS1_STEP_DIR_PRESENT
  if (AXIS1_STEP_PIN >= 0 && AXIS1_STEP_PIN <= 255) pinMode(AXIS1_STEP_PIN, OUTPUT);
  #endif
  #ifdef AXIS2_STEP_DIR_PRESENT
  if (AXIS2_STEP_PIN >= 0 && AXIS2_STEP_PIN <= 255) pinMode(AXIS2_STEP_PIN, OUTPUT);
  #endif
  #ifdef AXIS3_STEP_DIR_PRESENT
  if (AXIS3_STEP_PIN >= 0 && AXIS3_STEP_PIN <= 255) pinMode(AXIS3_STEP_PIN, OUTPUT);
  #endif
  #ifdef AXIS4_STEP_DIR_PRESENT
  if (AXIS4_STEP_PIN >= 0 && AXIS4_STEP_PIN <= 255) pinMode(AXIS4_STEP_PIN, OUTPUT);
  #endif
  #ifdef AXIS5_STEP_DIR_PRESENT
  if (AXIS5_STEP_PIN >= 0 && AXIS5_STEP_PIN <= 255) pinMode(AXIS5_STEP_PIN, OUTPUT);
  #endif
  #ifdef AXIS6_STEP_DIR_PRESENT
  if (AXIS6_STEP_PIN >= 0 && AXIS6_STEP_PIN <= 255) pinMode(AXIS6_STEP_PIN, OUTPUT);
  #endif
  #ifdef AXIS7_STEP_DIR_PRESENT
  if (AXIS7_STEP_PIN >= 0 && AXIS7_STEP_PIN <= 255) pinMode(AXIS7_STEP_PIN, OUTPUT);
  #endif
  #ifdef AXIS8_STEP_DIR_PRESENT
  if (AXIS8_STEP_PIN >= 0 && AXIS8_STEP_PIN <= 255) pinMode(AXIS8_STEP_PIN, OUTPUT);
  #endif
  #ifdef AXIS9_STEP_DIR_PRESENT
  if (AXIS9_STEP_PIN >= 0 && AXIS9_STEP_PIN <= 255) pinMode(AXIS9_STEP_PIN, OUTPUT);
  #endif

  if (AXIS1_ENABLE_PIN >= 0 && AXIS1_ENABLE_PIN <= 255) { pinMode(AXIS1_ENABLE_PIN, OUTPUT); digitalWrite(AXIS1_ENABLE_PIN, !AXIS1_ENABLE_STATE); }
  if (AXIS2_ENABLE_PIN >= 0 && AXIS2_ENABLE_PIN <= 255) { pinMode(AXIS2_ENABLE_PIN, OUTPUT); digitalWrite(AXIS2_ENABLE_PIN, !AXIS2_ENABLE_STATE); }
  if (AXIS3_ENABLE_PIN >= 0 && AXIS3_ENABLE_PIN <= 255) { pinMode(AXIS3_ENABLE_PIN, OUTPUT); digitalWrite(AXIS3_ENABLE_PIN, !AXIS3_ENABLE_STATE); }
  if (AXIS4_ENABLE_PIN >= 0 && AXIS4_ENABLE_PIN <= 255) { pinMode(AXIS4_ENABLE_PIN, OUTPUT); digitalWrite(AXIS4_ENABLE_PIN, !AXIS4_ENABLE_STATE); }
  if (AXIS5_ENABLE_PIN >= 0 && AXIS5_ENABLE_PIN <= 255) { pinMode(AXIS5_ENABLE_PIN, OUTPUT); digitalWrite(AXIS5_ENABLE_PIN, !AXIS5_ENABLE_STATE); }
  if (AXIS6_ENABLE_PIN >= 0 && AXIS6_ENABLE_PIN <= 255) { pinMode(AXIS6_ENABLE_PIN, OUTPUT); digitalWrite(AXIS6_ENABLE_PIN, !AXIS6_ENABLE_STATE); }
  if (AXIS7_ENABLE_PIN >= 0 && AXIS7_ENABLE_PIN <= 255) { pinMode(AXIS7_ENABLE_PIN, OUTPUT); digitalWrite(AXIS7_ENABLE_PIN, !AXIS7_ENABLE_STATE); }
  if (AXIS8_ENABLE_PIN >= 0 && AXIS8_ENABLE_PIN <= 255) { pinMode(AXIS8_ENABLE_PIN, OUTPUT); digitalWrite(AXIS8_ENABLE_PIN, !AXIS8_ENABLE_STATE); }
  if (AXIS9_ENABLE_PIN >= 0 && AXIS9_ENABLE_PIN <= 255) { pinMode(AXIS9_ENABLE_PIN, OUTPUT); digitalWrite(AXIS9_ENABLE_PIN, !AXIS9_ENABLE_STATE); }
  #ifdef SHARED_ENABLE_PIN
    if (SHARED_ENABLE_PIN >= 0 && SHARED_ENABLE_PIN <= 255) { pinMode(SHARED_ENABLE_PIN, OUTPUT); digitalWrite(SHARED_ENABLE_PIN, !SHARED_ENABLE_STATE); }
  #endif
  #ifdef SHARED_ENABLE_PIN2
    if (SHARED_ENABLE_PIN2 >= 0 && SHARED_ENABLE_PIN2 <= 255) { pinMode(SHARED_ENABLE_PIN2, OUTPUT); digitalWrite(SHARED_ENABLE_PIN2, !SHARED2_ENABLE_STATE); }
  #endif
  #ifdef SHARED_ENABLE_PIN3
    if (SHARED_ENABLE_PIN3 >= 0 && SHARED_ENABLE_PIN3 <= 255) { pinMode(SHARED_ENABLE_PIN3, OUTPUT); digitalWrite(SHARED_ENABLE_PIN3, !SHARED3_ENABLE_STATE); }
  #endif
}

void Telescope::init(const char *fwName, int fwMajor, int fwMinor, const char *fwPatch, int fwConfig) {
  strcpy(firmware.name, fwName);
  firmware.version.major = fwMajor;
  firmware.version.minor = fwMinor;
  strcpy(firmware.version.patch, fwPatch);
  firmware.version.config = fwConfig;
  strcpy(firmware.date, __DATE__);
  strcpy(firmware.time, __TIME__);

  if (!nv.isKeyValid(INIT_NV_KEY)) {
    if (!nv.initError) {
      VF("MSG: NV, invalid key wipe "); V(nv.size); VLF(" bytes");
      if (nv.verify()) { VLF("MSG: NV, ready for reset to defaults"); }
    } else {
      DLF("WRN: NV, can't be accessed skipping verification!");
    }
  } else { VLF("MSG: NV, correct key found"); }

  if (!gpio.init()) initError.gpio = true;

  #ifdef SHARED_ENABLE_PIN
    pinModeEx(SHARED_ENABLE_PIN, OUTPUT);
    digitalWriteEx(SHARED_ENABLE_PIN, !SHARED_ENABLE_STATE);
  #endif
  #ifdef SHARED_ENABLE_PIN2
    pinModeEx(SHARED_ENABLE_PIN2, OUTPUT);
    digitalWriteEx(SHARED_ENABLE_PIN2, !SHARED2_ENABLE_STATE);
  #endif
  #ifdef SHARED_ENABLE_PIN3
    pinModeEx(SHARED_ENABLE_PIN3, OUTPUT);
    digitalWriteEx(SHARED_ENABLE_PIN3, !SHARED3_ENABLE_STATE);
  #endif
  
  #if SERIAL_B_ESP_FLASHING == ON
    addonFlasher.init();
  #endif

  weather.init();
  temperature.init();

  #if OPERATIONAL_MODE == WIFI && WEB_SERVER == ON
    wifiManager.init();
  #endif

  #ifdef MOUNT_PRESENT
    #if defined(ESP32) && STATUS_BUZZER >= 0
      // hack to trigger one-time ESP32 code debug message to get it out of the way early
      tone(STATUS_BUZZER_PIN, STATUS_BUZZER, 1);
    #endif
    mount.init();
    mountStatus.init();
  #endif

  #ifdef ROTATOR_PRESENT
    rotator.init();
  #endif

  #ifdef FOCUSER_PRESENT
    focuser.init();
  #endif

  delay(1000);

  #ifdef SHARED_ENABLE_PIN
    digitalWriteEx(SHARED_ENABLE_PIN, SHARED_ENABLE_STATE);
  #endif
  #ifdef SHARED_ENABLE_PIN2
    digitalWriteEx(SHARED_ENABLE_PIN2, SHARED2_ENABLE_STATE);
  #endif
  #ifdef SHARED_ENABLE_PIN3
    digitalWriteEx(SHARED_ENABLE_PIN3, SHARED3_ENABLE_STATE);
  #endif

  #ifdef MOUNT_PRESENT
    mount.begin();
  #endif

  #ifdef ROTATOR_PRESENT
    rotator.begin();
  #endif

  #ifdef FOCUSER_PRESENT
    focuser.begin();
  #endif

  #ifdef FEATURES_PRESENT
    features.init();
  #endif

  // write the default settings to NV
  if (!nv.hasValidKey()) {
    VLF("MSG: Telescope, writing defaults to NV");
    nv.write(NV_TELESCOPE_SETTINGS_BASE, reticleBrightness);
  }

  // init is done, write the NV key if necessary
  if (!nv.hasValidKey()) {
    nv.writeKey((uint32_t)INIT_NV_KEY);
    nv.wait();
    if (!nv.isKeyValid(INIT_NV_KEY)) { DLF("ERR: NV, failed to read back key!"); } else { VLF("MSG: NV, reset complete"); }
  }

  initError.nv = nv.initError;

  #if RETICLE_LED_DEFAULT != OFF && RETICLE_LED_PIN != OFF
    #if RETICLE_LED_MEMORY == ON
      reticleBrightness = nv.readI(NV_TELESCOPE_SETTINGS_BASE);
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
