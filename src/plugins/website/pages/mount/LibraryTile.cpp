// -----------------------------------------------------------------------------------
// Library tile
#include "LibraryTile.h"

#include "../KeyValue.h"
#include "../Pages.common.h"

int currentCatalog = 0;
char currentCatName[12] = "";
String currentObject = L_CAT_NO_OBJECT;
bool catalogIndexChanged = false;
bool downloadCatalogData = false;
bool uploadCatalogData = false;
String showMessage = L_CAT_NO_CAT;
unsigned long currentCatNameShowTime = 0;

// create the related webpage tile
void libraryTile(String &data)
{
  char temp[400] = "";

  // reset everything on page load
  currentCatalog = 0;
  currentCatName[0] = 0;
  currentObject = L_CAT_NO_OBJECT;
  catalogIndexChanged = false;
  downloadCatalogData = false;
  uploadCatalogData = false;
  showMessage = L_CAT_NO_CAT;

  // Scripts
  data.concat(FPSTR(html_script_ajax_library));

  sprintf_P(temp, html_tile_beg, "22em", "15em", L_PAGE_LIBRARY);
  data.concat(temp);
  data.concat(F("<div style='float: right; text-align: right;' id='lib_free' class='c'>"));
  data.concat(onStep.commandString(":L?#"));
  data.concat(" ");
  data.concat(L_RECS_AVAIL);
  data.concat(F("</div><br />"));
  data.concat(F("<hr>"));

  data.concat(FPSTR(html_libCatalogSelect));
  www.sendContentAndClear(data);
  data.concat(F("<br />"));
  data.concat(FPSTR(html_libShowMessage));
  www.sendContentAndClear(data);

  data.concat(FPSTR(html_catPrevRec));
  data.concat(FPSTR(html_catNextRec));
  data.concat(FPSTR(html_catSetTarget));
  data.concat(FPSTR(html_catShowMessage));
  www.sendContentAndClear(data);

  data.concat(F("<hr>"));

  sprintf_P(temp, html_collapsable_beg, L_CONTROLS "...");
  data.concat(temp);

  data.concat(FPSTR(html_libUploadCatalog));
  data.concat(FPSTR(html_libDownloadCatalog));
  data.concat(FPSTR(html_libClearCatalog));
  data.concat(F("<br /><br />"));
  www.sendContentAndClear(data);
  data.concat(FPSTR(html_libEditCatalog));
  data.concat(FPSTR(html_libCatalogHelp));

  data.concat(FPSTR(html_collapsable_end));
  data.concat(FPSTR(html_tile_end));
  www.sendContentAndClear(data);
}

