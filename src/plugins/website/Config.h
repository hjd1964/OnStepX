// ---------------------------------------------------------------------------------------------------------------------------------
// Configuration for OnStepX Webserver Plug-in

// ---------------------------------------------------------------------------------------------------------------------------------
// ADJUST THE FOLLOWING TO CONFIGURE YOUR PLUG-IN'S FEATURES -----------------------------------------------------------------------
// <-Req'd = always must set, <-Often = usually must set, Option = optional, Adjust = adjust as req'd, Infreq = infrequently changed

// For Wifi successful startup an AP will appear with an SSID of "ONSTEP", after connecting: the web-site is at "192.168.0.1" and
// the cmd channel is at "192.168.0.1:9999". If locked out selecting "Erase Flash: All Flash Contents" from the Arduino Tools menu 
// before uploading/flashing again can help restore access to the ESP8266.

//      Parameter Name              Value   Default  Notes                                                                      Hint
// DISPLAY -------------------------------------------------------------------------------------------------------------------------
#define DISPLAY_LANGUAGE             L_en //   L_en, English. Specify language with two letter country code, if supported.    Adjust
#define DISPLAY_WEATHER               OFF //    OFF, ON ambient conditions in locale default units.                           Option
#define DISPLAY_INTERNAL_TEMPERATURE  OFF //    OFF, ON internal MCU temp. in locale default units.                           Option
#define DISPLAY_WIFI_SIGNAL_STRENGTH   ON //     ON, Wireless signal strength reported via web interface. OFF otherwise.      Option
#define DISPLAY_RESET_CONTROLS         ON //     ON, ON to allow reset of OnStep, FWU for STM32 firmware upload pin HIGH.     Option

#define DISPLAY_SPECIAL_CHARS          ON //     ON, For standard ASCII special symbols (compatibility.)                      Infreq
#define DISPLAY_ADVANCED_CHARS         ON //     ON, For standard "RA/Dec" instead of symbols.                                Infreq
#define DISPLAY_HIGH_PRECISION_COORDS OFF //    OFF, ON for high precision coordinate display on status page.                 Infreq

// DRIVE CONFIGURATION -------------------------------------------------------------------------------------------------------------
#define DRIVE_CONFIGURATION            ON //    ON, to display/modify mount, rotator, focuser settings                        Option
#define DRIVE_MAIN_AXES_MICROSTEPS     ON //    ON, to display Axis1/2 Microsteps if available.                               Option
#define DRIVE_MAIN_AXES_CURRENT        ON //    ON, to display Axis1/2 IRUN if available.                                     Option
#define DRIVE_MAIN_AXES_REVERSE        ON //    ON, to display Axis1/2 Reverse if available.                                  Option

// THAT'S IT FOR USER CONFIGURATION!
// -------------------------------------------------------------------------------
