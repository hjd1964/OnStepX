// -----------------------------------------------------------------------------------
// Libraryhelp
#include "LibraryHelp.h"

#include "Pages.common.h"

void handleLibraryHelp() {
  String data = "";

  www.setContentLength(CONTENT_LENGTH_UNKNOWN);
  www.sendHeader("Cache-Control", "no-cache");
  www.send(200, "text/html", String());

  data.concat(F("<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Library Help</title></head><body>"));
  data.concat(FPSTR(html_libCatalogHelp1));
  data.concat(FPSTR(html_libCatalogHelp2));
  data.concat(F("</body></html>"));

  www.sendContentAndClear(data);
  www.sendContent("");
}
