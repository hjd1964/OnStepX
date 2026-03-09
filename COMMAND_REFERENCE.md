# OnStepX Command Reference

This reference is derived from the current command handlers in the source tree, primarily the `*.command.cpp` files plus the shared command processor in `src/libApp/commands/ProcessCmds.cpp`.

Scope notes:

- This covers the command families implemented in the current tree, not older wiki pages.
- Many commands are compile-time feature gated. If a subsystem is not enabled in the build, its commands will not exist.
- Plugin-defined commands are not documented here because they come from the active plugin set, not the core command handlers.
- A few command comments in source are stale. Where the code and comment disagree, this document follows the code path.

## Protocol Basics

### Frames

- Normal ASCII command frame: `:CC...#`
- Checksummed frame: `;CC...CCS#`
  - `;` starts a checksummed command.
  - The command processor validates the checksum and sequence character before dispatch.
- The parser ignores ASCII space, LF, and CR.
- Internal command buffer size is 80 bytes including framing, so current implementations are not limited to the older 40-character LX200 convention.

### Command Parsing

- `CC` is effectively a 1- or 2-character command family. The rest of the payload is the parameter string.
- Replies are either:
  - payload replies, usually terminated with `#`
  - numeric/boolean replies, returned as `1` for success and `0` for failure
- Some commands suppress the trailing `#` on purpose.

### Global Reply/Error Behavior

- Commands that leave `numericReply=true` return:
  - `1` for `CE_NONE` or `CE_1`
  - `0` for any other error
- `:GE#` returns the last command error code as a 2-digit payload.

### Command Error Codes

These are the codes returned by `:GE#`:

| Code | Symbol | Meaning |
| --- | --- | --- |
| 00 | `CE_NONE` | No error |
| 01 | `CE_0` | False/fail without a protocol error |
| 02 | `CE_CMD_UNKNOWN` | Unknown command |
| 03 | `CE_REPLY_UNKNOWN` | Invalid reply |
| 04 | `CE_PARAM_RANGE` | Parameter out of range |
| 05 | `CE_PARAM_FORM` | Bad parameter format |
| 06 | `CE_ALIGN_FAIL` | Align failed |
| 07 | `CE_ALIGN_NOT_ACTIVE` | Align not active |
| 08 | `CE_NOT_PARKED_OR_AT_HOME` | Not parked or at home |
| 09 | `CE_PARKED` | Already parked |
| 10 | `CE_PARK_FAILED` | Park failed |
| 11 | `CE_NOT_PARKED` | Not parked |
| 12 | `CE_NO_PARK_POSITION_SET` | No park position set |
| 13 | `CE_GOTO_FAIL` | Goto failed |
| 14 | `CE_LIBRARY_FULL` | Library full |
| 15 | `CE_SLEW_ERR_BELOW_HORIZON` | Target below horizon limit |
| 16 | `CE_SLEW_ERR_ABOVE_OVERHEAD` | Target above overhead limit |
| 17 | `CE_SLEW_ERR_IN_STANDBY` | Controller in standby |
| 18 | `CE_SLEW_ERR_IN_PARK` | Mount parked |
| 19 | `CE_SLEW_IN_SLEW` | Goto already active |
| 20 | `CE_SLEW_ERR_OUTSIDE_LIMITS` | Outside configured limits |
| 21 | `CE_SLEW_ERR_HARDWARE_FAULT` | Hardware fault |
| 22 | `CE_SLEW_IN_MOTION` | Mount already in motion |
| 23 | `CE_SLEW_ERR_UNSPECIFIED` | Other slew error |
| 25 | `CE_1` | Explicit true/success |

## Global Processor Commands

These come from `ProcessCmds.cpp`, not from a subsystem `*.command.cpp`.

| Command | Reply | Description |
| --- | --- | --- |
| `:SBB#` | `1` then switches to 460800 baud | Set serial baud rate |
| `:SBA#` | `1` then switches to 230400 baud | Set serial baud rate |
| `:SB0#` | `1` then switches to 115200 baud | Set serial baud rate |
| `:SB1#` | `1` then switches to 56700 baud | Set serial baud rate |
| `:SB2#` | `1` then switches to 38400 baud | Set serial baud rate |
| `:SB3#` | `1` then switches to 28800 baud | Set serial baud rate |
| `:SB4#` | `1` then switches to 19200 baud | Set serial baud rate |
| `:SB5#` | `1` then switches to 14400 baud | Set serial baud rate |
| `:SB6#` | `1` then switches to 9600 baud | Set serial baud rate |
| `:SB7#` | `1` then switches to 4800 baud | Set serial baud rate |
| `:SB8#` | `1` then switches to 2400 baud | Set serial baud rate |
| `:SB9#` | `1` then switches to 1200 baud | Set serial baud rate |
| `:GE#` | `CC#` | Get last command error code |
| `<0x06>` | `A`, `P`, or `CK_FAIL` style response | Special LX200 status query/checksum-fail path handled by the buffer/processor |

## Dispatcher Notes

In the full telescope build, command dispatch order is:

1. mount
2. guide
3. GPIO/plugin status helpers
4. goto
5. park
6. library
7. site
8. limits
9. home
10. PEC
11. axis 1 / axis 2
12. rotator
13. focuser
14. auxiliary features

That matters because some command names are reused by different subsystems, especially `:hP#` and `:hR#`. In an integrated mount build, mount park/unpark wins before the rotator/focuser handlers ever see those frames. The rotator/focuser `h*` commands are still valid for standalone or remote-node builds.

## Core Telescope / Firmware / Environment

