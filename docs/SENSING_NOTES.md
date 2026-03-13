# Sensing Notes

## Purpose

## These notes describe the shared sensing subsystem used across OnStepX for

- home sense
- limit sense
- park sense
- PEC sense
- some local focuser/button-style inputs

The main reason this deserves its own note is that a "sense input" in OnStepX
is not just a simple digital switch. The same general machinery supports:

- ordinary digital active-high or active-low inputs
- digital inputs with software time filtering
- analog threshold sensing
- analog hysteresis behavior
- runtime inversion of the logical sense state

There is also a related but separate `Button` helper for more button-specific
input behavior, including analog range comparison. That is covered near the end
of this note.

## Quick Summary

## The simplest mental model is

1. raw GPIO or analog input comes in
2. `Sense` turns it into a normalized ON/OFF state
3. higher-level code decides what ON/OFF means operationally

## Examples

- home code asks whether home sense is active/ON according to its configured
  `HIGH`/`LOW` meaning
- limit code asks whether the min/max sense is ON
- PEC asks whether the worm index sense is ON

The same low-level sensing code is reused in all of those cases.

## Where The Shared Sense Code Lives

## The main subsystem is

- `src/lib/sense/Sense.h`
- `src/lib/sense/Sense.cpp`

## Main API

- `sense.add(...)`
- `sense.isOn(...)`
- `sense.changed(...)`
- `sense.stableMillis(...)`
- `sense.reverse(...)`
- `sense.poll()`

In most feature code, the higher-level subsystem does not read the GPIO pin
directly. It registers a sense input once and then works with the normalized
result through a sense handle.

## How Sense Configuration Works

## Each sense input is defined by

- a pin
- an init mode such as `INPUT`, `INPUT_PULLUP`, or `INPUT_PULLDOWN`
- a packed trigger value

The packed trigger value normally combines pieces with `|`.

That is the usual configuration pattern.

## Examples

- `HIGH`
- `LOW`
- `HIGH | HYST(20)`
- `LOW | HYST(50)`
- `HIGH | THLD(512)`
- `LOW | THLD(300) | HYST(25)`

Combining flags numerically with `|` is typically the intended style.

## What The Trigger Encodes

## The trigger value carries

- the active state bit: `HIGH` or `LOW`
- an optional analog threshold via `THLD(v)`
- an optional hysteresis via `HYST(v)`

## Important rule

- if the threshold part is zero, the input is treated as digital
- if the threshold part is nonzero, the input is treated as analog

## So

- `HIGH | HYST(20)` is still digital
- `LOW | THLD(400) | HYST(15)` is analog

## Digital Sense

Digital mode is used when there is no threshold component.

## Behavior

- raw input is read with `digitalReadEx(pin)`
- `HYST(n)` is the required stable time in milliseconds before a digital state
  change is accepted
- the new digital sample must remain unchanged for that long before the
  logical state changes

## So in digital mode, `HYST()` does not mean an analog band. It means

- how long the signal must stay stable before the change is accepted

## Examples

- `HIGH`
  active-high digital input, no extra filtering
- `LOW`
  active-low digital input, no extra filtering
- `HIGH | HYST(10)`
  active-high digital input, require about 10ms of stable state before the
  logical result changes
- `LOW | HYST(50)`
  active-low digital input with heavier software debounce/noise filtering

## This is useful for

- switch debounce
- modest EMI/RFI filtering
- noisy digital inputs that should not chatter

## Analog Sense

Analog mode is used when a threshold component is present.

## Behavior

- raw input is read with `analogRead(...)`
- on ESP32, `analogReadMilliVolts(...)` is converted back into the common
  `ANALOG_READ_RANGE` scale for higher accuracy on this platform
- the threshold and hysteresis act like a software Schmitt trigger

## This means

- if the sample rises above `threshold + hysteresis`, the logical value becomes
  `HIGH`
- if the sample falls below `threshold - hysteresis`, the logical value becomes
  `LOW`
- inside the band between those two thresholds, the last logical state is
  retained

## That band is effectively a deadband zone

- the signal can move around inside it without causing repeated state changes
- a state change only happens after the signal leaves the deadband on one side
  or the other

## Examples

- `HIGH | THLD(512)`
  active-high analog threshold at mid-scale, no hysteresis band
- `LOW | THLD(300) | HYST(25)`
  active-low analog threshold input with a hysteresis band around the threshold
- `HIGH | THLD(800) | HYST(50)`
  active-high analog input that must move well above and below the threshold
  before changing state

## This is useful when

- a clean digital threshold is not available from hardware
- analog noise would otherwise cause state chatter

## Why Hysteresis Means Two Different Things

This is one of the easiest things to forget.

## In digital mode

- `HYST(n)` means time in milliseconds

## In analog mode

- `HYST(n)` means an analog hysteresis band around the threshold, creating a
  deadband zone where the current state is held

