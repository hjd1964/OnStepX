//--------------------------------------------------------------------------------------------------
// local and remote Auxiliary Features control switchboard
// -----------------------------------------------------------------------------------
#pragma once

#include "../../Common.h"

#include "FeaturesBase.h"

// -----------------------------------------------------------------------------
// Include the chosen implementation
// - CLIENT: Features is a proxy class
// - otherwise (local present): Features is the local hardware class
//   and REMOTE (server) support is compiled in as additional code paths.
// -----------------------------------------------------------------------------
#if defined(FEATURES_CAN_CLIENT_PRESENT)
  #include "remote/Features.h"   // defines class Features (proxy)
#elif defined(FEATURES_PRESENT)
  #include "local/Features.h"    // defines class Features (local hardware)
  // conditionally includes/declares server hooks when FEATURES_CAN_SERVER_PRESENT is defined
#endif