| Command | Reply | Description |
| --- | --- | --- |
| `:B+#` | none | Increase reticle brightness |
| `:B-#` | none | Decrease reticle brightness |
| `:ECtext#` | none | Echo text to debug output. `_` becomes space, trailing `&` becomes newline. |
| `:ERESET#` | none | Reset the MCU |
| `:ENVRESET#` | text | Mark NV storage to be cleared on next boot |
| `:ESPFLASH#` | `1`/none depending on build path | Put addon/ESP device into firmware flashing mode when supported |
| `:GVD#` | `MTH DD YYYY#` | Firmware build date |
| `:GVM#` | `name version#` | Firmware name plus version |
| `:GVN#` | `M.mm...#` | Firmware version number |
| `:GVP#` | `name#` | Product name |
| `:GVT#` | `HH:MM:SS#` | Firmware build time |
| `:GVC#` | `string#` | Firmware config/product description |
| `:GVH#` | `string#` | Firmware hardware/pinmap string |
| `:GX9A#` | `sn.n#` | Ambient temperature in C |
| `:GX9B#` | `n.n#` | Pressure in mbar |
| `:GX9C#` | `n.n#` | Relative humidity in percent |
| `:GX9E#` | `sn.n#` | Dew point in C |
| `:GX9F#` | `n#` or `0` | MCU temperature in C |
| `:SX9A,sn.n#` | `0/1` | Set ambient temperature |
| `:SX9B,n.n#` | `0/1` | Set pressure |
| `:SX9C,n.n#` | `0/1` | Set humidity |

## Time / Date / Site

| Command | Reply | Description |
| --- | --- | --- |
| `:Ga#` | `HH:MM:SS#` | Local standard time, 12-hour format |
| `:GC#` | `MM/DD/YY#` | Local standard calendar date |
| `:Gc#` | `24#` | Local time format indicator |
| `:GG#` | `sHH:MM#` | UTC offset to add to local time to get UT1 |
| `:Gg#` | `sDDD*MM#` | Current longitude |
| `:GgH#` | `sDDD*MM:SS.SSS#` | Longitude, highest precision |
| `:GL#` | `HH:MM:SS#` | Local standard time, 24-hour format |
| `:GLH#` | `HH:MM:SS.SSSS#` | Local standard time, highest precision |
| `:GM#` | `string#` | Site 1 name |
| `:GN#` | `string#` | Site 2 name |
| `:GO#` | `string#` | Site 3 name |
| `:GP#` | `string#` | Site 4 name |
| `:GS#` | `HH:MM:SS#` | Sidereal time |
| `:GSH#` | `HH:MM:SS.ss#` | Sidereal time, highest precision |
| `:Gt#` | `sDD*MM#` | Current latitude |
| `:GtH#` | `sDD*MM:SS.SSS#` | Latitude, highest precision |
| `:Gv#` | `sn.n#` | Elevation in meters |
| `:GX80#` | `HH:MM:SS.ss#` | UT1 time |
| `:GX81#` | `MM/DD/YY#` | UT1 date |
| `:GX89#` | `0` or `1` | Date/time ready status. `0` means ready, `1` means not ready. |
| `:SCMM/DD/YY#` | `0/1` | Set local date |
| `:SCMM/DD/YYYY#` | `0/1` | Set local date |
| `:SGsHH#` | `0/1` | Set UTC offset |
| `:SGsHH:MM#` | `0/1` | Set UTC offset. Comments indicate `MM` should be `00`, `30`, or `45`. |
| `:Sg(s)DDD*MM#` | `0/1` | Set longitude |
| `:Sg(s)DDD*MM:SS#` | `0/1` | Set longitude |
| `:Sg(s)DDD*MM:SS.SSS#` | `0/1` | Set longitude |
| `:SLHH:MM:SS#` | `0/1` | Set local time |
| `:SLHH:MM:SS.SSS#` | `0/1` | Set local time |
| `:SMname#` | `0/1` | Set site 1 name, max 15 chars |
| `:SNname#` | `0/1` | Set site 2 name, max 15 chars |
| `:SOname#` | `0/1` | Set site 3 name, max 15 chars |
| `:SPname#` | `0/1` | Set site 4 name, max 15 chars |
| `:StsDD*MM#` | `0/1` | Set latitude |
| `:StsDD*MM:SS#` | `0/1` | Set latitude |
| `:StsDD*MM:SS.SSS#` | `0/1` | Set latitude |
| `:SUs.s#` | `0/1` | Set DUT1 correction in seconds, nominally `-0.9` to `+0.9` |
| `:Svsn.n#` | `0/1` | Set elevation in meters |
| `:W0#` .. `:W3#` | none | Select active site slot |
| `:W?#` | `n#` | Query active site slot |

## Mount Position / Tracking / Rates

Available when `MOUNT_PRESENT` is enabled.

