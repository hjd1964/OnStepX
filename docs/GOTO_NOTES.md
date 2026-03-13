# Goto Notes

## Purpose

These notes describe how goto, sync, orientation choice, and target-limit
planning currently work in OnStepX.

This is a workflow note. It is meant to help explain how the system decides on
a sensible motion plan, not to document every goto command.

## For related topics also see

- [STARTUP_AUTHORITY_NOTES.md](STARTUP_AUTHORITY_NOTES.md)
- [HOMING_NOTES.md](HOMING_NOTES.md)
- [COMMAND_REFERENCE.md](COMMAND_REFERENCE.md)

## Quick Summary

## The cleanest mental model is

1. a user target is turned into mount coordinates
2. the system checks whether that target is reachable in one or more valid
   orientations
3. the system chooses a sensible branch and motion plan
4. goto executes that plan, possibly with waypoints or alternate orientation
   handling
5. runtime limits continue watching during the motion

## So goto in OnStepX is not just

- convert coordinates
- slew there

It is more like a small decision engine.

## The Main Distinction: Goto vs Sync

`goTo.setTarget(...)` is the common decision point for both goto and sync.

## That means both operations share

- target conversion
- orientation selection
- reachability checks
- axis1 wrap/unwind correction

## Then they diverge

- goto moves the mount to the chosen target
- sync keeps the current physical mount orientation and reassigns coordinates to
  match the chosen target

So sync is not a separate simpler target model. It reuses most of the same
target-planning logic, then applies it as a coordinate reassignment instead of
motion.

## What A Goto Request Really Does

## At a high level a goto request goes through these steps

1. make sure goto is allowed right now
2. convert the request into mount coordinates
3. decide which orientation options are valid
4. choose the actual target branch and pier side
5. add a waypoint/home step if needed
6. start the slew
7. continue monitoring limits and refine near the destination

The important thing is that the target may have more than one valid
representation.

## Examples

- east-side vs west-side orientation on a GEM
- axis1 target plus or minus 360 degrees when wrapping/unlimited motion exists
- alternate orientation choices on mounts such as `ALTALT`

## Workflow 1: Entry Conditions

Before goto or sync can proceed, the mount checks a number of ordinary
conditions:

- startup authority must already be trusted
- both axes must be enabled
- the mount must not be parked
- no other goto/guide/motion operation can already be active
- no goto-blocking limit/sense error can already exist
- no motor fault can already exist

For goto specifically, date/time readiness matters through the ordinary mount
and limit logic as well.

In other words, goto is not treated as a way to escape unknown state.

## Workflow 2: Converting The Target

## The incoming target may start life in

- native coordinates
- mount coordinates
- equatorial, horizontal, or `ALTALT` forms depending on mount type

## The goto code converts as needed so the mount can reason about

- target hour angle / declination where applicable
- target altitude for horizon checks
- target instrument coordinates for each orientation choice

This is why the same sky target can still lead to more than one valid motion
plan.

## Workflow 3: Choosing Orientation

This is one of the core pieces.

## OnStepX supports a family of orientation-selection policies

- `PSS_EAST_ONLY`
- `PSS_WEST_ONLY`
- `PSS_EAST`
- `PSS_WEST`
- `PSS_BEST`
- `PSS_AUTO`
- `PSS_SAME_ONLY`

## What these really mean

- some insist on one side only
- some prefer one side but can fall back to the other if needed
- some try to stay on the current side
- some ask the planner to choose what seems best

## `PSS_AUTO`

This was added mainly to support automatic meridian-flip style behavior when the
mount is in the meridian-overlap region and can legitimately continue tracking
on the other side without interruption.

In that sense, `AUTO` is not just "pick something convenient." It is meant to
prefer the side/orientation that keeps the mount on a sensible continuing
tracking path when overlap allows it.

This is still not the same thing on every mount type.

## For ordinary equatorial cases

- it usually picks a preferred side from current hour angle
- in overlap cases, that helps place the target on the side where tracking can
  continue unimpeded after the orientation change

## For `ALTAZM` and `ALTALT`

- it falls back to `PSS_BEST`

So `AUTO` is really an automatic orientation policy, not one fixed algorithm.

In practical terms, it is most at home on GEMs, especially imaging setups where
automatic meridian-flip style behavior is desirable. Compared with a fixed
preference such as `PSS_EAST`, `PSS_AUTO` is better at using meridian-overlap
freedom to end up on the side where tracking can keep going cleanly after the
change.

On fork mounts, the usual practical approach is often different. Users commonly
widen the meridian limits to allow much more freedom, then keep a fixed side
preference such as `PSS_EAST` or `PSS_WEST` for normal operation. If they want
the mount to use the other orientation, they typically change that preferred
side. Forks often do not need, and often should not do, automatic flip-style
behavior in the first place. So wide limits help create freedom, but the
selected side policy still drives whether the planner stays put or flips.

