// ---------------------------------------------------------------------------------------------------
// Guide, commands to move the mount in any direction at a series of fixed rates

#include "St4.h"

#ifdef MOUNT_PRESENT

#include "../../../lib/tasks/OnTask.h"

#include "../../Telescope.h"
#include "../Mount.h"
#include "../guide/Guide.h"
#include "../goto/Goto.h"
#include "../status/Status.h"

#if ST4_INTERFACE == ON
  #include "../../../lib/pushButton/PushButton.h"

  #if ST4_HAND_CONTROL == ON
    #include "../../../lib/serial/Serial_ST4_Master.h"
    #define debounceMs 100L
  #else
    #define debounceMs 5L
  #endif

  #include "../../../lib/serial/Serial_Local.h"

  Button st4Axis1Rev(ST4_RA_E_PIN, ST4_INTERFACE_INIT, LOW | HYST(debounceMs));
  Button st4Axis1Fwd(ST4_RA_W_PIN, ST4_INTERFACE_INIT, LOW | HYST(debounceMs));
  Button st4Axis2Fwd(ST4_DEC_N_PIN, ST4_INTERFACE_INIT, LOW | HYST(debounceMs));
  Button st4Axis2Rev(ST4_DEC_S_PIN, ST4_INTERFACE_INIT, LOW | HYST(debounceMs));

  // Single byte guide commands
  #define ccMe 14
  #define ccMw 15
  #define ccMn 16
  #define ccMs 17
  #define ccQe 18
  #define ccQw 19
  #define ccQn 20
  #define ccQs 21

  void st4Wrapper() {
    st4.poll();
  }

  void serialSt4Wrapper() {
    st4.pollSerial();
  }

  void St4::init() {
    long rate = round(1000.0F/HAL_FRACTIONAL_SEC);
    VF("MSG: Mount, ST4 start monitor task (rate "); V(rate); VF("ms priority 2)... ");
    if (tasks.add(rate, 0, true, 2, st4Wrapper, "St4Mntr")) { VLF("success"); } else { VLF("FAILED!"); }
  }

  void St4::poll() {
    static bool shcActive = false;

    st4Axis1Rev.poll();
    if (!shcActive) {
      st4Axis1Fwd.poll();
      st4Axis2Fwd.poll();
      st4Axis2Rev.poll();
    }

    #if ST4_HAND_CONTROL == ON

      if (shcActive) {
        // Smart Hand Controller deactivate
        if (!st4Axis1Rev.hasTone()) {
          pinMode(ST4_DEC_S_PIN, ST4_INTERFACE_INIT);
          pinMode(ST4_DEC_N_PIN, ST4_INTERFACE_INIT);

          serialST4.end();

          // stop the SHC task
          VLF("MSG: Mount, stop SerialST4 comms task");
          tasks.remove(handleSerialST4);
          handleSerialST4 = 0;

          VLF("MSG: SerialST4, deactivated");
          shcActive = false;
        }
        return;
      }

      // Smart Hand Controller activate
      if (!shcActive && st4Axis1Rev.hasTone() && st4Axis1Fwd.hasTone()) {
        pinMode(ST4_DEC_S_PIN, OUTPUT);     // SerialST4 clock
        pinMode(ST4_DEC_N_PIN, OUTPUT);     // SerialST4 data out

        serialST4.begin();

        // start the SHC comms task
        VF("MSG: Mount, start SerialST4 comms task (rate 100us priority 1)... ");
        handleSerialST4 = tasks.add(0, 0, true, 1, serialSt4Wrapper, "St4Comm");
        if (handleSerialST4) { VLF("success"); } else { VLF("FAILED!"); }
        tasks.setPeriodMicros(handleSerialST4, 100);
        tasks.setTimingMode(handleSerialST4, TM_MINIMUM);

        VLF("MSG: SerialST4, activated");
        shcActive = true;
        return;
      }

      // standard hand control
      const long Shed_ms = 4000;

      // stop any guide that might be triggered by combination button presses
      if (st4Axis1Rev.isDown() && st4Axis1Fwd.isDown()) guide.stopAxis1();
      if (st4Axis2Fwd.isDown() && st4Axis2Rev.isDown()) guide.stopAxis2();
  
      // see if a combination was down for long enough for an alternate mode
      static bool altModeA = false;
      static bool altModeB = false;

      #if GOTO_FEATURE == ON
        const long AltMode_ms = 2000;
        if (goTo.state == GS_NONE && !goTo.isHomePaused()) {
          if (st4Axis1Rev.timeDown() > AltMode_ms && st4Axis1Fwd.timeDown() > AltMode_ms && !altModeB) {
            if (!altModeA) { altModeA = true; mountStatus.soundBeep(); }
          }
          if (st4Axis2Fwd.timeDown() > AltMode_ms && st4Axis2Rev.timeDown() > AltMode_ms && !altModeA) {
            if (!altModeB) { altModeB = true; mountStatus.soundBeep(); }
          }
        }
      #endif

      // if the alternate mode is allowed & selected & hasn't timed out, handle it
      if ( (altModeA || altModeB) && (st4Axis2Fwd.timeUp() < Shed_ms || st4Axis2Rev.timeUp() < Shed_ms || st4Axis1Rev.timeUp() < Shed_ms || st4Axis1Fwd.timeUp() < Shed_ms) ) {

        if (altModeA) {
          uint8_t currentRateSelect = ((uint8_t)guide.settings.axis1RateSelect) & 0b11111110;

          // adjust guide rate faster
          if (st4Axis1Fwd.wasPressed() && !st4Axis1Rev.wasPressed()) {
            if (!mount.isTracking()) { SERIAL_LOCAL.transmit(":B+#"); mountStatus.soundClick(); } else {
              if (currentRateSelect <= (GOTO_FEATURE == ON ? 6 : 4)) {
                  guide.settings.axis1RateSelect = (GuideRateSelect)(currentRateSelect + 2);
                  guide.settings.axis2RateSelect = (GuideRateSelect)(currentRateSelect + 2);
                  if (GUIDE_SEPARATE_PULSE_RATE == ON && guide.settings.axis1RateSelect <= GR_1X) guide.settings.pulseRateSelect = guide.settings.axis1RateSelect;
                  mountStatus.soundClick();
                }
            }
          }

          // adjust guide rate slower
          if (st4Axis1Rev.wasPressed() && !st4Axis1Fwd.wasPressed()) {
            if (!mount.isTracking()) { SERIAL_LOCAL.transmit(":B-#"); mountStatus.soundClick(); } else {
              if (currentRateSelect >= 2) {
                guide.settings.axis1RateSelect = (GuideRateSelect)(currentRateSelect - 2);
                guide.settings.axis2RateSelect = (GuideRateSelect)(currentRateSelect - 2);
                if (GUIDE_SEPARATE_PULSE_RATE == ON && guide.settings.axis1RateSelect <= GR_1X) guide.settings.pulseRateSelect = guide.settings.axis1RateSelect;
                mountStatus.soundClick(); }
            }
          }

          // tracking on/off
          if (st4Axis2Fwd.wasPressed() && !st4Axis2Rev.wasPressed()) {
            mount.tracking(!mount.isTracking());
            mountStatus.soundClick();
          }

          // sync or accept align
          if (st4Axis2Rev.wasPressed() && !st4Axis2Fwd.wasPressed()) {
            #if GOTO_FEATURE == ON
              if (goTo.alignDone()) SERIAL_LOCAL.transmit(":CS#"); else goTo.alignAddStar();
            #else
              SERIAL_LOCAL.transmit(":CS#");
            #endif
            mountStatus.soundClick();
          }
        }

        if (altModeB) {
          #if ST4_HAND_CONTROL_FOCUSER == ON
            const char moveSlow[2][8] = {":F12#", ":F22#"};
            const char moveFast[2][8] = {":F14#", ":F24#"};
            const char moveIn[2][8]   = {":F1-#", ":F2-#"};
            const char moveOut[2][8]  = {":F1+#", ":F2+#"};
            static int fs = 0;
            static int fn = 0;
            static int focuserNumber = 0;

            // select focuser 1 or 2
            if (!fn && !fs) {
              if (st4Axis1Fwd.wasPressed() && !st4Axis1Rev.wasPressed()) { focuserNumber = 1; mountStatus.soundClick(); }
              if (st4Axis1Rev.wasPressed() && !st4Axis1Fwd.wasPressed()) { focuserNumber = 0; mountStatus.soundClick(); }
            }

            // move selected focuser out
            if (!fn) {
              if (st4Axis2Rev.isDown() && st4Axis2Fwd.isUp()) {
                if (fs == 0) { SERIAL_LOCAL.transmit(moveSlow[focuserNumber]); fs++; } else
                if (fs == 1) { SERIAL_LOCAL.transmit(moveIn[focuserNumber]); fs++; } else
                if (fs == 2 && st4Axis2Rev.timeDown() > 4000) { SERIAL_LOCAL.transmit(moveFast[focuserNumber]); fs++; } else
                if (fs == 3) { SERIAL_LOCAL.transmit(moveIn[focuserNumber]); fs++; }
              }
              if (st4Axis2Rev.isUp()) { if (fs > 0) { SERIAL_LOCAL.transmit(":F1Q#:F2Q#"); fs = 0; } }
            }

            // move selected focuser in
            if (!fs) {
              if (st4Axis2Fwd.isDown() && st4Axis2Rev.isUp()) {
                if (fn == 0) { SERIAL_LOCAL.transmit(moveSlow[focuserNumber]); fn++; } else
                if (fn == 1) { SERIAL_LOCAL.transmit(moveOut[focuserNumber]); fn++; } else
                if (fn == 2 && st4Axis2Fwd.timeDown() > 4000) { SERIAL_LOCAL.transmit(moveFast[focuserNumber]); fn++; } else
                if (fn == 3) { SERIAL_LOCAL.transmit(moveOut[focuserNumber]); fn++; }
              }
              if (st4Axis2Fwd.isUp()) { if (fn > 0) { SERIAL_LOCAL.transmit(":F1Q#:F2Q#"); fn = 0; } }
            }
          #else
            // select next user catalog item
            if (st4Axis1Fwd.wasPressed() && !st4Axis1Rev.wasPressed()) { SERIAL_LOCAL.transmit(":LN#"); mountStatus.soundClick(); }

            // select previous user catalog item
            if (st4Axis1Rev.wasPressed() && !st4Axis1Fwd.wasPressed()) { SERIAL_LOCAL.transmit(":LB#"); mountStatus.soundClick(); }

            // goto user catalog item
            if (st4Axis2Fwd.wasPressed() && !st4Axis2Rev.wasPressed()) { SERIAL_LOCAL.transmit(":LIG#"); mountStatus.soundClick(); }

            // turn sound on/off
            if (st4Axis2Rev.wasPressed() && !st4Axis2Fwd.wasPressed()) { mountStatus.soundClick(); mountStatus.soundToggleEnable(); mountStatus.soundClick(); }
          #endif
        }
      } else {
        if (altModeA || altModeB) {
          #if ST4_HAND_CONTROL_FOCUSER == ON
            SERIAL_LOCAL.transmit(":F1Q#:F2Q#");
          #endif
          altModeA = false;
          altModeB = false;
          mountStatus.soundBeep();
        }
    #endif

    if (axis1.isEnabled()) {
      // guide E/W
      bool pulseGuiding = GUIDE_SEPARATE_PULSE_RATE == ON && ST4_HAND_CONTROL != ON;
      GuideAction st4GuideActionAxis1 = GA_BREAK;
      static GuideAction lastSt4GuideActionAxis1 = GA_BREAK;
      if (st4Axis1Fwd.isDown() && st4Axis1Rev.isUp()) st4GuideActionAxis1 = GA_FORWARD;
      if (st4Axis1Rev.isDown() && st4Axis1Fwd.isUp()) st4GuideActionAxis1 = GA_REVERSE;

      if (st4GuideActionAxis1 != lastSt4GuideActionAxis1) {
        lastSt4GuideActionAxis1 = st4GuideActionAxis1;
        if (st4GuideActionAxis1 != GA_BREAK) {
          #if ST4_HAND_CONTROL == ON && GOTO_FEATURE == ON
            if (goTo.isHomePaused()) goTo.homeContinue(); else
            if (goTo.state == GS_GOTO) goTo.abort(); else
          #endif
          guide.startAxis1(st4GuideActionAxis1, pulseGuiding ? guide.settings.pulseRateSelect : guide.settings.axis1RateSelect, GUIDE_TIME_LIMIT*1000);
        } else guide.stopAxis1();
      }

      // guide N/S
      GuideAction st4GuideActionAxis2 = GA_BREAK;
      static GuideAction lastSt4GuideActionAxis2 = GA_BREAK;
      if (st4Axis2Fwd.isDown() && st4Axis2Rev.isUp()) st4GuideActionAxis2 = GA_FORWARD;
      if (st4Axis2Rev.isDown() && st4Axis2Fwd.isUp()) st4GuideActionAxis2 = GA_REVERSE;

      if (st4GuideActionAxis2 != lastSt4GuideActionAxis2) {
        lastSt4GuideActionAxis2 = st4GuideActionAxis2;
        if (st4GuideActionAxis2 != GA_BREAK) {
          #if ST4_HAND_CONTROL == ON && GOTO_FEATURE == ON
            if (goTo.isHomePaused()) goTo.homeContinue(); else
            if (goTo.state == GS_GOTO) goTo.abort(); else
          #endif
          guide.startAxis2(st4GuideActionAxis2, pulseGuiding ? guide.settings.pulseRateSelect : guide.settings.axis2RateSelect, GUIDE_TIME_LIMIT*1000);
        } else guide.stopAxis2();
      }

    }

    #if ST4_HAND_CONTROL == ON
    }
    #endif
  }

  #if ST4_HAND_CONTROL == ON
  void St4::pollSerial() {
    char c = serialST4.poll();

    // process any single byte guide commands
    switch (c) {
      case ccMe: guide.startAxis1(GA_REVERSE, guide.settings.axis1RateSelect, GUIDE_TIME_LIMIT*1000); break;
      case ccMw: guide.startAxis1(GA_FORWARD, guide.settings.axis1RateSelect, GUIDE_TIME_LIMIT*1000); break;
      case ccMn: guide.startAxis2(GA_FORWARD, guide.settings.axis2RateSelect, GUIDE_TIME_LIMIT*1000); break;
      case ccMs: guide.startAxis2(GA_REVERSE, guide.settings.axis2RateSelect, GUIDE_TIME_LIMIT*1000); break;
      case ccQe: case ccQw: guide.stopAxis1(); break;
      case ccQn: case ccQs: guide.stopAxis2(); break;
    }
  }
  #endif

#endif

St4 st4;

#endif
