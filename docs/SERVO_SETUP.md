# Servo Setup Guide

## Purpose

This guide is for setting up a mount axis that uses servo feedback in OnStepX.

It is written for users who want a practical bring-up sequence:
- what to configure first
- what to test before moving faster
- what to adjust when things go the wrong way

This guide is intentionally light on control theory. The goal is to help you
get to a safe, working setup without having to understand every low-level
detail first.

## Before you begin

Have these basics sorted out first:
- the controller pinmap matches your hardware
- power wiring is correct
- motor wiring is correct enough that the driver powers up safely
- encoder wiring is correct enough that counts can be read reliably
- hard limits or other motion safeguards are planned before unattended use

Important safety note:
- do not leave a new servo setup tracking unattended until you have verified
  limits, stop behavior, and the direction of both the motor and encoder
- a servo that is "mostly working" can still drive hard into a stop if the
  setup is wrong

## What "servo" means here

In OnStepX, a servo axis is a motor axis that closes the loop using encoder
feedback.

In practical terms that means:
- the motor moves the axis
- the encoder tells OnStepX where the axis actually is
- OnStepX keeps correcting the motor so the axis reaches and holds the target

For the mount axes, the main items are usually:
- a servo-capable driver model such as `SERVO_PE`, `SERVO_EE`, `SERVO_TMC2209`,
  `SERVO_TMC5160`, or `SERVO_KTECH`
- an encoder type such as `AB`, `KTECH_IME`, or another supported encoder
- sensible axis scale and direction settings
- usable PID values

This guide is mainly about the built-in servo axis types. ODrive-based systems
have their own workflow and deserve separate documentation.

## How the current `#define`s tie together

If you are comparing old example configs, this is the compile-time chain that
matters in the current code.

### 1. `AXIS*_DRIVER_MODEL` decides whether the axis is treated as a servo

If `AXIS1_DRIVER_MODEL` or `AXIS2_DRIVER_MODEL` is in the servo-driver range,
`Config.defaults.h` defines `AXIS*_SERVO_PRESENT`.

That is the switch that makes the mount axis build as a servo axis instead of a
step/dir axis.

### 2. `AXIS*_ENCODER` selects the encoder class

When `AXIS*_SERVO_PRESENT` is true, `Mount.axis.cpp` selects the encoder from
`AXIS*_ENCODER`.

Examples:
- `AB` selects a quadrature encoder
- `KTECH_IME` selects the KTech encoder class
- `SERIAL_BRIDGE` selects the serial bridge encoder path

### 3. `AXIS*_SERVO_FEEDBACK` selects the feedback controller

`Mount.axis.cpp` currently uses:
- `PID` for a single PID set
- `DUAL_PID` for separate tracking and goto PID sets

That means these defines feed directly into the live controller:
- `AXIS*_PID_P`
- `AXIS*_PID_I`
- `AXIS*_PID_D`
- `AXIS*_PID_P_GOTO`
- `AXIS*_PID_I_GOTO`
- `AXIS*_PID_D_GOTO`
- `AXIS*_PID_SENSITIVITY`

### 4. `AXIS*_SERVO_FLTR` selects the encoder filter

Current mount-axis choices are wired in through `Mount.axis.cpp`:
- `OFF`
- `KALMAN`
- `LEARNING` for Axis1
- `ROLLING`
- `WINDOWING`

If you are starting fresh, `OFF` is the simplest baseline.

### 5. Some servo driver models also use motor-side scaling

For stepper-based servo drivers such as `SERVO_TMC2209`, `SERVO_TMC5160`, and
`SERVO_KTECH`, the code uses this ratio:

```cpp
AXIS*_MOTOR_STEPS_PER_DEGREE / AXIS*_STEPS_PER_DEGREE
```

That ratio is passed into the servo driver in `Mount.axis.cpp`.

