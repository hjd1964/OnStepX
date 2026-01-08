//--------------------------------------------------------------------------------------------------
// Power monitor + safety (auxiliary/AF)
//
//   - Samples V/I channels (optional) with smoothing
//   - Applies safety shutoffs to Features (per-channel OC, combined OC, OV/UV, MCU OT)
//   - Optional fan control based on telescope.mcuTemperature (NAN => unavailable)
//
// Intended to be scheduled periodically (e.g. tasks.add(200, 0, true, 7, ...)).

#pragma once

#include "../../../Common.h"

#ifdef POWER_MONITOR_PRESENT

#include "../../../lib/analog/Analog.h"
#include <math.h>

class Features;
class Telescope;

class PowerMonitor {
public:
  void init();

  // Scheduled poll (called by tasks)
  void poll();

  // Telemetry access (NAN if never sampled / pin not present).
  float getVoltage(uint8_t ch) const { return (ch < 8) ? voltageV[ch] : NAN; }
  float getCurrent(uint8_t ch) const { return (ch < 8) ? currentA[ch] : NAN; }

  bool hasVoltage(uint8_t ch) const { return (ch < 8) && !isnan(voltageV[ch]); }
  bool hasCurrent(uint8_t ch) const { return (ch < 8) && !isnan(currentA[ch]); }

private:
  float voltageV[8] = {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};
  float currentA[8] = {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};

  enum FanMode : uint8_t { FM_OFF = 0, FM_LOW, FM_MID, FM_HIGH };
  FanMode fanMode = FM_OFF;
  uint32_t nextFanModeChangeTime = 0;
};

extern PowerMonitor powerMonitor;

#endif