| Command | Reply | Description |
| --- | --- | --- |
| `:GA#` | `sDD*MM#` or `sDD*MM'SS#` | Current mount altitude |
| `:GAH#` | `sDD*MM'SS.SSS#` | Current mount altitude, highest precision |
| `:GD#` | `sDD*MM#` or `sDD*MM:SS#` | Current declination |
| `:GDH#` | `sDD*MM:SS.SSS#` | Current declination, highest precision |
| `:GR#` | `HH:MM.T#` or `HH:MM:SS#` | Current right ascension |
| `:GRH#` | `HH:MM:SS.SSSS#` | Current right ascension, highest precision |
| `:GT#` | `n.nnnnn#` or `0#` | Tracking rate in Hz, `0` if not tracking |
| `:GZ#` | `DDD*MM#` or `DDD*MM'SS#` | Current azimuth |
| `:GZH#` | `DDD*MM'SS.SSS#` | Current azimuth, highest precision |
| `:GXTD#` | `n.nnnnnnnn#` | Tracking rate offset, Dec, arcsec per sidereal second |
| `:GXTR#` | `n.nnnnnnnn#` | Tracking rate offset, RA, arcsec per sidereal second |
| `:GX40#` | `DDD*MM:SS#` | Axis1 instrument angle |
| `:GX41#` | `DDD*MM:SS#` | Axis2 instrument angle |
| `:GX42#` | `n.nnnnnn#` | Axis1 instrument angle in decimal degrees |
| `:GX43#` | `n.nnnnnn#` | Axis2 instrument angle in decimal degrees |
| `:GX44#` | `n#` | Axis1 encoder count |
| `:GX45#` | `n#` | Axis2 encoder count |
| `:GXE4#` | `n#` | Axis1 steps per degree |
| `:GXE5#` | `n#` | Axis2 steps per degree |
| `:GXEE#` | `0#`, `1#`, ... | Mount coordinate mode (`MOUNT_COORDS - 1`) |
| `:GXEF#` | `1` or `0` | Axis2 tangent-arm presence flag |
| `:GXEG#` | `1` or `0` | Axis1 sector-gear presence flag |
| `:GXEM#` | `n#` | Current mount type |
| `:GXF3#` | `sn.nnnnnn#` | Axis1 step frequency |
| `:GXF4#` | `sn.nnnnnn#` | Axis2 step frequency |
| `:GXFA#` | `50%#` | Reported workload placeholder |
| `:GXFF#` | `n.nnnnnn#` | Axis1 index position |
| `:GXFG#` | `n.nnnnnn#` | Axis2 index position |
| `:STn.n#` | `0/1` | Set tracking rate in Hz. `0` stops tracking. |
| `:SEO#` | `0/1` | Save absolute encoder origin and reset, when supported |
| `:SX40,n#` | `0/1` | Stage encoder axis1 angle in degrees |
| `:SX41,n#` | `0/1` | Stage encoder axis2 angle in degrees |
| `:SX42,1#` | `0/1` | Sync mount from staged encoder axis values |
| `:SX43,0#` | `0/1` | Allow SWS to control sync mode |
| `:SX44,deg1,deg2#` | `0/1` | Stage and sync both encoder axes |
| `:SXEM,n#` | `0/1` | Set mount type for next restart |
| `:SXTD,n.n#` | `0/1` | Set Dec tracking rate offset, arcsec per sidereal second |
| `:SXTR,n.n#` | `0/1` | Set RA tracking rate offset, arcsec per sidereal second |
| `:TS#` | none | Solar tracking rate |
| `:TK#` | none | King tracking rate |
| `:TL#` | none | Lunar tracking rate |
| `:TQ#` | none | Sidereal tracking rate |
| `:T+#` | none | Increase master sidereal clock by 0.02 Hz |
| `:T-#` | none | Decrease master sidereal clock by 0.02 Hz |
| `:TR#` | none | Reset master sidereal clock |
| `:Te#` | `0/1` | Enable tracking |
| `:Td#` | `0/1` | Disable tracking |
| `:To#` | `0/1` | Enable full compensation model |
| `:Tr#` | `0/1` | Enable refraction compensation |
| `:Tn#` | `0/1` | Disable compensation |
| `:T1#` | `0/1` | Single-axis tracking mode |
| `:T2#` | `0/1` | Dual-axis tracking mode |
| `:$BDn#` | `0/1` | Set Dec/Alt backlash in arcsec |
| `:$BRn#` | `0/1` | Set RA/Azm backlash in arcsec |
| `:%BD#` | `n#` | Get Dec/Alt backlash in arcsec |
| `:%BR#` | `n#` | Get RA/Azm backlash in arcsec |

### Mount Type Values

Current core constants are:

| Value | Mount type |
| --- | --- |
| `1` | GEM |
| `2` | FORK |
| `3` | ALTAZM |
| `4` | ALTALT |
| `5` | GEM_TA |
| `6` | GEM_TAC |
| `7` | FORK_TA |
| `8` | FORK_TAC |
| `9` | ALTAZM_UNL |

## Goto / Sync / Alignment

| Command | Reply | Description |
| --- | --- | --- |
| `:AW#` | `1` | Write alignment model to NV |
| `:A?#` | `mno#` | Alignment status: max stars, current star, last required star |
| `:A1#` .. `:A9#` | `0/1` | Start manual alignment with that many stars |
| `:A+#` | `0/1` | Accept current align point |
| `:CS#` | none | Sync to current target coordinates |
| `:CM#` | `N/A#` or `E1#`..`E9#` | Sync to current catalog/database object |
| `:D#` | `0x7f#` while moving, otherwise raw `#` | LX200 distance-bar style motion indicator |
| `:Gr#` | `HH:MM.T#` or `HH:MM:SS#` | Get target RA |
| `:GrH#` | `HH:MM:SS.SSSS#` | Get target RA, highest precision |
| `:Gd#` | `sDD*MM#` or `sDD*MM:SS#` | Get target Dec |
| `:GdH#` | `sDD*MM:SS.SSS#` | Get target Dec, highest precision |
| `:Gal#` | `sDD*MM#` or `sDD*MM'SS#` | Get target altitude |
| `:GaH#` | `sDD*MM'SS.SSS#` | Get target altitude, highest precision |
| `:Gz#` | `DDD*MM#` or `DDD*MM'SS#` | Get target azimuth |
| `:GzH#` | `DDD*MM'SS.SSS#` | Get target azimuth, highest precision |
| `:MA#` | `0`..`9` | Goto target Alt/Az |
| `:MD#` | `0`, `1`, or `2` | Destination pier side for current target |
| `:MN#` | `0`..`9` | Goto current position on opposite pier side |
| `:MNe#` | `0`..`9` | Force same-position goto to east pier side |
| `:MNw#` | `0`..`9` | Force same-position goto to west pier side |
| `:MP#` | `0`..`9` | Polar-align goto |
| `:MS#` | `0`..`9` | Goto current target |
| `:SrHH:MM.T#` | `0/1` | Set target RA |
| `:SrHH:MM:SS#` | `0/1` | Set target RA |
| `:SrHH:MM:SS.SSSS#` | `0/1` | Set target RA |
| `:SdsDD*MM#` | `0/1` | Set target Dec |
| `:SdsDD*MM:SS#` | `0/1` | Set target Dec |
| `:SdsDD*MM:SS.SSS#` | `0/1` | Set target Dec |
| `:SasDD*MM#` | `0/1` | Set target altitude |
| `:SasDD*MM'SS#` | `0/1` | Set target altitude |
| `:SasDD*MM'SS.SSS#` | `0/1` | Set target altitude |
| `:SzDDD*MM#` | `0/1` | Set target azimuth |
| `:SzDDD*MM'SS#` | `0/1` | Set target azimuth |
| `:SzDDD*MM'SS.SSS#` | `0/1` | Set target azimuth |

