// -----------------------------------------------------------------------------------
// Locale English (default,) ISO ISO639-1 language code "en"
#pragma once

// General
#define L_UPLOAD "Upload"
#define L_DOWNLOAD "Download"
#define L_ACCEPT "Accept"
#define L_CONTINUE "Continue"
#define L_ARE_YOU_SURE "Are you sure"
#define L_LOGOUT "Logout"

#define L_UT "UT"
#define L_LST "LST"
#define L_LONG "Longitude"
#define L_LAT "Latitude"
#define L_EAST "East"
#define L_WEST "West"
#define L_ZENITH "Zenith"
#define L_NCP "NCP"
#define L_SCP "SCP"

#define L_NONE "None"
#define L_AUTO "Auto"
#define L_SET_LABEL "Goto"
#define L_OK "Ok"
#define L_ON "On"
#define L_OFF "Off"
#define L_START "Start"
#define L_DELTA "Delta"
#define L_ZERO "Zero"
#define L_SPAN "Span"
#define L_POWER "Pwr"
#define L_GUIDE "Guide"
#define L_CENTER "Center"
#define L_BLUE "Blue"
#define L_GREEN "Green"
#define L_RATE "rate"
#define L_RANGE "range"
#define L_UNKNOWN "Unknown"
#define L_FAULT "Fault"
#define L_POINT "Point"
#define L_COMPLETE "Complete"
#define L_ACTIVE "Active"
#define L_INACTIVE "Inactive"

// ---------------------------- Index ----------------------------
// date/time
#define L_TIME_BROWSER_UT "UT (web browser)"

// weather
#define L_TEMPERATURE "Temperature"
#define L_AMBIENT_TEMPERATURE "Ambient Temperature"
#define L_PRESSURE "Barometric Pressure"
#define L_HUMIDITY "Relative Humidity"
#define L_DEW_POINT "Dew Point Temperature"

// operation
#define L_TRACKING "Tracking"
#define L_SITE "Site"
#define L_COORDINATES "Coordinates"
#define L_CURRENT "Position"
#define L_TARGET "Target&nbsp;&nbsp;"
#define L_ENCODER "Encodr"
#define L_POLAR_ALIGN "Polar Alignment"

// pier side
#define L_PIER_SIDE "Pier Side"
#define L_MERIDIAN_FLIPS "meridian flips"
#define L_ALIGN_MESSAGE "Mount relative to"
#define L_MERIDIAN_FLIP_W_TO_E "Meridian Flip, West to East"
#define L_MERIDIAN_FLIP_E_TO_W "Meridian Flip, East to West"

#define L_OPERATIONS "Operations"

// parking
#define L_PARKING "Parking"
#define L_PARKED "Parked"
#define L_NOT_PARKED "Not Parked"
#define L_PARKING "Parking"
#define L_PARK_FAILED "Park Failed"

// home
#define L_HOMING "Homing"
#define L_AT_HOME "At Home"
#define L_SLEWING "Slewing"
#define L_PPS_SYNC "PPS Sync"

// tracking
#define L_REFR_COMP_RA "RC"
#define L_REFR_COMP_BOTH "RCD"
#define L_FULL_COMP_RA "FC"
#define L_FULL_COMP_BOTH "FCD"
#define L_TRACKING "Tracking"
#define L_CURRENT_MAXRATE "Current MaxRate"
#define L_DEFAULT_MAXRATE "Default MaxRate"
#define L_CURRENT_MAXSLEW "Maximum slew speed"

#define L_FREQUENCY_ADJUST "Frequency Adjust"

// driver status
#define L_DRIVER "Driver"
#define L_STATUS "Status"
#define L_STATE "State"
#define L_COMMS_FAILURE "Comms failure"
#define L_STANDSTILL "Standstill"
#define L_OPEN_LOAD "Open load"
#define L_SHORT_GND "Short gnd"
#define L_SHUTDOWN_OVER "Shutdown over"
#define L_PRE_WARNING "Pre-warning"
#define L_DRIVER_FAULT "Fault"

// misc. errors/workload
#define L_INTERNAL_TEMP "Controller Temperature"
#define L_LAST_GENERAL_ERROR "Last Error"
#define L_WORKLOAD "Workload"
#define L_WIRELESS_SIGNAL_STRENGTH "Wireless signal strength"

// --------------------------- Control ---------------------------

