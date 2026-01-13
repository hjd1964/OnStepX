//--------------------------------------------------------------------------------------------------
// Power monitor + safety (auxiliary/AF)

#include "PowerMonitor.h"

#ifdef POWER_MONITOR_PRESENT

#include "../../../lib/tasks/OnTask.h"

// Adjust these two includes to match your tree (these are the typical locations)
#include "../Features.h"
#include "../../Telescope.h"

static void powerWrapper() { powerMonitor.poll(); }

bool PowerMonitor::init() {
  VLF("MSG: PowerMonitor, starting: power monitor");

  VF("MSG: PowerMonitor, start voltage/current monitoring task (rate 200ms priority 7)... ");
  if (tasks.add(200, 0, true, 7, powerWrapper, "pwrMtr")) { VLF("success"); } else { VLF("FAILED!"); return false; }

  #if defined(FAN_PIN)
    fanMode = FM_OFF;
  #endif

  return true;
}

void PowerMonitor::poll() {
  float v = 0.0F;
  bool disableFeatures = false;

  // clear fault flags; they are re-asserted on each poll if the condition is present
  for (int j = 0; j < 8; j++) {
    ocFault[j] = false;
    uvFault[j] = false;
    ovFault[j] = false;
    otFault[j] = false;
  }

  // ---------------- Voltage sampling ----------------
  #if defined(POWER_MONITOR_VOLTAGE_PRESENT)
    const int voltageSensePin[8] = V_SENSE_PINS;

    for (int j = 0; j < 8; j++) {
      if (voltageSensePin[j] >= 0) {
        if (isnan(averagedVoltage[j])) delay(10);

        v = analog.readV((int16_t)voltageSensePin[j]);

        const float scaledVoltage = (float)(V_SENSE_FORMULA);

        if (isnan(averagedVoltage[j])) averagedVoltage[j] = scaledVoltage;
        else averagedVoltage[j] = (averagedVoltage[j] * 6.0F + scaledVoltage) / 7.0F;

        #if defined(V_SENSE_LIMIT_LOW) && defined(V_SENSE_LIMIT_HIGH)
          if (j != V_SENSE_LIMIT_EXCLUDE &&
              (averagedVoltage[j] < V_SENSE_LIMIT_LOW || averagedVoltage[j] > V_SENSE_LIMIT_HIGH)) {
            VLF("MSG: PowerMonitor, OV/UV");
            if (averagedVoltage[j] < V_SENSE_LIMIT_LOW) uvFault[j] = true;
            if (averagedVoltage[j] > V_SENSE_LIMIT_HIGH) ovFault[j] = true;
            disableFeatures = true;
          }
        #endif
      }
    }
  #endif

  Y;

  // ---------------- Current sampling ----------------
  #if defined(POWER_MONITOR_CURRENT_PRESENT)

    const int currentSensePin[8] = I_SENSE_PINS;
    #if defined(I_SENSE_CHANNEL_MAX) && (I_SENSE_COMBINED_MAX)
      float currentSenseTotal = 0.0F;
    #endif

    for (int j = 0; j < 8; j++) {
      if (currentSensePin[j] >= 0) {
        if (isnanf(averagedCurrent[j])) delay(10);

        v = analog.readV((int16_t)currentSensePin[j]);

        const float currentI = (float)(I_SENSE_FORMULA);

        if (isnanf(averagedCurrent[j])) averagedCurrent[j] = currentI;
        else averagedCurrent[j] = (averagedCurrent[j] * 6.0F + currentI) / 7.0F;

        #if defined(I_SENSE_CHANNEL_MAX)
          const float currentSenseMax[8] = I_SENSE_CHANNEL_MAX;

          if (fabsf(averagedCurrent[j]) > currentSenseMax[j]) {
            VF("MSG: PowerMonitor, OC channel"); V(j);
            VF(" current="); V(averagedCurrent[j]); VLF("A");
            ocFault[j] = true;
            features.deviceOff(j);
          }

          #if defined(I_SENSE_COMBINED_MAX)
            currentSenseTotal += fabsf(averagedCurrent[j]);
            if (currentSenseTotal > I_SENSE_COMBINED_MAX) {
              VF("MSG: PowerMonitor, OC combined");
              VF(" current="); V(currentSenseTotal); VLF("A");
              for (int k = 0; k < 8; k++) ocFault[k] = true;
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
          for (int k = 0; k < 8; k++) otFault[k] = true;
          disableFeatures = true;
        }
      #endif

      #if defined(FAN_THRESHOLD_HIGH) && defined(FAN_POWER_HIGH)
        if (telescope.mcuTemperature > FAN_THRESHOLD_HIGH) {
          if (fanMode != FM_HIGH) {
            VLF("MSG: PowerMonitor, fan high");
            analog.write((int16_t)FAN_PIN, (float)(FAN_POWER_HIGH/100.0F));
            fanMode = FM_HIGH;
          }
        } else
      #endif

      #if defined(FAN_THRESHOLD_MID) && defined(FAN_POWER_MID)
        if (telescope.mcuTemperature > FAN_THRESHOLD_MID) {
          if (fanMode != FM_MID) {
            VLF("MSG: PowerMonitor, fan mid");
            analog.write((int16_t)FAN_PIN, (float)(FAN_POWER_MID/100.0F));
            fanMode = FM_MID;
          }
        } else
      #endif

      #if defined(FAN_THRESHOLD_LOW) && defined(FAN_POWER_LOW)
        if (telescope.mcuTemperature > FAN_THRESHOLD_LOW) {
          if (fanMode != FM_LOW) {
            VLF("MSG: PowerMonitor, fan low");
            analog.write((int16_t)FAN_PIN, (float)(FAN_POWER_LOW/100.0F));
            fanMode = FM_LOW;
          }
        } else
      #endif

      if (fanMode != FM_OFF) {
        VLF("MSG: PowerMonitor, fan off");
        analog.write((int16_t)FAN_PIN, 0.0F);
        fanMode = FM_OFF;
      }
    }

    if (lastFanMode != fanMode) nextFanModeChangeTime = millis() + 30000UL;
  #endif

  if (disableFeatures) {
    VLF("MSG: PowerMonitor, disable all features");
    for (int j = 0; j < 8; j++) features.deviceOff(j);
  }
}

PowerMonitor powerMonitor;

#endif