## `PSS_BEST`

`BEST` is distance-aware.

It compares the reachable east- and west-orientation targets in instrument
space and may choose the alternate orientation if that target is closer.

## This is why OnStepX can sometimes choose what looks like a shortcut

- through the pole
- through an alternate valid orientation
- through a less-obvious wrapped branch

That behavior can look surprising, but it is intentional.

In practical terms, `PSS_BEST` is often attractive for visual use because it
can reduce unnecessary flips and choose the shorter or less awkward path. That
can make the mount feel nicer at the eyepiece even when it is not the same
choice an imaging workflow would prefer.

## Workflow 4: Reachability And Target Unwinding

After a tentative target exists, `Limits::validateTarget(...)` checks whether
the target is reachable.

## This includes

- altitude min/max
- east-side axis1 window
- west-side axis1 window
- meridian overlap limits for GEMs
- axis2 limits

These sketches help visualize why the same configured GEM limits can look
different depending on which side of the pier the mount is currently using:

![GEM limits example when east of the pier](<GEM LIMITS EXAMPLE EAST OF PIER.jpg>)

When east of the pier, the active limit window is interpreted against the
east-side geometry shown above.

![GEM limits example when west of the pier](<GEM LIMITS EXAMPLE WEST OF PIER.jpg>)

When west of the pier, the same ideas map onto the mirrored west-side geometry.

## But there is another subtle step

- axis1 may be normalized by plus or minus 360 degrees to find a sensible
  branch

That means the planner does not necessarily use the raw first axis1 value that
falls out of the transform.

## Instead it may

- keep the current branch if it is already best
- unwind to a nearby branch if that is closer
- shift by `+360` or `-360` to make the target fit the valid motion window

This is why a coordinate that "looks outside range" on first glance may still
be reachable after normalization.

It is also why wrapped or effectively unlimited axes feel smarter than a simple
"move to exactly this number" implementation.

## What `AXIS1_WRAP` Changes

## If `AXIS1_WRAP == ON`

- axis1 is configured with a 360-degree wrap
- its configured range becomes `-360 .. +360`
- sync-threshold style coordinate-jump checks are bypassed

That does not mean the mount ignores all planning.

## It means

- axis1 has a broader wrapped coordinate space to work in
- goto can choose a sensible branch in that larger space

This is most relevant on azimuth-like axes and other mounts where bounded
`-180 .. +180` thinking is too restrictive.

## Workflow 5: Waypoints And Home During Orientation Change

If an equatorial goto changes pier side and `MFLIP_SKIP_HOME == OFF`, OnStepX
does not always go straight to the final destination.

## Instead it may

- insert a waypoint at home
- or insert an avoid waypoint first, then go to home, then continue

## That is why the goto stage machine includes

- `GG_WAYPOINT_AVOID`
- `GG_WAYPOINT_HOME`
- near-destination refine stages
- final destination

So a meridian flip or alternate-orientation goto is often a multi-stage motion,
not one direct straight-line idea.

## Horizon Avoidance

There are two different horizon-avoidance ideas in the goto system.

The first is path-shaping during side/orientation changes.

## When a pier-side change is involved, goto may first route through

- about 4 hours HA
- or about 3 hours HA

before going home and then heading to the final target.

## The reason is practical horizon avoidance

- some otherwise valid geometry would pass too low near the horizon during a
  flip or alternate-orientation transition
- low-latitude cases can make that especially awkward

So the planner uses a small detour heuristic rather than pretending all valid
math paths are equally safe in practice.

The second is runtime rate shaping based on distance from the altitude limit.

## There is a runtime slowdown heuristic

- in equatorial mode with `MOUNT_HORIZON_AVOIDANCE == ON`
- the axis frequencies are scaled down near the minimum altitude limit
- the amount of slowdown varies with distance from that limit
- the code decides whether to slow RA-like or Dec-like motion depending on
  whether declination is increasing or decreasing

This is best thought of as practical protective behavior, not pure geometry.

## Workflow 6: Near Destination Refinement

Goto does not always drive directly to the literal final target at full intent.

## Depending on encoder/tolerance context, it may

- slew to a near-destination first
- wait to settle
- refine in one or more additional stages
- then finish at the final destination

That final approach is not only about positional refinement. In practice, the
near-destination exit can also be used to normalize backlash on arrival by
finishing with a consistent last-direction approach instead of simply accepting
whatever direction the earlier slew happened to end with.

## This is why the code carries

- `GG_NEAR_DESTINATION_START`
- `GG_NEAR_DESTINATION_WAIT`
- `GG_NEAR_DESTINATION`
- `GG_DESTINATION`

## It is also why encoder presence changes behavior

- with suitable encoder/tolerance support, more refined near-target behavior is
  allowed
- during homing/parking or on simpler cases, that refinement can be reduced or
  skipped

## Runtime Limits Are Not The Same As Target Validation

