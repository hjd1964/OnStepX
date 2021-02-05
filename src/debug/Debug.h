// Helper macros for debugging, with less typing

#if DEBUG != OFF
  #define D(x)       DebugSer.print(x)
  #define DF(x)      DebugSer.print(F(x))
  #define DL(x)      DebugSer.println(x)
  #define DLF(x)     DebugSer.println(F(x))
#else
  #define D(x)
  #define DF(x)
  #define DL(x)
  #define DLF(x)
#endif
#if DEBUG == VERBOSE
  #define V(x)       DebugSer.print(x)
  #define VF(x)      DebugSer.print(F(x))
  #define VL(x)      DebugSer.println(x)
  #define VLF(x)     DebugSer.println(F(x))
#else
  #define V(x)
  #define VF(x)
  #define VL(x)
  #define VLF(x)
#endif

#if DEBUG == PROFILER
  #define TASKS_PROFILER_ENABLE
#endif
