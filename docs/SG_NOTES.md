# StallGuard Notes

StallGuard is a sensorless stall/load detector available in supported Trinamic
TMC stepper drivers. OnStepX can use the driver's StallGuard result while an
axis is slewing to detect a motor overload, obstruction, mechanical bind, or
loss of motion.

This feature is not a substitute for good mechanical limits, balanced loading,
proper current settings, or careful first testing. Treat it as an extra layer of
protection.

## Requirements

StallGuard support requires:

- `DRIVER_TMC_STEPPER`
- `DRIVER_TMC_STEPPER_STALLGUARD`
- a TMC driver supported by the StallGuard helper
- the correct slewing decay mode for the driver family

Driver mode requirements:

- TMC2209: `decaySlewing` must be `STEALTHCHOP`
- TMC2130, TMC2160, TMC5160, TMC5161: `decaySlewing` must be `SPREADCYCLE`

StallGuard is only evaluated while the axis is moving above the configured arm
speed. Low speed motion is intentionally ignored because StallGuard readings are
often noisy or unusable there.

## Quick Setup

A typical configuration enables StallGuard, lets the model learn, then runs with
training off:

```cpp
#define DRIVER_TMC_STEPPER
#define DRIVER_TMC_STEPPER_STALLGUARD

#define SG_ENABLED ON
#define SG_TRAIN OFF
#define SG_FLOOR 20
#define SG_RAMP_ALLOW 40
#define SG_SENS 50
#define SG_ARM_FPS 150
#define SG_FAULT_MS 200
```

Suggested first-use workflow:

1. Verify normal slewing works without stalls or missed steps.
2. Enable StallGuard with a conservative `SG_FLOOR`, such as 0 to 20.
3. Use `SG_TRAIN ON` for a while if you want faster model learning.
4. Turn `SG_TRAIN OFF` for actual protection.
5. Increase `SG_FLOOR` gradually if real stalls are not detected soon enough.
6. Increase `SG_RAMP_ALLOW` if false trips happen during acceleration or
   direction changes.

## Runtime Settings

These settings are exposed as axis parameters when StallGuard support is built
in. Defaults come from the matching `#define` values in `Config.h`.

### SG Enable

Config define: `SG_ENABLED`

Master enable for StallGuard on the axis.

When set to `OFF`, StallGuard detection is disabled. Disabling StallGuard also
clears the saved StallGuard model from non-volatile storage.

### SG Train

Config define: `SG_TRAIN`

Training controls how aggressively the detector learns the normal StallGuard
baseline.

`ON`:

- learns quickly
- seeds uninitialized bins from valid samples
- suppresses stall fault latching
- disables `SG_FLOOR`

`OFF`:

- learns slowly from healthy samples
- can seed uninitialized bins from warmed-up, steady, nonzero samples when
  `SG_AUTO_SEED` is `ON` (the default)
- allows stall fault latching
- applies `SG_FLOOR`
- is the normal protected operating mode

Use training to build the model, not as the final protection mode.

### SG Floor, %

Config define: `SG_FLOOR`

Range: 0 to 100

Raises the trip threshold during steady motion. Higher values make StallGuard
more sensitive and trip sooner.

This setting only applies when training is off. It fades out during ramps so the
detector is less likely to false-trip while acceleration is changing the load.

With the default conversion, each percent can add up to 2 raw SG counts before
caps and ramp fade-out. For example, `SG_FLOOR 37` can add up to 74 counts
during steady motion.

### SG Ramp Allow, %

Config define: `SG_RAMP_ALLOW`

Range: 0 to 100

Adds extra headroom during acceleration, deceleration, and direction changes.
Higher values make StallGuard less sensitive during ramps.

Use this when the detector works well at steady speed but false-trips during
speed changes.

### SG Sens, %

Config define: `SG_SENS`

Range: 0 to 100

Maps to the driver's internal StallGuard sensitivity register.

For TMC2209 this maps to `SGTHRS`. For StallGuard2 drivers such as TMC2130,
TMC2160, TMC5160, and TMC5161, it maps to `SGT`.

Changing this setting changes the raw StallGuard behavior, so the learned model
is reset when the value changes at runtime.

### SG Arm, FPS

