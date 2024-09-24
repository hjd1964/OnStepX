// -----------------------------------------------------------------------------------
// Servo Calibrate tile
#include "ServoCalibrateTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

#if DISPLAY_SERVO_CALIBRATION == ON

// create the related webpage tile
void servoCalibrateTile(String &data)
{
  char temp[240] = "";

  sprintf_P(temp, html_tile_text_beg, "28em", "15em", "Servo Calibration");
  data.concat(temp);
  data.concat(F("<br /><hr>"));

  data.concat(html_servoCalibrateSection);

  // tracking control to examine results
  data.concat(html_servoCalibrateTrackingSection);
  data.concat(html_servoCalibrateNormal);
  data.concat(html_servoCalibrateFixed);
  
  // start or stop recording
  data.concat(html_servoCalibrateRecordSection);
  data.concat(html_servoCalibrateStart);
  data.concat(html_servoCalibrateStop);

  www.sendContentAndClear(data);

  // data handling
  data.concat(html_servoCalibrateDataSection);
  data.concat(html_servoCalibrateClear);
  
  data.concat(html_servoCalibrateLoad);
  data.concat(html_servoCalibrateSave);
  data.concat(html_servoCalibrateLoadBackup);
  data.concat(html_servoCalibrateSaveBackup);

  data.concat(html_servoCalibrateHighPass);
  data.concat(html_servoCalibrateLowPass);

  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);
}

// use Ajax key/value pairs to pass related data to the web client in the background
void servoCalibrateTileAjax(String &data)
{
  data.concat(F("tphd_t|")); data.concat(state.siteTemperatureStr); data.concat("\n");
  data.concat(F("tphd_p|")); data.concat(state.sitePressureStr); data.concat("\n");
  data.concat(F("tphd_h|")); data.concat(state.siteHumidityStr); data.concat("\n");
  data.concat(F("tphd_d|")); data.concat(state.siteDewPointStr); data.concat("\n");

  www.sendContentAndClear(data);
}

// pass related data back to OnStep
extern void servoCalibrateTileGet()
{
  
  String get = www.arg("svc");
  if (!get.equals(EmptyStr))
  {
    if (get.equals("trkN")) { onStep.commandBool(":SX4E,T#"); }
    if (get.equals("trkF")) { onStep.commandBool(":SX4E,F#"); }
    if (get.equals("rec")) { onStep.commandBool(":SX4E,R#"); }
    if (get.equals("stop")) { onStep.commandBool(":SX4E,W#"); }
    if (get.equals("bufClear")) { onStep.commandBool(":SX4E,!#"); }
    if (get.equals("bufLoad")) { onStep.commandBool(":SX4E,L#"); }
    if (get.equals("bufSave")) { onStep.commandBool(":SX4E,S#"); }
    if (get.equals("bufLoadBak")) { onStep.commandBool(":SX4E,V#"); }
    if (get.equals("bufSaveBak")) { onStep.commandBool(":SX4E,B#"); }
    if (get.equals("bufHighPass")) { onStep.commandBool(":SX4E,H#"); }
    if (get.equals("bufLowPass")) { onStep.commandBool(":SX4E,A#"); }
  }
}

#endif
