
//--------------------------------------------------------------------------------------------------
// OnStepX focuser control

#include "Focuser.h"

#ifdef FOCUSER_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;
#include "../../lib/weather/Weather.h"
#include "../../lib/temperature/Temperature.h"
#include "../Telescope.h"

void tcfWrapper() { focuser.tcfMonitor(); }
void parkWrapper0() { focuser.parkMonitor(0); }
void parkWrapper1() { focuser.parkMonitor(1); }
void parkWrapper2() { focuser.parkMonitor(2); }
void parkWrapper3() { focuser.parkMonitor(3); }
void parkWrapper4() { focuser.parkMonitor(4); }
void parkWrapper5() { focuser.parkMonitor(5); }

void Focuser::init() {
  // wait a moment for any background processing that may be needed
  delay(1000);
  
  // confirm the data structure size
  if (FocuserSettingsSize < sizeof(FocuserSettings)) { initError.nv = true; DL("ERR: Focuser::init(); FocuserSettingsSize error NV subsystem writes disabled"); nv.readOnly(true); }

  // init settings stored in NV
  if (!validKey) {
    for (int index = 0; index < FOCUSER_MAX; index++) {
      VF("MSG: Focuser"); V(index + 1); VLF(", writing default settings to NV");
      settings[index].tcf.enabled = false;
      settings[index].tcf.coef = 0.0F;
      settings[index].tcf.deadband = 1;
      settings[index].tcf.t0 = 0.0F;
      settings[index].dcPower = 50;
      settings[index].backlash = 0.0F;
      settings[index].position = 0.0F;
      writeSettings(index);
    }
  }

  // get settings
  for (int index = 0; index < FOCUSER_MAX; index++) {
    readSettings(index);

    // defaults
    axis[index] = NULL;
    parked[index] = true;
    parkHandle[index] = 0;
    moveRate[index] = 100;
    tcfSteps[index] = 0;

    if (driverModel[index] != OFF) {
      axis[index] = new Axis;
      if (axis[index] != NULL) {
        V("MSG: Focuser"); V(index + 1); V(", init (Axis"); V(index + 4); VL(")");
        axis[index]->init(index + 4, false);

        // TCF defaults to disabled at startup
        settings[index].tcf.enabled = false;

        if (settings[index].position < axis[index]->settings.limits.min) {
          settings[index].position = axis[index]->settings.limits.min;
          initError.value = true;
          DLF("ERR, Focuser.init(): bad NV park pos < _LIMIT_MIN (set to _LIMIT_MIN)");
        }

        if (settings[index].position > axis[index]->settings.limits.max) {
          settings[index].position = axis[index]->settings.limits.max;
          initError.value = true;
          DLF("ERR, Focuser.init(): bad NV park pos > _LIMIT_MAX steps (set to _LIMIT_MAX)");
        }

        axis[index]->resetPositionSteps(0);
        axis[index]->setBacklashSteps(settings[index].backlash);
        axis[index]->setFrequencyMax(slewRateDesired[index]);
        axis[index]->setFrequencyMin(slewRateMinimum[index]);
        axis[index]->setFrequencySlew(slewRateDesired[index]);
        axis[index]->setSlewAccelerationTime(accelerationTime[index]);
        axis[index]->setSlewAccelerationTimeAbort(rapidStopTime[index]);
        if (powerDown[index]) axis[index]->setPowerDownTime(DEFAULT_POWER_DOWN_TIME);

        unpark(index);
      }
    }
  }

  // start task for temperature compensated focusing
  VF("MSG: Focuser, starting TCF task (rate 1s priority 6)... ");
  if (tasks.add(1000, 0, true, 6, tcfWrapper, "FocPoll")) { VL("success"); } else { VL("FAILED!"); }
}

