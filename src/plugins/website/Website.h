// Website plugin
#pragma once

#if (SERIAL_IP_MODE != WIFI_STATION && SERIAL_IP_MODE != WIFI_ACCESS_POINT) || WEB_SERVER != ON
    #error "Configuration (Config.h): The website plugin requires SERIAL_RADIO be set to WIFI_STATION or WIFI_ACCESS_POINT"
#endif

class Website {
public:
  // the initialization method must be present and named: void init();
  void init();

  void loop();

private:

};

extern Website website;