#if DISPLAY_SPECIAL_CHARS == ON
  #define HOME_CH "&#x1F3E0;"
  #define ARROW_DR "&#x27A5;"
  #define ARROW_UR "&#x27A6;"
  #define ARROW_R2 "&#x27A4;"
  #define CAUTION_CH "&#9888;"
  #define CLOCK_CH "&#x1F565;"
  #define ARROW_LL "&lt;&lt;"
  #define ARROW_L "&lt;"
  #define ARROW_R "&gt;"
  #define ARROW_RR "&gt;&gt;"
  #define ARROW_DD "&lt;&lt;"
  #define ARROW_D "&lt;"
  #define ARROW_U "&gt;"
  #define ARROW_UU "&gt;&gt;"
  #define SIDEREAL_CH "&#9733;"
  #define LUNAR_CH "&#9790;"
  #define SOLAR_CH "&#9737;"
  #define KING_CH "&#9812;"
#else
  #define HOME_CH "H"
  #define ARROW_DR "-&gt;"
  #define ARROW_UR "-&gt;"
  #define ARROW_R2 "&gt;"
  #define CAUTION_CH "!"
  #define CLOCK_CH "T"
  #define ARROW_LL "&lt;&lt;"
  #define ARROW_L "&lt;"
  #define ARROW_R "&gt;"
  #define ARROW_RR "&gt;&gt;"
  #define ARROW_DD "&lt;&lt;"
  #define ARROW_D "&lt;"
  #define ARROW_U "&gt;"
  #define ARROW_UU "&gt;&gt;"
  #define SIDEREAL_CH "*"
  #define LUNAR_CH "("
  #define SOLAR_CH "O"
  #define KING_CH "K"
#endif

#define BUTTON_N "N"
#define BUTTON_S "S"
#define BUTTON_E "E"
#define BUTTON_W "W"
#define BUTTON_SYNC "@"

#define L_HINT_CLOCK "Set Date/Time"
#define L_HINT_CONTINUE_GOTO "Continue Goto (when paused at home)"
#define L_HOME_PARK_TITLE "Homing and Parking"
#define L_FIND_HOME "Find Home"
#define L_RESET_HOME "Home (Reset)"
#define L_UNPARK "Un-Park"
#define L_PARK "Park"
#define L_HINT_EMERGENCY_STOP "Emergency Stop!"
#define L_HINT_RATE_SIDEREAL "Sidereal Rate"
#define L_HINT_RATE_LUNAR "Lunar Rate"
#define L_HINT_RATE_SOLAR "Solar Rate"
#define L_HINT_RATE_KING "King Rate"
#define L_HINT_FOC_SEL "Select Focuser"
#define L_HINT_DEROTATE_ON "Derotate On"
#define L_HINT_ROTATOR_REV "Reverse Derotator Motion"
#define L_HINT_ROTATOR_PAR "Move Rotator to Parallactic angle"
#define L_HINT_DEROTATE_OFF "Derotate Off"

#define L_ALIGN "Align"
#define L_AUX_FEATURES "Auxiliary Features"

#define L_FOCUSER "Focuser"
#define L_ROTATOR "Rotator"
#define L_DEROTATOR "Derotator"
#define L_CAMERA_EXPOSURE "Exposure"
#define L_CAMERA_DELAY "Delay"
#define L_CAMERA_COUNT "Count"

// ---------------------------- Library --------------------------

#define L_CATALOG "Catalog"
#define L_CATALOGS "Catalogs"
#define L_RECS_AVAIL "records available"
#define L_CAT_DATA "Data (downloaded OR to be uploaded.)"

#define L_CAT_EXAMPLE1 "Fields:<br /><pre>"
#define L_CAT_EXAMPLE2 "  A Catalog Name record is required for row 1, this<br />"
#define L_CAT_EXAMPLE3 "  consists of a '$' followed by up to 10 characters.<br /><br />"
#define L_CAT_EXAMPLE4 "  Object Name is a sequence of up to 11 characters.<br /><br />"
#define L_CAT_EXAMPLE5 "  Cat is a category code as follows:<br />"
#define L_CAT_EXAMPLE6 "  RA (Jnow) is in Hours, Minutes, and Seconds.<br />"
#define L_CAT_EXAMPLE7 "  Dec (Jnow) is in +/- Degrees, Minutes, Seconds.<br />"

