#pragma once

#ifndef TARGET_IP_ADDR1
#define TARGET_IP_ADDR1     {192,168,0,1} // ..,168,0,1}, IP Address to connect to (OnStep for example)
#endif

#ifndef TARGET_IP_ADDR2
#define TARGET_IP_ADDR2     {192,168,0,1} // ..,168,0,1}, IP Address to connect to
#endif

#ifndef PASSWORD_DEFAULT
#define PASSWORD_DEFAULT       "password" // "password", default pwd for wifi settings
#endif

#ifndef AP_ENABLED
#define AP_ENABLED                  false //      false, to disable the soft AP
#endif

#ifndef PROD_ABV
#define PROD_ABV                "Unknown"
#endif

#ifndef AP_SSID
#define AP_SSID                  PROD_ABV //      "...", Wifi Access Point SSID.
#endif

#ifndef AP_PASSWORD
#define AP_PASSWORD            "password" //  "password", Wifi Access Point password.
#endif

#ifndef AP_CHANNEL
#define AP_CHANNEL                      7 //           7, Wifi Access Point channel.
#endif

#ifndef AP_IP_ADDR
#define AP_IP_ADDR          {192,168,0,1} // ..,168,0,1}, Wifi Access Point IP Address.
#endif

#ifndef AP_GW_ADDR
#define AP_GW_ADDR          {192,168,0,1} // ..,168,0,1}, Wifi Access Point GATEWAY Address.
#endif

#ifndef AP_SN_MASK
#define AP_SN_MASK        {255,255,255,0} // ..55,255,0}, Wifi Access Point SUBNET Mask.
#endif

#ifndef STA_ENABLED
#define STA_ENABLED                false  //       false, normally not enabled
#endif

#ifndef STA_DHCP_ENABLED
#define STA_DHCP_ENABLED           false  //      false, true to use LAN DHCP addresses
#endif

#ifndef STA_SSID
#define STA_SSID                   "Home" //     "Home", Wifi Station SSID to connnect to.
#endif

#ifndef STA_PASSWORD
#define STA_PASSWORD           "password" //  "password", Wifi Station mode password.
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
