#pragma once

// -------------------------------------------------------------------------------------------------
// general bt settings

// -------------------------------------------------------------------------------------------------
// bluetooth manager settings

#ifndef PASSWORD_DEFAULT
#define PASSWORD_DEFAULT       "password" // default pwd for wifi settings
#endif

// first station

#ifndef ONSTEP1_BT_NAME
#define ONSTEP1_BT_NAME          "OnStep" // bluetooth Name for easy identification, leave blank to disable
#endif
#ifndef ONSTEP1_BT_ADDR
#define ONSTEP1_BT_ADDR                "" // bluetooth MAC address to connect to
#endif
#ifndef ONSTEP1_BT_PASSKEY
#define ONSTEP1_BT_PASSKEY             "" // bluetooth four digit passkey, leave blank to disable security
#endif

// first alternate station

#ifndef ONSTEP2_BT_NAME
#define ONSTEP2_BT_NAME                "" // bluetooth Name for easy identification, leave blank to disable
#endif
#ifndef ONSTEP2_BT_ADDR
#define ONSTEP2_BT_ADDR                "" // bluetooth MAC address to connect to
#endif
#ifndef ONSTEP2_BT_PASSKEY
#define ONSTEP2_BT_PASSKEY             "" // bluetooth four digit passkey, leave blank to disable security
#endif

// second alternate station

#ifndef ONSTEP3_BT_NAME
#define ONSTEP3_BT_NAME                "" // bluetooth Name for easy identification, leave blank to disable
#endif
#ifndef ONSTEP3_BT_ADDR
#define ONSTEP3_BT_ADDR                "" // bluetooth MAC address to connect to
#endif
#ifndef ONSTEP3_BT_PASSKEY
#define ONSTEP3_BT_PASSKEY             "" // bluetooth four digit passkey, leave blank to disable security
#endif
