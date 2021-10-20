#pragma once

#ifndef MAC
#define MAC {0xDE,0xAD,0xBE,0xEF,0xFE,0xED} // a unique MAC address
#endif

#ifndef TARGET_IP_ADDR1
#define TARGET_IP_ADDR1     {192,168,0,1} // ..,168,0,1}, IP Address to connect to (OnStep for example)
#endif

#ifndef TARGET_IP_ADDR2
#define TARGET_IP_ADDR2     {192,168,0,1} // ..,168,0,1}, IP Address to connect to
#endif

#ifndef PASSWORD_DEFAULT
#define PASSWORD_DEFAULT       "password" // "password", default pwd for ethernet settings
#endif

#ifndef DHCP_ENABLED
#define DHCP_ENABLED                 true //       true, true to use LAN DHCP addresses
#endif

#ifndef STA_IP_ADDR
#define STA_IP_ADDR         {192,168,0,2} //  ..168,0,2}, Wifi Station IP Address.
#endif

#ifndef STA_GW_ADDR
#define STA_GW_ADDR         {192,168,0,1} // ..,168,0,1}, Wifi Station GATEWAY Address.
#endif

#ifndef STA_SN_MASK
#define STA_SN_MASK       {255,255,255,0} // ..55,255,0}, Wifi Station SUBNET Mask.
#endif
