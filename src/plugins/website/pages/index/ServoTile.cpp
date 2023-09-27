// -----------------------------------------------------------------------------------
// Controller Status tile
#include "ServoTile.h"

#if DISPLAY_SERVO_MONITOR == ON

#include "../KeyValue.h"
#include "../Pages.common.h"

int _servo_axis = 0;

// create the related webpage tile
void servoTile(String &data)
{
  char temp[800] = "";

  // javascript to keep servo canvas updated
  strcpy_P(temp, html_servoScript1);
  data.concat(temp);
  www.sendContentAndClear(data);

  strcpy_P(temp, html_servoScript2);
  data.concat(temp);
  www.sendContentAndClear(data);

  strcpy_P(temp, html_servoScript3);
  data.concat(temp);
  www.sendContentAndClear(data);

  // servo monitor tile start
  sprintf_P(temp, html_tile_beg, "28em", "15em", "Servo <span class='c'>Axis</span><span id='svoA' class='c'>?</span> Monitor");
  data.concat(temp);
  data.concat(F("<div style='float: right; text-align: right;'>"));
  data.concat(F("Delta <span id='svoD' class='c'>?</span>, "));
  data.concat(F("Pwr <span id='svoP' class='c'>?</span>%"));
  data.concat(F("</div><br /><hr>"));

  // allow selecting servo axis
  data.concat(F("<div>"));
  sprintf_P(temp, html_servoSelect, 0, 0, 'x');
  data.concat(temp);
  for (int i=1; i<9; i++) {
    char command[10];
    sprintf(command, ":GXS%d#", i);
    if (onStep.command(command, temp)) {
      sprintf_P(temp, html_servoSelect, i, i, '0'+ i); data.concat(temp);
    }
  }
  data.concat(F("</div>"));

  // the servo canvas
  strcpy_P(temp, html_servoGraph);
  data.concat(temp);

  #if DISPLAY_SERVO_ORIGIN_CONTROLS == ON
    sprintf_P(temp, html_servoZeroEncoders);
    data.concat(temp);
  #endif

  // servo monitor tile end
  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);
}

// use Ajax key/value pairs to pass related data to the web client in the background
void servoTileAjax(String &data)
{
  char temp[120] = "", command[10];

  if (_servo_axis == 0) strcpy(temp, "svoA|?\n"); else sprintf(temp, "svoA|%d\n", _servo_axis); data.concat(temp);

  sprintf(command, ":GXS%d#", _servo_axis);
  if (_servo_axis >= 1 && _servo_axis <= 9 && onStep.command(command, temp)) {
    char *temp1 = strchr(temp,',');
    if (temp1 != NULL) {
      temp1[0] = 0;
      temp1++;
      data.concat(F("svoD|")); data.concat(temp); data.concat("\n");
      data.concat(F("svoP|")); data.concat(temp1); data.concat("\n");
    } else { data.concat(F("svoD|?\n")); data.concat(F("svoP|?\n")); }
  } else { data.concat(F("svoD|?\n")); data.concat(F("svoP|?\n")); _servo_axis = 0; }

  data.concat(keyValueBoolEnabled("svax1", _servo_axis == 0));
  data.concat(keyValueBoolEnabled("svax2", _servo_axis == 0));
  data.concat(keyValueBoolEnabled("svax3", _servo_axis == 0));
  data.concat(keyValueBoolEnabled("svax4", _servo_axis == 0));
  data.concat(keyValueBoolEnabled("svax5", _servo_axis == 0));
  data.concat(keyValueBoolEnabled("svax6", _servo_axis == 0));
  data.concat(keyValueBoolEnabled("svax7", _servo_axis == 0));
  data.concat(keyValueBoolEnabled("svax8", _servo_axis == 0));
  data.concat(keyValueBoolEnabled("svax9", _servo_axis == 0));

  www.sendContentAndClear(data);
}

// pass related data back to OnStep
void servoTileGet()
{
  String v;

  // set chart to display the axis requested
  v = www.arg("svax");
  if (!v.equals(EmptyStr)) {
    int axis = v.toInt();
    if (axis >= 0 && axis <= 9) _servo_axis = axis;
  }

  // trigger encoder bridge to set zero
  v = www.arg("sv");
  if (v.equals("zro")) { onStep.commandBool(":SEO#"); }
}

#endif
