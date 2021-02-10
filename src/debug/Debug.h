// Helper macros for debugging, with less typing
#pragma once

#if DEBUG != OFF
  #define D(x)       SERIAL_DEBUG.print(x)
  #define DF(x)      SERIAL_DEBUG.print(F(x))
  #define DL(x)      SERIAL_DEBUG.println(x)
  #define DLF(x)     SERIAL_DEBUG.println(F(x))
#else
  #define D(x)
  #define DF(x)
  #define DL(x)
  #define DLF(x)
#endif
#if DEBUG == VERBOSE
  #define V(x)       SERIAL_DEBUG.print(x)
  #define VF(x)      SERIAL_DEBUG.print(F(x))
  #define VL(x)      SERIAL_DEBUG.println(x)
  #define VLF(x)     SERIAL_DEBUG.println(F(x))
#else
  #define V(x)
  #define VF(x)
  #define VL(x)
  #define VLF(x)
#endif

#if DEBUG == PROFILER
  #define TASKS_PROFILER_ENABLE
#endif