### `:MS#` / `:MA#` / `:MN#` / `:MP#` Return Codes

| Value | Meaning |
| --- | --- |
| `0` | Goto accepted |
| `1` | Below horizon limit |
| `2` | Above overhead limit |
| `3` | Controller in standby |
| `4` | Mount parked |
| `5` | Goto already in progress |
| `6` | Outside limits |
| `7` | Hardware fault |
| `8` | Already in motion |
| `9` | Unspecified error |

### Alignment Model Extended Commands

`ALIGN_MAX_NUM_STARS > 1` builds expose:

| Command | Reply | Description |
| --- | --- | --- |
| `:GX00#` | `n#` | `ax1Cor` in arcsec |
| `:GX01#` | `n#` | `ax2Cor` in arcsec |
| `:GX02#` | `n#` | `altCor` in arcsec |
| `:GX03#` | `n#` | `azmCor` in arcsec |
| `:GX04#` | `n#` | `doCor` in arcsec |
| `:GX05#` | `n#` | `pdCor` in arcsec |
| `:GX06#` | `n#` | `ffCor` for FORK/ALTAZM, else `0` |
| `:GX07#` | `n#` | `dfCor` for GEM-style mounts, else `0` |
| `:GX08#` | `n#` | `tfCor` in arcsec |
| `:GX0a#` | `n#` | `hcp` in degrees |
| `:GX0b#` | `n#` | `hca` in arcsec |
| `:GX0c#` | `n#` | `dcp` in degrees |
| `:GX0d#` | `n#` | `dca` in arcsec |
| `:GX09#` | `n#` | Number of uploaded stars, then resets internal star index |
| `:GX0A#` | `HH:MM:SS#` | Uploaded star actual HA |
| `:GX0B#` | `sDD*MM:SS#` | Uploaded star actual Dec |
| `:GX0C#` | `HH:MM:SS#` | Uploaded star mount HA |
| `:GX0D#` | `sDD*MM:SS#` | Uploaded star mount Dec |
| `:GX0E#` | `n#` | Uploaded star pier side, then advances star index |
| `:SX00,n#` .. `:SX0d,n#` | `0/1` | Set alignment model coefficients listed above |
| `:SX09,0#` | `0/1` | Reset alignment upload state |
| `:SX09,1#` | `0/1` | Build model from uploaded stars |
| `:SX09,2#` | `0/1` | Force model active |
| `:SX0A,HH:MM:SS#` | `0/1` | Upload actual HA for current star |
| `:SX0B,sDD*MM:SS#` | `0/1` | Upload actual Dec for current star |
| `:SX0C,HH:MM:SS#` | `0/1` | Upload mount HA for current star |
| `:SX0D,sDD*MM:SS#` | `0/1` | Upload mount Dec for current star |
| `:SX0E,n#` | `0/1` | Upload pier side and advance to next star |

### Goto Extended Settings

| Command | Reply | Description |
| --- | --- | --- |
| `:GX92#` | `n.nnn#` | Current slew period in us/step |
| `:GX93#` | `n.nnn#` | Base/default slew period in us/step |
| `:GX94#` | `n` plus optional ` N` | Current pier side: `0` none, `1` east, `2` west |
| `:GX95#` | `0` or `1` | Auto meridian flip enabled |
| `:GX96#` | `E`, `W`, `B`, or `A` | Preferred pier side |
| `:GX97#` | `n.n#` | Current step rate in deg/s |
| `:GX99#` | `n.nnn#` | Fastest allowed slew period in us/step |
| `:SX92,n.nnn#` | `0/1` | Set current slew period in us/step |
| `:SX93,[1-5]#` | none | Slew preset: `5`=50%, `4`=66.7%, `3`=100%, `2`=150%, `1`=200% |
| `:SX95,0#` or `:SX95,1#` | `0/1` | Disable/enable automatic meridian flip |
| `:SX96,E#` | `0/1` | Preferred pier side east |
| `:SX96,W#` | `0/1` | Preferred pier side west |
| `:SX96,B#` | `0/1` | Preferred pier side best |
| `:SX96,A#` | `0/1` | Preferred pier side automatic |
| `:SX98,0#` or `:SX98,1#` | `0/1` | Disable/enable pause at home during meridian flip |
| `:SX99,1#` | `0/1` | Continue after pause at home |

## Guide / Manual Motion

