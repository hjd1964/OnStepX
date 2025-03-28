#pragma once

// -------------------------------------------------------------------------------------------------
// general ip settings

#ifndef HOST_NAME
#define HOST_NAME                  "Unknown"
#endif

// default ETHERNET_XXX disabled
#ifndef OPERATIONAL_MODE
#define OPERATIONAL_MODE                 OFF
#endif

// optional web-server
#ifndef WEB_SERVER
#define WEB_SERVER                       OFF // ON for website at port 80
#endif

// optional LX200 command server IP channels 9996 to 9999 (listens to clients) used by SWS
// OFF or STANDARD (port 9999), or PERSISTENT (ports 9996 to 9998), or BOTH
#ifndef COMMAND_SERVER
#define COMMAND_SERVER                   OFF
#endif

// optional Arduino Serial class work-alike IP channels 9996 to 9999 as a server (listens to clients)
// OFF or STANDARD (port 9999), or PERSISTENT (ports 9996 to 9998), or BOTH
#ifndef SERIAL_SERVER
#define SERIAL_SERVER                    OFF    // SERIAL_SIP, SERIAL_PIP1, etc.
#endif

// optional Arduino Serial class work-alike IP channel to port 9998 as a client (connects to a server)
#ifndef SERIAL_CLIENT
#define SERIAL_CLIENT                    OFF    // ON for SERIAL_IP at port 9998
#endif

// mDNS defaults
#ifndef MDNS_SERVER
#define MDNS_SERVER                      OFF
#endif

#ifndef MDNS_NAME
#define MDNS_NAME                  HOST_NAME
#endif

// -------------------------------------------------------------------------------------------------
// ethernet manager settings

#ifndef MAC
#define MAC  {0xDE,0xAD,0xBE,0xEF,0xFE,0xED}    // unique MAC address
#endif

#ifndef PASSWORD_DEFAULT
#define PASSWORD_DEFAULT          "password"    // default password for settings
#endif

#ifndef DHCP_ENABLED
#define DHCP_ENABLED                    true    // true to use LAN DHCP addresses
#endif

#ifndef STA_IP_ADDR
#define STA_IP_ADDR            {192,168,0,2}    // IP Address
#endif

#ifndef STA_GW_ADDR
#define STA_GW_ADDR            {192,168,0,1}    // GATEWAY Address
#endif

#ifndef STA_SN_MASK
#define STA_SN_MASK          {255,255,255,0}    // SUBNET Mask
#endif

#ifndef STA_HOST_NAME
#define STA_HOST_NAME                 "Home"    // Host name to connect to (OnStep for example)
#endif

#ifndef STA_TARGET_IP_ADDR
#define STA_TARGET_IP_ADDR     {192,168,0,1}    // IP Address to connect to
#endif
#ifndef STA_TARGET
#define STA_TARGET        STA_TARGET_IP_ADDR
#endif

// station 1

#ifndef STA1_HOST_NAME
#define STA1_HOST_NAME         STA_HOST_NAME
#endif

#ifndef STA1_TARGET_IP_ADDR 
#define STA1_TARGET_IP_ADDR       STA_TARGET
#endif

// station 2

#ifndef STA2_HOST_NAME
#define STA2_HOST_NAME                    ""
#endif

#ifndef STA2_TARGET_IP_ADDR 
#define STA2_TARGET_IP_ADDR    {192,168,0,1}
#endif

// station 3

#ifndef STA3_HOST_NAME
#define STA3_HOST_NAME                    ""
#endif

#ifndef STA3_TARGET_IP_ADDR 
#define STA3_TARGET_IP_ADDR    {192,168,0,1}
#endif

// station 4

#ifndef STA4_HOST_NAME
#define STA4_HOST_NAME                    ""
#endif

#ifndef STA4_TARGET_IP_ADDR 
#define STA4_TARGET_IP_ADDR    {192,168,0,1}
#endif

// station 5

#ifndef STA5_HOST_NAME
#define STA5_HOST_NAME                    ""
#endif

#ifndef STA5_TARGET_IP_ADDR 
#define STA5_TARGET_IP_ADDR    {192,168,0,1}
#endif

// station 6

#ifndef STA6_HOST_NAME
#define STA6_HOST_NAME                    ""
#endif

#ifndef STA6_TARGET_IP_ADDR 
#define STA6_TARGET_IP_ADDR    {192,168,0,1}
#endif
