//--------------------------------------------------------------------------------------------------
// Power monitor + safety (auxiliary/AF)

#include "PowerMonitor.h"

#ifdef POWER_MONITOR_PRESENT

#include "../../../lib/tasks/OnTask.h"

// Adjust these two includes to match your tree (these are the typical locations)
#include "../Features.h"
#include "../../Telescope.h"

static void powerWrapper() { powerMonitor.poll(); }

void PowerMonitor::init() {
  VLF("MSG: Auxiliary, starting: power monitor");

  // Run twice per second; priority level 7 as precise timing isn't very important
  tasks.add(200, 0, true, 7, powerWrapper);

  #if defined(FAN_PIN)
    fanMode = FM_OFF;
  #endif
}

void PowerMonitor::poll() {
  float v = 0.0F;
  bool disableFeatures = false;

  #if defined(I_SENSE_CHANNEL_MAX) && (I_SENSE_COMBINED_MAX)
    float currentSenseTotal = 0.0F;
  #endif

  // ---------------- Voltage sampling ----------------
  #if defined(V_SENSE_PINS) && defined(V_SENSE_FORMULA)
    const int voltageSensePin[8] = V_SENSE_PINS;

    for (int j = 0; j < 8; j++) {
      if (voltageSensePin[j] >= 0) {
        if (isnan(voltageV[j])) delay(10);

        // Updated analog layer: volts best-effort.
        v = analog.readV((int16_t)voltageSensePin[j]);

        const float vv = (float)(V_SENSE_FORMULA);

        if (isnan(voltageV[j])) voltageV[j] = vv;
        else voltageV[j] = (voltageV[j] * 6.0F + vv) / 7.0F;

        #if defined(V_SENSE_LIMIT_LOW) && defined(V_SENSE_LIMIT_HIGH)
          if (j != V_SENSE_LIMIT_EXCLUDE &&
              (voltageV[j] < V_SENSE_LIMIT_LOW || voltageV[j] > V_SENSE_LIMIT_HIGH)) {
            VLF("MSG: PowerMonitor, OV/UV");
            disableFeatures = true;
          }
        #endif
      }
    }
  #endif

  Y;

  // ---------------- Current sampling ----------------
  #if defined(I_SENSE_PINS) && defined(I_SENSE_FORMULA)
    const int currentSensePin[8] = I_SENSE_PINS;

    for (int j = 0; j < 8; j++) {
      if (currentSensePin[j] >= 0) {
        if (isnanf(currentA[j])) delay(10);

        // Updated analog layer: volts best-effort (fed into I_SENSE_FORMULA).
        v = analog.readV((int16_t)currentSensePin[j]);

        const float ii = (float)(I_SENSE_FORMULA);

        if (isnanf(currentA[j])) currentA[j] = ii;
        else currentA[j] = (currentA[j] * 6.0F + ii) / 7.0F;

        #if defined(I_SENSE_CHANNEL_MAX)
          const float currentSenseMax[8] = I_SENSE_CHANNEL_MAX;

          if (fabsf(currentA[j]) > currentSenseMax[j]) {
            VF("MSG: PowerMonitor, OC channel"); V(j);
            VF(" current="); V(currentA[j]); VLF("A");
            features.deviceOff(j);
          }

          #if defined(I_SENSE_COMBINED_MAX)
            currentSenseTotal += fabsf(currentA[j]);
            if (currentSenseTotal > I_SENSE_COMBINED_MAX) {
              VF("MSG: PowerMonitor, OC combined");
              VF(" current="); V(currentSenseTotal); VLF("A");
              disableFeatures = true;
            }
          #endif
        #endif
      }
    }
  #endif

  Y;

  // ---------------- Fan / OT safety ----------------
  #if defined(FAN_PIN)
    FanMode lastFanMode = fanMode;

    // telescope.mcuTemperature exists; NAN means unavailable.
    if (!isnan(telescope.mcuTemperature) && (int32_t)(millis() - nextFanModeChangeTime) > 0) {

      #if defined(FAN_THRESHOLD_OT)
        if (telescope.mcuTemperature > FAN_THRESHOLD_OT) {
          VLF("MSG: PowerMonitor, MCU OT");
          disableFeatures = true;
        }
      #endif

      #if defined(FAN_THRESHOLD_HIGH) && defined(FAN_POWER_HIGH)
        if (telescope.mcuTemperature > FAN_THRESHOLD_HIGH) {
          if (fanMode != FM_HIGH) {
            VLF("MSG: PowerMonitor, FAN high");
            analog.write((int16_t)FAN_PIN, (float)(FAN_POWER_HIGH / 100.0F));
            fanMode = FM_HIGH;
          }
        } else
      #endif

      #if defined(FAN_THRESHOLD_MID) && defined(FAN_POWER_MID)
        if (telescope.mcuTemperature > FAN_THRESHOLD_MID) {
          if (fanMode != FM_MID) {
            VLF("MSG: PowerMonitor, FAN mid");
            analog.write((int16_t)FAN_PIN, (float)(FAN_POWER_MID / 100.0F));
            fanMode = FM_MID;
          }
        } else
      #endif

      #if defined(FAN_THRESHOLD_LOW) && defined(FAN_POWER_LOW)
        if (telescope.mcuTemperature > FAN_THRESHOLD_LOW) {
          if (fanMode != FM_LOW) {
            VLF("MSG: PowerMonitor, FAN low");
            analog.write((int16_t)FAN_PIN, (float)(FAN_POWER_LOW / 100.0F));
            fanMode = FM_LOW;
          }
        } else
      #endif

      if (fanMode != FM_OFF) {
        VLF("MSG: PowerMonitor, FAN off");
        analog.write((int16_t)FAN_PIN, 0.0f);
        fanMode = FM_OFF;
      }
    }

    if (lastFanMode != fanMode) nextFanModeChangeTime = millis() + 30000UL;
  #endif

  // ---------------- Global disable ----------------
  #if (defined(V_SENSE_PINS) && defined(V_SENSE_FORMULA)) || (defined(I_SENSE_PINS) && defined(I_SENSE_FORMULA))
    if (disableFeatures) {
      VLF("MSG: PowerMonitor, disable all features");
      for (int j = 0; j < 8; j++) features.deviceOff(j);
    }
  #endif
}

PowerMonitor powerMonitor;

#endif
