OnStepX Telescope Controller
===========================
# Important
There is no release of OnStepX yet but it has basic testing done now and all features are implemented so I consider it ready testing by advanced OnStep users.

# What is OnStepX?
OnStepX is the advanced version of the OnStep computerized telescope controller with support for both stepper motors (step/dir interface) and servo motors (PD or II interface using AB quadrature or CW/CCW or Pulse/Dir encoders.)

It supports:
* Telescope Mount control (Alt/Azm and Equatorial GEM/Fork.)  Optional support for Eq mounts with Tangent Arm Declination.  Usually the Goto capability is enabled, but that's optional as well for those who just want basic mount control.
* Telescope Rotator control (including Alt/Azm de-rotation.)
* Telescope Focuser control (up to 6 focusers so it can handle collimation as well as focusing.)
* Telescope Accessory control (combination of up to 8 dew-heaters, switches, analog PWM.)

# Features
OnStepX supports a wide variety of connection options.  Several serial
"command channels" can be utilized. One of the these is normally devoted to a USB
connection and for the other(s) choose from the following:

* Bluetooth
* ESP8266 WiFi
* Arduino M0/Ethernet Shield
* Even another USB port or RS232 serial isn't very difficult to add.

In the case of running OnStepX on an ESP32 it can provide its own Bluetooth or WiFi IP command channels without additional hardware by simply activating the feature in OnStepX.

Other software in the OnStep ecosystem include:

* an [ASCOM](http://ascom-standards.org/) driver (with IP and Serial support),
* an Android App useable over WiFi or Bluetooth equipped Phones/Tablets,
* a "built-in" website (on the Ethernet and/or WiFi device),
* a full planetarium program that controls all features ([Sky Planetarium](http://stellarjourney.com/index.php?r=site/software_sky)).

OnStep is compatible with the LX200 protocol. This means it can be controlled
from other planetarium software, like: Sky Safari, CdC (even without ASCOM),
Stellarium, etc.

There are also [INDI](http://www.indilib.org/about.html) drivers so it can be used from Linux, with CdC or KStars.

# Documentation
Detailed documentation, including the full set of features, detailed designs for
PCBs, instructions on how to build a controller, how to configure the firmware
for your particular mount, can all be found the [OnStep Group Wiki](https://groups.io/g/onstep/wiki/home).

# Change Log
All the changes are tracking in git, and a detailed list can be accessed using the
following git command:
 
git log --date=short --pretty=format:"%h %ad %<(20)%an %<(150,trunc)%s"

# Support
Questions and discussion should be on the mailing list (also accessible via the
web) at the [OnStep Group](https://groups.io/g/onstep/).

# License
OnStep is open source free software, licensed under the GPL.

See [LICENSE.txt](./LICENSE.txt) file.

# Author
[Howard Dutton](http://www.stellarjourney.com)

