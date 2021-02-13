// ---------------------------------------------------------------------------------------------------------------------------------
// Configuration for OnStepX infrequently used eXtended options

//                                 *** Most users will not change anything in this file! ***

// ---------------------------------------------------------------------------------------------------------------------------------
// ADJUST THE FOLLOWING TO CONFIGURE YOUR CONTROLLER FEATURES ----------------------------------------------------------------------
// <-Req'd = always must set, <-Often = usually must set, Option = optional, Adjust = adjust as req'd, Infreq = infrequently changed

// DEBUG ------------------------------------------------------------ see https://onstep.groups.io/g/main/wiki/6-Configuration#DEBUG
// Enable additional debugging and/or status messages on the specified SERIAL_DEBUG port
// Note that the SERIAL_DEBUG port cannot be used for normal communication with OnStep

//      Parameter Name              Value   Default     Notes                                                                   Hint
#define SERIAL_A_BAUD_DEFAULT      115200 //        9600, n. Where n=9600,19200,57600,115200 (common baud rates.)             Infreq
#define DEBUG                     VERBOSE //         OFF, Use "ON" for background error messages only, use "VERBOSE" for all  Infreq
                                          //              error and status messages, use "CONSOLE" for VT100 debug console,
                                          //              or use "PROFILER" for VT100 task profiler.
#define SERIAL_DEBUG             SERIAL_A //    SERIAL_A, Or use any h/w serial port. Serial1 or Serial2, etc. as supported.  Infreq
#define SERIAL_DEBUG_BAUD          115200 //        9600, n. Where n=9600,19200,57600,115200 (common baud rates.)             Infreq

// MOUNT COORDINATES ------------------------------------------- see https://onstep.groups.io/g/main/wiki/6-Configuration#MOUNT_TYPE
#define MOUNT_COORDS          TOPOCENTRIC // TOPOCENTRIC, Applies refraction to coordinates to/from OnStep, except exactly    Infreq
                                          //              at the poles. Use TOPO_STRICT to apply refraction even in that case.
                                          //              Use OBSERVED_PLACE for no refraction.