| Command | Reply | Description |
| --- | --- | --- |
| `:GX90#` | `n.nn#` | Pulse guide rate |
| `:Mgdn#` | none | Pulse guide for `n` ms in direction `d` where `d` is `w`, `e`, `n`, or `s` |
| `:MGdn#` | `0/1` | Same as `:Mgdn#`, numeric form |
| `:Mw#` | none | Move west at current guide rate |
| `:Me#` | none | Move east at current guide rate |
| `:Mn#` | none | Move north at current guide rate |
| `:Ms#` | none | Move south at current guide rate |
| `:Mp#` | none | Spiral-search motion |
| `:Q#` | none | Stop all slews, abort goto |
| `:Qe#` / `:Qw#` | none | Stop east/west motion |
| `:Qn#` / `:Qs#` | none | Stop north/south motion |
| `:RAn.n#` | none | Set axis1 custom guide rate in deg/s |
| `:REn.n#` | none | Set axis2 custom guide rate in deg/s |
| `:RG#` | none | Guide rate preset 1x |
| `:RC#` | none | Centering rate preset 8x |
| `:RM#` | none | Find rate preset 20x |
| `:RF#` | none | Fast rate preset 48x |
| `:RS#` | none | Slew rate preset, half current goto rate |
| `:R0#` .. `:R9#` | none | Numeric guide-rate preset |

## Park / Home / Limits / Status

### Park

| Command | Reply | Description |
| --- | --- | --- |
| `:hP#` | `0/1` | Park mount |
| `:hQ#` | `0/1` | Set current position as park |
| `:hR#` | `0/1` | Unpark mount |

### Home

| Command | Reply | Description |
| --- | --- | --- |
| `:h?#` | `hasSense,axis1Offset,axis2Offset#` | Home status. Source comment mentions auto-home, but current code returns 3 fields only. |
| `:hA0#` / `:hA1#` | none | Disable/enable automatic home at boot |
| `:hC#` | none | Move to home |
| `:hC1,R#` | none | Toggle axis1 home-sense reversal |
| `:hC1,n#` | none | Set axis1 home offset in arcsec |
| `:hC2,R#` | none | Toggle axis2 home-sense reversal |
| `:hC2,n#` | none | Set axis2 home offset in arcsec |
| `:hF#` | none | Reset mount at home/cold-start position |

### Limits

| Command | Reply | Description |
| --- | --- | --- |
| `:Gh#` | `sDD*#` | Horizon limit |
| `:Go#` | `DD*#` | Overhead limit |
| `:GXE9#` | `n#` | East meridian limit in minutes |
| `:GXEA#` | `n#` | West meridian limit in minutes |
| `:GXEe#` | `n#` | Axis1 minimum limit in degrees |
| `:GXEw#` | `n#` | Axis1 maximum limit in degrees |
| `:GXEB#` | `n#` | Axis1 maximum limit in hours |
| `:GXEC#` | `n#` | Axis2 minimum limit in degrees |
| `:GXED#` | `n#` | Axis2 maximum limit in degrees |
| `:ShsDD#` | `0/1` | Set lower altitude limit |
| `:SoDD#` | `0/1` | Set overhead altitude limit |
| `:SXE9,n#` | `0/1` | Set east meridian limit in minutes |
| `:SXEA,n#` | `0/1` | Set west meridian limit in minutes |

### Status

| Command | Reply | Description |
| --- | --- | --- |
| `:Gm#` | `E#`, `W#`, or `N#` | Meridian pier side |
| `:GU#` | status string | Human-readable packed status |
| `:Gu#` | packed bytes | Bit-packed status |
| `:GW#` | 4-char status | Mount type, tracking, parked/home, align-done |
| `:SX97,0#` | `0/1` | Disable buzzer |
| `:SX97,1#` | `0/1` | Enable buzzer |
| `:SX97,2#` | `0/1` | Beep |
| `:SX97,3#` | `0/1` | Alert tone |
| `:SX97,4#` | `0/1` | Click |

#### `:GU#` Status Characters

The reply is an ordered string assembled from active conditions. Characters currently used by the code include:

| Char | Meaning |
| --- | --- |
| `n` | Not tracking |
| `N` | No goto active |
| `p` | Not parked |
| `I` | Parking in progress |
| `P` | Parked |
| `F` | Park failed |
| `e` | Sync-to-encoders-only mode |
| `H` | At home |
| `h` | Homing |
| `B` | Auto-home at boot |
| `S` | PPS synced |
| `G` | Pulse guide active |
| `g` | Guide active |
| `r` | Refraction compensation enabled |
| `s` | Single-axis compensation mode |
| `t` | Full compensation model enabled |
| `(` | Lunar tracking rate selected |
| `O` | Solar tracking rate selected |
| `k` | King tracking rate selected |
| `w` | Meridian flip paused at home |
| `u` | Pause-at-home enabled |
| `z` | Buzzer enabled |
| `a` | Automatic meridian flip enabled |
| `R` | PEC data recorded |
| `/`, `,`, `~`, `;`, `^` | PEC state |
| `E`, `K`, `A`, `L` | GEM, FORK, ALTAZM, ALTALT |
| `o`, `T`, `W` | Pier side none, east, west |
| final digits | pulse-guide rate, guide rate, general error code |

#### `:Gu#` Packed Status Layout

`Gu` is the binary/pseudo-binary status form. The current implementation fills bytes as:

| Byte | Contents |
| --- | --- |
| `0` | tracking/goto/PPS/pulse-guide plus compensation mode |
| `1` | tracking-rate selection plus sync-to-encoders and guide-active |
| `2` | home/homing/auto-home/home-pause/buzzer/auto-flip |
| `3` | mount type plus pier side |
| `4` | PEC state and PEC-recorded flag |
| `5` | park state |
| `6` | pulse-guide selection |
| `7` | guide-rate selection |
| `8` | general limits/error code |

