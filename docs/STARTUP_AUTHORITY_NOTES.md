# Startup Authority And Coordinate Trust Notes

## Purpose

These notes describe how startup authority and coordinate trust currently work
for the mount, and how that affects automatic motion such as goto, sync,
park/unpark, and home/return.

## Core idea

The mount tracks a runtime trust state whose only question is:
"Do we trust the current coordinate basis enough for coordinate-based
automatic motion?"

If trust is true:
- coordinate-based automatic operations are allowed

If trust is false:
- coordinate-based automatic operations are blocked
- manual guide recovery may still be allowed, depending on whether the mount
  requires startup authority and whether it is in a commissioning/recovery state
- plain sidereal tracking is not gated by startup authority itself, but it may
  still be blocked by other safety policy

## Two useful distinctions

1. Immediate startup authority
- trust is available immediately at boot from an already-available authority
  source

2. Deferred startup authority
- boot starts untrusted
- an authoritative startup-time action later establishes trust

Date/time readiness is separate from both of the above.

Limit enforcement is related but separate too:
- startup authority answers whether the coordinate basis is trusted
- date/time readiness answers whether sky-referenced operation is ready
- limit enforcement can become active as soon as both are available

## Motion safety whenever soft limits are disabled

This policy is not limited to mounts with absolute encoders or coordinate
memory. It applies whenever soft limit enforcement is disabled.

Typical cases include:

- initial setup of a basic mount before reset/home or the first accepted goto
  enables normal limit enforcement
- first-time absolute-encoder-origin or coordinate-memory commissioning
- startup states where coordinate trust or date/time is not ready

While soft limits are disabled:

- ordinary directional guiding is available only when startup-authority policy
  also permits manual recovery motion
- `GUIDE_TIME_NO_LIMITS` replaces `GUIDE_TIME_LIMIT` for each permitted
  directional guide command
- `GUIDE_TIME_NO_LIMITS` defaults to 5 seconds, after which another guide
  command is required to continue moving
- setting `GUIDE_TIME_NO_LIMITS` to `0` allows unlimited directional guide time
- pulse guides and spiral guides are rejected
- physical limit switches, motor faults, stall detection, and other
  non-coordinate protections remain active
- switch-based home remains available when configured because its sensors
  provide an authoritative recovery path

Goto, sync, reset/home, park, and unpark continue to follow their own startup
trust, date/time, and state checks. On a basic mount that already has session
trust, the first accepted goto or authoritative reset/home can transition the
mount into normal limit-enforced operation. During untrusted commissioning,
coordinate-based automatic operations remain blocked until an authoritative
origin or sensed home establishes trust.

## What currently establishes trust

Trust can currently come from either immediate startup authority sources or
later authoritative actions.

Immediate startup authority sources:
- paired mount-axis absolute position sources at boot, with origin(s)
  established
- trusted mount coordinate memory restore

Boot policy is selected by MOUNT_STARTUP_MODE.
Default:
- SA_AUTO

Modes:
- SA_STRICT:
  boot stays untrusted unless an immediate startup authority source succeeds
- SA_AUTO:
  if neither mount axis has an absolute position source and mount coordinate
  memory is OFF, boot grants legacy session trust automatically
- SA_PERMISSIVE:
  boot grants session trust automatically even when stricter authority sources
  exist

Later authoritative actions:
- authoritative set-origin at a known home position
- authoritative reset at a known home position
- successful switch-based home
- SWS v2.10d or later hosting absolute encoders (for pointing correction)

Ordinary sync is not used to restore trust.

## What currently revokes trust

Trust is revoked conservatively when the system can no longer honestly assume
the coordinate basis is valid.

Currently implemented:
- motor/encoder fault
- NV fault (default behavior, because NV_INIT_ERROR_REVOKES_AUTHORITY defaults to ON)

Important non-revocation note:
- a motion stop caused by limits or StallGuard is not, by itself, a trust
  revocation event

## What is blocked while untrusted

Blocked:
- goto
- set park
- park
- unpark
- normal sync
- home/return unless switch-based home is available

Allowed:
- manual guide on simple mounts that do not require startup authority
- manual guide during first-time coordinate-memory commissioning, when no saved
  coordinate-memory record exists yet
- plain sidereal tracking, subject to the normal tracking safety policy

Important tracking note:
- startup authority does not itself gate plain sidereal tracking
- however, tracking is still subject to other conditions such as parked state,
  date/time-dependent enable rules, and limit policy