// get focuser temperature in deg. C
float Focuser::getTemperature() {
  float t = temperature.getChannel(0);
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
CommandError Focuser::setTcfEnable(int index, bool value) {
  if (index < 0 || index > 5) return CE_CMD_UNKNOWN;
  if (parked[index]) return CE_PARKED;

  settings[index].tcf.enabled = value;
  if (value) {
    settings[index].tcf.t0 = getTemperature();
   } else {
     target[index] += tcfSteps[index];
     tcfSteps[index] = 0;
   }
  writeSettings(index);
  return CE_NONE;
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

// get TCF deadband, in steps
int Focuser::getTcfDeadband(int index) {
  if (index < 0 || index > 5) return 0;
  return settings[index].tcf.deadband;
}

// set TCF deadband, in steps
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

// get backlash in steps
int Focuser::getBacklash(int index) {
  if (index < 0 || index > 5) return 0;
  return settings[index].backlash;
}

// set backlash in steps
CommandError Focuser::setBacklash(int index, int value) {
  if (index < 0 || index > 6) return CE_CMD_UNKNOWN;
  if (value < 0 || value > 10000) return CE_PARAM_RANGE;
  if (parked[index]) return CE_PARKED;

  settings[index].backlash = value;
  writeSettings(index);
  axis[index]->setBacklash(getBacklash(index));
  return CE_NONE;
}

// start slew in the specified direction
CommandError Focuser::slew(int index, Direction dir) {
  if (index < 0 || index > 5) return CE_CMD_UNKNOWN;
  if (axis[index] == NULL) return CE_PARAM_RANGE;
  if (parked[index]) return CE_PARKED;

  axis[index]->setFrequencyBase(0.0F);
  axis[index]->motor.resetTargetToMotorPosition();
  return axis[index]->autoSlew(dir, moveRate[index]);
}

// move focuser to a specific location (in steps)
CommandError Focuser::gotoTarget(int index, long target) {
  if (index < 0 || index > 5) return CE_CMD_UNKNOWN;
  if (axis[index] == NULL) return CE_PARAM_RANGE;
  if (parked[index]) return CE_PARKED;

  VF("MSG: Focuser"); V(index + 1); V(", goto target coordinate set ("); V(target/axis[index]->getStepsPerMeasure()); VL("um)");
  VF("MSG: Focuser"); V(index + 1); V(", starting goto at slew rate ("); V(slewRateDesired[index]); VL("um/s)");

  axis[index]->setFrequencyBase(0.0F);
  axis[index]->setTargetCoordinateSteps(target + tcfSteps[index]);
  return axis[index]->autoSlewRateByDistance(slewRateDesired[index]*accelerationTime[index], slewRateDesired[index]);
}

// park focuser at its current location
CommandError Focuser::park(int index) {
  if (index < 0 || index > 5) return CE_PARAM_RANGE;
  if (axis[index] == NULL) return CE_NONE;
  if (parked[index]) return CE_NONE;

  setTcfEnable(index, false);

  VF("MSG: Focuser"); V(index + 1); VLF(", parking");
  axis[index]->setBacklash(0.0F);
  float position = axis[index]->getInstrumentCoordinate();

  settings[index].position = position;
  writeSettings(index);
  parked[index] = true;

  startParkMonitor(index);
  return axis[index]->autoSlewRateByDistance(slewRateDesired[index]*accelerationTime[index], slewRateDesired[index]);
}

// unpark focuser
CommandError Focuser::unpark(int index) {
  if (index < 0 || index > 5) return CE_PARAM_RANGE;
  if (axis[index] == NULL) return CE_NONE;
  if (!parked[index]) return CE_NOT_PARKED;

  axis[index]->setBacklash(0.0F);
  float position = settings[index].position;
  axis[index]->setInstrumentCoordinatePark(position);
  V("MSG: Focuser"); V(index + 1); V(", unpark motor position "); VL(axis[index]->getMotorPositionSteps());

  axis[index]->enable(true);
  axis[index]->setBacklash(settings[index].backlash);
  axis[index]->setTargetCoordinate(position);
  parked[index] = false;
  settings[index].position = position;
  target[index] = position - tcfSteps[index];

  startParkMonitor(index);
  return axis[index]->autoSlewRateByDistance(slewRateDesired[index]*accelerationTime[index], slewRateDesired[index]);
}

// start park/unpark monitor
void Focuser::startParkMonitor(int index) {
  VF("MSG: Focuser"); V(index + 1); VF(", start park monitor task (rate 1s priority 6)... ");
  if (parkHandle != 0) tasks.remove(parkHandle[index]);
  switch (index) {
    case 0: parkHandle[index] = tasks.add(1000, 0, true, 6, parkWrapper0, "FocPrk1"); break;
    case 1: parkHandle[index] = tasks.add(1000, 0, true, 6, parkWrapper1, "FocPrk2"); break;
    case 2: parkHandle[index] = tasks.add(1000, 0, true, 6, parkWrapper2, "FocPrk3"); break;
    case 3: parkHandle[index] = tasks.add(1000, 0, true, 6, parkWrapper3, "FocPrk4"); break;
    case 4: parkHandle[index] = tasks.add(1000, 0, true, 6, parkWrapper4, "FocPrk5"); break;
    case 5: parkHandle[index] = tasks.add(1000, 0, true, 6, parkWrapper5, "FocPrk6");
  }
  if (parkHandle[index]) { VL("success"); } else { VL("FAILED!"); }
}

void Focuser::readSettings(int index) {
  nv.readBytes(NV_FOCUSER_SETTINGS_BASE + index*FocuserSettingsSize, &settings[index], sizeof(FocuserSettings));
  if (fabs(settings[index].tcf.coef) > 999.0F) { settings[index].tcf.coef = 0.0F;  initError.value = true; DLF("ERR, Focuser.init(): bad NV |tcf.coef| > 999.0 um/deg. C (set to 0.0)"); }
  if (settings[index].tcf.deadband < 1 )       { settings[index].tcf.deadband = 1; initError.value = true; DLF("ERR, Focuser.init(): bad NV tcf.deadband < 1 steps (set to 1)"); }
  if (settings[index].tcf.deadband > 10000 )   { settings[index].tcf.deadband = 1; initError.value = true; DLF("ERR, Focuser.init(): bad NV tcf.deadband > 10000 steps (set to 1)"); }
  if (fabs(settings[index].tcf.t0) > 60.0F)    { settings[index].tcf.t0 = 10.0F;   initError.value = true; DLF("ERR, Focuser.init(): bad NV |tcf.t0| > 60.0 deg. C (set to 10.0)"); }
  if (settings[index].backlash < 0)            { settings[index].backlash = 0;     initError.value = true; DLF("ERR, Focuser.init(): bad NV backlash < 0 steps (set to 0)"); }
  if (settings[index].backlash > 10000)        { settings[index].backlash = 0;     initError.value = true; DLF("ERR, Focuser.init(): bad NV backlash > 10000 steps (set to 0)"); }
}

void Focuser::writeSettings(int index) {
  nv.updateBytes(NV_FOCUSER_SETTINGS_BASE + index*FocuserSettingsSize, &settings[index], sizeof(FocuserSettings));
}

// poll TCF to move the focusers as required
void Focuser::tcfMonitor() {
  float t = getTemperature();
  for (int index = 0; index < FOCUSER_MAX; index++) {
    if (axis[index] != NULL) {
      if (!axis[index]->isSlewing()) {
        if ((long)(millis() - afterSlewWait[index]) > 0) {
          afterSlewWait[index] = millis();
          if (settings[index].tcf.enabled) {
            Y;

            // only allow TCF if telescope temperature is good
            if (!isnan(t)) {
              // get offset in microns due to TCF
              float offset = settings[index].tcf.coef * (settings[index].tcf.t0 - t);
              // convert to steps
              offset *= (float)axis[index]->getStepsPerMeasure();
              // apply deadband
              long steps = lroundf(offset/settings[index].tcf.deadband)*settings[index].tcf.deadband;
              // update target if required
              if (tcfSteps[index] != steps) {
                VF("MSG: Focuser"); V(index + 1); V(", TCF offset changed moving to target "); 
                if (steps >=0 ) V("+ "); else V("- "); V(fabs(steps/axis[index]->getStepsPerMeasure())); VL("um");
                tcfSteps[index] = steps;
                axis[index]->setTargetCoordinateSteps(target[index] + tcfSteps[index]);
              }
            }

            // move to the target at 20 um/s
            if (!axis[index]->atTarget()) {
              axis[index]->setSynchronized(false);
              axis[index]->setFrequencyBase(20.0F);
            } else {
              axis[index]->setFrequencyBase(0.0F);
            }

          } else tcfSteps[index] = 0;
        }
      } else {
        afterSlewWait[index] = millis() + 2000;
        target[index] = axis[index]->getInstrumentCoordinateSteps() - tcfSteps[index];
      }
    }
  }
}

// poll for park completion
void Focuser::parkMonitor(int index) {
  #ifdef MOUNT_PRESENT
    if (!axis[index]->isSlewing()) {

      if (parked[index]) {
        axis[index]->enable(false);
        #if DEBUG == VERBOSE
          long offset = axis[index]->getInstrumentCoordinateSteps() - axis[index]->getMotorPositionSteps();
          VF("MSG: Focuser"); V(index + 1); VF(", parked motor target   "); VL(axis[index]->getTargetCoordinateSteps() - offset);
          VF("MSG: Focuser"); V(index + 1); VF(", parked motor position "); VL(axis[index]->getMotorPositionSteps());
        #endif
      }

      tasks.setDurationComplete(parkHandle[index]);
      parkHandle[index] = 0;
      VF("MSG: Focuser"); V(index + 1); VLF(", stop park monitor task");
    }
  #endif
}

Focuser focuser;

#endif