## Object Library

| Command | Reply | Description |
| --- | --- | --- |
| `:LB#` | none | Previous object matching current constraints |
| `:LCn#` | none | Select record number `n` |
| `:LI#` | `name,type#` | Current object name and type |
| `:LIG#` | none | Load current object into goto target |
| `:LR#` | `name,type,ra,dec#` | Current object info, then advance to next record |
| `:LWname,type#` | `0/1` | Write current target RA/Dec to next free record |
| `:LN#` | none | Next object matching current constraints |
| `:L$#` | `1` | Move to catalog name record |
| `:LD#` | none | Clear current record |
| `:LL#` | none | Clear current catalog |
| `:L!#` | none | Clear all catalogs |
| `:L?#` | `n#` | Free records across all catalogs |
| `:Lon#` | `0/1` | Select catalog `0..14` |

Object type codes written/read by library commands are:

`UNK`, `OC`, `GC`, `PN`, `DN`, `SG`, `EG`, `IG`, `KNT`, `SNR`, `GAL`, `CN`, `STR`, `PLA`, `CMT`, `AST`

## PEC

Available when PEC support is enabled for axis 1.

| Command | Reply | Description |
| --- | --- | --- |
| `:GX91#` | `n#` | PEC analog value |
| `:GXE6#` | `n.nnnnnn#` | Steps per sidereal second |
| `:GXE7#` | `n#` | Worm rotation steps from NV |
| `:GXE8#` | `n#` | PEC buffer size in seconds |
| `:SXE7,n#` | `0/1` | Set worm rotation steps |
| `:VH#` | `nnnnn#` | PEC index sense position in sidereal seconds |
| `:VR#` | `snnn,nnn#` | Current PEC segment correction plus segment index |
| `:VRn#` | `snnn#` | PEC correction for segment `n` |
| `:Vrn#` | `x0x1...x9#` | Ten-byte hex frame of PEC data starting at segment `n` |
| `:VS#` | `n.nnnnnn#` | Steps per sidereal second of worm rotation |
| `:VW#` | `nnnnnn#` | Worm rotation steps |
| `:WR+#` | `0/1` | Rotate PEC table forward one second |
| `:WR-#` | `0/1` | Rotate PEC table backward one second |
| `:WRn,sn#` | none | Write PEC correction for segment `n` |
| `:$QZ+#` | none | Enable PEC playback |
| `:$QZ-#` | none | Disable PEC |
| `:$QZ/#` | none | Arm PEC recording |
| `:$QZZ#` | none | Clear PEC buffer |
| `:$QZ!#` | none | Save PEC data to NV |
| `:$QZ?#` | `I#`, `p#`, `P#`, `r#`, `R#`, optionally with `.#` | PEC status |

PEC status characters from `:$QZ?#`:

| Char | Meaning |
| --- | --- |
| `I` | Ignore/off |
| `p` | Ready to play |
| `P` | Playing |
| `r` | Ready to record |
| `R` | Recording |
| `.` | Index detected this second |

### `:GXUa#` Driver Status Flags

Local ASCII replies are comma-separated flag mnemonics:

| Flag | Meaning |
| --- | --- |
| `ST` | Standstill |
| `OA` | Output A open load |
| `OB` | Output B open load |
| `GA` | Output A short to ground |
| `GB` | Output B short to ground |
| `OT` | Over-temperature |
| `PW` | Over-temperature warning |
| `GF` | Driver fault |

## Focuser

Available in focuser-enabled or remote-focuser builds.

Addressing rules:

- `:FA#` returns the currently selected focuser number.
- `:FA1#` .. `:FA6#` selects the active focuser.
- `:F...#` uses the active focuser.
- `:F1...#` .. `:F6...#` directs a command to a specific focuser immediately.

Unit rules for local ASCII focuser commands:

- Uppercase `B`, `D`, `G`, `I`, `M`, `R`, `S` use microns.
- Lowercase `b`, `d`, `g`, `i`, `m`, `r`, `s` use raw steps.
- Other focuser commands are unitless or fixed-unit as noted below.

| Command | Reply | Description |
| --- | --- | --- |
| `:FA#` | `n` | Get active focuser number |
| `:FA1#` .. `:FA6#` | `0/1` | Select active focuser |
| `:hP#` | `0/1` | Park all focusers in standalone/remote focuser builds |
| `:hR#` | `0/1` | Unpark all focusers in standalone/remote focuser builds |
| `:Fa#` | `1` | Primary focuser present/selected |
| `:FT#` | `M1#`, `S3#`, etc. | Focuser status plus goto-rate digit |
| `:Fp#` | `0` or `1` | Mode. Current implementation returns `1` for DC/pseudo-absolute, `0` otherwise. |
| `:FI#` / `:Fi#` | `n#` | Full-in/min position |
| `:FM#` / `:Fm#` | `n#` | Maximum position |
| `:Fe#` | `n.n#` | Temperature delta from TCF baseline |
| `:Ft#` | `n.n#` | Focuser temperature in C |
| `:Fu#` | `n.nnnnn#` | Microns per step |
| `:FB#` / `:Fb#` | `n#` | Backlash |
| `:FBn#` / `:Fbn#` | `0/1` | Set backlash |
| `:FC#` | `n.nnnnn#` | TCF coefficient in microns per C |
| `:FCsn.n#` | `0/1` | Set TCF coefficient |
| `:Fc#` | `0` or `1` | TCF enabled status |
| `:Fc0#` / `:Fc1#` | `0/1` | Disable/enable TCF |
| `:FD#` / `:Fd#` | `n#` | TCF deadband |
| `:FDn#` / `:Fdn#` | `0/1` | Set TCF deadband |
| `:FP#` | `n#` | DC motor power percent |
| `:FPn#` | `0/1` | Set DC motor power percent |
| `:FQ#` | none | Stop focuser |
| `:F1#` .. `:F9#` | none | Set move/goto rate preset |
| `:FW#` | `n#` | Working goto rate in um/s |
| `:F+#` | none | Move inward |
| `:F-#` | none | Move outward |
| `:FG#` / `:Fg#` | `sn#` | Current position |
| `:FRsn#` / `:Frsn#` | none | Relative goto |
| `:FSn#` / `:Fsn#` | `0/1` | Absolute goto |
| `:FZ#` | none | Zero current position |
| `:FH#` | none | Set current position as home |
| `:Fh#` | none | Move to home |
| `:GXU4#` .. `:GXU9#` | flags | Driver status for focuser axes that expose `Axis.command.cpp` |