#define L_CAT_NO_NAME "Catalog selected, has no name."
#define L_CAT_GET_NAME_FAIL "Catalog selected, get name failed."
#define L_CAT_NO_CAT "No catalog selected."
#define L_CAT_NO_OBJECT "No object selected."
#define L_SELECTED "selected"
#define L_CAT_DOWNLOAD_FAIL "Download failed, bad data."
#define L_CAT_DOWNLOAD_SUCCESS "Download success."
#define L_CAT_DOWNLOAD_INDEX_FAIL "Download failed, couldn't index to catalog."
#define L_CAT_DATA_REMOVED "Catalog data removed"
#define L_CAT_UPLOAD_FAIL "Upload failed, bad catalog name."
#define L_CAT_UPLOAD_INDEX_FAIL "Upload failed, indexing catalog name rec."
#define L_CAT_DELETE_FAIL "Upload failed, deleting catalog name rec."
#define L_CAT_WRITE_NAME_FAIL "Upload failed, writing catalog name rec."
#define L_CAT_UPLOAD_NO_NAME_FAIL "Upload failed, line 1 must contain catalog name."
#define L_CAT_BAD_FORM "Upload failed, bad format in line# "
#define L_CAT_UPLOAD_BAD_OBJECT_NAME "Upload failed, bad object name in line# "
#define L_CAT_BAD_CATEGORY "Upload failed, bad category in line# "
#define L_CAT_BAD_RA "Upload failed, bad RA format in line# "
#define L_CAT_BAD_DEC "Upload failed, bad Dec format in line# "
#define L_CAT_UPLOAD_RA_FAIL "Upload failed, sending RA in line# "
#define L_CAT_UPLOAD_DEC_FAIL "Upload failed, sending Dec in line# "
#define L_CAT_UPLOAD_LINE_FAIL "Upload failed, sending line# "
#define L_CAT_UPLOAD_SUCCESS "Upload success"
#define L_CAT_UPLOAD_LINES_WRITTEN "lines written"
#define L_CAT_UPLOAD_SELECT_FAIL "Upload failed, unable to select catalog."
#define L_CAT_UPLOAD_NO_CAT "Upload failed, no catalog selected."
#define L_CAT_CLEAR "Clear Catalog"
#define L_CAT_CLEAR_LIB "Clear Library"

// --------------------------- Encoders --------------------------

// general settings
#define L_ENC_SYNC_TO_ENC "Sync OnStep -> Encoders"
#define L_ENC_SYNC_TO_ONS "Sync Encoders -> OnStep"
#define L_ENC_ORIGIN_TO_ONS "Set OnStep -> Encoders Origin"
#define L_ENC_AUTO_SYNC "Automatically Sync Encoders -> OnStep"
#define L_ENC_CONF "Encoder Configuration"
#define L_ENC_SET_TPD "Counts per degree"
#define L_ENC_SYNC_THLD "Auto Sync threshold"
#define L_ENC_SYNC_THLD_UNITS "0 to 9999 arc-sec"
#define L_ENC_AUTO_RATE "OnStep rate control"
#define L_NOW "Now"

// ------------------------------ PEC ----------------------------

#define L_PEC_STATUS "Status"
#define L_PEC_CONTROL "PEC Control"
#define L_PEC_PLAY "Play"
#define L_STOP "Stop"
#define L_PEC_CLEAR "Clear"
#define L_PEC_REC "Record"
#define L_PEC_CLEAR_MESSAGE "Clear erases the memory buffer not EEPROM.  During recording corrections are averaged 3:1 favoring the buffer unless cleared in which case the full correction is used."
#define L_PEC_EEWRITE "Write to EEPROM"
#define L_PEC_EEWRITE_MESSAGE "Writes PEC data to EEPROM so it's remembered if OnStep is restarted.  Writing the data can take a few seconds."
#define L_DISABLED_MESSAGE "DISABLED"
#define L_PEC_IDLE "Idle"
#define L_PEC_WAIT_PLAY "Play waiting to start"
#define L_PEC_PLAYING "Playing"
#define L_PEC_WAIT_REC "Record waiting for index to arrive"
#define L_PEC_RECORDING "Recording"
#define L_PEC_UNK "Unknown"
#define L_PEC_EEWRITING "writing to EEPROM"

// --------------------------- Settings --------------------------

#define L_REFINE_POLAR_ALIGN "Refine Polar Alignment"
#define L_REFINE_PA "Refine PA"
#define L_REFINE_MESSAGE1 "Setup &amp; 3+ Star align mount. Goto bright star near NCP or SCP with Dec in 50 to 80&deg; range (N or S.) "
#define L_REFINE_MESSAGE2 "Press [Refine PA] button. Use mount's PA adjust controls to center the star again. "
#define L_REFINE_MESSAGE3 "Optionally align the mount again."

