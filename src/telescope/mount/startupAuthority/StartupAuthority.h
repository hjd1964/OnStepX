//--------------------------------------------------------------------------------------------------
// telescope mount startup authority
#pragma once

#include "../../../Common.h"

#ifdef MOUNT_PRESENT

#include "../../../libApp/commands/ProcessCmds.h"

class StartupAuthority {
  public:
    enum class Reason : uint8_t { NotEstablished, NvFault, MotorFault };

    void init();
    void begin();

    inline bool trusted() const { return trustedValue; }
    bool required() const;
    CommandError validateManualMotion() const;
    void setTrusted(bool state, Reason reason = Reason::NotEstablished);
    void logStatus() const;

    inline bool trustEstablished() const { return trustEstablishedValue; }
    inline void setCommissioningAllowed(bool state) { commissioningAllowed = state; }

    inline bool absoluteCoordinateOriginsEstablished() const { return absoluteCoordinateOriginsEstablishedValue; }
    inline void clearAbsoluteCoordinateOriginsEstablished() { absoluteCoordinateOriginsEstablishedValue = false; }

  private:
    bool absoluteCoordinateOriginsEstablishedValue = true;
    bool commissioningAllowed = false;
    bool trustEstablishedValue = false;
    bool trustedValue = false;
    Reason untrustedReason = Reason::NotEstablished;
};

extern StartupAuthority startupAuthority;

#endif
