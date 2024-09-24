// -----------------------------------------------------------------------------------
// The home page, status information
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

#include "AmbientTile.h"
#include "StatusTile.h"
#include "AxisTile.h"
#if DISPLAY_SERVO_MONITOR == ON
  #include "ServoTile.h"
#endif
#if DISPLAY_SERVO_CALIBRATION == ON
  #include "ServoCalibrateTile.h"
#endif

#if DRIVE_CONFIGURATION == ON
  const char html_configAxesNotes[] PROGMEM =
  "<br />Notes:<ul>"
  "<li>" L_ADV_SET_FOOTER_MSG0 "</li>"
  "<li>" L_ADV_SET_FOOTER_MSG1 "</li>"
  "<li>" L_ADV_SET_FOOTER_MSG2 "</li>";

  const char html_configAxesNotesOnStep[] PROGMEM =
  #if DRIVE_MAIN_AXES_CURRENT == ON
	"<li>" L_ADV_SET_FOOTER_MSG3 "</li>"
  #endif
  "<li>" L_ADV_SET_FOOTER_MSG4 "</li>"
  "</ul>";
#endif
