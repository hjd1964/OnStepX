//--------------------------------------------------------------------------------------------------
// OnStepX focuser control

#include "Focuser.h"

#ifdef FOCUSER_PRESENT

#include "../../lib/weather/Weather.h"
#include "../../lib/temperature/Temperature.h"
#include "../Telescope.h"

// initialize all focusers
void Focuser::init(bool validKey) {
  #if AXIS4_DRIVER_MODEL != OFF
    VL("MSG: Focuser1, init. (Axis4)");
    axis4.init(4, false, validKey);
    focuserAxis[0] = &axis4;
  #endif
  #if AXIS5_DRIVER_MODEL != OFF
    VL("MSG: Focuser2, init. (Axis5)");
    axis5.init(5, false, validKey);
    focuserAxis[1] = &axis5;
  #endif
  #if AXIS6_DRIVER_MODEL != OFF
    VL("MSG: Focuser3, init. (Axis6)");
    axis6.init(6, false, validKey);
    focuserAxis[2] = &axis6;
  #endif
  #if AXIS7_DRIVER_MODEL != OFF
    VL("MSG: Focuser4, init. (Axis7)");
    axis7.init(7, false, validKey);
    focuserAxis[3] = &axis7;
  #endif
  #if AXIS8_DRIVER_MODEL != OFF
    VL("MSG: Focuser5, init. (Axis8)");
    axis8.init(8, false, validKey);
    focuserAxis[4] = &axis8;
  #endif
  #if AXIS9_DRIVER_MODEL != OFF
    VL("MSG: Focuser6, init. (Axis9)");
    axis9.init(9, false, validKey);
    focuserAxis[5] = &axis9;
  #endif

  for (int index = 0; index <= 5; index++) {
    if (focuserAxis[index] != NULL) {
      focuserAxis[index]->setFrequencyMax(slewRateDesired[index]);
      focuserAxis[index]->setFrequencySlew(slewRateDesired[index]);
      focuserAxis[index]->setSlewAccelerationRate(accelerationRate[index]);
      focuserAxis[index]->setSlewAccelerationRateAbort(rapidStopRate[index]);
    }
  }

  if (FocuserSettingsSize < sizeof(Settings)) { initError.nv = true; DL("ERR: Focuser::init(); FocuserSettingsSize error NV subsystem writes disabled"); nv.readOnly(true); }

  // get settings stored in NV ready
  if (!validKey) {
    VLF("MSG: Focusers, writing default settings to NV");
    for (int index = 0; index <= 5; index++) writeSettings(index);
  }
  for (int index = 0; index <= 5; index++) readSettings(index);

}

// get focuser temperature in deg. C
float Focuser::getTemperature() {
  #if FOCUSER_TEMPERATURE == OFF
    float t = weather.getTemperature();
  #else
    float t = temperature.getChannel(0);
  #endif
  if (isnan(t)) t = 10.0;
  return t;
}

// check for DC motor focuser
bool Focuser::isDC(int index) {
  if (index < 0 || index > 5) return false;
  return dcMode[index];
}

// get DC power in %
int Focuser::getDcPower(int index) {
  if (index < 0 || index > 5) return 0;
  return settings[index].dcPower;
}

// set DC power in %
bool Focuser::setDcPower(int index, int value) {
  if (index < 0 || index > 5) return false;
  if (value < 0 || value > 100) return false;
  settings[index].dcPower = value;
  writeSettings(index);
  return true;
}

// get TCF enable
bool Focuser::getTcfEnable(int index) {
  if (index < 0 || index > 5) return false;
  return settings[index].tcf.enabled;
}

// set TCF enable
bool Focuser::setTcfEnable(int index, bool value) {
  if (index < 0 || index > 5) return false;
  settings[index].tcf.enabled = value;
  writeSettings(index);
  return true;
}

// get TCF coefficient, in microns per deg. C
float Focuser::getTcfCoef(int index) {
  if (index < 0 || index > 5) return 0.0F;
  return settings[index].tcf.coef;
}

// set TCF coefficient, in microns per deg. C
bool Focuser::setTcfCoef(int index, float value) {
  if (index < 0 || index > 5) return false;
  if (abs(value) >= 1000.0F) return false;
  settings[index].tcf.coef = value;
  writeSettings(index);
  return true;
}

// get TCF deadband, in microns
int Focuser::getTcfDeadband(int index) {
  if (index < 0 || index > 5) return 0;
  return settings[index].tcf.deadband;
}

// set TCF deadband, in microns
bool Focuser::setTcfDeadband(int index, int value) {
  if (index < 0 || index > 5) return false;
  if (value < 1 || value > 32767) return false;
  settings[index].tcf.deadband = value;
  writeSettings(index);
  return true;
}

// get TCF T0, in deg. C
float Focuser::getTcfT0(int index) {
  if (index < 0 || index > 5) return 0.0F;
  return settings[index].tcf.t0;
}

// set TCF T0, in deg. C
bool Focuser::setTcfT0(int index, float value) {
  if (index < 0 || index > 5) return false;
  if (abs(value) > 100.0F) return false;
  settings[index].tcf.t0 = value;
  writeSettings(index);
  return true;
}

// get backlash in microns
int Focuser::getBacklash(int index) {
  if (index < 0 || index > 5) return 0;
  return settings[index].backlash;
}

// set backlash in microns
bool Focuser::setBacklash(int index, int value) {
  if (index < 0 || index > 6) return false;
  if (value < 0 || value > 10000) return false;
  settings[index].backlash = value;
  writeSettings(index);
  return true;
}

void Focuser::readSettings(int index) {
  nv.readBytes(NV_FOCUSER_SETTINGS_BASE + index*FocuserSettingsSize, &settings[index], sizeof(Settings));
}

void Focuser::writeSettings(int index) {
  nv.updateBytes(NV_FOCUSER_SETTINGS_BASE + index*FocuserSettingsSize, &settings[index], sizeof(Settings));
}

#endif
