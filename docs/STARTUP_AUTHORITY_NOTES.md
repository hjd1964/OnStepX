# Startup Authority Notes

## 1. Set date, time, and location

These settings describe the sky at the observing site and are needed at startup for normal OnStepX operation.

### Supply valid date/time and site location

Date/time is either set by the user manually or automatically from an RTC or GPS (called a TLS Time/Location Source).
Location is restored from the last setting automatically, or set by the user manually, or supplied by GPS.

The mount's physical starting position is established separately, as described below.
Tracking-only startup has an exception noted under [Tracking only](#tracking-only).

## 2. Choose the startup authority policy

The **startup authority policy**, selected by `MOUNT_STARTUP_MODE`, controls when
OnStepX accepts its position reference for coordinate-based operations. The
firmware calls that acceptance **coordinate trust**.

| Policy | What it permits |
| --- | --- |
| `SA_AUTO` (default) | Grants session trust at boot on a basic mount without absolute encoders or coordinate memory. If absolute encoders or coordinate memory are configured, they must provide a valid startup position before session trust is granted. |
| `SA_STRICT` | Requires a valid position from absolute encoders or coordinate memory, or successful sensed homing, before session trust is granted. |
| `SA_PERMISSIVE` | Grants session trust at boot regardless of the configured position sources. Runtime fault handling still applies. |

Goto, ordinary sync, set-park, park, and unpark require coordinate trust. Without
it, software limits are disabled and coordinate-based return home is blocked;
sensed homing uses a separate recovery path.

Software limits remain disabled until coordinate trust is established and valid
date/time is available.

## 3. Establish the starting position

The position reference relates OnStepX's coordinates to the physical positions
of both mount axes. Establish it in one of the following ways.

### By user action

For a basic mount under `SA_AUTO`, session trust is already granted at boot **on the
assumption** that the user will establish the initial position reference through home or sync:

- **Home:** With clutches, physically place the mount home before powering on.
  Without clutches, power on and guide to physical home, then reset home with
  `:hF#` to begin normal operation.
- **Sync:** Point at a known sky position, supply its coordinates, and sync.
  An accepted sync establishes or corrects the coordinate reference.

Sync can establish the position reference when the startup authority policy
permits it. The firmware's trust flag must already be set for ordinary sync to
be accepted. Sync and a plain home reset cannot restore revoked trust or
establish missing trust under `SA_STRICT`.

### From home switches

Run sensed homing. Successful homing establishes the starting position at home and
coordinate trust so normal operation can begin.

Sensed homing is allowed while initially untrusted, including under `SA_STRICT`,
subject to date/time, parked-state, motion, and hardware checks. A failed automatic
home does not establish missing trust, and startup does not continue past it.

Coordinate memory is marked untrusted during sensed homing. After power loss
mid-home, complete sensed homing or the applicable known-origin workflow before
resuming coordinate-based operation.

### From absolute encoders or coordinate memory

Power on and allow startup to recover the position automatically from the configured source.
Once a valid position has been accepted and normal readiness checks are complete,
normal operation can begin.

If the source has not been initialized, follow
[First-time origin setup](#first-time-origin-setup). Technical differences between
absolute encoders and coordinate memory are collected at the end of this guide.

For a previously parked mount, use the parked startup workflow below.

Trusted paired absolute encoder positions supplied through SWS v2.10d or later
can also establish trust when OnStepX accepts that position input.

## Starting from park

Park successfully before powering down. When relying on saved park coordinates,
leave the axes at that physical position while power is off.

On startup, restore/unpark rather than reset home. Unpark requires coordinate
trust, valid date/time, and healthy motors. Saved park state does not establish
trust or override coordinate-memory authority.

If the mount was moved independently of its saved coordinates, reestablish its
actual position reference using the appropriate workflow.

See [Parking notes](PARKING_NOTES.md) for commands and automatic restoration.

## Tracking only

Sidereal tracking has separate controls:

- With `TRACK_AUTOSTART` and `TRACK_WITHOUT_LIMITS` both OFF (defaults), tracking
  stops while software limits are disabled.
- `TRACK_AUTOSTART ON` starts tracking automatically and permits it without
  active software limits. An unparked equatorial mount without automatic homing
  can start tracking without active software limits before date/time is ready.
- `TRACK_WITHOUT_LIMITS ON` permits requested tracking without active software
  limits.

These options do not grant coordinate trust or bypass goto, sync, or unpark
trust checks. Hardware protections remain active.

## First-time origin setup

Where absolute encoders or coordinate memory require first-time origin setup:

1. Physically establish the known home/origin, using directional guiding as allowed
   by [Motion safety](#motion-safety-whenever-soft-limits-are-disabled).
2. Stop all motion and tracking, then use `:SEO#` to initialize the applicable
   origin (usually using the SWS or Website control via the `DISPLAY_COORDINATE_ORIGIN` option).
3. Complete any required restart, then follow normal startup.

Origin setup declares a known physical reference. It is not a general fault-clear
operation. Restart requirements are listed in the technical notes below.

## Motion safety whenever soft limits are disabled

- Directional guiding remains available on basic mounts while untrusted.
- For coordinate memory, initial guiding requires both saved-position records to
  be missing. Untrusted, unreadable, or incompatible records do not reopen that
  permission.
- With absolute position sources and coordinate memory OFF, guiding is allowed
  before trust has first been established in the current session, but blocked
  after established trust is lost.
- `GUIDE_TIME_NO_LIMITS` replaces the normal guide timeout. The default is 5 seconds
  per command; another command is needed to continue. Setting it to `0` removes
  the timeout.
- Pulse and spiral guides are rejected.
- Tracking is disallowed by default; see [Tracking only](#tracking-only) for exceptions.

Hardware protections remain active. Sensed homing remains available subject to
its usual checks.

## Recovering after a fault

An NV or reported motor/encoder fault can revoke coordinate trust. Clearing the
fault does not automatically restore it, even if the mount was parked or disabled.
A stop caused by limits or StallGuard alone does not revoke trust.

Resolve any NV fault before restoring trust. Do not cycle motor/driver power
independently while the microcontroller remains powered. After motor/driver power loss,
restore that power and restart OnStepX to repeat motor/encoder initialization and
any required motor calibration. Then follow the normal startup workflow for the
configured position source. `SA_PERMISSIVE` does not bypass runtime fault handling.

NV faults block or revoke trust when `NV_INIT_ERROR_REVOKES_AUTHORITY` is ON
(default). Setting it OFF disables that NV check; motor/encoder fault handling
still applies.

## Technical notes: position sources

- **Absolute encoders** supply live absolute position readings. Their origins and
  relationship to the physical axes must remain valid. Mount-axis absolute
  position sources are required on both axes; a single absolute axis is rejected
  by configuration validation. Supported pairs use servo absolute encoders or
  ODrive configured for absolute positioning.
- **Coordinate memory** restores saved positions. The axes must not move
  independently of the drive system, including while powered down. Clutch
  movement or other independent movement invalidates that position reference.
  Restore requires a valid record marked trusted; alternating A/B records and
  CRC checks protect the saved data.
- **Origin initialization:** Absolute-encoder origins saved with `:SEO#` are used
  after restart; supported platforms restart automatically. Coordinate-memory
  origin initialization can establish trust immediately without a restart.
  Runtime/NV encoder origins use `UINT32_MAX` for unset; nonzero compile-time
  `AXIS*_ENCODER_ORIGIN` values establish the corresponding origins.
- **Sync limits:** `AXIS1_LIMIT_SYNC` and `AXIS2_LIMIT_SYNC` can restrict coordinate
  changes during sync. Both default to OFF.

## Status and diagnostics

`:Gu#` exposes the coordinate trust status.

If startup or goto/sync/unpark is blocked, enable debug output and capture the log
from startup through the rejected command. It reports the startup policy, trust,
date/time readiness, and position-source details to help distinguish missing
date/time or origins from NV and motor/encoder faults. Fault reasons remain
latched until trust is established again or OnStepX restarts.
