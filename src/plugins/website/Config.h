// ---------------------------------------------------------------------------------------------------------------------------------
// Configuration for OnStepX Webserver Plug-in

// ---------------------------------------------------------------------------------------------------------------------------------
// ADJUST THE FOLLOWING TO CONFIGURE YOUR PLUG-IN'S FEATURES -----------------------------------------------------------------------
// <-Req'd = always must set, <-Often = usually must set, Option = optional, Adjust = adjust as req'd, Infreq = infrequently changed

// For Wifi successful startup an AP will appear with an default SSID of "OnStepX" and default password of "password".
// After connecting: the web-site is at "192.168.0.1" and the cmd channels are at "192.168.0.1:9996 to 9999".
// If locked out selecting "Erase Flash: All Flash Contents" from the Arduino IDE Tools menu before uploading/flashing again
// can help restore access to the ESP32; or see OnStepX's Extended.config.h to wipe NV.

//      Parameter Name              Value   Default  Notes                                                                      Hint
// DISPLAY -------------------------------------------------------------------------------------------------------------------------
#ifndef DISPLAY_LANGUAGE
#define DISPLAY_LANGUAGE             L_en //   L_en, English. Or L_ce, L_de, L_en, L_us, L_es two letter country code.        Adjust
#endif
#ifndef DISPLAY_WEATHER
#define DISPLAY_WEATHER               OFF //    OFF, ON ambient conditions in locale default units.                           Option
#endif
#ifndef DISPLAY_INTERNAL_TEMPERATURE
#define DISPLAY_INTERNAL_TEMPERATURE  OFF //    OFF, ON internal MCU temp. in locale default units.                           Option
#endif
#ifndef DISPLAY_WIFI_SIGNAL_STRENGTH
#define DISPLAY_WIFI_SIGNAL_STRENGTH   ON //     ON, Wireless signal strength reported via web interface. OFF otherwise.      Option
#endif
#ifndef DISPLAY_SERVO_MONITOR
#define DISPLAY_SERVO_MONITOR         OFF //    OFF, ON to display the servo monitor (any axis.)                              Option
#endif
#ifndef DISPLAY_SERVO_ORIGIN_CONTROLS
#define DISPLAY_SERVO_ORIGIN_CONTROLS OFF //    OFF, ON to display control to set the absolute encoder origin for servos.     Option
#endif
#ifndef DISPLAY_RESET_CONTROLS
#define DISPLAY_RESET_CONTROLS         ON //     ON, ON to allow reset of OnStep, FWU for STM32 firmware upload pin HIGH.     Option
#endif

#ifndef DISPLAY_SPECIAL_CHARS
#define DISPLAY_SPECIAL_CHARS          ON //     ON, For standard ASCII special symbols (compatibility.)                      Infreq
#endif
#ifndef DISPLAY_HIGH_PRECISION_COORDS
#define DISPLAY_HIGH_PRECISION_COORDS OFF //    OFF, ON for high precision coordinate display on status page.                 Infreq
#endif

// DRIVE CONFIGURATION -------------------------------------------------------------------------------------------------------------
#ifndef DRIVE_CONFIGURATION
#define DRIVE_CONFIGURATION            ON //    ON, to display/modify mount, rotator, focuser settings                        Option
#endif
#ifndef DRIVE_MAIN_AXES_MICROSTEPS
#define DRIVE_MAIN_AXES_MICROSTEPS     ON //    ON, to display Axis1/2 Microsteps if available.                               Option
#endif
#ifndef DRIVE_MAIN_AXES_CURRENT
#define DRIVE_MAIN_AXES_CURRENT        ON //    ON, to display Axis1/2 IRUN if available.                                     Option
#endif
#ifndef DRIVE_MAIN_AXES_REVERSE
#define DRIVE_MAIN_AXES_REVERSE        ON //    ON, to display Axis1/2 Reverse if available.                                  Option
#endif

// HOME OFFSET ---------------------------------------------------------------------------------------------------------------------
#ifndef HOME_OFFSET_RANGE_AXIS1
#define HOME_OFFSET_RANGE_AXIS1      7200 //  7200, allow adjusting home offset up to +/- 2 degrees                           Infreq
#endif
#ifndef HOME_OFFSET_RANGE_AXIS2
#define HOME_OFFSET_RANGE_AXIS2      7200 //  7200, allow adjusting home offset up to +/- 2 degrees                           Infreq
#endif

// THAT'S IT FOR USER CONFIGURATION!
// -------------------------------------------------------------------------------
