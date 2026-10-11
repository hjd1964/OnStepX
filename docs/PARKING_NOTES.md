# Parking Notes

Parking saves a destination and records whether the mount successfully reached
it. Coordinate trust is separate: set-park, park, and unpark require a trusted
coordinate basis.

## Commands

| Command | Purpose |
| --- | --- |
| `:hQ#` | Save the current position as the park destination. This does not park the mount. |
| `:hP#` | Request a slew to the saved park position. Wait for parking to finish successfully before powering down. |
| `:hR#` | Restore the parked mount to operation and start tracking. It does not slew the mount back to park. |

An accepted park command means the operation started, not that parking finished.

## Normal parked startup

1. Establish normal operation and save the desired park position.
2. Request park and wait for a successful parked state.
3. Power down. When relying on saved park coordinates, leave the axes at that
   physical position, including while power is off.
4. Restore microcontroller and motor/driver power in the intended sequence.
   Supply valid date/time and site settings, then restore/unpark before goto.

Do not use a home reset when resuming from a valid parked position. If the mount
was moved independently of its saved coordinates, do not assign the old park
coordinates to its new position. Reestablish its actual coordinate basis using
the workflow appropriate to the configured position source.

Once date/time is ready, OnStepX attempts parked-position restoration:

- With `TRACK_AUTOSTART OFF`, it restores the position while leaving the mount
  parked and tracking off; use unpark to resume operation.
- With `TRACK_AUTOSTART ON`, successful restoration also unparks and starts
  tracking.

Both paths require coordinate trust and healthy motors. A saved parked state
does not itself establish startup authority.

When coordinate memory is enabled, parking does not override its authority or
bypass its trust requirements. A saved park position cannot replace a missing
or untrusted coordinate basis.

## Faults and recovery

Motor/encoder faults revoke trust even when the mount is parked or disabled.
After motor/driver power loss, restore power and restart OnStepX to repeat
motor/encoder initialization and any required motor calibration. Unpark is not
an authority-recovery operation. Resolve NV faults before restoring trust.

See [Startup authority notes](STARTUP_AUTHORITY_NOTES.md) for the applicable
home, encoder-origin, and coordinate-memory recovery workflows.

## Compatibility behavior

The legacy unpark-from-home path is available only when the controller reports
home, coordinate memory is OFF, `SA_STRICT` is not selected, and external
absolute encoders are not active. It restores saved park coordinates without
slewing; it is not an instruction to physically move the mount home first.

Parking is unavailable in TRACKER mode.
