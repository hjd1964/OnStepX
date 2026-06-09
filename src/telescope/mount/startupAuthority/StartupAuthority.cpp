//--------------------------------------------------------------------------------------------------
// telescope mount startup authority

#include "StartupAuthority.h"

#ifdef MOUNT_PRESENT

#include "../../Telescope.h"
#include "../Mount.h"
#include "../goto/Goto.h"
#include "../limits/Limits.h"
#include "../site/Site.h"

void StartupAuthority::init() {
  absoluteCoordinateOriginsEstablishedValue = true;
}

void StartupAuthority::begin() {
  trustedValue = false;
  trustEstablishedValue = false;
  commissioningAllowed = false;
}

void StartupAuthority::setTrusted(bool state) {
  #if NV_INIT_ERROR_REVOKES_AUTHORITY == ON
    if (state && initError.nv) {
      DLF("WRN: Mount, startup authority trust rejected due to NV fault");
      return;
    }
  #endif

  trustedValue = state;
  if (state) {
    trustEstablishedValue = true;
    commissioningAllowed = false;
  } else limits.enabled(false);

  // If we have a trusted coordinate basis and date/time is ready, limits can
  // be enforced immediately without waiting for a later goto/reset/unpark path.
  if (trustedValue && site.isDateTimeReady()) limits.enabled(true);
}

bool StartupAuthority::required() const {
  #if MOUNT_COORDS_MEMORY == ON
    return true;
  #else
    return goTo.absoluteEncodersPresent || axis1.motor->hasAbsoluteEncoder() || axis2.motor->hasAbsoluteEncoder();
  #endif
}

CommandError StartupAuthority::validateManualMotion() const {
  if (!required()) return CE_NONE;

  // Commissioning mode: on first boot/fresh NV there may be no trusted
  // coordinate memory or encoder origin yet.  Allow manual motion so the user
  // can place the mount and establish the one-time origin.
  if (!trustedValue) {
    #if MOUNT_COORDS_MEMORY == ON
      if (commissioningAllowed) return CE_NONE;
    #else
      if (!trustEstablishedValue) return CE_NONE;
    #endif
    return CE_SLEW_ERR_UNSPECIFIED;
  }

  if (!site.isDateTimeReady()) return CE_SLEW_ERR_IN_STANDBY;
  if (!limits.isEnabled()) return CE_SLEW_ERR_OUTSIDE_LIMITS;

  return CE_NONE;
}

StartupAuthority startupAuthority;

#endif