In practical terms:
- `AXIS*_STEPS_PER_DEGREE` is still the mount axis scale
- `AXIS*_MOTOR_STEPS_PER_DEGREE` matters when the motor-side step scale is not
  the same as the encoder-side axis scale

These two are rarely the same.  Sometimes the encoders are on the mount axes and there is not reduction and other times there is.  Then the encoder has a resolution as well.  On the other hand the motor almost always has an associated reduction and full-steps per rotation and 256 micro-steps per full-step.

### 6. Direction and origin are separate inputs to `ServoMotor`

The final `ServoMotor` object is built with:
- `AXIS*_REVERSE`
- `AXIS*_ENCODER_REVERSE`
- `AXIS*_ENCODER_ORIGIN`

These do different jobs:
- `AXIS*_REVERSE` changes commanded motor direction
- `AXIS*_ENCODER_REVERSE` changes encoder sign
- `AXIS*_ENCODER_ORIGIN` is the stored zero/origin for absolute encoder cases

There is also a runtime reverse path.

In practical terms:
- `AXIS*_ENCODER_REVERSE` is the baseline wiring/sign correction for the encoder
- `AXIS*_REVERSE` is the baseline motion-direction choice for the axis
- if the axis reverse state is changed at runtime, OnStepX also flips the
  control direction so the feedback loop still makes sense

That is why direction can appear to be handled at more than one level. It is.
Those levels are there for different reasons.

### 7. `AXIS*_TARGET_TOLERANCE` is not servo-only, but it matters a lot for servo behavior

`AXIS*_TARGET_TOLERANCE` is passed into the `Axis` object and affects when the
system considers the axis near its target.

That is why many servo example configs override it even though it is not
strictly a servo-only define.

### 8. Absolute encoder authority is inferred from encoder type

In `Validate.h`, some encoder types automatically imply absolute encoder
authority on servo axes. That affects startup trust behavior.

So for servo users, `AXIS*_ENCODER` is doing more than just picking a driver.
It can also affect how startup authority is determined.

## Practical takeaway about old example configs

Some example servo configs are older than the current code and may not reflect
today's best defaults.

Treat them as:
- examples of working combinations
- hints for starting values
- not as an exact description of the current wiring between settings

If there is a conflict, trust the current code path in:
- [Config.defaults.h](/c:/Users/hjd19/Documents/Programs/GitHub/OnStepX/src/Config.defaults.h)
- [Mount.axis.cpp](/c:/Users/hjd19/Documents/Programs/GitHub/OnStepX/src/telescope/mount/Mount.axis.cpp)
- [Validate.h](/c:/Users/hjd19/Documents/Programs/GitHub/OnStepX/src/Validate.h)

## Minimum settings to get right first

For each servo mount axis, focus on these first:

1. Driver model
- set the axis driver model to the correct servo type

Example:

```cpp
#define AXIS1_DRIVER_MODEL SERVO_PE
```

2. Axis scale
- set `AXIS*_STEPS_PER_DEGREE` correctly
- for stepper-based servo drivers, also check `AXIS*_MOTOR_STEPS_PER_DEGREE`
  if your hardware needs it

If the scale is wrong, almost everything else becomes confusing.

3. Encoder type
- set `AXIS*_ENCODER` to the actual encoder you are using

4. Directions
- `AXIS*_REVERSE` controls motor direction
- `AXIS*_ENCODER_REVERSE` controls encoder direction

These are separate on purpose. Do not assume that fixing one also fixes the
other.

The practical rule is:
- use `AXIS*_ENCODER_REVERSE` to make encoder counts agree with real motion
- use `AXIS*_REVERSE` to make commanded axis motion go the right way

Do not use motor reverse as a substitute for fixing an encoder that counts the
wrong direction.

If the motor and encoder direction are NOT in agreement the axis will run-away.
There is code that attempts to detect run-away, or oscillation, which will hopefully
shut the axis down in short order if this happens.

5. Conservative limits
- set reasonable axis limits before doing long moves
- keep first tests short and slow

