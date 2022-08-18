// -----------------------------------------------------------------------------------
// Web server
#pragma once

#include "../../../Common.h"

#include "../EthernetManager.h"

#if (OPERATIONAL_MODE == ETHERNET_W5100 || OPERATIONAL_MODE == ETHERNET_W5500) && \
    WEB_SERVER == ON

  // Turn ON to allow webserver debug messages
  #ifndef DEBUG_WEBSERVER
    #define DEBUG_WEBSERVER OFF
  #endif

  #if DEBUG_WEBSERVER == ON
    #define W(x) V(x)
    #define WF(x) VF(x)
    #define WL(x) VL(x)
    #define WLF(x) VLF(x)
  #else
    #define W(x)
    #define WF(x)
    #define WL(x)
    #define WLF(x)
  #endif

  // misc.
  #ifndef WEB_SOCKET_TIMEOUT
  #define WEB_SOCKET_TIMEOUT     250
  #endif
  #ifndef WEB_HANDLER_COUNT_MAX
  #define WEB_HANDLER_COUNT_MAX  24
  #endif
  #define PARAMETER_COUNT_MAX    12
  #define CONTENT_LENGTH_UNKNOWN -1
  #define CONTENT_LENGTH_NOT_SET -2
  #define HTTP_UNKNOWN           0
  #define HTTP_GET               1
  #define HTTP_PUT               2
  #define HTTP_POST              3

  #define sendContentAndClear(x) sendContent(x); x = "";

  typedef void (* webFunction) ();
  
  class WebServer {
    public:
      void begin(long port = 80, long timeToClose = 50, bool autoReset = false);

      void handleClient();

      void on(String fn, webFunction handler);
      void onNotFound(webFunction handler);

      // get argument value by identifier
      String arg(String id);
      // get argument value by index
      String arg(int i);
      // get argument name by index
      String argName(int i);
      // get arguments count
      int args();                     
      // check if argument exists
      bool hasArg(String id);

      // check http last method used, returns HTTP_UNKNOWN, HTTP_GET, HTTP_PUT, or HTTP_POST
      inline int method() { return lastMethod; }

      // return uniform resource identifier
      String uri() { return requestedHandler; }

      // return modified since state
      bool modifiedSince() { return modifiedSinceFound; }

      void setContentLength(long length);
      void setResponseHeader(const char *str);
      void sendHeader(const char* key, const char* val, bool first = false);
      void send(int code, const char* content_type = "text/html", const String& content = "");
      void sendContent(String s);
      void sendContent(const char * s);

      EthernetServer *webServer = NULL;
      EthernetClient client;
    private:
      int  getHandler(String* line);
      void processGet(String* line);
      void processPut(String* line);
      void processPost(String* line);
 
      char responseHeader[200] = "";
      bool modifiedSinceFound = false;
  
      webFunction notFoundHandler = NULL;
      webFunction handlers[WEB_HANDLER_COUNT_MAX];
      String handlers_fn[WEB_HANDLER_COUNT_MAX];
      int handler_count = 0;
      
      String requestedHandler;
      String parameters[PARAMETER_COUNT_MAX];
      String values[PARAMETER_COUNT_MAX];
      int parameter_count;
      int port = -1;
      bool autoReset = false;
      long timeToClose = 100;

      int lastMethod = HTTP_UNKNOWN;

      long length;
      String header;
  };

  extern WebServer www;

#endif