Rate preset meaning:

| Preset | Meaning |
| --- | --- |
| `1` | 1 um/s move rate |
| `2` | 10 um/s move rate |
| `3` | 100 um/s move rate |
| `4` | 0.5x goto rate, move mode |
| `5` | 0.5x goto rate |
| `6` | 0.66x goto rate |
| `7` | 1x goto rate |
| `8` | 1.5x goto rate |
| `9` | 2x goto rate |

## Rotator

Available in rotator-enabled or remote-rotator builds.

| Command | Reply | Description |
| --- | --- | --- |
| `:rA#` | `0/1` | Rotator active |
| `:hP#` | `0/1` | Park rotator in standalone/remote rotator builds |
| `:hR#` | `0/1` | Unpark rotator in standalone/remote rotator builds |
| `:rT#` | `M1#`, `SD3#`, etc. | Status string plus rate digit |
| `:rI#` | `n#` | Minimum angle in degrees |
| `:rM#` | `n#` | Maximum angle in degrees |
| `:rD#` | `n.n#` | Degrees per step |
| `:rb#` | `n#` | Backlash in steps |
| `:rbn#` | `0/1` | Set backlash |
| `:rQ#` | none | Stop motion |
| `:r1#` .. `:r9#` | none | Set move/goto rate preset |
| `:rW#` | `n.n#` | Working slew rate in deg/s |
| `:rc#` | none | Continuous-move no-op, accepted for compatibility |
| `:r>#` | none | Move clockwise |
| `:r<#` | none | Move counter-clockwise |
| `:rG#` | `sDDD*MM#` | Current angle |
| `:rrsDDD*MM#` | none | Relative goto |
| `:rSsDDD*MM#` | `0/1` | Absolute goto |
| `:rZ#` | none | Zero position |
| `:rF#` | none | Set current position to half travel |
| `:rC#` | none | Move to half-travel / home target |
| `:r+#` | none | Enable derotation |
| `:r-#` | none | Disable derotation |
| `:rP#` | none | Move to parallactic angle |
| `:rR#` | none | Toggle derotator reverse direction |
| `:GX98#` | `D#`, `R#`, or `N#` | Rotator capability: derotate, rotate-only, or none |
| `:GXU3#` | flags | Driver status for rotator axis in standalone/remote builds |

Rotator rate presets:

| Preset | Meaning |
| --- | --- |
| `1` | 0.01 deg/s move rate |
| `2` | 0.1 deg/s move rate |
| `3` | 1.0 deg/s move rate |
| `4` | 0.5x goto rate used as move rate |
| `5` | 0.5x goto rate |
| `6` | 0.66x goto rate |
| `7` | 1x goto rate |
| `8` | 1.5x goto rate |
| `9` | 2x goto rate |

## Auxiliary Features

Available when auxiliary features are enabled.

Feature slots are numbered `1..8`.

### Discovery

| Command | Reply | Description |
| --- | --- | --- |
| `:GXY0#` | `xxxxxxxx#` | Eight-character bitmap of active feature slots, `1` = present |
| `:GXYn#` | `name,purpose#` | Slot name plus purpose code |

### Slot State

| Command | Reply | Description |
| --- | --- | --- |
| `:GXXn#` | purpose-specific payload | Get current state for slot `n` |
| `:SXXn,Vv#` | `0/1` | Generic value set |
| `:SXXn,Zf#` | `0/1` | Dew-heater zero temperature |
| `:SXXn,Sf#` | `0/1` | Dew-heater span temperature |
| `:SXXn,Ef#` | `0/1` | Intervalometer exposure seconds |
| `:SXXn,Df#` | `0/1` | Intervalometer delay seconds |
| `:SXXn,Cf#` | `0/1` | Intervalometer count |

### `:GXXn#` Reply Shapes

The payload depends on the slot purpose:

| Purpose | Reply shape |
| --- | --- |
| Switch / momentary switch / cover switch | `value` plus optional power telemetry |
| Analog output | `value` plus optional power telemetry |
| Dew heater | `enabled,zero,span,deltaT` plus optional power telemetry |
| Intervalometer | `currentCount,exposure,delay,count` |

When power monitoring is compiled in, the local ASCII implementation appends:

`;<volts>,<amps>,<flags>`

Where flags are a five-character string using `P`, `C`, `U`, `V`, `T` or `!`.

## Axis / Motor / Driver Service Commands

Axis service commands are implemented by `Axis.command.cpp`. For the main telescope/mount build these are reachable for axis 1 and axis 2, if present. The rotator exposes axis3, if present. The focuser(s) expose any of axis 4 to axis 9, if present.

