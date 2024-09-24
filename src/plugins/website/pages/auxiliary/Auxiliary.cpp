// -----------------------------------------------------------------------------------
// Telescope auxiliary feature related functions 
#include "Auxiliary.h"

#include "../Page.h"
#include "../Pages.common.h"

void processAuxGet();

void handleAux() {
  char temp[240] = "";
  char temp1[80] = "";

  state.updateAuxiliary(false, true);

  SERIAL_ONSTEP.setTimeout(webTimeout);
  onStep.serialRecvFlush();

  processAuxGet();

  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/html", String());

  String data = FPSTR(html_head_begin);
  data.concat(FPSTR(html_main_css_begin));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_main_css_core));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_main_css_control));
  data.concat(FPSTR(html_main_css_buttons));
  data.concat(FPSTR(html_main_css_end));
  data.concat(FPSTR(html_head_end));
  www.sendContentAndClear(data);

  // show this page
  data.concat(FPSTR(html_body_begin));
  www.sendContentAndClear(data);
  pageHeader(PAGE_AUXILIARY);
  data.concat(FPSTR(html_onstep_page_begin));

  // OnStep wasn't found, show warning and info.
  if (!status.onStepFound) {
    data.concat(FPSTR(html_bad_comms_message));
    data.concat(FPSTR(html_page_and_body_end));
    www.sendContentAndClear(data);
    www.sendContent("");
    return;
  }

  // scripts
  sprintf_P(temp, html_script_ajax_get, "auxiliary-ajax-get.txt");
  data.concat(temp);
  data.concat(FPSTR(html_script_ajax_shortcuts));

  // active ajax page is: auxAjax();
  data.concat(F("<script>var ajaxPage='auxiliary-ajax.txt';</script>\n"));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_script_ajax));
  www.sendContentAndClear(data);

  // Auxiliary Features --------------------------------------
  int j = 0;
  if (status.auxiliaryFound == SD_TRUE) {

    for (int i = 0; i < 8; i++) {
      state.selectFeature(i);

      if (state.featurePurpose() <= 0) continue;

      char title[40];
      strcpy(title, state.featureName());
      strcat(title, " ");
      switch (state.featurePurpose()) {
        case SWITCH: strcat(title, "Switch"); break;
        case ANALOG_OUTPUT: strcat(title, "Analog Out"); break;
        case DEW_HEATER: strcat(title, "Dew Heater"); break;
        case INTERVALOMETER: strcat(title, "Intervalometer"); break;
        default: strcat(title, "Unknown");
      }
      sprintf_P(temp, html_tile_beg, "27em", "8em", title); data.concat(temp);

      data.concat(F("<div style='float: right; text-align: right;' class='c'>"));
      #if defined(V_SENSE_PINS) && defined(V_SENSE_FORMULA)
        int voltageSensePin[8] = V_SENSE_PINS;
        if (voltageSensePin[i] >= 0) {
          sprintf(temp,"<span id='vout%d'>?</span>V", i + 1);
          data.concat(temp);
        }
        #if defined(I_SENSE_PINS) && defined(I_SENSE_FORMULA)
          data.concat(" @ ");
        #endif
      #endif
      #if defined(I_SENSE_PINS) && defined(I_SENSE_FORMULA)
        int currentSensePin[8] = I_SENSE_PINS;
        if (currentSensePin[i] >= 0) {
          sprintf(temp,"<span id='iout%d'>?</span>A", i + 1);
          data.concat(temp);
        }
      #endif
      data.concat(F("</div><br /><hr>"));

      if (state.featurePurpose() == SWITCH) {
        data.concat(F("<div style='float: left; width: 8em; height: 2em; line-height: 2em'>"));

        data.concat(F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>"));
        sprintf_P(temp, html_auxOnSwitch, i + 1, i + 1); data.concat(temp);
        sprintf_P(temp, html_auxOffSwitch, i + 1, i + 1); data.concat(temp);
        data.concat(F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>"));

        data.concat(F("</div>\n"));
        www.sendContentAndClear(data);
        j++;
      } else
      if (state.featurePurpose() == ANALOG_OUTPUT) {
        data.concat(F("<div style='float: left; width: 8em; height: 2em; line-height: 2em'>"));
        data.concat(F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>"));
        data.concat(FPSTR(html_auxAnalog));
        sprintf(temp, "%d' onchange=\"sz('x%dv1',this.value)\">", state.featureValue1(), i + 1);
        data.concat(temp);
        data.concat(F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>"));
        sprintf(temp,"<span id='x%dv1'>%d</span>%%", i + 1, (int)lround((state.featureValue1()/255.0)*100.0));
        data.concat(temp);
        data.concat(F("</div>\n"));
        www.sendContentAndClear(data);
        j++;
      } else
      if (state.featurePurpose() == DEW_HEATER) {
        data.concat(F("<div style='float: left; width: 8em; height: 2em; line-height: 2em'>"));
        data.concat(F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>"));
        sprintf_P(temp, html_auxOnSwitch, i + 1, i + 1); data.concat(temp);
        sprintf_P(temp, html_auxOffSwitch, i + 1, i + 1); data.concat(temp);
        data.concat(F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>"));
        dtostrf(celsiusToNativeRelative(state.featureValue4()), 3, 1, temp1);
        sprintf(temp,"&Delta;<span id='x%dv4'>%s</span>&deg;" TEMPERATURE_UNITS_ABV "\n", i + 1, temp1);
        data.concat(temp);
        data.concat(F("</div>\n"));

        data.concat(F("<div style='float: left; text-align: right; width: 8em; height: 2em; line-height: 2em'>"));
        data.concat(L_ZERO " (100% " L_POWER ")");
        data.concat(F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>"));
        data.concat(FPSTR(html_auxHeater));
        sprintf(temp,"%d' onchange=\"sz('x%dv2',this.value)\">", (int)lround(celsiusToNativeRelative(state.featureValue2())*10.0F), i + 1);
        data.concat(temp);
        data.concat(F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>"));
        dtostrf(celsiusToNativeRelative(state.featureValue2()), 3, 1, temp1);
        sprintf(temp,"<span id='x%dv2'>%s</span>&deg;" TEMPERATURE_UNITS_ABV "\n", i + 1, temp1);
        data.concat(temp);
        data.concat(F("</div>\n"));
        
        data.concat(F("<div style='float: left; text-align: right; width: 8em; height: 2em; line-height: 2em'>"));
        data.concat(L_SPAN " (0% " L_POWER ")");
        data.concat(F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>"));
        data.concat(FPSTR(html_auxHeater));
        sprintf(temp,"%d' onchange=\"sz('x%dv3',this.value)\">", (int)lround(celsiusToNativeRelative(state.featureValue3())*10.0), i + 1);
        data.concat(temp);
        data.concat(F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>"));
        dtostrf(celsiusToNativeRelative(state.featureValue3()), 3, 1, temp1);
        sprintf(temp,"<span id='x%dv3'>%s</span>&deg;" TEMPERATURE_UNITS_ABV "\n", i + 1, temp1);
        data.concat(temp);
        data.concat(F("</div>\n"));

        www.sendContentAndClear(data);
        j++;
      } else
      if (state.featurePurpose() == INTERVALOMETER) {
        data.concat(F("<div style='float: left; width: 8em; height: 2em; line-height: 2em'>"));
        data.concat(F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>"));
        data.concat(FPSTR(html_auxStartStop1));
        sprintf(temp,"x%dv1",i+1);
        data.concat(temp);
        data.concat(FPSTR(html_auxStartStop2));
        sprintf(temp,"x%dv1",i+1);
        data.concat(temp);
        data.concat(FPSTR(html_auxStartStop3));
        data.concat(F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>"));
        sprintf(temp,"<span id='x%dv1'>-</span>\n",i+1);
        data.concat(temp);
        data.concat(F("</div>\n"));

        data.concat(F("<div style='float: left; text-align: right; width: 8em; height: 2em; line-height: 2em'>"));
        data.concat(L_CAMERA_COUNT);
        data.concat(F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>"));
        data.concat(FPSTR(html_auxCount));
        sprintf(temp,"%d' onchange=\"sz('x%dv4',this.value)\">",(int)state.featureValue4(),i+1);
        data.concat(temp);
        data.concat(F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>"));
        dtostrf(state.featureValue4(),0,0,temp1);
        sprintf(temp,"<span id='x%dv4'>%s</span> x\n",i+1,temp1);
        data.concat(temp);
        data.concat(F("</div>\n"));

        data.concat(F("<div style='float: left; text-align: right; width: 8em; height: 2em; line-height: 2em'>"));
        data.concat(L_CAMERA_EXPOSURE);
        data.concat(F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>"));
        data.concat(FPSTR(html_auxExposure));
        sprintf(temp,"%d' onchange=\"sz('x%dv2',this.value)\">",(int)timeToByte(state.featureValue2()),i+1);
        data.concat(temp);
        data.concat(F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>"));
        float v; int d;
        v=state.featureValue2(); if (v < 1.0) d=3; else if (v < 10.0) d=2; else if (v < 30.0) d=1; else d=0;
        dtostrf(v,0,d,temp1);
        sprintf(temp,"<span id='x%dv2'>%s</span> sec\n",i+1,temp1);
        data.concat(temp);
        data.concat(F("</div>\n"));

        data.concat(F("<div style='float: left; text-align: right; width: 8em; height: 2em; line-height: 2em'>"));
        data.concat(L_CAMERA_DELAY);
        data.concat(F("</div><div style='float: left; width: 14em; height: 2em; line-height: 2em'>"));
        data.concat(FPSTR(html_auxDelay));
        sprintf(temp,"%d' onchange=\"sz('x%dv3',this.value)\">",(int)timeToByte(state.featureValue3()),i+1);
        data.concat(temp);
        data.concat(F("</div><div style='float: left; width: 4em; height: 2em; line-height: 2em'>"));
        v=state.featureValue3(); if (v < 10.0) d=2; else if (v < 30.0) d=1; else d=0;
        dtostrf(v,0,d,temp1);
        sprintf(temp,"<span id='x%dv3'>%s</span> sec\n",i+1,temp1);
        data.concat(temp);
        data.concat(F("</div>\n"));

        www.sendContentAndClear(data);
        j++;
      }
      data.concat(FPSTR(html_auxAuxE));
    }
    data.concat(F("<br class='clear' />"));
  }

  data.concat(FPSTR(html_auxEnd));
  
  data.concat(FPSTR(html_page_and_body_end));
  www.sendContentAndClear(data);

  www.sendContent("");
}

void auxAjaxGet() {
  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/plain", String());

  processAuxGet();

  www.sendContent("");
}

#if defined(V_SENSE_PINS) && defined(V_SENSE_FORMULA)
  extern float featureVoltage[8];
#endif
#if defined(I_SENSE_PINS) && defined(I_SENSE_FORMULA)
  extern float featureCurrent[8];
#endif

void auxAjax() {
  String data="";
  char temp[120]="";

  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/plain", String());

  // update auxiliary feature values
  if (status.auxiliaryFound == SD_TRUE) {

    for (int i = 0; i < 8; i++) {
      state.selectFeature(i);

      #if defined(V_SENSE_PINS) && defined(V_SENSE_FORMULA)
        if (!isnan(featureVoltage[i])) {
          if (state.featureValue1()) {
            sprintf(temp, "vout%d|", i + 1);
            data.concat(temp);
            sprintF(temp, "%3.1f\n", featureVoltage[i]);
            data.concat(temp);
          } else {
            sprintf(temp, "vout%d|0.0\n", i + 1);
            data.concat(temp);
          }
        }
      #endif

      #if defined(I_SENSE_PINS) && defined(I_SENSE_FORMULA)
        if (!isnan(featureCurrent[i])) {
          if (fabs(featureCurrent[i]) > 0.2499F) {
            sprintf(temp, "iout%d|", i + 1);
            data.concat(temp);
            sprintF(temp, "%3.1f\n", featureCurrent[i]);
            data.concat(temp);
          } else {
            sprintf(temp, "iout%d|0.0\n", i + 1);
            data.concat(temp);
          }
        }
      #endif

      if (state.featurePurpose() == SWITCH) {
        if (state.featureValue1() != 0) {
          sprintf(temp,"sw%d_on|%s\n",i+1,"selected"); data.concat(temp);
          sprintf(temp,"sw%d_off|%s\n",i+1,"unselected"); data.concat(temp);
        } else {
          sprintf(temp,"sw%d_on|%s\n",i+1,"unselected"); data.concat(temp);
          sprintf(temp,"sw%d_off|%s\n",i+1,"selected"); data.concat(temp);
        }
      } else

      if (state.featurePurpose() == ANALOG_OUTPUT) {
        sprintf(temp,"x%dv1|%d\n",i+1,(int)lround((state.featureValue1()/255.0)*100.0)); data.concat(temp);
      } else

      if (state.featurePurpose() == DEW_HEATER) {
        char s[40];
        if (state.featureValue1() != 0) {
          sprintf(temp,"sw%d_on|%s\n",i+1,"selected"); data.concat(temp);
          sprintf(temp,"sw%d_off|%s\n",i+1,"unselected"); data.concat(temp);
        } else {
          sprintf(temp,"sw%d_on|%s\n",i+1,"unselected"); data.concat(temp);
          sprintf(temp,"sw%d_off|%s\n",i+1,"selected"); data.concat(temp);
        }
        dtostrf(celsiusToNativeRelative(state.featureValue2()),3,1,s); sprintf(temp,"x%dv2|%s\n",i+1,s); data.concat(temp);
        dtostrf(celsiusToNativeRelative(state.featureValue3()),3,1,s); sprintf(temp,"x%dv3|%s\n",i+1,s); data.concat(temp);
        dtostrf(celsiusToNativeRelative(state.featureValue4()),3,1,s); sprintf(temp,"x%dv4|%s\n",i+1,s); data.concat(temp);
      } else

      if (state.featurePurpose() == INTERVALOMETER) {
        char s[40];
        float v; int d;
        
        v=state.featureValue1();
        if (fabs(v) < 0.001) sprintf(temp,"x%dv1|-\n",i+1); else sprintf(temp,"x%dv1|%d\n",i+1,(int)v); data.concat(temp);
        v=state.featureValue2(); if (v < 1.0) d=3; else if (v < 10.0) d=2; else if (v < 30.0) d=1; else d=0;
        dtostrf(v,0,d,s); sprintf(temp,"x%dv2|%s\n",i+1,s); data.concat(temp);
        v=state.featureValue3(); if (v < 10.0) d=2; else if (v < 30.0) d=1; else d=0;
        dtostrf(v,0,d,s); sprintf(temp,"x%dv3|%s\n",i+1,s); data.concat(temp);
        sprintf(temp,"x%dv4|%d\n",i+1,(int)state.featureValue4()); data.concat(temp);
      }
    }
  }

  www.sendContentAndClear(data);
  www.sendContent("");

  state.lastAuxPageLoadTime = millis();
}

void processAuxGet() {
  String v;
  char temp[80] = "";
  char temp1[40] = "";

  // Auxiliary Feature set Value1 to Value4
  for (char c = '1'; c <= '8'; c++) {
    state.selectFeature(c - '1');

    sprintf(temp, "x%cv1", c);
    v = www.arg(temp);
    if (!v.equals(EmptyStr)) {
      sprintf(temp, ":SXX%c,V%s#", c, v.c_str());
      onStep.commandBool(temp);
    }

    if (state.featurePurpose() == DEW_HEATER) {
      sprintf(temp, "x%cv2", c);
      v = www.arg(temp);
      if (!v.equals(EmptyStr)) {
        dtostrf(nativeToCelsiusRelative(v.toFloat()/10.0), 0, 1, temp1);
        sprintf(temp, ":SXX%c,Z%s#", c, temp1);
        onStep.commandBool(temp);
      }
      sprintf(temp, "x%cv3", c);
      v = www.arg(temp);
      if (!v.equals(EmptyStr)) {
        dtostrf(nativeToCelsiusRelative(v.toFloat()/10.0), 0, 1, temp1);
        sprintf(temp, ":SXX%c,S%s#", c, temp1);
        onStep.commandBool(temp);
      }
    } else

    if (state.featurePurpose() == INTERVALOMETER) {
      sprintf(temp, "x%cv2", c);
      v = www.arg(temp);
      if (!v.equals(EmptyStr)) {
        dtostrf(byteToTime(v.toInt()), 0, 3, temp1);
        sprintf(temp, ":SXX%c,E%s#", c, temp1);
        onStep.commandBool(temp);
      }
      sprintf(temp, "x%cv3", c); v = www.arg(temp);
      if (!v.equals(EmptyStr)) {
        dtostrf(byteToTime(v.toInt()), 0, 2, temp1);
        sprintf(temp, ":SXX%c,D%s#", c, temp1);
        onStep.commandBool(temp);
      }
      sprintf(temp, "x%cv4", c); v = www.arg(temp);
      if (!v.equals(EmptyStr)) {
        dtostrf(v.toFloat(), 0, 0, temp1);
        sprintf(temp, ":SXX%c,C%s#", c, temp1);
        onStep.commandBool(temp);
      }
    }
  }

  state.lastAuxPageLoadTime = millis();
}
