#pragma once

// -------------------------------------------------------------------------------------------------
// general ip settings

// first, you must have an Wifi or Ethernet device:  OFF or WIFI, ETHERNET_W5100, ETHERNET_W5500
#ifndef OPERATIONAL_MODE
#define OPERATIONAL_MODE OFF
#endif

// optional web-server
#ifndef WEB_SERVER
#define WEB_SERVER OFF                       // ON for website at port 80
#endif

// optional LX200 command server IP channels 9996 to 9999 (listens to clients) used by SWS
// OFF or STANDARD (port 9999), or PERSISTENT (ports 9996 to 9998), or BOTH
#ifndef COMMAND_SERVER
#define COMMAND_SERVER OFF
#endif

// optional Arduino Serial class work-alike IP channels 9996 to 9999 as a server (listens to clients)
// OFF or STANDARD (port 9999), or PERSISTENT (ports 9996 to 9998), or BOTH
#ifndef SERIAL_SERVER
#define SERIAL_SERVER OFF                    // SERIAL_SIP, SERIAL_PIP1, etc.
#endif

// optional Arduino Serial class work-alike IP channel to port 9998 as a client (connects to a server)
#ifndef SERIAL_CLIENT
#define SERIAL_CLIENT OFF                    // ON for SERIAL_IP at port 9998
#endif

// mDNS defaults
#ifndef MDNS_SERVER
#define MDNS_SERVER OFF                      // by default mDNS is disabled
#endif

#ifndef MDNS_NAME
#define MDNS_NAME "arduino"
#endif

// -------------------------------------------------------------------------------------------------
// ethernet manager settings

#ifndef MAC
#define MAC {0xDE,0xAD,0xBE,0xEF,0xFE,0xED}  // unique MAC address
#endif

#ifndef PASSWORD_DEFAULT
#define PASSWORD_DEFAULT       "password"    // default password for settings
#endif

#ifndef STA_TARGET_IP_ADDR
#define STA_TARGET_IP_ADDR  {192,168,0,1}    // IP Address to connect to (OnStep for example)
#endif

#ifndef DHCP_ENABLED
#define DHCP_ENABLED                 true    // true to use LAN DHCP addresses
#endif

#ifndef STA_IP_ADDR
#define STA_IP_ADDR         {192,168,0,2}    // IP Address
#endif

#ifndef STA_GW_ADDR
#define STA_GW_ADDR         {192,168,0,1}    // GATEWAY Address
#endif

#ifndef STA_SN_MASK
#define STA_SN_MASK       {255,255,255,0}    // SUBNET Mask
#endif
