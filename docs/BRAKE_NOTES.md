# Mount brakes

`MOUNT_BRAKE ON` enables startup-only brake release and a motor fault latch in the
mount class. Both mount axes must be present, `MOUNT_ENABLE_IN_STANDBY` must be
`ON`, and `AXIS1_POWER_DOWN` and `AXIS2_POWER_DOWN` must be `OFF`.

One required output and an optional hold output control the brake circuit:

| Setting | Purpose |
| --- | --- |
| `MOUNT_BRAKE_PIN` | Brake release/pull-in control |
| `MOUNT_BRAKE_HOLD_PIN` | Optional lower-current holding control; default `OFF`; must differ from the release pin |
| `MOUNT_BRAKE_RELEASE_STATE` | Active level of the high-current control; default `HIGH` |
| `MOUNT_BRAKE_HOLD_STATE` | Active level of the hold control; defaults to the release state |

Both pins must be output-capable native MCU GPIOs. Encoded `GPIO_PIN(...)`
expander outputs and `DAC_PIN(...)` outputs are rejected by validation.

All configured controls inactive means the brake is mechanically engaged. The external
brake circuit supplies the current levels; firmware controls its inputs.
One circuit may control both mount brakes.

The inactive levels are written before and again after configuring the outputs,
so cores that ignore writes before output configuration also start with the
brake engaged, before mount motor initialization. The existing
driver calibration runs with the brake engaged, followed by the existing motor
enable calls. Calibration itself is unchanged. Brake release requires successful
driver initialization, both motors enabled, and no reported motor fault.

Startup then performs this sequence:

1. Wait `MOUNT_BRAKE_STARTUP_DELAY` (default 250 ms).
2. Activate the high-current control.
3. Wait `MOUNT_BRAKE_RELEASE_DELAY` (default 500 ms).
4. Activate the lower-current hold control, leaving high current active.
5. Wait `MOUNT_BRAKE_HOLD_DELAY` (default 100 ms).
6. Deactivate high current, leaving only the hold control active.

With `MOUNT_BRAKE_HOLD_PIN OFF`, startup performs only steps 1 through 3 and leaves
`MOUNT_BRAKE_PIN` active to keep the brake released. The hold delay and handoff are
skipped, and the unused hold pin is never accessed. A fault or explicit disable
deactivates the release pin to engage the brake.

Each delay accepts an integer from **0 to 10000 milliseconds**, inclusive. These
waits occur during startup and service the scheduler, including fault monitoring.
Drivers that acknowledge enable asynchronously have up to one second to do so
before this sequence starts.

A separate mount task checks the existing axis motor fault reports every 10 ms.
The driver's own status update interval still applies. A fault observed by this
task or another caller of `Mount::motorFault()` on either
mount axis immediately deactivates all configured controls and latches the fault until
reboot, including during the release sequence. A cleared driver fault cannot
release the brake again or re-enable the mount. The existing mount fault handling
continues to abort motion and disable the motors.

A transient fault that clears before the mount observes it may be missed, even
if an axis-level check saw it and aborted motion. This version leaves that
behavior unchanged. Clearing a fault report does not guarantee that every motor
backend remains enabled; CAN motor backends can stop themselves independently.

An explicit mount disable also deactivates all configured brake controls. Brake release is
only performed at startup, so a reboot is required after an explicit disable too.
Normal tracking stops and parking retain motor power because standby enable is
required.

The `:ERESET#` command and the reboot after setting absolute encoder origins
explicitly disable the mount before flushing NV storage and resetting. This
commands brake engagement before motor power is disabled. If the storage flush
fails and the reset is cancelled, the mount stays disabled with the brake engaged.

Startup failure, failure to create the brake monitor task, or failure to enable
either motor keeps the brake engaged. Driver calibration has no success return;
the post-calibration checks use the existing readiness and fault reports.

Servo-internal shutdown behavior is unchanged in this version: a servo can
disable its own driver before the mount detects its fault and engages the brake.
This feature does not alter that sequence.

Leave `MOUNT_BRAKE OFF` (the default) to retain normal mount operation.