This is another important distinction.

## Target validation answers

- "is this planned target/orientation allowed?"

## Runtime limit monitoring answers

- "while the mount is moving or tracking, have we crossed something dangerous?"

## Runtime checks include

- horizon / overhead limits
- meridian east/west limits
- axis min/max limits
- sensed min/max limits

## Those runtime checks can

- stop one axis
- stop the whole mount
- abort goto
- trigger automatic meridian flip in some GEM tracking cases

## So there are really two layers

- planned target acceptance
- live motion policing

## Automatic Meridian Flip

If automatic meridian flip is enabled and the mount is tracking, some runtime
limit crossings do not simply stop motion.

Instead the limit code may request a new goto to the current sky position on
the opposite side:

- east-side request if currently west and beyond the west meridian limit
- west-side request if currently east and beyond the axis1 max side

## So automatic meridian flip is implemented as

- a limit-triggered alternate-orientation goto

not as a separate completely different motion engine.

## How Non-GEM Mounts Differ

For fork and alt/az style mounts, some axis-limit reasoning is based on shaft
angles rather than GEM-style pier-side interpretation.

The limit code therefore sometimes normalizes the current axis1 position back
into a "normal east-like" coordinate form before checking limits.

That is one reason the same target can feel simpler on some mounts and much
more orientation-sensitive on others.

For fork mode specifically, this is why wide meridian limits are often used as
a practical way to increase motion freedom without constantly tripping a
side-change style decision. But wide limits are not the whole story. Orientation
selection still matters. In typical fork use, that usually means keeping
`PSS_EAST` or `PSS_WEST` fixed until the user intentionally changes it to get
the opposite orientation, even though the generous limit window would allow
more than one valid branch.

## `ALTALT` And Other Less Common Modes

`ALTALT` is one of the modes that can seem obscure from the outside.

One useful way to picture it is by re-aiming the primary axis of a familiar
equatorial mount.

- on a GEM, that axis points parallel to Earth's rotation axis
- on an `ALTAZM`, that same idea would point the primary axis at the zenith
- on an `ALTALT`, the primary axis points north on the horizon at `ALT = 0`

That is why `ALTALT` is not just "equatorial with funny numbers." It is a
different mount geometry with its own natural strengths and weak spots.

## In practice this means

- target conversion differs
- `PSS_AUTO` falls back to `PSS_BEST`
- tracking and motion behavior are not identical to GEM or ordinary alt/az
- the mount can move naturally through the zenith and celestial-pole regions
- it is therefore one of the most natural mount geometries for satellite
  tracking
- the field will still rotate even if the mount is otherwise tracking the
  general sky motion correctly
- the geometry is weakest for targets near due north or due south on the
  horizon

So it is better to think of `ALTALT` as another consumer of the same decision
framework, but with a different singularity pattern and a different relationship
between its primary axis and Earth's rotation.

## Special Geometry: Sector Gear And Tangent Arm

Sector gear axis1 and tangent-arm axis2 are special enough that ordinary full
range goto thinking does not always apply.

## For these mounts

- target reachability is influenced by limited physical travel
- some checks are relaxed or specialized
- origin handling can matter more than wide-range coordinate elegance

## So goto on these mounts is partly about

- choosing a useful target in a constrained motion envelope

not simply navigating a wide continuous axis range.

## No-Goto Builds

## If `GOTO_FEATURE == OFF`

- real automatic goto motion does not exist
- sync still reuses the target-selection logic
- the old fake-completion paths for home/park are intentionally gone

## So this mode still benefits from

- coordinate planning
- orientation choice for sync-like operations

But it is not pretending to offer the full automatic motion planner.

## Examples

## Example 1: Ordinary GEM goto

- user asks for a sky target
- planner computes east and west solutions
- both may be reachable
- preferred side or best-distance logic chooses one
- goto may go directly, or through home if the side changes

## Example 2: `PIER_SIDE_BEST`

- both orientations are valid
- alternate orientation is closer in instrument space
- planner chooses the alternate branch
- this can look like a shortcut through the pole or a less-obvious orientation

## Example 3: Wrapped axis1

- raw target axis1 value is not ideal
- planner shifts by `+360` or `-360`
- resulting branch is closer or simply the only valid one inside limits

## Example 4: Horizon-avoidance flip

- side change is needed
- direct route would be too low near the horizon
- goto first detours to an avoid waypoint
- then goes home
- then continues to the final destination

## Good Mental Summary

## The cleanest way to think about goto in OnStepX is

- transforms produce candidate target representations
- limits decide which ones are reachable
- pier-side/orientation policy chooses among them
- wrap normalization may unwind coordinates into better branches
- waypoints and heuristics make the chosen path practical
- runtime limits keep watching after motion begins

## So the goto system is best understood as

- a target-planning and motion-decision engine

not just a coordinate conversion followed by a slew.