#define L_PARK "Park"
#define L_SET_PARK "Set-Park"
#define L_SET_PARK_CURRENT_COORDS "Set park position at current coordinates"

#define L_TRK_FASTER "faster"
#define L_TRK_SLOWER "slower"
#define L_TRK_RESET "Reset (default)"
#define L_TRK_COMP "Compensated Tracking Rate (Pointing Model/Refraction)"
#define L_TRK_FULL "Full"
#define L_TRK_REFR "Refraction Only"
#define L_TRK_DUAL "Dual Axis"
#define L_TRK_SINGLE "Single Axis"

#define L_BUZZER "Goto Alert, Buzzer"

#define L_MERIDIAN_FLIP_AUTO "Automatic Meridian Flip at Limit"
#define L_MERIDIAN_FLIP_NOW "Now"
#define L_MERIDIAN_FLIP_PAUSE "Meridian Flip, Pause at Home"

// ------------------------ Configuration ------------------------

#define L_BASIC_SET_TITLE "Basic:"

#define L_MAX_SLEW_SPEED "Maximum Slew Speed"

// goto Rate
#define L_VSLOW "VSlow"
#define L_SLOW "Slow"
#define L_NORMAL "Normal"
#define L_FAST "Fast"
#define L_VFAST "VFast"

// backlash
#define L_BACKLASH "Backlash"
#define L_BACKLASH_RANGE_AXIS1 "Axis1, 0 to 3600 arc-sec"
#define L_BACKLASH_RANGE_AXIS2 "Axis2, 0 to 3600 arc-sec"
#define L_BACKLASH_RANGE_AXIS3UP "Backlash, 0 to 32767 steps"

// deadband/TCF
#define L_DEADBAND "TCF Deadband"
#define L_TCF_COEF "TCF Coef"
#define L_TCF_COEF_EN "TCF Enable"
#define L_DEADBAND_RANGE_AXIS4UP "TCF Deadband, 1 to 32767 steps"
#define L_TCF_COEF_RANGE_AXIS4UP "TCF Coef, &pm;999.0 microns/deg. C"
#define L_TCF_COEF_EN_AXIS4UP "TCF Enable, 0 false or 1 true"

// limits
#define L_LIMITS_TITLE "Horizon and Overhead Limits"
#define L_LIMITS_RANGE_HORIZON "Horizon, min altitude &pm;30&deg;"
#define L_LIMITS_RANGE_OVERHEAD "Overhead, max altitude 60 to 90&deg;"
#define L_LIMITS_RANGE_MERIDIAN_E "Past Meridian, East of pier &pm;270&deg;"
#define L_LIMITS_RANGE_MERIDIAN_W "Past Meridian, West of pier &pm;270&deg;"

// location
#define L_LOCATION_TITLE "Time and Location"
#define L_LOCATION_LONG "Longitude deg. min. sec. &pm;180, W is +"
#define L_LOCATION_LAT  "Latitude deg. min. sec. &pm;90, N is +"
#define L_LOCATION_RANGE_UTC_OFFSET "UTC Offset, in hours and min. -14 to +12"
#define L_LOCATION_MESSAGE_UTC_OFFSET "Opposite of a time-zone value, this is for Standard Time (not Daylight Time.)"

// advanced configuration
#define L_REVERT "Revert to Defaults"
#define L_ADV_SET_TITLE "Advanced"
#define L_ADV_SET_HEADER_MSG "unless noted otherwise changes below take effect after rebooting OnStep:"
#define L_ADV_MOUNT_TYPE " select 1 GEM, 2 EQ Fork, or 3 Alt/Azm"
#define L_ADV_BOOL "0 false or 1 true"

#define L_ADV_SET_SPWR "Steps per worm rotation"
#define L_ADV_SET_SPD "Steps per degree"
#define L_ADV_SET_SPM "Steps per micron"
#define L_ADV_SET_REV "Rev. direction"
#define L_ADV_SET_MIN "Minimum position"
#define L_ADV_SET_MAX "Maximum position"

