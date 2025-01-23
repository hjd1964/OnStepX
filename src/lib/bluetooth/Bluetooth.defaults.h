#pragma once

// -------------------------------------------------------------------------------------------------
// general bt settings

#ifndef HOST_NAME
#define HOST_NAME               "Unknown"
#endif

#ifndef SERIAL_BT_NAME
#define SERIAL_BT_NAME          HOST_NAME
#endif

// -------------------------------------------------------------------------------------------------
// bluetooth manager settings

#ifndef PASSWORD_DEFAULT
#define PASSWORD_DEFAULT       "password" // default pwd for wifi settings
#endif

// station 1

#ifndef STA1_BT_NAME
#define STA1_BT_NAME             "OnStep" // the bluetooth server name this station is connecting to
#endif
#ifndef STA1_BT_ADDR
#define STA1_BT_ADDR                   "" // bluetooth MAC address to connect to
#endif
#ifndef STA1_BT_PASSKEY
#define STA1_BT_PASSKEY                "" // bluetooth four digit passkey, leave blank to disable security
#endif

// station 2

#ifndef STA2_BT_NAME
#define STA2_BT_NAME                   ""
#endif
#ifndef STA2_BT_ADDR
#define STA2_BT_ADDR                   ""
#endif
#ifndef STA2_BT_PASSKEY
#define STA2_BT_PASSKEY                ""
#endif

// station 3

#ifndef STA3_BT_NAME
#define STA3_BT_NAME                   ""
#endif
#ifndef STA3_BT_ADDR
#define STA3_BT_ADDR                   ""
#endif
#ifndef STA3_BT_PASSKEY
#define STA3_BT_PASSKEY                ""
#endif

// station 4

#ifndef STA4_BT_NAME
#define STA4_BT_NAME                   ""
#endif
#ifndef STA4_BT_ADDR
#define STA4_BT_ADDR                   ""
#endif
#ifndef STA4_BT_PASSKEY
#define STA4_BT_PASSKEY                ""
#endif

// station 5

#ifndef STA5_BT_NAME
#define STA5_BT_NAME                   ""
#endif
#ifndef STA5_BT_ADDR
#define STA5_BT_ADDR                   ""
#endif
#ifndef STA5_BT_PASSKEY
#define STA5_BT_PASSKEY                ""
#endif

// station 6

#ifndef STA6_BT_NAME
#define STA6_BT_NAME                   ""
#endif
#ifndef STA6_BT_ADDR
#define STA6_BT_ADDR                   ""
#endif
#ifndef STA6_BT_PASSKEY
#define STA6_BT_PASSKEY                ""
#endif
