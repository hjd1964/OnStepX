# Homing Notes

## Purpose

These notes describe how mount homing currently works in OnStepX.

This is a workflow note, not a deep explanation of low-level input filtering.
For the shared sensing subsystem itself, see:

- [SENSING_NOTES.md](SENSING_NOTES.md)

## For how homing relates to startup trust/authority, also see

- [STARTUP_AUTHORITY_NOTES.md](STARTUP_AUTHORITY_NOTES.md)

## Quick Summary

## There are three related but different ideas

1. Return to home
   Move the mount to the configured home coordinates
2. Sensed home
   Use home switches/sensors to find home
3. Reset at home
   Rebuild the coordinate basis from the known home position

Those are often used together, but they are not the same operation.

## The Main Entry Points

## The core mount home commands are

- `:hC#`
  move to home
- `:hF#`
  reset the mount at home / cold-start position
- `:hA0#` / `:hA1#`
  disable/enable automatic home at boot
- `:hC1,R#` / `:hC2,R#`
  toggle home-sense reversal for axis1/axis2
- `:hC1,n#` / `:hC2,n#`
  set home sense offset in arc-seconds

## Status command

- `:h?#`
  returns `hasSense,axis1Offset,axis2Offset`

## What "Home" Means

## The configured home position is derived from

- mount type
- configured home defaults
- site latitude where relevant
- optional sense offsets
- special geometry such as meridian-flip handling

That home position is refreshed by `Home::init()` and then returned through
`home.getPosition(...)`.

So "home" is not just a fixed pair of axis numbers. It is computed in context.

## It is also important to separate

- mount orientation
- coordinates assigned to that orientation

Without home switches, "home" is primarily a coordinate definition that the
system returns to or resets against. In that case, home is not independently
observed from hardware. It is whatever mount orientation the current coordinate
basis says corresponds to the home coordinates.

That means the practical truth about what mount orientation counts as "home"
can shift when the coordinate basis shifts.

## Examples

- an authoritative reset at home reassigns the coordinate basis
- sync/model/origin changes can improve accuracy by shifting how the system
  maps mount orientation into coordinates
- after those changes, the home coordinates may correspond somewhat differently
  to mount orientation than before

With home switches, the home procedure has a stronger mount-orientation anchor
because the system is observing a real sense event during homing.

## So it is best to think of home as

- a reference in coordinate space

that may or may not be strongly anchored to mount orientation depending on
whether home sensing is available.

## The Three Main Workflows

## Workflow 1: Return To Home Without Sensors

This is the simple "move to home coordinates" path.

## It happens when

- `home.request()` is called
- no home switches/sense inputs are configured

## Behavior

- goto must be available
- startup authority must already be trusted
- date and time must be ready
- the mount slews to the computed home coordinates

This is just motion to a known coordinate target. It does not establish truth
from hardware sensing.

## Workflow 2: Sensed Home

This is the switch/sensor-based path.

## It happens when

- `home.request()` is called
- home sense is available

## Behavior

- goto must be available
- if startup authority is not trusted, sensed home is still allowed because it
  can establish trust
- the guide subsystem starts a special home-guiding mode
- each axis uses `autoSlewHome()` to find the home switch state
- the axis may first move off the switch or toward it depending on the current
  sensed state
- the axis refines through fast/slow/fine homing stages

## If sensing succeeds

- `home.guideDone(true)` runs
- an authoritative reset is performed
- startup authority becomes trusted

## If sensing fails

- `home.guideDone(false)` runs
- a non-authoritative reset/recovery path is attempted
- the home operation is marked failed

## What The Axis Class Actually Does During Sensed Home

The mount-level home request is only part of the story. The detailed switch
search sequence lives down in the axis class.

Each relevant axis uses `Axis::autoSlewHome()`.

## The important sequence is

1. Check the current home-sense state
2. Choose an initial direction
3. Slew until the sense state changes in the expected way
4. Decelerate to a stop
5. Refine through additional slower passes

## More concretely

- if home sense is already ON, the axis first moves forward off the switch
- if home sense is OFF, the axis first moves reverse toward the switch
- when the sense state changes appropriately, the axis stops
- the homing stage advances from `HOME_FAST` to `HOME_SLOW`
- then usually to `HOME_FINE`

## During the refinement sequence

- the slew rate is reduced
- the axis repeats the search with tighter motion
- if the switch state is still not in the expected condition after the slow
  pass, an approach correction path is used before the final fine pass

This is why sensed home is more than "drive until switch closes once." It is a
small staged search procedure implemented at the axis level.

## Timeout also matters

- if no explicit timeout is supplied, the axis derives one from the configured
  home distance limit and slew rate
- if the search times out, the homing slew aborts

## Motion safety still applies

- motion errors abort homing
- motor faults abort homing
- common min/max sense conflicts can also abort the slew

## So the full sensed-home sequence is

- mount/home layer decides that sensed home should happen
- guide layer starts coordinated home-guiding
- axis layer performs the actual staged switch search
- home layer interprets success/failure and performs reset/recovery

## Workflow 3: Reset At Home

This is the coordinate-basis rebuild step.

## It happens through

- `home.reset(...)`
- `:hF#`
- startup reset logic
- successful sensed-home completion

## Behavior

- tracking is stopped
- tracking-rate offsets are cleared
- goto first-use state is reset
- motor positions are reset to zero where appropriate
- home instrument coordinates are written back
- nominal index positions are captured
- backlash and slow slew defaults are restored

## If the reset is authoritative

- startup trust is granted

## If it is not authoritative

- it does not grant trust by itself

## Authoritative vs Non-Authoritative Reset

