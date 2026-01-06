//--------------------------------------------------------------------------------------------------
// local and remote telescope rotator control switchboard

#pragma once

#include "RotatorBase.h"

// -----------------------------------------------------------------------------
// Include the chosen implementation
// - CLIENT: Rotator is a proxy class
// - otherwise (local present): Rotator is the local hardware class
//   and REMOTE (server) support is compiled in as additional code paths.
// -----------------------------------------------------------------------------
#if defined(ROTATOR_CAN_CLIENT_PRESENT)
  #include "remote/Rotator.h"   // defines class Rotator (proxy)
#elif defined(ROTATOR_PRESENT)
  #include "local/Rotator.h"    // defines class Rotator (local hardware)
  // conditionally includes/declares server hooks when ROTATOR_CAN_SERVER_PRESENT is defined
#endif