#define L_ADV_SET_SPECIAL "Step/Dir Driver:<br /><br />A value of -1 is equivalent to Config.h OFF. <i>Use caution with these!</i>"
#define L_ADV_SET_IMMEDIATE "Servo Driver:<br /><br />Settings take effect immediately no reboot required. <i>Use caution with these!</i>"
#define L_ADV_SET_TRAK_us "Microsteps"
#define L_ADV_SET_GOTO_us "Microsteps Goto"
#define L_ADV_SET_HOLD_ma "mA Hold Current"
#define L_ADV_SET_TRAK_ma "mA Tracking Current"
#define L_ADV_SET_SLEW_ma "mA Slewing Current"
#define L_ADV_SET_P "Tracking Porportional"
#define L_ADV_SET_I "Tracking Integral"
#define L_ADV_SET_D "Tracking Derivative"
#define L_ADV_SET_P_GOTO "Slewing Porportional"
#define L_ADV_SET_I_GOTO "Slewing Integral"
#define L_ADV_SET_D_GOTO "Slewing Derivative"

#define L_ADV_ENABLE "Enable Advanced Configuration"
#define L_ADV_DISABLE "Disable Advanced Configuration (all defaults on reboot)"
#define L_ADV_SET_AXIS_NO_EDIT "Editing of this axis has been disabled."
#define L_ADV_SET_NO_EDIT "Editing of all axes is disabled at the moment.  Perhaps this feature is not enabled (so the related configuration defaults are active) or you need to reboot OnStep."
#define L_ADV_SET_FOOTER_MSG1 "After pressing [Upload] take note of changes above since the upload may have been rejected due to an invalid parameter."
#define L_ADV_SET_FOOTER_MSG2 "When you choose to [Revert] that group becomes unavailable for editing until you reboot OnStep."
#define L_ADV_SET_FOOTER_MSG3 "If IRUN is set to other than the Config.h default, IHOLD is set to 50% (HALF.)"
#define L_ADV_SET_FOOTER_MSG4 "Changing the ratio of Axis1 Steps per worm rotation and Steps per degree may corrupt the Library NV memory area, backup any catalogs first if needed."

// reset control
#define L_RESET_TITLE "OnStep Reboot Control:"
#define L_WIPE_RESET "Wipe all settings and Reboot OnStep"
#define L_RESET "Reboot OnStep"
#define L_RESET_FWU "Reboot OnStep for firmware upload"
#define L_RESET_MSG1 "OnStep must be At Home or Parked, otherwise the reboot request is ignored."

// ----------------------------  SETUP ----------------------------

#define L_NETWORK_PERFORMANCE "Performance and compatibility"
#define L_NETWORK_CMD_TO "Command channel serial read time-out"
#define L_NETWORK_WWW_TO "Web channel serial read time-out"

#define L_NETWORK_STA_TITLE "Station mode (connect to an Access-Point)"
#define L_NETWORK_PWD "Password"
#define L_NETWORK_PWD_MSG "8 char min."
#define L_NETWORK_EN_DHCP "Enable DHCP"
#define L_NETWORK_EN_DHCP_MSG "Note: above addresses are ignored if DHCP is enabled"
#define L_NETWORK_EN_STA "Enable Station Mode"
#define L_IP_ADDRESS "IP Address"
#define L_GATEWAY "Gateway"
#define L_SUBNET "Subnet"
#define L_NETWORK_CHA "Channel"
#define L_NETWORK_EN_AP_MODE "Enable Access-Point Mode"

#define L_NETWORK_CONFIG_SECURITY "Network Configuration Security"

#define L_NETWORK_AP "Access-Point mode"

#define L_NETWORK_RESTART_MSG "You must <u>manually</u> restart for changes to take effect."

#define L_NETWORK_TITLE "Enter password to change configuration:"

#define L_NETWORK_ADVICE1 "Setup:<br/><br/>"
#define L_NETWORK_ADVICE2 "Enable either Station <b>OR</b> Access Point mode, enabling both can cause performance issues.<br/><br/>"
#define L_NETWORK_ADVICE3 "If you want OnStep to connect to your home WiFi network, enable only Station mode and "
#define L_NETWORK_ADVICE4 "disable Access Point mode. If OnStep cannot connect to your home WiFi network it will "
#define L_NETWORK_ADVICE5 "automatically re-enable Station mode; this is useful when you are away at a dark site and/or "
#define L_NETWORK_ADVICE6 "to guard against becoming locked out.<br/><br/>"
#define L_NETWORK_ADVICE7 "If locked out of WiFi you can use the Arduino IDE to reset the ESP flash memory (and "
#define L_NETWORK_ADVICE8 "hence go back to defaults.) From the Arduino Tools menu, change Erase Flash 'Only Sketch' "
#define L_NETWORK_ADVICE9 "to Erase Flash 'All Flash Contents' and upload the WiFi-Bluetooth sketch again."

