# CAN Notes

## Purpose

These notes describe the current CAN interface in its most useful practical
role: interconnecting remote OnStepX peripheral features.

This note is mainly about using CAN so the main controller can talk to:

- a remote rotator
- one or more remote focusers
- a remote auxiliary-features node

It is not mainly about low-level motor/axis CAN protocols. Those are better
left for a later axis-oriented note.

## Quick Summary

The cleanest mental model is:

1. the main OnStepX controller runs the mount as usual
2. some peripherals can live on their own CAN-connected nodes
3. the main controller forwards the normal rotator, focuser, or auxiliary
   feature operations to those remote nodes

So the current CAN feature set is best thought of as a way to distribute
peripherals around the system, not as "the whole mount over CAN."

## What CAN Is Good For Here

This design is most useful when you want the mount controller and some
peripheral functions to be physically separate.

Typical reasons include:

- keeping focuser or rotator electronics close to the OTA
- reducing long motor/sensor wiring runs
- letting one small remote node host accessory features near where they are used
- building a more modular system where peripherals can be added or moved more
  easily

In other words, CAN is being used here as a practical interconnect between
cooperating OnStepX feature nodes.

## Connectors And Cable

For this CAN interconnect role, the preferred connector choices are:

- RJ12 as the standard light-duty connector
- GX12-6 as the standard rugged connector

The standard 6-pin wiring order is:

1. `GND`
2. `+12V`
3. `CAN_L`
4. `CAN_H`
5. `+12V`
6. `GND`

This duplicates power and ground at the outer pairs and keeps the CAN pair
together in the middle.

With quality connectors and cables, this arrangement is intended to carry:

- `12V @ 2A`

Recommended cable guidance:

- twisted-pair `24 AWG` is optimal
- straight quality RJ12 cable of `26 AWG` is acceptable for short runs of about
  `2 to 3 meters`

So the practical idea is:

- use RJ12 for lighter-duty short interconnects
- use GX12-6 where a more rugged connector is needed
- keep cable quality high if you expect the same cable to carry both CAN and
  accessory power

## Termination

The mount controller is treated as one terminating end of the CAN bus.

In the standard arrangement:

- the mount controller has a built-in `120 Ohm` termination resistor
- the remote nodes are daisy-chained in the normal CAN fashion
- the far end of the chain gets the second `120 Ohm` termination resistor

For this system, that far-end termination can be provided by plugging a custom
terminating modular jack into the last node in the chain.

So the intended layout is the normal two-ended CAN termination pattern:

- one `120 Ohm` terminator at the mount controller
- one `120 Ohm` terminator at the last node

not a terminator at every intermediate node.

## The Current Remote Peripheral Types

The current CAN remote-node support is centered on three feature families:

### Rotator

The main controller can treat a rotator as remote.

That means:

- the rotator logic still looks like rotator functionality from the user side
- but the actual rotator feature can live on a CAN-connected node
- the remote node hosts the local Axis3 rotator implementation and answers the
  requests

This is a natural fit when the rotator hardware is physically mounted on the
instrument side and you do not want all of that wiring returning to the main
controller.

### Focusers

The focuser support is especially useful for distributed systems.

The current model allows:

- up to six remote focusers
- heartbeat-based discovery/presence tracking
- the main controller to proxy focuser operations over CAN

This is a good fit for:

- separate main and guide focusers
- multiple optical trains
- distributed focus hardware mounted near different instruments

From the system-design point of view, this is one of the more attractive uses
of CAN in the current tree because focusers are naturally peripheral and often
benefit from being located close to the mechanics they drive.

### Auxiliary Features

Auxiliary features can also live on a remote CAN node.

This is useful for things such as:

- switches
- dew-heater style outputs
- analog outputs
- intervalometer or related accessory functions
- other feature-board style I/O that is convenient to place away from the main
  controller

So a remote auxiliary node can act like a small distributed utility board for
the telescope system.

## The Main Idea: Proxy And Remote Roles

For these feature families, CAN is used in a client/server style arrangement.

On the main controller:

- `ON` means "this feature is reached through CAN"
- the main controller becomes a client/proxy for that feature

On the remote feature node:

- `REMOTE` means "this node hosts the real implementation"
- that node answers the CAN requests for its local feature

So the usual pattern is:

- the mount/main controller proxies
- the physically-near peripheral node serves

## What This Feels Like In Practice

From the user-facing point of view, this is meant to feel like the feature is
still part of the overall system.

Conceptually:

- you talk to the main controller in the ordinary way
- the main controller forwards the relevant focuser/rotator/auxiliary operation
  over CAN
- the remote node performs the work and reports back

So the goal is not to create a second separate user workflow. The goal is to
let the feature live somewhere else electrically and physically while still
participating in the same overall system.

## Presence And Heartbeats

The remote-node model uses simple heartbeats.

That matters because the main controller should not treat a remote feature as
available just because it was configured at compile time.

Instead:

- remote nodes emit a heartbeat about once per second
- the client side considers the feature present only while that heartbeat
  remains fresh

This is especially helpful for distributed peripherals because it gives the
main controller a simple way to decide whether a remote focuser, rotator, or
feature board is really there and alive right now.

For focusers, the model is a little richer:

- each remote focuser has its own heartbeat slot
- the main controller can therefore keep track of multiple remote focusers
  independently

## Typical System Layouts

Some natural ways to use the current CAN feature set are:

### Main Mount Controller Plus Remote Rotator

Use the main controller for the mount itself, and put the rotator logic on a
CAN-connected node near the camera train.

That helps when:

- the rotator is physically far from the main controller
- you want cleaner wiring through the moving instrument area

### Main Mount Controller Plus Remote Focusers

Keep the mount local, but move one or more focusers onto remote CAN nodes.

That is useful when:

- focusers are spread around the instrument package
- you have more than one focuser role
- you want each focuser controller physically near its motor and sensors

### Main Mount Controller Plus Remote Auxiliary Board

Use a CAN-connected auxiliary node as a distributed accessory board.

That is a good fit for:

- dew-heater outputs
- power or switch functions
- intervalometer or small observatory accessory control

### Mixed Peripheral Layout

These roles can be mixed.

For example:

- main controller handles the mount
- a remote node handles the rotator
- one or more remote nodes handle focusers
- another remote node hosts accessory features

That is the real strength of this CAN feature set: it lets the system be
physically distributed while still acting like one coordinated instrument.

## What This Note Is Not Trying To Cover

This note is intentionally not trying to go deep into:

- low-level frame packing details
- backend-specific controller quirks
- direct CAN motor/encoder protocols
- a future "axis over CAN" design

Those are all real topics, but they are different topics.

For this note, the important point is simply:

- CAN is currently a practical interconnect for remote rotator, focuser, and
  auxiliary-feature nodes

## Related Notes

For the current command-family coverage, see
[COMMAND_REFERENCE.md](COMMAND_REFERENCE.md).

For serial transport/background comparison, see
[SERIAL_NOTES.md](SERIAL_NOTES.md).

## Good Mental Summary

The cleanest way to think about the current CAN feature set in OnStepX is:

- the mount controller stays central
- selected peripherals can live on their own CAN-connected nodes
- rotator, focusers, and auxiliary features are the current distributed-feature
  sweet spot

So CAN is best understood here as a practical peripheral-interconnect feature
for building a cleaner, more modular telescope system.
