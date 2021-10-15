// -----------------------------------------------------------------------------------
// Helper macros for debugging, with less typing
#pragma once

#include "../../Common.h"

#if DEBUG == PROFILER
  #define TASKS_PROFILER_ENABLE
#endif

#if defined(DEBUG) && DEBUG != OFF
  #if defined(REMOTE) && DEBUG == REMOTE
    // echo strings to OnStep debug interface (supports embedded spaces and cr/lf)
    extern void debugPrint(const char* s);
    extern bool debugRemoteConnected;

    #define D(x)     { if (debugRemoteConnected) { SERIAL_ONSTEP.print(":EC"); SERIAL_ONSTEP.print(x); SERIAL_ONSTEP.print("#"); delay(50); } }
    #define DF(x)    { if (debugRemoteConnected) { SERIAL_ONSTEP.print(":EC"); debugPrint(x); SERIAL_ONSTEP.print("#"); delay(50); } }
    #define DL(x)    { if (debugRemoteConnected) { SERIAL_ONSTEP.print(":EC"); SERIAL_ONSTEP.print(x); SERIAL_ONSTEP.print("&#"); delay(50); } }
    #define DLF(x)   { if (debugRemoteConnected) { SERIAL_ONSTEP.print(":EC"); debugPrint(x); SERIAL_ONSTEP.print("&#"); delay(50); } }
  #else
    #define D(x)     SERIAL_DEBUG.print(x)
    #define DF(x)    SERIAL_DEBUG.print(F(x))
    #define DL(x)    SERIAL_DEBUG.println(x)
    #define DLF(x)   SERIAL_DEBUG.println(F(x))
  #endif
#else
  #define D(x)
  #define DF(x)
  #define DL(x)
  #define DLF(x)
#endif

#if (defined(VERBOSE) && DEBUG == VERBOSE) || (defined(REMOTE) && DEBUG == REMOTE)
  #define V(x)       D(x)
  #define VF(x)      DF(x)
  #define VL(x)      DL(x)
  #define VLF(x)     DLF(x)
#else
  #define V(x)
  #define VF(x)
  #define VL(x)
  #define VLF(x)
#endif
