#pragma once

#ifndef AXIS1_ENABLE_STATE
#define AXIS1_ENABLE_STATE     LOW  // default state of ENable pin for motor power on
#endif
#ifndef AXIS2_ENABLE_STATE
#define AXIS2_ENABLE_STATE     LOW
#endif
#ifndef AXIS3_ENABLE_STATE
#define AXIS3_ENABLE_STATE     LOW
#endif
#ifndef AXIS4_ENABLE_STATE
#define AXIS4_ENABLE_STATE     LOW
#endif
#ifndef AXIS5_ENABLE_STATE
#define AXIS5_ENABLE_STATE     LOW
#endif
#ifndef AXIS6_ENABLE_STATE
#define AXIS6_ENABLE_STATE     LOW
#endif
#ifndef AXIS7_ENABLE_STATE
#define AXIS7_ENABLE_STATE     LOW
#endif
#ifndef AXIS8_ENABLE_STATE
#define AXIS8_ENABLE_STATE     LOW
#endif
#ifndef AXIS9_ENABLE_STATE
#define AXIS9_ENABLE_STATE     LOW
#endif

#ifndef AXIS1_STEP_STATE
#define AXIS1_STEP_STATE       HIGH // default signal transition state for a step
#endif
#ifndef AXIS2_STEP_STATE
#define AXIS2_STEP_STATE       HIGH
#endif
#ifndef AXIS3_STEP_STATE
#define AXIS3_STEP_STATE       HIGH
#endif
#ifndef AXIS4_STEP_STATE
#define AXIS4_STEP_STATE       HIGH
#endif
#ifndef AXIS5_STEP_STATE
#define AXIS5_STEP_STATE       HIGH
#endif
#ifndef AXIS6_STEP_STATE
#define AXIS6_STEP_STATE       HIGH
#endif
#ifndef AXIS7_STEP_STATE
#define AXIS7_STEP_STATE       HIGH
#endif
#ifndef AXIS8_STEP_STATE
#define AXIS8_STEP_STATE       HIGH
#endif
#ifndef AXIS9_STEP_STATE
#define AXIS9_STEP_STATE       HIGH
#endif

#if AXIS1_STEP_STATE == AXIS2_STEP_STATE == AXIS3_STEP_STATE == AXIS4_STEP_STATE == AXIS5_STEP_STATE == AXIS6_STEP_STATE == AXIS7_STEP_STATE == AXIS8_STEP_STATE == AXIS9_STEP_STATE == HIGH
  #define DRIVER_STEP_DEFAULTS
#endif