- by default, tracking is disallowed when limits are not enforced

## Parking workflow

See [Parking notes](PARKING_NOTES.md) for commands and the parked startup workflow.

Parking is part of the same trust model because park/set-park/unpark all depend
on the mount coordinate basis being believable.

While untrusted:
- set park is blocked
- park is blocked
- unpark/restore is blocked

Why:
- set park stores a coordinate-based reference that should not be accepted if
  the current coordinate basis is already in doubt
- park moves to a coordinate-based destination; unpark may restore saved
  coordinates, so both require a trusted coordinate basis

Operationally this means:
- a parked state is only useful if it was established from a trusted coordinate
  basis
- unpark/restore is allowed when resuming from previously trusted parked state
- unpark/restore should not be thought of as a way to recover trust from unknown
  state
- the legacy "unpark from home" exception is only allowed on simpler mounts; it
  is disabled for SA_STRICT, for mounts using coordinate memory, and for mounts
  with absolute position sources

## Workflow 1: Boot With Immediate Startup Authority

This is the cleanest startup path.

On these mounts, trust can be granted automatically at boot because the mount
already has an authoritative coordinate basis before any manual recovery is
needed.

Current immediate startup authority sources are:
- paired mount-axis absolute position sources
- trusted mount coordinate memory

## Workflow 1a: Boot With Paired Mount-Axis Absolute Coordinate Authority

This applies when:
- both mount axes report absolute position at startup
- the required encoder origin(s) are already established
- startup reset is therefore allowed to treat the mount-axis absolute readings
  as authoritative for the initial coordinate basis

This is intentionally stricter than "one axis happens to be absolute."
For the mount axes, absolute coordinate authority is only valid as a paired
startup basis.

In practice this covers the supported mount-axis absolute-authority cases,
including:
- paired servo absolute encoders
- paired ODrive absolute-authority configuration

For runtime/NV origin storage:
- UINT32_MAX is used as the "origin not established" sentinel

Compile-time configured nonzero AXIS*_ENCODER_ORIGIN values are treated as
already established by configuration.

## Workflow 1b: Boot With Trusted Mount Coordinate Memory

Mount coordinate memory acts much like an "effective absolute startup source"
in the trust model:
- it can re-establish a coordinate basis at boot
- but only if the saved record is marked trusted

Important assumption:
- this depends on the mount being moved only by the drive system
- if clutches are present, they must not slip and must not be disengaged while
  coordinate memory is being relied on as a startup authority source

Mount coordinate memory already uses:
- alternating A/B records
- CRC validation

It now also uses a trust bit:
- trusted records may restore authority
- untrusted records do not restore authority

This separates:
- storage validity
- semantic trust

So coordinate memory is not just "last known position storage." It is also a
startup authority source when, and only when, the saved record is trusted.

## Workflow 2: Boot Without Immediate Startup Authority

What happens here depends on MOUNT_STARTUP_MODE.

In SA_STRICT:
- if the mount has neither:
  - paired mount-axis absolute position sources
  - trusted mount coordinate memory
- trust is not granted automatically at startup

In SA_AUTO:
- if neither mount axis has an absolute position source and mount coordinate
  memory is OFF, boot grants legacy session trust automatically
- a single absolute axis also prevents this fallback
- this preserves the long-standing "power up and goto" workflow on simpler
  mounts

If mount coordinate memory is configured but the saved record is missing or
untrusted, SA_AUTO does not silently fall back to permissive trust. In that
case the system stays untrusted until an authoritative action succeeds.

In SA_PERMISSIVE:
- boot grants session trust automatically regardless of startup authority
  sources

Current recovery model:
1. user manually guides to a known place if allowed by startup authority policy
2. user performs an authoritative action:
   - set origin
   - authoritative reset at a known home position
   - successful switch-based home
   - reboot with a valid startup authority source
3. trust becomes true again

Important practical consequence:
- the default SA_AUTO mode preserves permissive startup for simple mounts where
  neither axis has an absolute position source and coordinate memory is OFF
- SA_STRICT remains available for observatory and interlock-sensitive installs
- SA_STRICT controls whether boot trust is granted automatically; it does not,
  by itself, make manual guide require startup authority on otherwise simple
  mounts

## Manual guide while untrusted

Manual guide is treated as recovery motion, not coordinate-based automatic
motion.

However, it is not always allowed while untrusted.

