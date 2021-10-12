// Helper macros for debugging, with less typing

#include "Debug.h"

#ifdef TASKS_PROFILER_ENABLE

  #include "../tasks/OnTask.h"

  #warning "MSG: Warning the OnTask PROFILER is for debugging only, there is siginficant overhead associated with enabling this feature."
  #define PROFILER_VT100 ON
  
  char scale_unit(double *d) {
    if (abs(lround(*d)) > 999) { *d /= 1000.0; return 'm'; } else
      if (abs(lround(*d)) > 999) { *d /= 1000.0; return ' '; } else return 'u';
  }
  
  void profiler() {
    char s[120];
    char aau = 'u'; char axu = 'u'; char rtu = 'u'; char rau = 'u'; char rxu = 'u';
    static int count;
    static double AAA,AXA,RTT,RAA,RXA;
  
    static int handle = tasks.getFirstHandle();
  
    handle = tasks.getNextHandle();
    if (!handle) {
      AAA /= count; AXA /= count; RAA /= count; RXA /= count;
      aau = scale_unit(&AAA); axu = scale_unit(&AXA); rtu = scale_unit(&RTT); rau = scale_unit(&RAA); rxu = scale_unit(&RXA);
  
      sprintf(s, "                     ----------- ------------        ----------   --------   ----------");
      SERIAL_DEBUG.print(s); Y;
      SERIAL_DEBUG.println(); Y;
  
      sprintf(s, "                    avgd %5ld%cs  avgd %4ld%cs    totaled %4ld%cs avgd %4ld%cs avgd %4ld%cs", 
      lround(AAA), aau, lround(AXA), axu, lround(RTT), rtu, lround(RAA), rau, lround(RXA), rxu); Y;
    
      SERIAL_DEBUG.print(s); Y;
      SERIAL_DEBUG.println(); Y;
  
      count = 0;
      handle = tasks.getFirstHandle();
      #if PROFILER_VT100 == ON
        SERIAL_DEBUG.print("\x1b[J");  // clear to end of screen
        SERIAL_DEBUG.print("\x1b[H");  // cursor to upper left
        SERIAL_DEBUG.print("\x1b[K");  // clear to end of line
      #endif
      SERIAL_DEBUG.println(); Y;
      sprintf(s, "Profiler %2d.%02d%s                                                           Task Profiler", 1, 0, "a");
      SERIAL_DEBUG.println(s); Y;
      SERIAL_DEBUG.println();
    }
  
    char *name = tasks.getNameStr(handle);
    
    if (!strstr(name, "Profilr")) {
      double AA = tasks.getArrivalAvg(handle); Y;
      double AX = tasks.getArrivalMax(handle); Y;
      double RT = tasks.getRuntimeTotal(handle); Y;
      double RTcount = tasks.getRuntimeTotalCount(handle); Y;
      double RA; if (RTcount == 0) RA = 0; else RA = RT/RTcount; 
      double RX = tasks.getRuntimeMax(handle); Y;
      count++; AAA += AA; AXA += AX; RTT += RT; RAA += RA; RXA += RX;
      aau = scale_unit(&AA); axu = scale_unit(&AX); rtu = scale_unit(&RT); rau = scale_unit(&RA); rxu = scale_unit(&RX);
      
      sprintf(s, "[%-10s] arrives avg %5ld%cs, max ±%4ld%cs; run total %4ld%cs, avg %4ld%cs, max %4ld%cs", 
      name, lround(AA), aau, lround(AX), axu, lround(RT), rtu, lround(RA), rau, lround(RX), rxu); Y;
    
      SERIAL_DEBUG.print(s); Y;
      SERIAL_DEBUG.println();
    }
  }
#endif
