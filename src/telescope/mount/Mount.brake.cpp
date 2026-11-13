//--------------------------------------------------------------------------------------------------
// Mount brakes: startup release and latched motor faults

#include "Mount.h"

#ifdef MOUNT_PRESENT

#if MOUNT_BRAKE == ON
#include "../../lib/tasks/OnTask.h"
#include "../Telescope.h"

namespace {
  void mountBrakeWrapper() { mount.motorFault(); }
}

void Mount::engageBrake() {
  digitalWriteEx(MOUNT_BRAKE_PIN, !MOUNT_BRAKE_RELEASE_STATE);
  #if MOUNT_BRAKE_HOLD_PIN != OFF
    digitalWriteEx(MOUNT_BRAKE_HOLD_PIN, !MOUNT_BRAKE_HOLD_STATE);
  #endif
  brakeReleased = false;
}

void Mount::releaseBrake() {
  if (motorFault() || !axis1.motor->enabled || !axis2.motor->enabled) {
    latchBrakeFault();
    return;
  }

  digitalWriteEx(MOUNT_BRAKE_PIN, MOUNT_BRAKE_RELEASE_STATE);
  tasks.yield(MOUNT_BRAKE_RELEASE_DELAY);

  if (motorFault() || !axis1.motor->enabled || !axis2.motor->enabled) {
    digitalWriteEx(MOUNT_BRAKE_PIN, !MOUNT_BRAKE_RELEASE_STATE);
    latchBrakeFault();
    return;
  }

  #if MOUNT_BRAKE_HOLD_PIN != OFF
    digitalWriteEx(MOUNT_BRAKE_HOLD_PIN, MOUNT_BRAKE_HOLD_STATE);
    tasks.yield(MOUNT_BRAKE_HOLD_DELAY);

    digitalWriteEx(MOUNT_BRAKE_PIN, !MOUNT_BRAKE_RELEASE_STATE);

    if (motorFault() || !axis1.motor->enabled || !axis2.motor->enabled) {
      digitalWriteEx(MOUNT_BRAKE_HOLD_PIN, !MOUNT_BRAKE_HOLD_STATE);
      latchBrakeFault();
      return;
    }

    #endif

  brakeReleased = true;
}

void Mount::initBrake() {
  // Establish the engaged output before either mount motor is initialized.
  engageBrake();
  pinModeEx(MOUNT_BRAKE_PIN, OUTPUT);
  #if MOUNT_BRAKE_HOLD_PIN != OFF
    pinModeEx(MOUNT_BRAKE_HOLD_PIN, OUTPUT);
  #endif
  // Some cores ignore writes until OUTPUT is configured; establish the inactive levels again.
  engageBrake();
}

void Mount::latchBrakeFault() {
  if (brakeFaultLatched) return;
  brakeFaultLatched = true;
  engageBrake();
  DLF("ERR: Mount, motor fault latched; brakes engaged until reboot!");
}

void Mount::beginBrake() {
  // The normal calibration and coordinate initialization have already completed with brakes engaged.
  brakeMonitoring = true;
  if (initError.driver || !axis1.motor->ready || !axis2.motor->ready) {
    latchBrakeFault();
    return;
  }

  // Keep fault sampling separate from the slower tracking monitor. No waits occur in this task.
  if (!tasks.add(10, 0, true, 2, mountBrakeWrapper, "MtBrake")) {
    initError.driver = true;
    latchBrakeFault();
    return;
  }

  // Some drivers confirm enable asynchronously. Waiting here does not block runtime motion commands.
  const uint32_t started = millis();
  while (!axis1.motor->enabled || !axis2.motor->enabled) {
    if (motorFault() || (uint32_t)(millis() - started) >= 1000U) {
      latchBrakeFault();
      return;
    }
    tasks.yield(1);
  }

  tasks.yield(MOUNT_BRAKE_STARTUP_DELAY);
  if (motorFault()) return;
  if (!axis1.motor->enabled || !axis2.motor->enabled) {
    latchBrakeFault();
    return;
  }

  releaseBrake();
}
#endif

bool Mount::motorFault() {
  #if MOUNT_BRAKE == ON
    if (brakeFaultLatched) return true;
  #endif
  const bool fault = axis1.motorFault() || axis2.motorFault();
  #if MOUNT_BRAKE == ON
    if (brakeMonitoring && fault) latchBrakeFault();
  #endif
  return fault;
}

#endif
