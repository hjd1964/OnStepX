// ---------------------------------------------------------------------------------------------------------------------------------
// Optional Configuration for OnStepX

//                                 *** Most users will not change anything in this file! ***

// ---------------------------------------------------------------------------------------------------------------------------------
// ADJUST THE FOLLOWING TO CONFIGURE YOUR CONTROLLER FEATURES ----------------------------------------------------------------------
// <-Req'd = always must set, <-Often = usually must set, Option = optional, Adjust = adjust as req'd, Infreq = infrequently changed

// DEBUG ------------------------------------------------------------ see https://onstep.groups.io/g/main/wiki/6-Configuration#DEBUG
// Enable additional debugging and/or status messages on the specified DebugSer port
// Note that the DebugSer port cannot be used for normal communication with OnStep

//      Parameter Name              Value   Default  Notes                                                                      Hint
#define DEBUG                         OFF // default OFF, use "ON" for background errors only, use "VERBOSE" for all errors   Infreq
                                          //              and status messages, use "CONSOLE" for VT100 debug console, use
                                          //              "PROFILER" for VT100 task profiler
#define SERIAL_DEBUG             SERIAL_A //..IAL_A, Default SERIAL_A... or use Serial4 if supported, for example.            Infreq
#define SERIAL_DEBUG_BAUD          115200 //   9600, n. Where n=9600,19200,57600,115200 (common baud rates.)                  Infreq 
