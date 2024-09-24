// -----------------------------------------------------------------------------------
// Servo Calibrate tile
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

#if DISPLAY_SERVO_CALIBRATION == ON

extern void servoCalibrateTile(String &data);
extern void servoCalibrateTileAjax(String &data);
extern void servoCalibrateTileGet();

const char html_servoCalibrateSection[] PROGMEM = 
"This is for equatorial telescope mounts where the servo stepper motor based RA axis drive system accuracy is presumed to "
"nominally exceed the accuracy of the RA axis encoder w/repeatability sufficient for performance improvement.<br/><br/>";

// tracking control to examine results
const char html_servoCalibrateTrackingSection[] PROGMEM = 
"Fixed rate (step/dir) tracking:<br/>";
const char html_servoCalibrateNormal[] PROGMEM = "<button onpointerdown=\"s('svc','trkN')\" type='button'>Track Normally</button> ";
const char html_servoCalibrateFixed[] PROGMEM = "<button onpointerdown=\"s('svc','trkF')\" type='button'>Track Fixed Rate</button><br/><br/>";

// start or stop recording
const char html_servoCalibrateRecordSection[] PROGMEM = 
"Calibration recording:<br/>";
const char html_servoCalibrateStart[] PROGMEM = "<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('svc','rec')\" type='button'>Record</button> ";
const char html_servoCalibrateStop[] PROGMEM = "<button onpointerdown=\"s('svc','stop')\" type='button'>Stop</button><br/><br/>";

// data handling
const char html_servoCalibrateDataSection[] PROGMEM = 
"Calibration buffer:<br/>";
const char html_servoCalibrateClear[] PROGMEM = "<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('svc','bufClear')\" type='button'>Clear</button>&nbsp;&nbsp;&nbsp;";
const char html_servoCalibrateLoad[] PROGMEM = "<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('svc','bufLoad')\" type='button'>Load</button> ";
const char html_servoCalibrateSave[] PROGMEM = "<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('svc','bufSave')\" type='button'>Save</button><br/><br/>";

const char html_servoCalibrateLoadBackup[] PROGMEM = "<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('svc','bufLoadBak')\" type='button'>Load Backup</button> ";
const char html_servoCalibrateSaveBackup[] PROGMEM = "<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('svc','bufSaveBak')\" type='button'>Save Backup</button><br/><br/>";

const char html_servoCalibrateHighPass[] PROGMEM = "<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('svc','bufHighPass')\" type='button'>High Pass Filter</button> ";
const char html_servoCalibrateLowPass[] PROGMEM = "<button onpointerdown=\"if (confirm('" L_ARE_YOU_SURE "?')) s('svc','bufLowPass')\" type='button'>Low Pass Filter</button><br/>";

#endif