This is one of the key distinctions.

## Authoritative reset means

- the software has a good reason to believe the home position being applied is
  truly correct

## Examples

- successful sensed home
- startup reset using valid paired absolute authority
- explicit authoritative origin/home workflows

## Non-authoritative reset means

- coordinates are being reassigned to home, but the software is not treating
  that as proof of truth

## This distinction matters because

- authoritative reset can establish startup trust
- non-authoritative reset cannot

## How Trust Interacts With Homing

Homing is one of the main bridges between untrusted and trusted state.

## Important rules

- plain return-to-home without sensors requires trust first
- sensed home can be used to regain trust
- authoritative reset grants trust
- non-authoritative reset does not

## There is also one permissive carveout

- in `SA_AUTO` on simple mounts with no absolute encoders and no coordinate
  memory, home/reset is allowed to bypass the sync-limit style coordinate jump
  restriction

That keeps older/simple mounts practical without weakening the stricter cases
globally.

## Auto Home At Boot

## If `automaticAtBoot` is enabled

- autostart requests home after boot

This shares the same sensed-home or return-home machinery as a normal home
request. It is not a separate home implementation.

## That means

- success behaves like normal homing success
- failure behaves like normal homing failure
- auto-home does not silently pretend success if the sensed-home flow fails

## No-Goto Behavior

## If `GOTO_FEATURE == OFF`

- home request is rejected
- reset/home-with-motion request is rejected
- sensed home through guide-home is also rejected

This is intentional.

## In no-goto builds

- there is no fast automatic slew/home capability
- home/return is not supposed to pretend to exist as a supported motion mode

## How Home Sense Offsets Work

Each mount axis can have a stored sense offset.

## These offsets mean

- the physical home sense event is not necessarily the final logical home
  coordinate

## Typical use

- a switch is placed somewhere repeatable but not exactly at the desired final
  home location
- the offset defines the difference between switch-detected home and desired
  coordinate home

## Commands

- `:hC1,n#`
- `:hC2,n#`

## Examples

- `:hC1,120#`
  set axis1 home offset to +120 arcsec
- `:hC2,-300#`
  set axis2 home offset to -300 arcsec

## Two Offset Modes

The config option `MOUNT_HOME_AT_OFFSETS` changes how offsets are used.

## When `MOUNT_HOME_AT_OFFSETS == OFF`

- sensed home resets at the switch-derived home first
- then, if offsets are in use, the mount may perform a finishing move toward
  the true home coordinates after the authoritative reset

## When `MOUNT_HOME_AT_OFFSETS == ON`

- offsets are folded directly into the computed home position

## So the difference is basically

- separate finishing move
vs
- home already means the offset-adjusted position

## Home Sense Reversal

Each axis can also reverse the meaning of its home sense input.

## Commands

- `:hC1,R#`
- `:hC2,R#`

This is layered on top of the usual home-sense logic and is applied through
`setReversal()`.

Axis1 also has a latitude-related reversal for equatorial mounts in the
southern hemisphere, so axis1 home sense behavior is slightly more subtle than
just a plain stored boolean.

## Examples

## Example 1: Simple mount, no home switches

- user powers on
- mount already has trusted startup authority or permissive `SA_AUTO`
- `:hC#` moves to home coordinates
- `:hF#` can reset at the home position

## Example 2: Mount with home switches

- user or autostart requests home
- guide-home uses switch sensing to find home
- successful sensed home performs authoritative reset
- trust is granted

## Example 3: Switch is repeatable but not exactly at desired home

- home sense is configured
- nonzero offsets are stored
- sensed home finds the switch
- reset establishes truth
- final coordinate home may be offset from the literal switch crossing

## Special Geometry Cases

Sector gear axis1 and tangent-arm axis2 are special.

In those cases, homing and reset do not always follow the plain "reset both
axes to zero and write home coordinates" pattern.

For these geometries, home is not only about returning to a meaningful
coordinate reference. It is also about resetting the mechanism so later gotos
and tracking have useful travel remaining inside the limited range.

## So on a sector gear or tangent-arm mount, homing is often better understood as

- mechanical re-baselining of the constrained axis
- restoring useful travel for later motion
- re-establishing the coordinate-to-mechanism relationship for that limited
  range

Because of that, some of the ordinary coordinate-based prechecks are also less
meaningful here. On these mounts the code relaxes or skips parts of the normal
"how far are we from home in coordinates?" logic before sensed homing, because
operation is fundamentally constrained and often more manual.

## Instead

- origin/position handling is adjusted to fit the special axis geometry
- successful sensed home may set origin more directly for the special axis

So if behavior looks a little different on sector gear or tangent-arm mounts,
that is expected.

## Failure Modes

## Homing can fail because of

- goto disabled
- motion already active
- missing date/time for plain return-home
- distance-to-home outside the configured sensed-home search limit
- switch search timeout
- motion error or motor fault during homing
- reset failure after the home-search phase

The code tries to fail conservatively rather than pretending home succeeded.

## Relationship To Sensing

Homing depends heavily on the shared sensing subsystem, but that low-level
behavior is documented separately.

## If you need to understand

- how `HIGH | HYST(20)` is interpreted
- how analog threshold/hysteresis sensing works
- how runtime reversal works

## see

- [SENSING_NOTES.md](SENSING_NOTES.md)

## Good Mental Summary

## The cleanest summary is

- return-to-home is motion to a known coordinate target
- sensed home is a hardware-based search procedure
- reset-at-home rebuilds the coordinate basis
- authoritative homing/reset can establish startup trust

Those are related operations, but they are not interchangeable.
