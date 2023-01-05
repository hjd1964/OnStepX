// Website plugin

#include "Website.h"
#include "Common.h"
#include "pages/Pages.h"

void pollWebSvr() {
  www.handleClient();
}

void Website::init() {
  VLF("MSG: Website Plugin");

  VLF("MSG: Set webpage handlers");
  www.on("/index.htm", handleRoot);
  www.on("/index.txt", indexAjax);

  www.on("/mount.htm", handleMount);
  www.on("/mount-ajax-get.txt", mountAjaxGet);
  www.on("/mount-ajax.txt", mountAjax);
  www.on("/libraryHelp.htm", handleLibraryHelp);

  www.on("/rotator.htm", handleRotator);
  www.on("/rotator-ajax-get.txt", rotatorAjaxGet);
  www.on("/rotator-ajax.txt", rotatorAjax);

  www.on("/focuser.htm", handleFocuser);
  www.on("/focuser-ajax-get.txt", focuserAjaxGet);
  www.on("/focuser-ajax.txt", focuserAjax);

  www.on("/auxiliary.htm", handleAux);
  www.on("/auxiliary-ajax-get.txt", auxAjaxGet);
  www.on("/auxiliary-ajax.txt", auxAjax);

  www.on("/", handleRoot);
  
  www.onNotFound(handleNotFound);

  VLF("MSG: Starting port 80 web server");
  www.begin();

  // allow time for the background servers to come up
  delay(2000);

  if (status.onStepFound) {
    status.update(false);
    delay(100);
  }

  state.init();

  VF("MSG: Setup, starting web server polling");
  VF(" task (rate 10ms priority 7)... ");
  if (tasks.add(10, 0, true, 7, pollWebSvr, "webPoll")) { VL("success"); } else { VL("FAILED!"); }

  VLF("MSG: Website Plugin ready");
}

Website website;