## Recommended bring-up order

This is the safest order for a fresh servo setup.

### 1. Bring up one axis at a time

If possible, get Axis1 behaving first, then copy the pattern to Axis2.

Trying to solve both axes at once can hide simple mistakes.

### 2. Verify encoder direction before worrying about motor direction

The encoder direction must make physical sense.

When the axis moves in a positive direction, the encoder should report that
same direction. If it does not:
- fix `AXIS*_ENCODER_REVERSE` first

Do this before serious PID tuning.

### 3. Verify motor direction

Once the encoder reads correctly:
- use a small manual move
- confirm the axis moves the expected way
- if not, fix `AXIS*_REVERSE`

This order matters.

If the encoder sign is wrong, the control loop can "fight" the motion and a
later motor-direction change may only hide the real problem.

### 4. Start with small motions only

For early tests:
- use guide moves or very short slews
- keep your hand near power-off
- do not start with full-length gotos

### 5. Confirm the axis can stop cleanly

Before tuning for performance, verify that:
- the axis starts moving promptly
- the axis stops when motion stops
- there is no runaway, buzzing, or repeated overshoot

If anything looks wrong, stop and correct direction, scale, or wiring before
going further.

## A simple way to think about PID

You do not need to be a controls engineer to get started.

Use this mental model:
- `P` reacts to present error
- `I` corrects lingering offset
- `D` helps calm motion and reduce overshoot

OnStepX commonly uses:
- one PID set for tracking
- another PID set for slewing/goto

Those are the `_GOTO` values:
- `AXIS*_PID_P_GOTO`
- `AXIS*_PID_I_GOTO`
- `AXIS*_PID_D_GOTO`

Often since this uses feed-forward the goto parameters are less important.  Also
these can be changed instantly at runtime from the SWS or Website Plugin which makes tuning far far faster.

## How motion feed-forward works now

This is one area where older servo examples can be misleading.

In current OnStepX, the servo path does not rely on PID correction alone.
The motion command already includes a feed-forward component, and the exact
motor-side behavior depends on the servo driver type.

### TMC stepper servos

For TMC stepper servos such as `SERVO_TMC2209` and `SERVO_TMC5160`:
- motion feed-forward is automatic
- OnStepX combines the requested motion with the feedback correction and sends
  a velocity command to the driver

In user terms:
- you do not usually "turn feed-forward on"
- it is part of how these servo drivers normally work

The main coupling setting is the counts-to-steps ratio:
- it starts from the config default based on
  `AXIS*_MOTOR_STEPS_PER_DEGREE / AXIS*_STEPS_PER_DEGREE`
- that ratio is also a runtime-adjustable axis parameter

So for TMC stepper servos, the important question is usually not whether
feed-forward exists, but whether the counts-to-steps coupling is correct.

### DC servos

For DC servo drivers such as `SERVO_PE`, `SERVO_EE`, `SERVO_TMC2130_DC`, and
`SERVO_TMC5160_DC`:
- the same upstream motion command still includes feed-forward
- but the driver side now has more explicit runtime coupling controls

The practical runtime knobs are:
- minimum motor power
- maximum motor power
- reversal protection delay

That means DC servo behavior is more dependent on the runtime motor-output
settings than the TMC stepper-servo path is.

### DC servo velocity control loop

DC servos can also use the newer motor-side velocity loop.

When enabled:
- the driver uses encoder-derived velocity as a measured feedback signal
- the motor-side control becomes feed-forward plus a velocity PI correction

When not enabled:
- the motor-side control is basically feed-forward duty mapping

In practical terms:
- without the velocity loop, DC servos depend more heavily on a good power map
- with the velocity loop, DC servos can regulate motor behavior more directly
  from measured encoder speed

### Encoder velocity support

The newer DC velocity-loop behavior depends on encoder velocity estimation.

That comes from the encoder subsystem itself, not from the PID section.

