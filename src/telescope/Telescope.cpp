//--------------------------------------------------------------------------------------------------
// OnStepX telescope control

#include "../Common.h"

#include "../lib/tasks/OnTask.h"
#include "../lib/gpioEx/GpioEx.h"
#include "../lib/nv/Nv.h"
#include "../lib/analog/Analog.h"
#include "../lib/convert/Convert.h"
#include "../lib/canPlus/CanPlus.h"

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

void mcuTempWrapper() { telescope.mcuTemperature = (telescope.mcuTemperature*9.0F + HAL_TEMP())/10.0F; }

#if STATUS_LED != OFF && STATUS_LED_PIN != OFF
  void statusFlash() {
    static uint8_t cycle = 0;
    if (cycle++ > 16) cycle = 0;

    #if GPIO_DEVICE != OFF
      if (gpio.lateInitError) initError.gpio = true;
    #endif

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
  sstrcpy(firmware.name, fwName);
  firmware.version.major = fwMajor;
  firmware.version.minor = fwMinor;
  sstrcpy(firmware.version.patch, fwPatch);
  firmware.version.config = fwConfig;
  sstrcpy(firmware.date, __DATE__);
  sstrcpy(firmware.time, __TIME__);

  // --------------------------------------------------------------------------------------------------
  // start the NV/EEPROM subsystem for settings storage
  initError.nv = false;
  initError.value = false;
  bool success = nv().init();
  NvVolume &nvVolume = nv().volume();

  success = success && (nvVolume.mount("OnStepX", NV_VOLUME_SIGNATURE) == NvVolume::Status::Ok);
  VF("MSG: Nv, volume ");
  if (success) { VLF("'OnStepX' mounted"); } else { VLF("invalid/unformatted"); }

  if (!success) {

    // automatic kv partition sizing
    uint32_t vSize = nvVolume.byteCount() - 32;
    uint32_t kvSize = vSize;
    if (vSize > 1039) {

      // start the volume format
      success = nvVolume.formatBegin("OnStepX", NV_VOLUME_SIGNATURE) == NvVolume::Status::Ok;
      if (success) { VF("MSG: Nv, volume 'OnStepX' format started ("); V(vSize); VLF(" bytes)"); }

      // first add the KV partition
           if (vSize < 1536) kvSize = 1040;     // 1080 class (no PEC/Lib)
      else if (vSize < 3072) kvSize = 1536;     // 2KB class (~0.5K for Lib)
      else if (vSize < 6144) kvSize = 2048;     // 4KB class (~2K for PEC/Lib)
      else if (vSize < 16384) kvSize = 3072;    // 8KB class (~5K for PEC/Lib)
      else kvSize = 4080;                       // and up

      success = success && nvVolume.formatAddPartition("KV", kvSize);
      if (success) { VF("MSG: Nv, volume format added 'KV' partition ("); V(kvSize); VLF(" bytes)"); }

      // next any PEC partition
      uint32_t pecSize = 0;
      #if AXIS1_PEC == ON
        pecSize = ((PEC_BUFFER_SIZE_LIMIT) >> 4) << 4;
        if (kvSize + pecSize > vSize) { pecSize = 0; }

        if (pecSize > 15) {
          success = success && nvVolume.formatAddPartition("PEC", pecSize);
          if (success) { VF("MSG: Nv, volume format added 'PEC' partition ("); V(pecSize); VLF(" bytes)"); }
        }
      #endif

      // next any Library partition
      uint32_t libSize = vSize - pecSize - kvSize;
      if (libSize >= 16) {
        success = success && nvVolume.formatAddPartition("LIBRARY", 0);
        if (success) { VF("MSG: Nv, volume format added 'LIBRARY' partition ("); V(libSize); VLF(" bytes)"); }
      }

      // finally commit the volume format
      success = success && (nvVolume.formatCommit() == NvVolume::Status::Ok);
      if (success) { VLF("MSG: Nv, volume format done"); }

      // try to mount the volume again
      success = success && (nvVolume.mount("OnStepX", NV_VOLUME_SIGNATURE) == NvVolume::Status::Ok);
      if (success) { VLF("MSG: Nv, volume 'OnStepX' mounted"); } else { DLF("WRN: Nv, volume 'OnStepX' mount FAILED!"); }

    } else {
      DLF("WRN: Nv, volume compatible storage device NOT FOUND!");
      success = false;
    }
  }

  // Bind global KV instance to the KV partition index
  success = success && (nv().kv().init(nvVolume, "KV") == KvPartition::Status::Ok);
  if (success) { VLF("MSG: Nv, partition 'KV' mounted"); } else { DLF("WRN: Nv, partition 'KV' mount FAILED!"); }

  if (!success) {
    VLF("WRN: Nv, init FAILED!");
    initError.nv = true;
  }
  nv().kv().resetInitErrorFlag();
  // --------------------------------------------------------------------------------------------------

  #if RETICLE_LED_DEFAULT >= 0 && RETICLE_LED_PIN != OFF
    analog.pwmInit(RETICLE_LED_DEFAULT);
  #endif

  #ifdef USES_HW_SPI
    SPI.begin();
  #endif

  #if CAN_PLUS != OFF
    canPlus.init();
  #endif

  #if GPIO_DEVICE != OFF
    if (!gpio.init()) initError.gpio = true;
  #endif

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

  mcuTemperature = HAL_TEMP();
  if (!isnan(mcuTemperature)) {
    VF("MSG: System, start MCU temperature monitor task (rate 500ms priority 7)... ");
    if (tasks.add(500, 0, true, 6, mcuTempWrapper, "SysTemp")) { VLF("success"); } else { VLF("FAILED!"); }
  }
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

  #if defined(ROTATOR_PRESENT) || defined(ROTATOR_CLIENT_PRESENT)
    rotator.init();
  #endif

  #if defined(FOCUSER_PRESENT) || defined(FOCUSER_CLIENT_PRESENT)
    focuser.init();
  #endif

  #if defined(FEATURES_PRESENT) || defined(FEATURES_CLIENT_PRESENT)
    features.init();
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

  #if defined(ROTATOR_PRESENT) || defined(ROTATOR_CLIENT_PRESENT)
    rotator.begin();
  #endif

  #if defined(FOCUSER_PRESENT) || defined(FOCUSER_CLIENT_PRESENT)
    focuser.begin();
  #endif

  #if defined(FEATURES_PRESENT) || defined(FEATURES_CLIENT_PRESENT)
    features.begin();
  #endif

  #if RETICLE_LED_DEFAULT != OFF && RETICLE_LED_PIN != OFF
    #if RETICLE_LED_MEMORY == ON
      if (!nv().kv().getOrInit("TELESCOPE_SETTINGS", reticleBrightness)) { DLF("WRN: Nv, init failed for TELESCOPE_SETTINGS"); }
      reticleBrightness = constrain(reticleBrightness, 0, 255);
    #endif

    pinMode(RETICLE_LED_PIN, OUTPUT);

    float duty = (float)reticleBrightness*(1.0F/255.0F);
    analog.write(RETICLE_LED_PIN, RETICLE_LED_INVERT == ON ? duty : 1.0F - duty);
  #endif

  // bring up status LED and flash error codes
  #if STATUS_LED != OFF && STATUS_LED_PIN != OFF
    int pin = STATUS_LED_PIN;
    pinModeEx(pin, OUTPUT);
    VF("MSG: Telescope, start status LED task (rate 500ms priority 4)... ");
    if (tasks.add(500, 0, true, 4, statusFlash, "StaLed")) { VLF("success"); } else { VLF("FAILED!"); }
  #endif

  // --------------------------------------------------------------------------------------------------
  // init is done let the user see what's in the KV
  #if DEBUG != OFF
    KvPartition::Stats stats;
    if (success && nv().kv().stats(stats) == KvPartition::Status::Ok) {
      VF("MSG: Nv, partition 'KV' data blocks used = ");
      V(stats.dataBlocksTotal - stats.dataBlocksFree); VF(" (of "); V(stats.dataBlocksTotal); VF(")");
      VF(" key slots used = ");
      V(stats.slotsTotal - stats.slotsFree); VF(" (of "); V(stats.slotsTotal); VLF(")");
    }
  #endif

  // and capture any errors
  if (nv().kv().getInitErrorFlag()) initError.nv = true;
  // --------------------------------------------------------------------------------------------------

}

Telescope telescope;