Config define: `SG_ARM_FPS`

Range: 50 to 2000 fullsteps/second

Minimum motor speed where StallGuard detection is armed.

Below this speed the detector is reset and no stall is reported. Raise this if
low-speed readings are unstable. Lower it only if the driver produces reliable
SG readings at lower speeds.

### SG Fault, ms

Config define: `SG_FAULT_MS`

Range: 50 to 500 milliseconds

How long a bad condition must persist before a stall is latched.

Lower values react faster but are more sensitive to noise. Higher values are
more tolerant of brief load changes.

## Compile-Time Settings

These settings are not ordinary runtime axis parameters.

### DRIVER_TMC_STEPPER_STALLGUARD_BASIC

Limits the runtime parameter list to the basic driver parameters. Use this only
when a smaller or simpler parameter set is needed.

### SG_AUTO_SEED

Default: `ON`

Allows an uninitialized bin to seed while training is off, but only from a
warmed-up, nonzero, steady sample when the detector is not latched.

This lets the protected mode become useful without requiring explicit training
for every speed/direction/angle bin. The seed is still conservative because the
trip threshold is placed below the observed sample by the built-in margin.

Set this to `OFF` if you want the older behavior where training-off bins only
adapt slowly through the normal healthy-sample path.

### SG_ANGLE_BINS

Default: 1

Range: 1 to 16

Enables separate learned models across the axis-1 rotation. This can help when
the load varies significantly with axis angle, such as imbalance over a full RA
rotation.

Current behavior:

- `1`: angle binning disabled
- `2` to `16`: axis 1 uses that many angle bins
- axis 2 uses the normal non-angle model

More bins can model more position-dependent variation, but each bin gets fewer
samples and therefore takes longer to learn.

### SG_BIN0_FPS

Default: 0

Optional low-speed anchor for speed bin 0, in fullsteps/second. Leave at 0 for
normal behavior. Raising it can smooth low-speed bin behavior in specialized
setups.

### SG_ARM_WARM_MS

Default: 200 ms

Warm-up time after arming before `sg == 0` is treated as a hard bad sample.
Before warm-up completes, zero readings are ignored instead of being learned or
counted as a fault.

### SG_EVAL_MIN_MS

Default: 20 ms

Minimum time between StallGuard evaluations. This keeps learning and fault
timing reasonably independent of the controller loop rate.

### SG_MODEL_SAVE_MS

Default: 300000 ms

Minimum interval between learned-model save requests on normal NV endurance
devices. Actual writes are deferred until the axis drops below the arm speed.

### SG_MODEL_SAVE_MS_MID

Default: 150000 ms

Save interval used for medium-endurance NV devices.

### SG_MODEL_SAVE_MS_HIGH

Default: 60000 ms

Save interval used for high-endurance NV devices.

### SG_FLOOR_R0 and SG_FLOOR_R1

Defaults: `0.08f` and `0.25f`

These internal thresholds decide how `SG_FLOOR` fades out as ramping increases.
At or below `SG_FLOOR_R0`, the floor is fully applied. At or above
`SG_FLOOR_R1`, the floor is fully removed.

Most users should tune `SG_FLOOR` and `SG_RAMP_ALLOW` first.

### SG_FLOOR_COUNTS_PER_PCT

Default: 2

Converts `SG_FLOOR` percent into raw StallGuard counts before caps and ramp
fade-out.

### SG_MODEL_INIT, SG_MODEL_BASE, SG_MODEL_DEV

Optional compile-time seed arrays for the learned model.

These are mainly useful for advanced users who already captured a known-good
model and want to compile it in as the starting point.

Axis-specific versions are also available:

- `AXIS1_SG_MODEL_INIT`
- `AXIS1_SG_MODEL_BASE`
- `AXIS1_SG_MODEL_DEV`
- `AXIS2_SG_MODEL_INIT`
- `AXIS2_SG_MODEL_BASE`
- `AXIS2_SG_MODEL_DEV`

## How Detection Works

The driver reports a raw StallGuard result named `sg`.

In general:

- higher `sg` means lighter load / more margin
- lower `sg` means heavier load / closer to stall
- `0` may mean unusable data during warm-up or low-speed operation

