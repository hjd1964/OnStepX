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

  void St4::init() {
    #ifdef HAL_SLOW_PROCESSOR
      VF("MSG: Mount, ST4 start monitor task (rate 5ms priority 1)... ");
      if (tasks.add(5, 0, true, 1, st4Wrapper, "St4Mntr")) { VLF("success"); } else { VLF("FAILED!"); }
    #else
      VF("MSG: Mount, ST4 start monitor task (rate 1.7ms priority 1)... ");
      uint8_t handle = tasks.add(0, 0, true, 1, st4Wrapper, "St4Mntr");
      if (handle) { VLF("success"); } else { VLF("FAILED!"); }
      tasks.setPeriodMicros(handle, 1700);
      tasks.setTimingMode(handle, TM_MINIMUM);
    #endif
  }

  // monitor ST4 port for guiding, basic hand controller, and smart hand controller
  void St4::poll() {

    st4Axis1Rev.poll();
    static bool shcActive = false;
    if (!shcActive) {
      st4Axis1Fwd.poll();
      st4Axis2Fwd.poll();
      st4Axis2Rev.poll();
    }

    #if ST4_HAND_CONTROL == ON
      if (st4Axis1Rev.hasTone()) {
        if (!shcActive) {
          // Smart Hand Controller activate
          if (st4Axis1Fwd.hasTone()) {
            pinMode(ST4_DEC_S_PIN, OUTPUT);     // clock
            pinMode(ST4_DEC_N_PIN, OUTPUT);     // send data
            digitalWriteF(ST4_DEC_S_PIN, HIGH); // idle
            shcActive = true;
            serialST4.begin();
            VLF("MSG: SerialST4, activated");
          }
          return;
        } else {
          // Smart Hand Controller active
          char c = serialST4.poll();
          // process any single byte guide commands
          if (c == ccMe) guide.startAxis1(GA_REVERSE, guide.settings.axis1RateSelect, GUIDE_TIME_LIMIT*1000);
          if (c == ccMw) guide.startAxis1(GA_FORWARD, guide.settings.axis1RateSelect, GUIDE_TIME_LIMIT*1000);
          if (c == ccMn) guide.startAxis2(GA_FORWARD, guide.settings.axis2RateSelect, GUIDE_TIME_LIMIT*1000);
          if (c == ccMs) guide.startAxis2(GA_REVERSE, guide.settings.axis2RateSelect, GUIDE_TIME_LIMIT*1000);
          if (c == ccQe || c == ccQw) guide.stopAxis1();
          if (c == ccQn || c == ccQs) guide.stopAxis2();
          return;
        }
      } else {
        if (shcActive) {
          // Smart Hand Controller deactivate
          pinMode(ST4_DEC_S_PIN, ST4_INTERFACE_INIT);
          pinMode(ST4_DEC_N_PIN, ST4_INTERFACE_INIT);
          shcActive = false;
          serialST4.end();
          VLF("MSG: SerialST4, deactivated");
          return;
        }
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
            if (!altModeA) { altModeA = true; mountStatus.sound.beep(); }
          }
          if (st4Axis2Fwd.timeDown() > AltMode_ms && st4Axis2Rev.timeDown() > AltMode_ms && !altModeA) {
            if (!altModeB) { altModeB = true; mountStatus.sound.beep(); }
          }
        }
      #endif

      // if the alternate mode is allowed & selected & hasn't timed out, handle it
      if ( (altModeA || altModeB) && (st4Axis2Fwd.timeUp() < Shed_ms || st4Axis2Rev.timeUp() < Shed_ms || st4Axis1Rev.timeUp() < Shed_ms || st4Axis1Fwd.timeUp() < Shed_ms) ) {

        // make sure no cmdSend() is being processed
        //if (!cmdWaiting())
        {
          if (altModeA) {
            int r = (int)guide.settings.axis1RateSelect;
            if (st4Axis1Fwd.wasPressed() && !st4Axis1Rev.wasPressed()) {
              #if GOTO_FEATURE == ON
                if (goTo.state == GS_NONE) SERIAL_LOCAL.transmit(":B+#"); else { if (r >= 7) r=8; else if (r >= 5) r=7; else if (r >= 2) r=5; else if (r < 2) r=2; }
              #else
                SERIAL_LOCAL.transmit(":B+#");
              #endif
              mountStatus.sound.click();
            }
            if (st4Axis1Rev.wasPressed() && !st4Axis1Fwd.wasPressed()) {
              #if GOTO_FEATURE == ON
                if (goTo.state == GS_NONE) SERIAL_LOCAL.transmit(":B-#"); else { if (r <= 5) r=2; else if (r <= 7) r=5; else if (r <= 8) r=7; else if (r > 8) r=8; }
              #else
                SERIAL_LOCAL.transmit(":B-#");
              #endif
              mountStatus.sound.click();
            }
            if (st4Axis2Rev.wasPressed() && !st4Axis2Fwd.wasPressed()) {
              #if GOTO_FEATURE == ON
                if (goTo.alignDone()) SERIAL_LOCAL.transmit(":CS#"); else goTo.alignAddStar();
              #else
                SERIAL_LOCAL.transmit(":CS#");
              #endif
              mountStatus.sound.click();
            }
            if (st4Axis2Fwd.wasPressed() && !st4Axis2Rev.wasPressed()) { mount.tracking(!mount.isTracking()); mountStatus.sound.click(); }
            guide.settings.axis1RateSelect = (GuideRateSelect)r;
            guide.settings.axis2RateSelect = (GuideRateSelect)r;
            if (GUIDE_SEPARATE_PULSE_RATE == ON && guide.settings.axis1RateSelect <= GR_1X) guide.settings.pulseRateSelect = guide.settings.axis1RateSelect;
          }
          if (altModeB) {
            #if ST4_HAND_CONTROL_FOCUSER == ON
              static int fs = 0;
              static int fn = 0;
              if (!fn && !fs) {
                if (st4Axis1Fwd.wasPressed() && !st4Axis1Rev.wasPressed()) { SERIAL_LOCAL.transmit(":F2#"); mountStatus.sound.click(); }
                if (st4Axis1Rev.wasPressed() && !st4Axis1Fwd.wasPressed()) { SERIAL_LOCAL.transmit(":F1#"); mountStatus.sound.click(); }
              }
              if (!fn) {
                if (st4Axis2Rev.isDown() && st4Axis2Fwd.isUp()) {
                  if (fs == 0) { SERIAL_LOCAL.transmit(":FS#"); fs++; } else
                  if (fs == 1) { SERIAL_LOCAL.transmit(":F-#"); fs++; } else
                  if (fs == 2 && st4Axis2Rev.timeDown() > 4000) { SERIAL_LOCAL.transmit(":FF#"); fs++; } else
                  if (fs == 3) { SERIAL_LOCAL.transmit(":F-#"); fs++; }
                }
                if (st4Axis2Rev.isUp()) { if (fs > 0) { SERIAL_LOCAL.transmit(":FQ#"); fs = 0; } }
              }
              if (!fs) {
                if (st4Axis2Fwd.isDown() && st4Axis2Rev.isUp()) {
                  if (fn == 0) { SERIAL_LOCAL.transmit(":FS#"); fn++; } else
                  if (fn == 1) { SERIAL_LOCAL.transmit(":F+#"); fn++; } else
                  if (fn == 2 && st4Axis2Fwd.timeDown() > 4000) { SERIAL_LOCAL.transmit(":FF#"); fn++; } else
                  if (fn == 3) { SERIAL_LOCAL.transmit(":F+#"); fn++; }
                }
                if (st4Axis2Fwd.isUp()) { if (fn > 0) { SERIAL_LOCAL.transmit(":FQ#"); fn = 0; } }
              }
            #else
              if (st4Axis1Fwd.wasPressed() && !st4Axis1Rev.wasPressed()) { SERIAL_LOCAL.transmit(":LN#"); mountStatus.sound.click(); }
              if (st4Axis1Rev.wasPressed() && !st4Axis1Fwd.wasPressed()) { SERIAL_LOCAL.transmit(":LB#"); mountStatus.sound.click(); }
              if (st4Axis2Fwd.wasPressed() && !st4Axis2Rev.wasPressed()) { SERIAL_LOCAL.transmit(":LIG#"); mountStatus.sound.click(); }
              if (st4Axis2Rev.wasPressed() && !st4Axis2Fwd.wasPressed()) { mountStatus.sound.click(); mountStatus.sound.enabled = !mountStatus.sound.enabled; mountStatus.sound.click(); }
            #endif
          }
        }
      } else {
        if (altModeA || altModeB) { 
          #if ST4_HAND_CONTROL_FOCUSER == ON
            SERIAL_LOCAL.transmit(":FQ#");
          #endif
          altModeA = false;
          altModeB = false;
          mountStatus.sound.beep();
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
            if (goTo.state == GS_GOTO) goTo.stop(); else
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
            if (goTo.state == GS_GOTO) goTo.stop(); else
          #endif
          guide.startAxis2(st4GuideActionAxis2, pulseGuiding ? guide.settings.pulseRateSelect : guide.settings.axis2RateSelect, GUIDE_TIME_LIMIT*1000);
        } else guide.stopAxis2();
      }

    }
    #if ST4_HAND_CONTROL == ON
    }
    #endif
  }
#endif

St4 st4;

#endif
