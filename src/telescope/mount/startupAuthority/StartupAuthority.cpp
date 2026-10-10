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
  untrustedReason = Reason::NotEstablished;
}

void StartupAuthority::setTrusted(bool state, Reason reason) {
  #if NV_INIT_ERROR_REVOKES_AUTHORITY == ON
    if (state && initError.nv) {
      if (!trustedValue) untrustedReason = Reason::NvFault;
      DLF("WRN: Mount, startup authority trust rejected due to NV fault");
      return;
    }
  #endif

  trustedValue = state;
  untrustedReason = state ? Reason::NotEstablished : reason;
  if (state) {
    trustEstablishedValue = true;
    commissioningAllowed = false;
  } else limits.enabled(false);

  // If we have a trusted coordinate basis and date/time is ready, limits can
  // be enforced immediately without waiting for a later goto/reset/unpark path.
  if (trustedValue && site.isDateTimeReady()) limits.enabled(true);
}

void StartupAuthority::logStatus() const {
  #if DEBUG != OFF
    if (trustedValue) { DF("MSG: Mount, startup authority trusted"); } else {
      DF("WRN: Mount, startup authority untrusted: ");
      switch (untrustedReason) {
        case Reason::NvFault:    DF("NV fault blocked or revoked trust (latched)"); break;
        case Reason::MotorFault: DF("motor/encoder fault revoked trust (latched)"); break;
        default: DF("no authoritative coordinate basis established"); break;
      }
    }
    DF(" (mode=");
    #if MOUNT_STARTUP_MODE == SA_PERMISSIVE
      DF("SA_PERMISSIVE");
    #elif MOUNT_STARTUP_MODE == SA_AUTO
      DF("SA_AUTO");
    #else
      DF("SA_STRICT");
    #endif
    DF(", date/time=");
    if (site.isDateTimeReady()) { DF("ready"); } else { DF("not set"); }
    DLF(")");

    if (!trustedValue && untrustedReason == Reason::NotEstablished) {
      DF("MSG: Mount, startup authority sources: absolute axes=");
      D(axis1.motor->hasAbsoluteEncoder()); DF("/"); D(axis2.motor->hasAbsoluteEncoder());
      DF(", encoder origins=");
      if (absoluteCoordinateOriginsEstablishedValue) { DF("established"); } else { DF("missing"); }
      DF(", coordinate memory=");
      #if MOUNT_COORDS_MEMORY == ON
        DLF("ON (see startup restore diagnostics)");
      #else
        DLF("OFF");
      #endif
    }
  #endif
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
