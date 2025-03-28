#pragma once

// -------------------------------------------------------------------------------------------------
// general ip settings

#ifndef HOST_NAME
#define HOST_NAME                "Unknown"
#endif

// default WIFI disabled
#ifndef OPERATIONAL_MODE
#define OPERATIONAL_MODE              OFF
#endif

// optional web-server
#ifndef WEB_SERVER
#define WEB_SERVER                    OFF // ON for website at port 80
#endif

// optional LX200 command server IP channels 9996 to 9999 (listens to clients) used by SWS
// OFF or STANDARD (port 9999), or PERSISTENT (ports 9996 to 9998), or BOTH
#ifndef COMMAND_SERVER
#define COMMAND_SERVER                OFF
#endif

// optional Arduino Serial class work-alike IP channels 9996 to 9999 as a server (listens to clients)
// OFF or STANDARD (port 9999), or PERSISTENT (ports 9996 to 9998), or BOTH
#ifndef SERIAL_SERVER
#define SERIAL_SERVER                 OFF // SERIAL_SIP, SERIAL_PIP1, etc.
#endif

// optional Arduino Serial class work-alike IP channel (ports 9996 to 9998) as a client (connects to a server)
#ifndef SERIAL_CLIENT
#define SERIAL_CLIENT                 OFF // ON to enable SERIAL_IP
#endif

// mDNS defaults
#ifndef MDNS_SERVER
#define MDNS_SERVER                   OFF
#endif

#ifndef MDNS_CLIENT
#define MDNS_CLIENT                   OFF
#endif

#ifndef MDNS_NAME
#define MDNS_NAME               HOST_NAME
#endif

// -------------------------------------------------------------------------------------------------
// wifi manager settings

#ifndef PASSWORD_DEFAULT
#define PASSWORD_DEFAULT       "password" // default pwd for wifi settings
#endif

#ifndef AP_ENABLED
#define AP_ENABLED                  false // to disable the soft AP
#endif

#ifndef AP_SSID
#define AP_SSID                 HOST_NAME // Wifi Access Point SSID
#endif

#ifndef AP_PASSWORD
#define AP_PASSWORD            "password" // Wifi Access Point password
#endif

#ifndef AP_CHANNEL
#define AP_CHANNEL                      7 // Wifi Access Point channel
#endif

#ifndef AP_IP_ADDR
#define AP_IP_ADDR          {192,168,0,1} // Wifi Access Point IP Address
#endif

#ifndef AP_GW_ADDR
#define AP_GW_ADDR          {192,168,0,1} // Wifi Access Point GATEWAY Address
#endif

#ifndef AP_SN_MASK
#define AP_SN_MASK        {255,255,255,0} // Wifi Access Point SUBNET Mask
#endif

// station

#ifndef STA_AP_FALLBACK
#define STA_AP_FALLBACK             false // activate SoftAP if station fails to connect
#endif

#ifndef STA_ENABLED
#define STA_ENABLED                 false // normally not enabled
#endif

#ifndef STA_AUTO_RECONNECT
#define STA_AUTO_RECONNECT          false // normally not enabled
#endif

#ifndef STA_SSID
#define STA_SSID                    "Home" // Wifi Station SSID to connnect to
#endif

#ifndef STA_PASSWORD
#define STA_PASSWORD            "password" // Wifi Station mode password
#endif

#ifndef STA_DHCP_ENABLED
#define STA_DHCP_ENABLED           false  // true to use LAN DHCP addresses
#endif
#define STA_DHCP        STA_DHCP_ENABLED

#ifndef STA_IP_ADDR
#define STA_IP_ADDR         {192,168,0,2} // Wifi Station IP Address
#endif

#ifndef STA_GW_ADDR
#define STA_GW_ADDR         {192,168,0,1} // Wifi Station GATEWAY Address
#endif

#ifndef STA_SN_MASK
#define STA_SN_MASK       {255,255,255,0} // Wifi Station SUBNET Mask
#endif

#ifndef STA_HOST_NAME
#define STA_HOST_NAME              "Home" // Host name to connect to (OnStep for example)
#endif

#ifndef STA_TARGET_IP_ADDR
#define STA_TARGET_IP_ADDR  {192,168,0,1} // IP Address to connect to
#endif
#ifndef STA_TARGET
#define STA_TARGET     STA_TARGET_IP_ADDR
#endif

// station 1

#ifndef STA1_HOST_NAME
#define STA1_HOST_NAME      STA_HOST_NAME
#endif

#ifndef STA1_SSID
#define STA1_SSID                STA_SSID
#endif

#ifndef STA1_PASSWORD
#define STA1_PASSWORD        STA_PASSWORD
#endif

#ifndef STA1_DHCP_ENABLED
#define STA1_DHCP_ENABLED        STA_DHCP
#endif

