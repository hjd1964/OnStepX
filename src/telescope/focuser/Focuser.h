//--------------------------------------------------------------------------------------------------
// local and remote telescope focuser control switchboard

#pragma once

#include "../../Common.h"

#include "FocuserBase.h"

// -----------------------------------------------------------------------------
// Include the chosen implementation
// - CLIENT: Focuser is a proxy class
// - otherwise (local present): Focuser is the local hardware class
//   and REMOTE (server) support is compiled in as additional code paths.
// -----------------------------------------------------------------------------
#if defined(FOCUSER_CAN_CLIENT_PRESENT)
  #include "remote/Focuser.h"   // defines class Focuser (proxy)
#elif defined(FOCUSER_PRESENT)
  #include "local/Focuser.h"    // defines class Focuser (local hardware)
  // conditionally includes/declares server hooks when FOCUSER_CAN_SERVER_PRESENT is defined
#endif
