
//--------------------------------------------------------------------------------------------------
// OnStepX focuser control

#include "Focuser.h"

#ifdef FOCUSER_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;
#include "../../lib/weather/Weather.h"
#include "../../lib/temperature/Temperature.h"
#include "../Telescope.h"

void tcfWrapper() { telescope.focuser.tcfPoll(); }

// initialize all focusers
void Focuser::init(bool validKey) {

  // wait a moment for any background processing that may be needed
  delay(1000);
  
  // init settings stored in NV
  if (!validKey) {
    VLF("MSG: Focusers, writing default settings to NV");
    for (int index = 0; index < FOCUSER_MAX; index++) {
      settings[index].tcf.enabled = false;
      settings[index].tcf.coef = 0.0F;
      settings[index].tcf.deadband = 1;
      settings[index].tcf.t0 = 0.0F;
      settings[index].dcPower = 50;
      settings[index].backlash = 0.0F;
      writeSettings(index);
    }
  }

  // get settings
  for (int index = 0; index < FOCUSER_MAX; index++) readSettings(index);

  for (int index = 0; index < FOCUSER_MAX; index++) {
    axis[index] = NULL;
    moveRate[index] = 100;
    tcfSteps[index] = 0;
    if (driverModel[index] != OFF) {
      axis[index] = new Axis;
      if (axis[index] != NULL) {
        V("MSG: Focuser"); V(index + 1); V(", init (Axis"); V(index + 4); VL(")");
        axis[index]->init(index + 4, false, validKey);
        axis[index]->resetPositionSteps(0);
        axis[index]->setBacklashSteps(settings[index].backlash);
        axis[index]->setFrequencyMax(slewRateDesired[index]);
        axis[index]->setFrequencyMin(slewRateMinimum[index]);
        axis[index]->setFrequencySlew(slewRateDesired[index]);
        if (powerDown[index]) axis[index]->setPowerDownTime(DEFAULT_POWER_DOWN_TIME);
      }
    }
  }

  if (FocuserSettingsSize < sizeof(Settings)) { initError.nv = true; DL("ERR: Focuser::init(); FocuserSettingsSize error NV subsystem writes disabled"); nv.readOnly(true); }

  // start task for temperature compensated focusing
  VF("MSG: Focuser, starting TCF task (rate 1s priority 7)... ");
  if (tasks.add(1000, 0, true, 7, tcfWrapper, "FocPoll")) { VL("success"); } else { VL("FAILED!"); }
}

// get focuser temperature in deg. C
float Focuser::getTemperature() {
  float t = temperature.getChannel(0);
  if (isnan(t)) t = 10.0;
  return t;
}

// check for DC motor focuser
bool Focuser::isDC(int index) {
  if (index < 0 || index > 5) return false;
  return false;
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
  if (value) settings[index].tcf.t0 = getTemperature(); else tcfSteps[index] = 0;
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
  if (value < 1 || value > 10000) return false;
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
  if (abs(value) > 60.0F) return false;
  settings[index].tcf.t0 = value;
  writeSettings(index);
  return true;
}

// poll TCF to move the focusers as required
void Focuser::tcfPoll() {
  for (int index = 0; index < FOCUSER_MAX; index++) {
    if (axis[index] != NULL) {
      if (settings[index].tcf.enabled) {
        Y;
        // get offset in microns due to TCF
        float offset = settings[index].tcf.coef * (getTemperature() - settings[index].tcf.t0);
        // convert to steps
        offset *= axis[index]->getStepsPerMeasure();
        // apply deadband
        long steps = lroundf(offset/settings[index].tcf.deadband)*settings[index].tcf.deadband;
        // move focuser if required
        if (tcfSteps[index] != steps) {
          tcfSteps[index] = steps;
          long t = axis[index]->getTargetCoordinateSteps();
          axis[index]->setTargetCoordinateSteps(t + tcfSteps[index]);
          axis[index]->setFrequencySlew(10);
          axis[index]->autoSlewRateByDistance(10);
        }
      }
    }
  }
}

// get backlash in steps
int Focuser::getBacklash(int index) {
  if (index < 0 || index > 5) return 0;
  return settings[index].backlash;
}

// set backlash in steps
bool Focuser::setBacklash(int index, int value) {
  if (index < 0 || index > 6) return false;
  if (value < 0 || value > 10000) return false;
  settings[index].backlash = value;
  writeSettings(index);
  return true;
}

void Focuser::setFrequencySlew(int index, float rate) {
  axis[index]->setFrequencySlew(rate);
  axis[index]->setSlewAccelerationRate((rate/accelerationRate[index])*accelerationRate[index]);
  axis[index]->setSlewAccelerationRateAbort(rapidStopRate[index]);
}

void Focuser::readSettings(int index) {
  nv.readBytes(NV_FOCUSER_SETTINGS_BASE + index*FocuserSettingsSize, &settings[index], sizeof(Settings));

  if (fabs(settings->tcf.coef) > 999.0F) { settings->tcf.coef = 0.0F;  initError.value = true; DLF("ERR, Focuser.init(): bad NV |tcf.coef| > 999.0 um/deg. C (set to 0.0)"); }
  if (settings->tcf.deadband < 1 )       { settings->tcf.deadband = 1; initError.value = true; DLF("ERR, Focuser.init(): bad NV tcf.deadband < 1 steps (set to 1)"); }
  if (settings->tcf.deadband > 10000 )   { settings->tcf.deadband = 1; initError.value = true; DLF("ERR, Focuser.init(): bad NV tcf.deadband > 10000 steps (set to 1)"); }
  if (fabs(settings->tcf.t0) > 60.0F)    { settings->tcf.t0 = 10.0F;   initError.value = true; DLF("ERR, Focuser.init(): bad NV |tcf.t0| > 60.0 deg. C (set to 10.0)"); }
  if (settings->backlash < 0)            { settings->backlash = 0;     initError.value = true; DLF("ERR, Focuser.init(): bad NV backlash < 0 steps (set to 0)"); }
  if (settings->backlash > 10000)        { settings->backlash = 0;     initError.value = true; DLF("ERR, Focuser.init(): bad NV backlash > 10000 steps (set to 0)"); }
}

void Focuser::writeSettings(int index) {
  nv.updateBytes(NV_FOCUSER_SETTINGS_BASE + index*FocuserSettingsSize, &settings[index], sizeof(Settings));
}

#endif