So if you are working on a DC servo and velocity control is enabled:
- encoder quality matters even more than usual
- noisy or weak encoder velocity estimates can make tuning harder

## Deadband and reversal protection

Two settings in the servo path can sound similar, but they do different jobs.

### Zero deadband

The servo driver also has a low-speed command deadband.

In practical terms:
- very small requested motor velocities are treated as zero
- this helps prevent constant tiny nudges near standstill
- it is not the same thing as backlash compensation or target tolerance

If this is set too high:
- the axis can feel reluctant to make very small corrections

If it is set too low:
- the motor may keep twitching or chattering around zero

### Reversal protection delay

DC servos also have a direction-change delay.

This is better thought of as reversal protection:
- when the commanded direction flips, the output is briefly held at zero
- that gives the drivetrain and driver a moment before power is applied in the
  opposite direction

This can be helpful when:
- the motor drive does not like abrupt reversals
- the axis has noticeable mechanical shock on direction changes
- you want gentler reversal behavior during tuning

So the practical distinction is:
- zero deadband suppresses tiny commands near zero
- reversal protection delay softens direction changes

## Practical PID advice

Start simple:
- begin with modest `P`
- keep `I` small
- use little or no `D` at first

What to watch for:
- if the axis feels weak or lags badly, `P` may be too low
- if it hunts or oscillates, `P` may be too high
- if it reaches the target but stays offset, add a little `I`
- if it overshoots and rings, reduce `P` or add a small amount of `D`

Make small changes, then test again.

Avoid changing many things at once.

## About `PID_SENSITIVITY`

`AXIS*_PID_SENSITIVITY` controls how OnStepX transitions between the tracking
PID set and the slewing PID set.

A practical rule of thumb:
- for many DC servos, a nonzero value can help
- for many stepper-based servos, `0` is a reasonable starting point

If you are unsure:
- start with the value used by a similar example config

## About target tolerance

`AXIS*_TARGET_TOLERANCE` gives the axis a little room around the target.

In user terms:
- too small can make the axis fussy and restless
- too large can make the final position sloppier than you want

If the axis seems to chatter around the target, this setting may be too tight.

## First successful motion checklist

Once you have basic motion, verify these before calling it "working":
- guide moves are in the correct direction
- encoder direction remains correct through the full move
- the axis does not run away when motion starts
- the axis does not oscillate badly when motion stops
- tracking starts and holds without obvious hunting
- limits and stop behavior work as expected

Only after that should you move on to longer slews and unattended use.

## Absolute encoders and origin

If your servo setup uses absolute encoders:
- the encoder origin must be established correctly
- startup trust behavior depends on that origin being believable

If you are not using absolute encoders:
- skip this part for initial bring-up
- get basic motion stable first

## Useful optional UI aids

Some test configs enable extra web controls such as:
- `DISPLAY_SERVO_MONITOR`
- `DISPLAY_SERVO_ORIGIN_CONTROLS`
- `DISPLAY_SERVO_CALIBRATION`

These can make bring-up easier, especially while checking direction, settling,
and origin behavior.

## Common mistakes

These are the usual trouble spots:
- wrong encoder direction
- wrong motor direction
- wrong steps-per-degree
- tuning before verifying scale and direction
- trying full-speed slews too early
- leaving tracking enabled before limits and stop behavior are proven

If the setup behaves strangely, go back to:
1. scale
2. encoder direction
3. motor direction
4. only then PID tuning

## A good first goal

Do not aim for perfect tracking on the first pass.

A good first milestone is:
- the axis moves the right way
- encoder feedback agrees with motion
- it stops cleanly
- it holds position without obvious instability

After that, refine tracking, slewing response, and final pointing performance.

## Future improvements for this guide

Useful follow-up documentation would be:
- a separate ODrive setup guide
- a short PID tuning cookbook with symptom-based advice
- a checklist for absolute encoder origin setup
- a servo troubleshooting page with "if it does this, check that"
