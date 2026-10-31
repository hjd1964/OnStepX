# Goto Overhead Avoidance Notes

## Purpose

These notes describe the optional goto overhead avoidance feature
(`GOTO_AVOID_OVERHEAD`): what it does, how to enable it, how it behaves, and its
limits.

## For related topics also see

- [GOTO_NOTES.md](GOTO_NOTES.md)
- [PARKING_NOTES.md](PARKING_NOTES.md)
- [HOMING_NOTES.md](HOMING_NOTES.md)

## Quick Summary

The overhead limit (`:So#`, 60 to 90 degrees) is enforced at run time: when the
mount crosses it during a slew the goto is stopped. A goto between two valid
positions can pass close to the zenith, so without avoidance it can be stopped
part way, leaving the mount pointing high with later gotos (including park)
refused until the position is recovered. The faster the slew, the further the
mount travels into the zone before it stops.

With avoidance enabled each slew of a goto is checked before any motion. If the
direct slew would come within a tolerance of the overhead limit the goto is
routed through one or two waypoints that keep it clear. If no clear route is
found the goto is refused and the mount does not move.

The runtime limits are unchanged and continue to watch every slew.

## Enabling

In `Config.h`:

```
#define GOTO_AVOID_OVERHEAD            ON
```

## Requirements

- `GOTO_FEATURE ON`
- `MOUNT_TYPE` GEM, FORK or ALTALT
- a 32-bit processor (not AVR)
- not `AXIS2_TANGENT_ARM`, `AXIS1_SECTOR_GEAR` or `AXIS1_WRAP`

These are checked at compile time.

ALTAZM mounts don't need the feature: axis2 is altitude, so a slew between two
positions below the overhead limit stays below it.

## When it is active

Avoidance runs when limits are enabled and the overhead limit is below 90
degrees. With the overhead limit at 90 degrees (disabled) gotos behave exactly
as without the feature.

## What A Goto Does With Avoidance

1. the target and pier side are chosen as usual
2. the whole route is checked before the goto command replies: from the current
   position through any meridian flip waypoint and home position to the target,
   including the final approach
3. each slew is first checked as it would normally run; most gotos go straight
   ahead with no change
4. a blocked slew gets one waypoint, or two, moved out from where the slew comes
   closest to the zenith, then brought back as close as the tolerance allows
5. if any slew has no clear route the goto is refused
6. during the goto each slew is checked again from the actual position as it
   starts, since tracking and the target move on while the mount slews

## How the check works

Each slew is predicted the way the axes actually move (acceleration, slew rate
and the axis2 rate percentage) and sampled closely enough that no limit can be
reached between samples. Near the horizon, where goto slows down, the check
covers every path the axes could take instead of relying on timing.

- direct slews are kept the tolerance clear of the overhead limit; the other
  limits are left as they always were
- slews to and from waypoints are kept the tolerance clear of all position
  limits (horizon, overhead, meridian and axis limits)
- a slew starting or ending closer to a limit than the tolerance only needs to
  keep that closer distance

## Refused gotos

A refused goto replies with the "goto above overhead" error (reply `2` to `:MS#`
and `:MA#`). Park and home requests return the error and are not started.

A refused goto does not move the mount. Any setup the goto request makes before
planning still applies, for example tracking starts on the first goto after
start up or a home reset, as it would for any goto.

Gotos are refused when every route would pass closer to the overhead limit than
the tolerance. For example, at higher latitudes with a low overhead limit the
pole can lie inside the tolerance. A meridian flip must pass the pole, so with
the meridian flip home waypoint off it is refused.

If a later slew of a goto can't be planned when it starts (for example after a
long pause at home) the goto stops before that slew. This can only be seen in
the debug log.

## Automatic Meridian Flips

An automatic meridian flip is planned in the background while tracking
continues past the meridian limit. Until the pier side changes the meridian
limit doesn't stop tracking or report an error for that flip. The planning time
is limited so tracking moves at most `GOTO_AVOID_FLIP_OVERSHOOT` past the limit.
If the flip can't be planned in time the usual meridian limit failure follows
(tracking stops and the limit error is reported).

## Planning Time

Planning happens inside the goto command, so the reply waits for it. Measured
on an ESP32 (200 random gotos per case, several latitudes and overhead limits):

- direct slews: about 1 ms, at most about 15 ms
- one waypoint: typically 10 to 25 ms, at most about 85 ms
- two waypoints or refused: at most about 0.2 s

Each slew of a route is planned this way, so a goto through a meridian flip
waypoint and home can take a few times as long. Slower processors take longer,
roughly 1 s per slew at most on the slowest supported boards (an estimate).

While a goto is being planned the other command channels wait. An abort (`:Q#`)
sent from another channel during planning is acted on as soon as planning ends,
before the mount moves.

## Settings

These have defaults in `src/Config.defaults.h` and can be changed by adding a
`#define` to `Config.h`.

| Setting | Default | Range | Description |
|---|---|---|---|
| `GOTO_AVOID_TOLERANCE` | 2.5 | 0.1 to 10 | distance in degrees slews are kept clear of limits |
| `GOTO_AVOID_STEP` | 20 | 5 to 45 | distance in degrees waypoints are moved per step away from a blocked slew |
| `GOTO_AVOID_REFINE` | 5 | 0.1 to `GOTO_AVOID_STEP`/2 | distance in degrees to stop moving waypoints back toward a blocked slew |
| `GOTO_AVOID_FLIP_OVERSHOOT` | 0.25 | 0 to 2 | distance in degrees tracking may pass the meridian limit while an automatic flip is planned |
| `GOTO_AVOID_SLICE_US` | 2000 | 200 to 20000 | planning time in microseconds per planner task call |

The tolerance covers everything the prediction doesn't model: the two axes not
starting at quite the same moment, servo following error, arrival error and
movement while planning. It should comfortably exceed the worst pointing error
your mount shows during slews. A larger tolerance keeps slews further from the
limits but refuses more gotos.

## Memory

On an ESP32 the feature adds about 1 KB of RAM and 10 to 11 KB of flash. While
planning, the deepest call path uses about 2 KB of stack.

## Debug Messages

With `DEBUG VERBOSE` each plan is logged: the start and end of each slew, the
result (direct, with waypoints, or failed), the waypoints, and the reason a
goto was refused or stopped.