Allowed while untrusted:
- simple mounts where startup authority is not required
- first-time coordinate-memory commissioning, when both coordinate-memory
  records are missing and the user must move the mount to establish the origin
- absolute-encoder origin commissioning before startup authority has ever been
  established
- switch-based home motion through the sensed-home path

Blocked while untrusted:
- mounts using coordinate memory after coordinate-memory records exist but are
  untrusted, wrong for the mount type, or unreadable
- mounts with known absolute startup authority sources after trust has been lost
  or rejected

This distinction avoids treating an NV failure or an untrusted coordinate-memory
record as if it were the same thing as a clean first boot.  Fresh coordinate
memory gets a one-time commissioning path; later failures stay conservative.

### First-time origin commissioning

First-time coordinate-memory and absolute-encoder-origin commissioning uses the
general [soft-limits-disabled motion policy](#motion-safety-whenever-soft-limits-are-disabled).
Because the coordinate basis is not yet trusted, coordinate-based automatic
operations are blocked and only the permitted recovery paths are available.

After the mount has been directionally guided to the known origin, stop all
motion and use `:SEO#` to establish the absolute-encoder origin or initialize
mount coordinate memory. Absolute-encoder origins are consumed during startup,
so restart the controller after saving the origin if the platform does not
reset automatically. Normal coordinate-based motion remains unavailable until
startup authority is trusted; soft limits become active when trust and
date/time are both ready.

## Workflow 3: Boot Untrusted, Then Establish Trust With Auto-Home

Auto-home with switches is best thought of as deferred startup authority.

It is not an immediate startup authority source.
Instead:
- boot begins untrusted
- switch-based home runs
- if successful, trust is established afterward

Switch-based home and auto-home-at-boot share the same sensed-home path.

Success:
- successful sensed home leads to authoritative reset
- trust is restored

Failure:
- coordinates are still reset for consistency
- failure is latched
- trust is not restored
- auto-home at boot does not continue as if homing succeeded

This behavior is intentionally consistent with normal switch-based return/home,
not just boot homing.

## Coordinate memory during homing

Important edge case:
when switch-based homing is in progress, mount coordinates are effectively
arbitrary relative to physical truth.

To prevent a power loss during homing from producing a misleading trusted
restore candidate:
- coordinate memory is marked untrusted as soon as switch-based homing starts
- periodic saves during HS_HOMING remain untrusted
- after successful authoritative homing, trust is restored normally

So if power is lost mid-home:
- the mount will come up with coordinate memory present but untrusted
- automatic coordinate-based motion will remain blocked
- the user can home/reset again, regain trust, and continue

## Workflow 4: Date/Time Not Ready

Date/time readiness and startup trust are related but not the same thing.

Trust answers:
- "Do we believe the coordinate basis is valid?"

Date/time answers:
- "Can we safely/usefully perform sky-referenced operations right now?"

So a mount may have trust while date/time is still not ready.

In that state:
- the coordinate basis may still be considered authoritative
- but some operations may remain postponed or blocked until date/time is ready

For mounts using trusted coordinate memory restore or trusted paired absolute
startup position:
- once date/time becomes ready, limit enforcement can be enabled immediately
- this does not require a later goto, reset/home, or unpark just to make
  limits active
- that in turn allows tracking requests to succeed normally under the usual
  limit-safety policy

Example:
- unpark recovery may wait for the normal site/time flow even when trust is
  already valid

## Workflow 5: Losing Trust During Runtime

If trust is lost after startup:
- coordinate-based automatic motion stops being allowed
- manual guide recovery may remain available, according to the same manual-guide
  policy used at startup
- sidereal tracking is still not gated by startup authority itself, but it
  remains subject to the normal tracking safety policy

The guiding rule is:
- if the system knows where it is, coordinate-based automatic motion is allowed
- if it does not know where it is, coordinate-based automatic motion is blocked
- guide-based manual recovery remains available only where startup authority
  policy allows it

### Motor/driver power removed while the microcontroller remains powered

Do not cycle motor/driver power independently while the microcontroller remains
powered. Power loss can invalidate motor configuration, communication setup,
calibration, or other startup state. Some simple drivers may tolerate this, but
OnStepX cannot assume that across all motor types or detect every power loss.

A reported motor/encoder fault revokes coordinate trust even when the axes are
disabled or the mount is parked. Clearing the hardware fault does not restore
trust. Goto, ordinary sync, and unpark remain blocked. Sync, unpark, and a plain
`:hF#` coordinate reset do not rerun motor initialization or restore revoked trust.

To recover first restore motor/driver power, then restart OnStepX to repeat motor/encoder
initialization and any required motor calibration. Finally, follow the normal startup workflow.

Note: Homing or setting an origin does not replace the restart required after
motor/driver power loss. `SA_PERMISSIVE` grants trust at startup; it does not
bypass runtime fault handling or restore trust when a fault clears.

## Important startup workflows to remember

In all cases Date/Time/Location must first be set, either manually or automatically from an RTC or GPS source.

The "common user workflow" in items 1–3 assumes the mount is not resuming from
a parked state. For parked startup, see item 5.

1. Basic mount with clutches, no absolute encoders, no coordinate memory:
- common user workflow is manually place at home, power on, then goto or sync or reset home (with `:hF#`).
- directional guiding remains available.
- SA_AUTO preserves this by granting session trust at boot.
- SA_STRICT blocks goto/sync until trust is established
  with home switches, sensed homing is allowed while untrusted and establishes
  trust on success.

2. Basic mount without clutches, no absolute encoders, no coordinate memory:
- common user workflow is to power on, issue guide commands to reach the home position,
  reset home (with `:hF#`) or sync, then goto.
- SA_AUTO preserves this by granting session trust at boot.
- SA_STRICT blocks goto/sync until trust is established
  with home switches, sensed homing is allowed while untrusted and establishes
  trust on success.

3. Mount with absolute encoders or coordinate memory:
- common user workflow is power on, then goto.
- sync range is often limited so absolute encoders or coordinate memory remain the authority.
- startup trust comes from paired absolute position sources with established
  origins, or a trusted coordinate-memory restore.
- first-time commissioning allows manual guiding to the known origin.  Then use
  set origin (with `:SEO#`) to initialize the applicable origin.
  Note: Mounts with absolute encoders require a restart after this operation.
- an NV failure or untrusted coordinate-memory record does not reopen first-time
  commissioning. Coordinate-based automatic motion and ordinary manual guiding
  remain blocked. Resolve any NV fault first, then restore trust through sensed
  homing or an applicable known-origin workflow.
- Coordinate memory requires the axes not be manually moved even when powered down.
  For this reason it's best used on mounts that physically enforce this;
  for example: by not having clutches, by having one-way worm/wheel gearing, etc.

4. User wants only sidereal tracking at startup:
- with both `TRACK_AUTOSTART` and `TRACK_WITHOUT_LIMITS` OFF (defaults), tracking
  stops while software limits are disabled.
- `TRACK_AUTOSTART ON` starts tracking automatically and permits tracking without
  active software limits.
- `TRACK_WITHOUT_LIMITS ON` permits requested tracking without active software
  limits.
- neither setting grants coordinate trust or bypasses the trust checks for
  goto, sync, or unpark.

5. Mount previously parked:
- leave the axes at their valid saved park position while powered down.
- on startup, restore/unpark using the normal trust and date/time checks;
  a saved parked state alone does not establish trust.
- see [Parking notes](PARKING_NOTES.md) for the full workflow.

## NV policy

The compile-time switch:
- NV_INIT_ERROR_REVOKES_AUTHORITY

Default:
- ON

Meaning:
- if NV reports an init/read fault that may undermine coordinate or motion
  confidence, trust is not granted or is revoked

This is intentionally strict, but it has an escape hatch for expert use.

## Status exposure

The trust state is exposed in :Gu# so clients can detect it without extra
status calls.

With debug output enabled, startup logs the effective `MOUNT_STARTUP_MODE`,
trust state, and date/time readiness. A rejected goto, sync, park, unpark, or
home operation also logs the authority status. NV and motor/encoder fault
reasons are retained until trust is established again or startup is restarted,
so a transient fault remains identifiable after the hardware recovers. When
no authority has been established, the diagnostic also lists absolute encoder
availability, encoder origin status, and the coordinate memory configuration.
Date/time readiness is reported separately; missing date/time does not itself
revoke coordinate trust.

Longer term, a small circular event log is probably a better place to record
why trust changed than trying to pack too much into live status.

## Open future work

- circular timestamped event log for trust loss/recovery reasons
- maybe future fault-tolerant KV/partition design
- refine the basic clutch/manual-home startup case so normal users are not
  surprised while still preserving the stricter safety model for systems with
  stronger authority sources