That dual meaning is intentional, but it is easy to misread in config.

## What "ON" Means

## The subsystem separates

- the filtered internal logical value (`HIGH` or `LOW`)
- whether that logical value counts as ON for the caller

`sense.isOn(...)` returns true when the filtered value matches the configured
active state.

## So

- active-high means HIGH is considered ON
- active-low means LOW is considered ON

## This lets higher-level code work with

- "is the home sense active?"
- "is the limit sense active?"

without caring whether the hardware is electrically active-high or active-low.

## Runtime Reversal

`sense.reverse(handle, state)` flips the interpretation at runtime.

This is different from the original config.

## Config answers

- what electrical state counts as active

## Runtime reversal answers

- should this sense be interpreted the opposite way right now

## In practice this is used by code such as

- `Axis::setHomeReverse()`

This is useful when the electrical truth stays the same but the motion logic
needs the opposite interpretation.

## Where `Sense` Is Used

## Shared `Sense` users currently include

- axis home sense
- axis min/max limit sense
- park sense
- park signal monitoring
- PEC worm index sense
- local focuser sense inputs

That is why it is better to think of `Sense` as a general-purpose input
normalization subsystem, not just "home switch code."

## Example: Home Sense

Axis home sense is registered in `Axis::init()`.

## Important behavior

- homing first checks whether home sense is already ON
- if it is ON, motion first goes forward off the switch
- if it is OFF, motion first goes reverse toward the switch

So home sense is not just an endpoint detector. It also affects the search
direction and homing flow.

## Typical home examples

- `AXIS1_SENSE_HOME HIGH`
  simple active-high home switch
- `AXIS1_SENSE_HOME LOW | HYST(20)`
  active-low home switch with digital debounce
- `AXIS1_SENSE_HOME HIGH | THLD(450) | HYST(20)`
  analog thresholded home sense with hysteresis

## Example: Limit Sense

Limit sense uses the same subsystem for min/max axis limits.

## Runtime behavior

- `Axis::poll()` updates the sensed limit flags
- `Limits::poll()` reacts and stops the relevant motion direction

## One important special case

- min and max can share the same physical pin

## By default

- the axis notes this as `commonMinMaxSense`
- the higher-level limit logic avoids treating one shared input as two separate
  independent events

## Config option

- `LIMIT_SENSE_STRICT == ON` disables that relaxation and treats shared min/max
  more strictly

## Typical limit examples

- `LIMIT_SENSE LOW`
  common active-low limit input
- `LIMIT_SENSE LOW | HYST(10)`
  common active-low limit input with digital filtering

## Example: Park Sense

## Parking uses the same sensing framework for

- `PARK_SENSE`
- `PARK_SIGNAL`

Those are not special sensing implementations. They are just more clients of
the shared normalized ON/OFF sensing subsystem.

## Example: PEC Sense

PEC can also use the same shared `Sense` subsystem.

## That means the PEC index input can also be

- digital
- analog thresholded
- analog with hysteresis

Then PEC adds its own higher-level logic on top, such as only accepting a new
index event after enough motion has occurred.

## So

- `Sense` decides whether the input is ON
- PEC decides whether that ON transition counts as a valid worm index event

## What `Sense` Does Not Do

The shared `Sense` subsystem does not implement every kind of analog input
pattern.

## In particular, it does not implement

- "pressed when analog value lies within a range"

## It is focused on

- digital filtered ON/OFF sensing
- analog threshold-plus-hysteresis ON/OFF sensing

## Related But Separate: `Button`

## There is a related helper in

- `src/lib/pushButton/PushButton.h`
- `src/lib/pushButton/PushButton.cpp`

## That helper is more button-focused and adds behavior such as

- press/release semantics
- click and double-click detection
- tone/frequency detection
- analog threshold mode
- analog range/value mode

That analog value/range mode is the "pseudo-digital using analog sampling and
range comparison" behavior.

## So if someone is looking for

- analog value-in-range detection
- button semantics
- tone-style input behavior

they should usually look at `Button`, not `Sense`.

## Practical Rules Of Thumb

## If the input is a normal switch

- start with plain `HIGH` or `LOW`

## If the switch chatters or is noisy

- add `HYST(n)` and remember that in digital mode it means milliseconds

## If the input is fundamentally analog

- use `THLD(...)`
- add `HYST(...)` if you want Schmitt-trigger-like stability

## If the meaning of the signal must flip at runtime

- use runtime reversal rather than redefining the electrical config

## Good Mental Summary

## The cleanest way to think about sensing in OnStepX is

- config defines how raw input should be interpreted
- `Sense` turns that raw input into a stable ON/OFF result
- higher-level features decide what ON/OFF means operationally

That separation is why the same low-level sensing subsystem can be reused for
home, limits, park, PEC, and other features without each subsystem reinventing
analog/digital input conditioning.
