//--------------------------------------------------------------------------------------------------
// Power monitor + safety (auxiliary/AF)
//
//   - Samples V/I channels (optional) with smoothing
//   - Applies safety shutoffs to Features (per-channel OC, combined OC, OV/UV, MCU OT)
//   - Optional fan control based on telescope.mcuTemperature (NAN => unavailable)
//

#pragma once

#include "../../../Common.h"

#ifdef POWER_MONITOR_PRESENT

#include "../../../lib/analog/Analog.h"
#include <math.h>

class Features;
class Telescope;

class PowerMonitor {
public:
  bool init();

  // telemetry access (NAN if never sampled / pin not present)
  float getVoltage(uint8_t ch) const { return (ch < 8) ? averagedVoltage[ch] : NAN; }
  float getCurrent(uint8_t ch) const { return (ch < 8) ? averagedCurrent[ch] : NAN; }

  bool hasVoltage(uint8_t ch) const { return (ch < 8) && !isnan(averagedVoltage[ch]); }
  bool hasCurrent(uint8_t ch) const { return (ch < 8) && !isnan(averagedCurrent[ch]); }

  // channel exists if any telemetry is available
  bool hasChannel(uint8_t ch) const { return hasVoltage(ch) || hasCurrent(ch); }

  // fault flags (best-effort; cleared each poll unless fault re-triggers)
  bool errOverCurrent(uint8_t ch) const { return (ch < 8) ? ocFault[ch] : false; }
  bool errUnderVoltage(uint8_t ch) const { return (ch < 8) ? uvFault[ch] : false; }
  bool errOverVoltage(uint8_t ch) const { return (ch < 8) ? ovFault[ch] : false; }
  bool errOverTemperature(uint8_t ch) const { return (ch < 8) ? otFault[ch] : false; }

  void poll();

private:
  float averagedVoltage[8] = {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};
  float averagedCurrent[8] = {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};

  bool ocFault[8] = {false, false, false, false, false, false, false, false};
  bool uvFault[8] = {false, false, false, false, false, false, false, false};
  bool ovFault[8] = {false, false, false, false, false, false, false, false};
  bool otFault[8] = {false, false, false, false, false, false, false, false};

  enum FanMode : uint8_t { FM_OFF = 0, FM_LOW, FM_MID, FM_HIGH };
  FanMode fanMode = FM_OFF;
  uint32_t nextFanModeChangeTime = 0;
};

extern PowerMonitor powerMonitor;

#endif