OnStepX learns a normal healthy `sg` baseline for each model bin. It then
computes a trip threshold below that baseline. If the live `sg` drops below the
threshold long enough, a stall is latched.

The simplified threshold is:

```text
trip = base - margin - rampExtra + floorAdd
```

Where:

- `base` is the learned normal SG value
- `margin` is safety headroom below normal
- `rampExtra` adds more headroom during acceleration/deceleration
- `floorAdd` raises sensitivity during steady motion

A sample is considered bad when:

```text
sg < trip
```

After warm-up, `sg == 0` is also treated as bad.

A single bad sample does not immediately stop the axis. Bad time accumulates in
milliseconds. Healthy samples decay that accumulator back toward zero. A stall is
declared only when bad time reaches `SG_FAULT_MS`.

## The Learned Model

The learned model is indexed by:

- angle bin, when enabled
- direction
- speed bin

`SG_BINS` is fixed at 6 speed bins. Direction has 2 bins:

- `0`: negative motion
- `1`: positive motion

`SG_ANGLE_BINS` controls the number of angle bins. With the default value of 1,
there is no angle-dependent model.

Examples:

- `SG_ANGLE_BINS 1`: `1 x 2 x 6 = 12` learned bins
- `SG_ANGLE_BINS 16`: `16 x 2 x 6 = 192` learned bins for axis 1

Each bin stores:

- `init`: whether the bin has been seeded
- `base`: learned healthy StallGuard baseline
- `dev`: learned downward variation/noise

Speed and angle bins are blended for threshold calculation so the trip threshold
does not jump abruptly at bin boundaries. Learning updates only the nearest bin.

## Training And Normal Operation

When a bin is first seeded, it starts with:

```text
base = sg
dev = 10
```

This does not make the detector trip at the current `sg`. The actual trip point
is below `base` because the margin is subtracted first.

With the current defaults:

```text
margin = 80 + 4 * dev
```

So a fresh seed with `dev = 10` begins with about 120 counts of margin before
floor and ramp effects.

Training on is intentionally permissive. It learns quickly and does not latch a
stall. Training off is intentionally protective. It learns slowly from healthy
samples and can latch a stall.

## Telemetry

The command:

```text
:GXSGn#
```

returns live StallGuard telemetry for axis `n` when supported:

```text
sg,trip,badMs,armed,latched#
```

Fields:

- `sg`: raw StallGuard result, or `-1` if no valid sample is available
- `trip`: current computed trip threshold
- `badMs`: accumulated bad-sample time
- `armed`: `1` when speed is above `SG_ARM_FPS`
- `latched`: `1` after a StallGuard stall has been declared

This is the best way to tune. Watch `sg` during normal slews, then adjust
`SG_FLOOR`, `SG_RAMP_ALLOW`, `SG_SENS`, and `SG_ARM_FPS` so normal motion stays
above `trip` while real stalls drop below it.

## Persistence

The learned model is saved to non-volatile storage. Writes are requested
periodically but delayed until the axis drops below the arm speed.

Changing settings that alter the meaning of the model causes the model to be
discarded and rebuilt. This includes:

- slewing microsteps
- goto current
- `SG_SENS`

Disabling `SG_ENABLED` deletes the saved model.

When angle bins are enabled, each axis-1 angle bin is stored separately. If
angle-bin storage is missing and a valid legacy model exists, that legacy model
may be used as the seed for each angle bin.

## Tuning Advice

Start conservatively:

- low `SG_FLOOR`
- moderate or high `SG_RAMP_ALLOW`
- `SG_TRAIN OFF` for actual protection
- `SG_FAULT_MS` near the default

If real stalls are missed:

- increase `SG_FLOOR`
- adjust `SG_SENS`
- lower `SG_FAULT_MS` carefully

If false stalls happen during acceleration:

- increase `SG_RAMP_ALLOW`
- raise `SG_ARM_FPS`
- reduce `SG_FLOOR`

If readings are unstable at low speed:

- raise `SG_ARM_FPS`
- verify the required decay mode
- verify motor current and mechanical loading

If behavior changes after changing motor current, microstepping, driver mode, or
SG sensitivity, let the model relearn before judging detection quality.