// use Ajax key/value pairs to pass related data to the web client in the background
void libraryTileAjax(String &data)
{
  data.concat(F("lib_free|"));
  data.concat(onStep.commandString(":L?#"));
  data.concat(" ");
  data.concat(L_RECS_AVAIL);
  data += "\n";

  data.concat("cat_message|" + currentObject + "\n");
  data.concat(F("cat_name|")); data.concat(currentCatName); data.concat("\n");

  if (currentCatalog > 0) {
    data.concat(F("cat_prev|enabled\n"));
    data.concat(F("cat_next|enabled\n"));
    data.concat(F("cat_select|enabled\n"));
  } else {
    data.concat(F("cat_prev|disabled\n"));
    data.concat(F("cat_next|disabled\n"));
    data.concat(F("cat_select|disabled\n"));
  }
  if (currentObject.equals(">") || currentObject.equals("-")) {
    data.concat(F("cat_prev|disabled\n"));
    data.concat(F("cat_select|disabled\n"));
  }
  if (currentObject.equals("<")) {
    data.concat(F("cat_next|disabled\n"));
    data.concat(F("cat_select|disabled\n"));
  }

  if (currentCatName[0] != 0) {
    if (!showMessage.equals(currentCatName))
    {
      if (currentCatNameShowTime == 0) currentCatNameShowTime = millis() + 6000;
    } else
      currentCatNameShowTime = 0;

    if ((long)(millis() - currentCatNameShowTime) > 0)
    {
      showMessage = L_CATALOG " ";
      showMessage += currentCatName;
      showMessage += " " L_SELECTED ".";
    }
  }

  if (showMessage != "")
  {
    data.concat("lib_message|" + showMessage + "\n");
    showMessage = "";
  }

  if (catalogIndexChanged)
  {
    if (currentCatalog == 0) data.concat(F("lib_clear|" L_CAT_CLEAR_LIB "\n")); else data.concat(F("lib_clear|" L_CAT_CLEAR "\n"));
    data.concat(F("cat_upload|disabled\n"));
    strcpy(currentCatName, "");
    if (currentCatalog != 0)
    {
      if (onStep.commandBool(":L$#"))
      {
        char temp[80] = "";
        onStep.command(":LR#", temp);
        if (temp[0] == '$')
        {
          for (int i = 1; i < 10; i++)
          {
            currentCatName[i - 1] = 0;
            currentCatName[i] = 0;
            if (temp[i] == ',' || temp[i] == 0 || temp[i] == '#') break;
            currentCatName[i - 1] = temp[i];
          }
          if (currentCatName[0] == 0)
          {
            showMessage = F(L_CAT_NO_NAME);
          }
          else
          {
            data.concat(F("cat_download|enabled\n"));
            showMessage = L_CATALOG " ";
            showMessage += currentCatName;
            showMessage += " " L_SELECTED ".";
          }
        }
        else
        {
          data.concat(F("cat_download|disabled\n"));
          showMessage = F(L_CAT_NO_NAME);
        }
      }
      else
      {
        data.concat(F("cat_download|disabled\n"));
        showMessage = F(L_CAT_GET_NAME_FAIL);
      }
    }
    else
    {
      data.concat(F("cat_download|disabled\n"));
      showMessage = F(L_CAT_NO_CAT);
    }
    data.concat(F("cat_data&\n"));
    catalogIndexChanged = false;
  }
  else if (downloadCatalogData)
  {
    if (currentCatalog != 0)
    {
      data.concat(F("cat_data&"));
      bool success = false;
      char temp[80] = "";
      sprintf(temp, ":Lo%ld#", (long)currentCatalog - 1);
      if (onStep.commandBool(temp))
      {
        data.concat("$");
        data.concat(currentCatName);
        data.concat("\r");

        while (true)
        {
          onStep.command(":LR#", temp);
          if (temp[0] == ',')
          {
            success = true;
            break;
          }
          if (temp[0] == 0) break;

          // isolate the individual fields (and error check)
          int i = 0;
          String r = temp;
          String nameS = "";
          String catS = "";
          String raS = "";
          String decS = "";

          r = r + ",";
          i = r.indexOf(",");
          if (i >= 0)
          {
            nameS = r.substring(0, i);
            r = r.substring(i + 1);
          }
          else
          {
            success = false;
            break;
          }
          i = r.indexOf(",");
          if (i >= 0)
          {
            catS = r.substring(0, i);
            r = r.substring(i + 1);
          }
          else
          {
            success = false;
            break;
          }
          i = r.indexOf(",");
          if (i >= 0)
          {
            raS = r.substring(0, i);
            r = r.substring(i + 1);
          }
          else
          {
            success = false;
            break;
          }
          i = r.indexOf(",");
          if (i >= 0)
          {
            decS = r.substring(0, i);
            r = r.substring(i + 1);
          }
          else
          {
            success = false;
            break;
          }

          // pad to the correct length (and error check)
          i = 11 - nameS.length();
          if (i >= 0)
          {
            for (int j = 0; j < i; j++)
              nameS = nameS + " ";
          }
          else
          {
            success = false;
            break;
          }
          i = 3 - catS.length();
          if (i >= 0)
          {
            for (int j = 0; j < i; j++)
              catS = catS + " ";
          }
          else
          {
            success = false;
            break;
          }

          r = nameS + "," + catS + "," + raS + "," + decS;

          data.concat(r);
          data.concat("\r");
        }
        if (!success)
          showMessage = F(L_CAT_DOWNLOAD_FAIL);
        else
          showMessage = F(L_CAT_DOWNLOAD_SUCCESS);
      }
      else
        showMessage = F(L_CAT_DOWNLOAD_INDEX_FAIL);
    }
    else
      showMessage = F(L_CAT_DOWNLOAD_INDEX_FAIL);
    data.concat("\n");

    downloadCatalogData = false;
  }

  www.sendContentAndClear(data);
}

