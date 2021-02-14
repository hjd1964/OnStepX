// Helper macros for debugging, with less typing
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../debug/Debug.h"

#include "../tasks/OnTask.h"
extern Tasks tasks;

#ifdef TASKS_PROFILER_ENABLE
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
      sprintf(s, "Profiler %2d.%02d%s                                                           Task Profiler", 1, 0, 'a');
      SERIAL_DEBUG.println(s); Y;
      SERIAL_DEBUG.println();
    }
  
    char *name = tasks.getNameStr(handle);
    
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
#endif

// Helper console for debugging
#if DEBUG == CONSOLE
  #include "../coordinates/Convert.h"
  extern Convert convert;
  #include "../coordinates/Transform.h"
  extern Transform transform;
  #include "../lib/Mount.h"
  extern Axis axis1;
  extern Axis axis2;
  #include "../lib/Clock.h"
  extern Clock clock;

  void debugConsole() {
    Coordinate mount, target;

    mount  = transform.instrumentToMount(axis1.getInstrumentCoordinate(), axis2.getInstrumentCoordinate());
    target = transform.instrumentToMount(axis1.getTargetCoordinate(), axis2.getTargetCoordinate());
    //native = transform.mountToNative(&mount, true);
    transform.equToHor(&mount);
    DL();
  
    D("DATE/TIME = "); D("12/01/21"); D(" "); DL("12:12:12");
    char reply[40];
    convert.doubleToHms(reply, clock.getSiderealTime(), false, PM_HIGH);
    D("LST = "); SERIAL_DEBUG.println(reply);
    SERIAL_DEBUG.println();
    D("current HA  = "); SERIAL_DEBUG.print(radToHrs(mount.h), 4); SERIAL_DEBUG.println(" hrs");
    D("current RA  = "); SERIAL_DEBUG.print(radToHrs(mount.r), 4); SERIAL_DEBUG.println(" hrs");
    D("current Dec = "); SERIAL_DEBUG.print(radToDeg(mount.d), 4); SERIAL_DEBUG.println(" degs");
    SERIAL_DEBUG.println();
    D("current Alt = "); SERIAL_DEBUG.print(radToDeg(mount.a), 4); SERIAL_DEBUG.println(" degs");
    D("current Azm = "); SERIAL_DEBUG.print(radToDeg(mount.z), 4); SERIAL_DEBUG.println(" degs");
    SERIAL_DEBUG.println();
    D("target  HA  = "); SERIAL_DEBUG.print(radToHrs(target.h), 4); SERIAL_DEBUG.println(" hrs");
    D("target  Dec = "); SERIAL_DEBUG.print(radToDeg(target.d), 4); SERIAL_DEBUG.println(" degs");

    D("\x1b[J");  // clear to end of screen
    D("\x1b[H");  // cursor to upper left
    D("\x1b[K");  // clear to end of line
  }
#endif
