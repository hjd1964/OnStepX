// -----------------------------------------------------------------------------------
// non-volatile storage base class

#if NV_DRIVER == NV_EEPROM
  #include "eeprom/EEPROM.h"

#elif NV_DRIVER == NV_ESP
  #include "esp/Esp.h"

#elif NV_DRIVER == NV_M0
  #include "m0/M0.h"

#elif NV_DRIVER == NV_2416 || NV_DRIVER == NV_2432 || NV_DRIVER == NV_AT24C32 || NV_DRIVER == NV_2464 || NV_DRIVER == NV_24128 || NV_DRIVER == NV_24256
  #include "24xx/24XX.h"

#elif NV_DRIVER == NV_MB85RC32 || NV_DRIVER == NV_MB85RC64 || NV_DRIVER == NV_MB85RC256
  #include "mb85rc/MB85RC.h"

#endif