#ifndef STA1_TARGET_IP_ADDR 
#define STA1_TARGET_IP_ADDR    STA_TARGET
#endif

#ifndef STA1_IP_ADDR
#define STA1_IP_ADDR          STA_IP_ADDR
#endif

#ifndef STA1_GW_ADDR
#define STA1_GW_ADDR          STA_GW_ADDR
#endif

#ifndef STA1_SN_MASK
#define STA1_SN_MASK          STA_SN_MASK
#endif

// station 2

#ifndef STA2_SSID
#define STA2_SSID                      ""
#endif

#ifndef STA2_PASSWORD
#define STA2_PASSWORD                  ""
#endif

#ifndef STA2_DHCP_ENABLED
#define STA2_DHCP_ENABLED           false
#endif

#ifndef STA2_IP_ADDR
#define STA2_IP_ADDR        {192,168,0,2}
#endif

#ifndef STA2_GW_ADDR
#define STA2_GW_ADDR        {192,168,0,1}
#endif

#ifndef STA2_SN_MASK
#define STA2_SN_MASK      {255,255,255,0}
#endif

#ifndef STA2_HOST_NAME
#define STA2_HOST_NAME                 ""
#endif

#ifndef STA2_TARGET_IP_ADDR 
#define STA2_TARGET_IP_ADDR {192,168,0,1}
#endif

// station 3

#ifndef STA3_SSID
#define STA3_SSID                      ""
#endif

#ifndef STA3_PASSWORD
#define STA3_PASSWORD                  ""
#endif

#ifndef STA3_DHCP_ENABLED
#define STA3_DHCP_ENABLED           false
#endif

#ifndef STA3_IP_ADDR
#define STA3_IP_ADDR        {192,168,0,2}
#endif

#ifndef STA3_GW_ADDR
#define STA3_GW_ADDR        {192,168,0,1}
#endif

#ifndef STA3_SN_MASK
#define STA3_SN_MASK      {255,255,255,0}
#endif

#ifndef STA3_HOST_NAME
#define STA3_HOST_NAME                 ""
#endif

#ifndef STA3_TARGET_IP_ADDR 
#define STA3_TARGET_IP_ADDR {192,168,0,1}
#endif

// station 4

#ifndef STA4_SSID
#define STA4_SSID                      ""
#endif

#ifndef STA4_PASSWORD
#define STA4_PASSWORD                  ""
#endif

#ifndef STA4_DHCP_ENABLED
#define STA4_DHCP_ENABLED           false
#endif

#ifndef STA4_IP_ADDR
#define STA4_IP_ADDR        {192,168,0,2}
#endif

#ifndef STA4_GW_ADDR
#define STA4_GW_ADDR        {192,168,0,1}
#endif

#ifndef STA4_SN_MASK
#define STA4_SN_MASK      {255,255,255,0}
#endif

#ifndef STA4_HOST_NAME
#define STA4_HOST_NAME                 ""
#endif

#ifndef STA4_TARGET_IP_ADDR 
#define STA4_TARGET_IP_ADDR {192,168,0,1}
#endif

// station 5

#ifndef STA5_SSID
#define STA5_SSID                      ""
#endif

#ifndef STA5_PASSWORD
#define STA5_PASSWORD                  ""
#endif

#ifndef STA5_DHCP_ENABLED
#define STA5_DHCP_ENABLED           false
#endif

#ifndef STA5_IP_ADDR
#define STA5_IP_ADDR        {192,168,0,2}
#endif

#ifndef STA5_GW_ADDR
#define STA5_GW_ADDR        {192,168,0,1}
#endif

#ifndef STA5_SN_MASK
#define STA5_SN_MASK      {255,255,255,0}
#endif

#ifndef STA5_HOST_NAME
#define STA5_HOST_NAME                 ""
#endif

#ifndef STA5_TARGET_IP_ADDR 
#define STA5_TARGET_IP_ADDR {192,168,0,1}
#endif

// station 6

#ifndef STA6_SSID
#define STA6_SSID                      ""
#endif

#ifndef STA6_PASSWORD
#define STA6_PASSWORD                  ""
#endif

#ifndef STA6_DHCP_ENABLED
#define STA6_DHCP_ENABLED           false
#endif

#ifndef STA6_IP_ADDR
#define STA6_IP_ADDR        {192,168,0,2}
#endif

#ifndef STA6_GW_ADDR
#define STA6_GW_ADDR        {192,168,0,1}
#endif

#ifndef STA6_SN_MASK
#define STA6_SN_MASK      {255,255,255,0}
#endif

#ifndef STA6_HOST_NAME
#define STA6_HOST_NAME                 ""
#endif

#ifndef STA6_TARGET_IP_ADDR 
#define STA6_TARGET_IP_ADDR {192,168,0,1}
#endif