// pass related data back to OnStep
void libraryTileGet()
{
  String v;
  int i;
  char temp[40] = "";

  // Library index
  v = www.arg("lib_index");
  if (!v.equals(EmptyStr))
  {
    currentObject = L_CAT_NO_OBJECT;
    currentCatalog = v.toInt();
    if (currentCatalog > 0 && currentCatalog < 16)
    {
      sprintf(temp, ":Lo%d#", currentCatalog - 1);
      if (onStep.commandBool(temp)) currentObject = onStep.commandString(":LI#"); else currentCatalog = 0;
    } else currentCatalog = 0;
    if (currentObject.equals(",UNK")) currentObject = "-";
    strcpy(currentCatName, "");
    currentCatNameShowTime = 0;
    catalogIndexChanged = true;
    showMessage = "";
  }

  v = www.arg("lib");
  if (!v.equals(EmptyStr))
  {
    if (currentCatalog >= 0 && currentCatalog < 16)
    {
      if (v.equals("clear")) // clear entire library
      {
        onStep.commandBool(":Lo0#");
        onStep.commandBlind(":L!#");
        strcpy(currentCatName, "");
        currentCatNameShowTime = 0;
        currentObject = L_CAT_NO_OBJECT;
        catalogIndexChanged = true;
        showMessage = L_CAT_DATA_REMOVED ".";
      }
    }

    if (currentCatalog >= 1 && currentCatalog < 16)
    {
      if (v.equals("clear")) // clear just this catalog
      {
        sprintf(temp, ":Lo%ld#", (long)currentCatalog - 1);
        onStep.commandBool(temp);
        onStep.commandBlind(":LL#");
        strcpy(currentCatName, "");
        currentCatNameShowTime = 0;
        currentObject = L_CAT_NO_OBJECT;
        catalogIndexChanged = true;
        showMessage = L_CAT_DATA_REMOVED ".";
      } else
      if (v.equals("cat_prev"))
      {
        onStep.commandBlind(":LB#");
        currentObject = onStep.commandString(":LI#");
        if (currentObject.startsWith("$")) currentObject = ">";
        if (currentObject.equals(",UNK")) currentObject = "-";
      } else
      if (v.equals("cat_next"))
      {
        onStep.commandBlind(":LN#");
        currentObject = onStep.commandString(":LI#");
        if (currentObject.equals(",UNK")) currentObject = "<";
      } else
      if (v.equals("cat_select"))
      {
        currentObject = onStep.commandString(":LIG#");
        currentObject += " " + String(L_SELECTED) + ".";
      } else
      if (v.equals("cat_download"))
      {
        sprintf(temp, ":Lo%ld#", (long)currentCatalog - 1);
        if (onStep.commandBool(temp)) downloadCatalogData = true; else currentCatalog = 0;
      }
    }
  }

  // Catalog upload
  // Object Name|Cat|---RA---|---Dec---
  // ccccccccccc,ccc,HH:MM:SS,sDD*MM:SS
  // NGC6813    ,DN ,19:41:08,+27*20:22
  v = www.arg("cat_upload");
  if (!v.equals(EmptyStr))
  {
    showMessage = "";
    currentObject = "";

    uploadCatalogData = true;
    if (currentCatalog > 0 && currentCatalog < 16)
    {

      sprintf(temp, ":Lo%d#", currentCatalog - 1);
      if (onStep.commandBool(temp))
      {
        v.replace("_", " ");
        int lineNum = 0;
        if (v.equals("DELETE"))
        {
          v = "";
          showMessage = L_CAT_DATA_REMOVED ".";
        }

        onStep.commandBlind(":LL#"); // clear this catalog
        while (v.length() > 0)
        { // any data left?
          lineNum++;
          String line, co, cat, ra, de;
          i = v.indexOf("\n");
          if (i >= 0)
          {
            line = v.substring(0, i);
            v = v.substring(i + 1);
          }
          else
          {
            line = v;
            v = "";
          }

          // catalog name?
          if (lineNum == 1)
          {
            line.trim();
            if (line.charAt(0) == '$')
            {
              co = line.substring(0);
              co.trim();
              if (co.length() < 2 || co.length() > 11)
              {
                showMessage = F(L_CAT_UPLOAD_FAIL);
                break;
              }
              if (!onStep.commandBool(":L$#"))
              {
                showMessage = F(L_CAT_UPLOAD_INDEX_FAIL);
                break;
              }
              if (!onStep.commandBool(":LD#"))
              {
                showMessage = F(L_CAT_DELETE_FAIL);
                break;
              }
              if (!onStep.commandBool((":LW" + co + "#").c_str()))
              {
                showMessage = F(L_CAT_WRITE_NAME_FAIL);
                break;
              }
              continue;
            }
            else
            {
              showMessage = L_CAT_UPLOAD_NO_NAME_FAIL;
              break;
            }
          }

          i = line.indexOf(",");
          if (i >= 0)
          {
            co = line.substring(0, i);
            line = line.substring(i + 1);
          }
          else
          {
            showMessage = F(L_CAT_BAD_FORM);
            showMessage += String(lineNum);
            break;
          }

          i = line.indexOf(",");
          if (i >= 0)
          {
            cat = line.substring(0, i);
            line = line.substring(i + 1);
          }
          else
          {
            showMessage = F(L_CAT_BAD_FORM);
            showMessage += String(lineNum);
            break;
          }

          i = line.indexOf(",");
          if (i >= 0)
          {
            ra = line.substring(0, i);
            line = line.substring(i + 1);
          }
          else
          {
            showMessage = F(L_CAT_BAD_FORM);
            showMessage += String(lineNum);
            break;
          }
          de = line;

          co.trim();
          cat.trim();
          ra.trim();
          de.trim();

          if (co.length() < 1 || co.length() > 11)
          {
            showMessage = F(L_CAT_UPLOAD_BAD_OBJECT_NAME);
            showMessage += String(lineNum);
            break;
          }

          if (cat != "UNK" && cat != "OC" && cat != "GC" && cat != "PN" &&
              cat != "DN" && cat != "SG" && cat != "EG" && cat != "IG" &&
              cat != "KNT" && cat != "SNR" && cat != "GAL" && cat != "CN" &&
              cat != "STR" && cat != "PLA" && cat != "CMT" && cat != "AST")
          {
            showMessage = F(L_CAT_BAD_CATEGORY);
            showMessage += String(lineNum);
            break;
          }

          if (!isDigit(ra.charAt(0)) || !isDigit(ra.charAt(1)) ||
              !isDigit(ra.charAt(3)) || !isDigit(ra.charAt(4)) ||
              !isDigit(ra.charAt(6)) || !isDigit(ra.charAt(7)) ||
              ra.charAt(2) != ':' || ra.charAt(5) != ':' || ra.length() != 8)
          {
            showMessage = F(L_CAT_BAD_RA);
            showMessage += String(lineNum);
            break;
          }

          if (!isDigit(de.charAt(1)) || !isDigit(de.charAt(2)) ||
              !isDigit(de.charAt(4)) || !isDigit(de.charAt(5)) ||
              !isDigit(de.charAt(7)) || !isDigit(de.charAt(8)) ||
              (de.charAt(0) != '+' && de.charAt(0) != '-') ||
              (de.charAt(3) != '*' && de.charAt(3) != ':') ||
              de.charAt(6) != ':' || de.length() != 9)
          {
            showMessage = F(L_CAT_BAD_DEC);
            showMessage += String(lineNum);
            break;
          }

          if (!onStep.commandBool((":Sr" + ra + "#").c_str()))
          {
            showMessage = F(L_CAT_UPLOAD_RA_FAIL);
            showMessage += String(lineNum);
            break;
          }

          if (!onStep.commandBool((":Sd" + de + "#").c_str()))
          {
            showMessage = F(L_CAT_UPLOAD_DEC_FAIL);
            showMessage += String(lineNum);
            break;
          }

          if (!onStep.commandBool((":LW" + co + "," + cat + "#").c_str()))
          {
            showMessage = F(L_CAT_UPLOAD_LINE_FAIL);
            showMessage += String(lineNum);
            break;
          }
        }
        if (showMessage == "")
          showMessage = L_CAT_UPLOAD_SUCCESS ", " + String(lineNum) + " " L_CAT_UPLOAD_LINES_WRITTEN ".";
      }
      else
        showMessage = F(L_CAT_UPLOAD_SELECT_FAIL);
    }
    else
      showMessage = F(L_CAT_UPLOAD_NO_CAT);
  }
}