| Command | Reply | Description |
| --- | --- | --- |
| `:GXAa,p#` | `value,min,max,type,name#` | Get axis parameter `p` for axis `a` (`1..9`) |
| `:GXAa,M#` | `name#` | Motor/driver name for axis `a` |
| `:GXAa,0#` | `n#` | Parameter count for axis `a` |
| `:GXSa#` | `delta,velocity#` | Servo-only delta and velocity for axis `a` |
| `:GXUa#` | `flags#` | Stepper driver status for axis `a` |
| `:SXAC,0#` | `0/1` | Use runtime NV axis settings |
| `:SXAC,1#` | `0/1` | Use compile-time `Config.h` axis settings |
| `:SXAa,R#` | `0/1` | Revert axis `a` settings to defaults on next boot |
| `:SXAa,p,value#` | `0/1` | Set axis parameter `p` for axis `a` |

### `:GXAa,p#` Reply Format

The reply is:

`value,min,max,type,name#`

Where:

- `value` is the current NV value
- `min` and `max` are the documented range
- `type` is the axis parameter type code returned by firmware
- `name` is the parameter name from the axis parameter table
- some `name` values are locale tokens such as `$1` or `$12` rather than literal English labels

Axis parameter type codes:

| Code | Symbol | Meaning |
| --- | --- | --- |
| `0` | `AXP_INVALID` | Invalid / placeholder entry |
| `1` | `AXP_BOOLEAN` | Boolean value stored in NV and applied on restart/reinit |
| `2` | `AXP_BOOLEAN_IMMEDIATE` | Boolean value applied immediately when set |
| `3` | `AXP_INTEGER` | Integer value stored in NV and applied on restart/reinit |
| `4` | `AXP_INTEGER_IMMEDIATE` | Integer value applied immediately when set |
| `5` | `AXP_FLOAT` | Floating-point value stored in NV and reported as a plain numeric field |
| `6` | `AXP_FLOAT_IMMEDIATE` | Floating-point value applied immediately when set |
| `7` | `AXP_FLOAT_RAD` | Angular float stored internally in radians |
| `8` | `AXP_FLOAT_RAD_INV` | Inverse angular float stored internally with radian/degree conversion |
| `9` | `AXP_POW2` | Power-of-two constrained integer-like value |
| `10` | `AXP_DECAY` | Driver decay-mode selector |

Protocol note:

- `:GXAa,p#` normalizes `AXP_FLOAT_RAD` and `AXP_FLOAT_RAD_INV` to reply type code `5` and converts the value/range to degree-based units for transport.
- Axis parameter values are float-backed in firmware; values are always float even when the logical type is boolean or integer so the UI can show the appropriate controls.
- Boolean parameters: some use logical `0`/`1`, while others use the firmware constants `OFF = -1` and `ON = -2`; so any UI can show 'True'/'False' or 'On'/'Off'.
- For `AXP_DECAY`, the UI shows the matching decay-mode text for the numeric value `1=MIXED`, `2=FAST`, `3=SLOW`, `4=SPREADCYCLE`, `5=STEALTHCHOP`.

Locale-backed axis parameter name tokens currently used by firmware:

| Token | Meaning |
| --- | --- |
| `$1` | Steps/degree |
| `$2` | Min limit, degs |
| `$3` | Max limit, degs |
| `$4` | Steps/um |
| `$5` | Min limit, um |
| `$6` | Max limit, um |
| `$7` | Reverse |
| `$8` | Microsteps |
| `$9` | Microsteps Goto |
| `$10` | Decay mode |
| `$11` | Decay mode Goto |
| `$12` | mA Hold |
| `$13` | mA Run |
| `$14` | mA Goto |
| `$15` | 256x Interpolate |
| `$16` | P tracking |
| `$17` | I tracking |
| `$18` | D tracking |
| `$19` | P slewing |
| `$20` | I slewing |
| `$21` | D slewing |
| `$22` | Rads/count |
| `$23` | Steps/count ratio |
| `$24` | Max accel, %/s/s |
| `$25` | Min power, % |
| `$26` | Max power, % |

## CAN Remote-Node Variants

The following files implement the same logical command families over packed CAN messages rather than the ASCII LX200-style transport:

- `src/telescope/focuser/local/Focuser.can.command.cpp`
- `src/telescope/rotator/local/Rotator.can.command.cpp`
- `src/telescope/auxiliary/local/Features.can.command.cpp`

These are not separate end-user command names. They are binary transport implementations of the same focuser, rotator, and auxiliary-feature command sets above.

Notable transport differences:

- CAN replies are binary-packed, not ASCII text.
- `:GXY0#` for CAN features returns a single bitmask byte rather than an eight-character ASCII bitmap.
- CAN focuser/rotator driver-status replies are packed bitfields rather than comma-separated text flags.

## Source Coverage

This document was built from the current handlers in:

- `src/telescope/Telescope.command.cpp`
- `src/telescope/mount/Mount.command.cpp`
- `src/telescope/mount/goto/Goto.command.cpp`
- `src/telescope/mount/guide/Guide.command.cpp`
- `src/telescope/mount/home/Home.command.cpp`
- `src/telescope/mount/library/Library.command.cpp`
- `src/telescope/mount/limits/Limits.command.cpp`
- `src/telescope/mount/park/Park.command.cpp`
- `src/telescope/mount/pec/Pec.command.cpp`
- `src/telescope/mount/site/Site.command.cpp`
- `src/telescope/mount/status/Status.command.cpp`
- `src/lib/axis/Axis.command.cpp`
- `src/telescope/focuser/local/Focuser.command.cpp`
- `src/telescope/rotator/local/Rotator.command.cpp`
- `src/telescope/auxiliary/local/Features.command.cpp`
- plus the corresponding CAN transport handlers
- plus `src/libApp/commands/ProcessCmds.cpp`