// --------------------------- Constants -------------------------

#define L_DOWN_TITLE "Serial Interface to OnStep is Down!"
#define L_DOWN_MESSAGE1 "If you just rebooted OnStep please wait for several seconds for everything to startup again, then check to be sure you're still connected to WiFi."
#define L_DOWN_MESSAGE2 "If this is a persistant problem here are other possible causes:<br /><ul>"
#define L_DOWN_MESSAGE3 "Baud rate mismatch due to reboot of only OnStep, power everything off/on again."
#define L_DOWN_MESSAGE4 "Incorrectly configured SWS baud rate."
#define L_DOWN_MESSAGE5 "Incorrectly configured OnStep baud rate."
#define L_DOWN_MESSAGE6 "Incorrect wiring of the com ports."
#define L_DOWN_MESSAGE7 "Incorrect wiring of power supply pins.  Gnd must be shared between OnStep's MCU and the Addon's MCU."

#define L_CONTROLLER "Controller" // was Status
#define L_PAGE_MOUNT "Mount" // was Control
#define L_PAGE_AUXILIARY "Auxiliary"
#define L_PAGE_LIBRARY "Library"
#define L_PAGE_ENCODERS "Encoders"
#define L_PAGE_PEC "PEC"
#define L_SETTINGS "Settings"
#define L_CONTROLS "Controls"
#define L_PAGE_CONFIG "Configuration"
#define L_PAGE_NETWORK "Network"

// ------------------------- MountStatus -------------------------

// general (background) errors
#define L_GE_NONE "None"
#define L_GE_MOTOR_FAULT "Motor/driver fault"
#define L_GE_ALT_MIN "Below horizon limit" 
#define L_GE_LIMIT_SENSE "Limit sense"
#define L_GE_DEC "Dec limit exceeded"
#define L_GE_AZM "Azm limit exceeded"
#define L_GE_UNDER_POLE "Under pole limit exceeded"
#define L_GE_MERIDIAN "Meridian limit exceeded"
#define L_GE_SYNC "Sync safety limit exceeded"
#define L_GE_PARK "Park failed"
#define L_GE_GOTO_SYNC "Goto sync failed"
#define L_GE_UNSPECIFIED "Unknown error"
#define L_GE_ALT_MAX "Above overhead limit"
#define L_GE_WEATHER_INIT "Weather sensor init failed"
#define L_GE_SITE_INIT "Time or loc. not updated"
#define L_GE_NV_INIT "Init NV/EEPROM error"
#define L_GE_OTHER "Unknown Error, code"

// command errors
#define L_CE_NONE "No Errors"
#define L_CE_0 "Reply 0"
#define L_CE_CMD_UNKNOWN "command unknown"
#define L_CE_REPLY_UNKNOWN "invalid reply"
#define L_CE_PARAM_RANGE "parameter out of range"
#define L_CE_PARAM_FORM "bad parameter format"
#define L_CE_ALIGN_FAIL "align failed"
#define L_CE_ALIGN_NOT_ACTIVE "align not active"
#define L_CE_NOT_PARKED_OR_AT_HOME "not parked or at home"
#define L_CE_PARKED "already parked"
#define L_CE_PARK_FAILED "park failed"
#define L_CE_NOT_PARKED "not parked"
#define L_CE_NO_PARK_POSITION_SET "no park position set"
#define L_CE_GOTO_FAIL "goto failed"
#define L_CE_LIBRARY_FULL "library full"
#define L_CE_GOTO_ERR_BELOW_HORIZON "goto below horizon"
#define L_CE_GOTO_ERR_ABOVE_OVERHEAD "goto above overhead"
#define L_CE_SLEW_ERR_IN_STANDBY "slew in standby"
#define L_CE_SLEW_ERR_IN_PARK "slew in park"
#define L_CE_GOTO_ERR_GOTO "already in goto"
#define L_CE_GOTO_ERR_OUTSIDE_LIMITS "goto outside limits"
#define L_CE_SLEW_ERR_HARDWARE_FAULT "hardware fault"
#define L_CE_MOUNT_IN_MOTION "mount in motion"
#define L_CE_GOTO_ERR_UNSPECIFIED "other"
#define L_CE_UNK "unknown"
