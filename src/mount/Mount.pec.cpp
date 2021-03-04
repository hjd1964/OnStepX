//--------------------------------------------------------------------------------------------------
// telescope mount control, PEC
#include <Arduino.h>
#include "../../Constants.h"
#include "../../Config.h"
#include "../../ConfigX.h"
#include "../HAL/HAL.h"
#include "../lib/nv/NV.h"
extern NVS nv;
#include "../pinmaps/Models.h"
#include "../debug/Debug.h"

#if AXIS1_DRIVER_MODEL != OFF && AXIS2_DRIVER_MODEL != OFF

#include "../coordinates/Transform.h"
#include "../motion/Axis.h"
#include "Mount.h"
#include "../telescope/Telescope.h"

void Mount::initPec() {

  // read the pec settings
  if (PecSize < sizeof(Pec)) { DL("ERR: Mount::initPec(); PecSize error NV subsystem writes disabled"); nv.readOnly(true); }
  nv.readBytes(NV_PEC_BASE, &pec, PecSize);

  #if AXIS1_PEC != ON
    pec.worm.rotationSteps = 0;
  #else
    pecWormRotationSeconds = pec.worm.rotationSteps/stepsPerSecondAxis1;
    pecBufferSize = ceil(pec.worm.rotationSteps/(axis1.getStepsPerMeasure()/240.0));

    if (pecBufferSize != 0) {
      if (pecBufferSize < 61) {
        pecBufferSize = 0;
        initError.nv = true;
        DLF("ERR: Mount::initPec(); invalid pecBufferSize PEC disabled");
      }
      if (pecBufferSize + NV_PEC_BUFFER_BASE >= nv.size - 1) {
        pecBufferSize = 0;
        initError.nv = true;
        DLF("ERR: Mount::initPec(); pecBufferSize exceeds available NV, PEC disabled");
      }
    }
    if (pecWormRotationSeconds > pecBufferSize) pecWormRotationSeconds = pecBufferSize;

    pecBuffer = (uint8_t*)malloc(pecBufferSize * sizeof(*pecBuffer));
    if (pecBuffer = NULL) {
      pecBufferSize = 0;
      DLF("MSG: Mount warning buffer exceeds available RAM, PEC disabled");
    } else {
      VF("MSG: Mount allocated PEC buffer, "); V(pecBufferSize * sizeof(*pecBuffer)); VLF(" bytes");
    }

    bool pecBufferNeedsInit = true;
    for (int i = 0; i < pecBufferSize; i++) {
      pecBuffer[i] = nv.read(NV_PEC_BUFFER_BASE + i);
      if (pecBuffer[i] != 0) pecBufferNeedsInit = false;
    }
    if (pecBufferNeedsInit) for (int i = 0; i < pecBufferSize; i++) nv.write(NV_PEC_BUFFER_BASE + i, 128);

    #if PEC_SENSE == OFF
      pec.worm.sensePositionSteps = 0;
      pec.state = PEC_NONE;
    #endif

    if (pec.state > PEC_RECORD) {
      pec.state = PEC_NONE;
      initError.nv = true;
      DLF("ERR: Mount::initPec(); bad NV pec.state");
    }

    if (!pec.recorded) pec.state = PEC_NONE;
  #endif
}

#endif