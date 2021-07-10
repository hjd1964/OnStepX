// ---------------------------------------------------------------------------------------------------
// Guide, commands to move the mount in any direction at a series of fixed rates

#include "../../Common.h"

#ifdef MOUNT_PRESENT

#include "../../tasks/OnTask.h"
extern Tasks tasks;

#include "../Telescope.h"
#include "Mount.h"

#if ST4_INTERFACE == ON
  #include "../../lib/pushButton/PushButton.h"

  #if ST4_HAND_CONTROL == ON
    #include "../../lib/serial/Serial_ST4_Master.h"
    #define debounceMs 100L
  #else
    #define debounceMs 5L
  #endif

  #include "../../lib/serial/Serial_Local.h"

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

  void mountSt4MonitorWrapper() {
    telescope.mount.st4Poll();
  }

  // initialize guiding
  void Mount::st4Init() {
    // start st4 monitor task
    #ifdef HAL_SLOW_PROCESSOR
      VF("MSG: Mount, start ST4 monitor task (rate 10ms priority 2)... ");
      if (tasks.add(10, 0, true, 2, mountSt4MonitorWrapper, "st4Mntr")) { VL("success"); } else { VL("FAILED!"); }
    #else
      VF("MSG: Mount, start ST4 monitor task (rate 1.5ms priority 2)... ");
      uint8_t handle = tasks.add(0, 0, true, 2, mountSt4MonitorWrapper, "st4Mntr");
      if (handle) { VL("success"); } else { VL("FAILED!"); }
      tasks.setPeriodMicros(handle, 1500);
    #endif
  }

  // handle the ST4 interface and hand controller features
  void Mount::st4Poll() {
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
            pinMode(ST4_DEC_S_PIN, OUTPUT);    // clock
            pinMode(ST4_DEC_N_PIN, OUTPUT);    // send data
            digitalWrite(ST4_DEC_S_PIN, HIGH); // idle
            shcActive = true;
            serialST4.begin();
            VLF("MSG: SerialST4 mode activated");
          }
          return;
        } else {
          // Smart Hand Controller active
          char c = serialST4.poll();
          // process any single byte guide commands
          if (c == ccMe) guideStartAxis1(GA_REVERSE, guideRateSelect, GUIDE_TIME_LIMIT*1000);
          if (c == ccMw) guideStartAxis1(GA_FORWARD, guideRateSelect, GUIDE_TIME_LIMIT*1000);
          if (c == ccMn) guideStartAxis2(GA_FORWARD, guideRateSelect, GUIDE_TIME_LIMIT*1000);
          if (c == ccMs) guideStartAxis2(GA_REVERSE, guideRateSelect, GUIDE_TIME_LIMIT*1000);
          if (c == ccQe || c == ccQw) guideStopAxis1(GA_BREAK);
          if (c == ccQn || c == ccQs) guideStopAxis2(GA_BREAK);
          return;
        }
      } else {
        if (shcActive) {
          // Smart Hand Controller deactivate
          pinMode(ST4_DEC_S_PIN, ST4_INTERFACE_INIT);
          pinMode(ST4_DEC_N_PIN, ST4_INTERFACE_INIT);
          shcActive = false;
          serialST4.end();
          VLF("MSG: SerialST4 mode deactivated");
          return;
        }
      }

      // standard hand control
      const long Shed_ms = 4000;
      const long AltMode_ms = 2000;

      // stop any guide that might be triggered by combination button presses
      if (st4Axis1Rev.isDown() && st4Axis1Fwd.isDown()) guideStopAxis1(GA_BREAK);
      if (st4Axis2Fwd.isDown() && st4Axis2Rev.isDown()) guideStopAxis2(GA_BREAK);
  
      // see if a combination was down for long enough for an alternate mode
      static bool altModeA = false;
      static bool altModeB = false;

      if (gotoState != GS_NONE && !meridianFlipHome.paused) {
        if (st4Axis1Rev.timeDown() > AltMode_ms && st4Axis1Fwd.timeDown() > AltMode_ms && !altModeB) {
          if (!altModeA) { altModeA = true; sound.beep(); }
        }
        if (st4Axis2Fwd.timeDown() > AltMode_ms && st4Axis2Rev.timeDown() > AltMode_ms && !altModeA) {
          if (!altModeB) { altModeB = true; sound.beep(); }
        }
      }
  
      // if the alternate mode is allowed & selected & hasn't timed out, handle it
      if ( (altModeA || altModeB) && (st4Axis2Fwd.timeUp() < Shed_ms || st4Axis2Rev.timeUp() < Shed_ms || st4Axis1Rev.timeUp() < Shed_ms || st4Axis1Fwd.timeUp() < Shed_ms) ) {

        // make sure no cmdSend() is being processed
        //if (!cmdWaiting())
        {
          if (altModeA) {
            int r = (int)guideRateSelect;
            if (st4Axis1Fwd.wasPressed() && !st4Axis1Rev.wasPressed()) {
              if (gotoState == GS_NONE) SERIAL_LOCAL.transmit(":B+#"); else { if (r >= 7) r=8; else if (r >= 5) r=7; else if (r >= 2) r=5; else if (r < 2) r=2; }
              sound.click();
            }
            if (st4Axis1Rev.wasPressed() && !st4Axis1Fwd.wasPressed()) {
              if (gotoState == GS_NONE) SERIAL_LOCAL.transmit(":B-#"); else { if (r <= 5) r=2; else if (r <= 7) r=5; else if (r <= 8) r=7; else if (r > 8) r=8; }
              sound.click();
            }
            if (st4Axis2Rev.wasPressed() && !st4Axis2Fwd.wasPressed()) {
              if (alignState.lastStar > 0 && alignState.currentStar > alignState.lastStar) SERIAL_LOCAL.transmit(":CS#"); else alignAddStar();
              sound.click();
            }
            if (st4Axis2Fwd.wasPressed() && !st4Axis2Rev.wasPressed()) {
              if (trackingState == TS_SIDEREAL) { setTrackingState(TS_NONE); sound.click(); } else
              if (trackingState == TS_NONE) { setTrackingState(TS_SIDEREAL); sound.click(); }
            }
            guideRateSelect = (GuideRateSelect)r;
            if (SEPARATE_PULSE_GUIDE_RATE == ON && guideRateSelect <= GR_1X) misc.pulseGuideRateSelect = guideRateSelect;
          }
          if (altModeB) {
            #if ST4_HAND_CONTROL_FOCUSER == ON
              static int fs=0;
              static int fn=0;
              if (!fn && !fs) {
                if (st4Axis1Fwd.wasPressed() && !st4Axis1Rev.wasPressed()) { SERIAL_LOCAL.transmit(":F2#"); sound.click(); }
                if (st4Axis1Rev.wasPressed() && !st4Axis1Fwd.wasPressed()) { SERIAL_LOCAL.transmit(":F1#"); sound.click(); }
              }
              if (!fn) {
                if (st4Axis2Rev.isDown() && st4Axis2Fwd.isUp()) { if (fs == 0) { cmdSend(":FS#",true); fs++; } else if (fs == 1) { cmdSend(":F-#",true); fs++; } else if ((st4s.timeDown() > 4000) && (fs == 2)) { fs++; cmdSend(":FF#",true); } else if (fs == 3) { cmdSend(":F-#",true); fs++; } }
                if (st4Axis2Rev.isUp()) { if (fs > 0) { cmdSend(":FQ#",true); fs=0; } }
              }
              if (!fs) {
                if (st4Axis2Fwd.isDown() && st4Axis2Rev.isUp()) { if (fn == 0) { cmdSend(":FS#",true); fn++; } else if (fn == 1) { cmdSend(":F+#",true); fn++; } else if ((st4n.timeDown() > 4000) && (fn == 2)) { fn++; cmdSend(":FF#",true); } else if (fn == 3) { cmdSend(":F+#",true); fn++; }  }
                if (st4Axis2Fwd.isUp()) { if (fn > 0) { cmdSend(":FQ#",true); fn=0; } }
              }
            #else
              if (st4Axis1Fwd.wasPressed() && !st4Axis1Rev.wasPressed()) { SERIAL_LOCAL.transmit(":LN#"); sound.click(); }
              if (st4Axis1Rev.wasPressed() && !st4Axis1Fwd.wasPressed()) { SERIAL_LOCAL.transmit(":LB#"); sound.click(); }
              if (st4Axis2Fwd.wasPressed() && !st4Axis2Rev.wasPressed()) { SERIAL_LOCAL.transmit(":LIG#"); sound.click(); }
              if (st4Axis2Rev.wasPressed() && !st4Axis2Fwd.wasPressed()) { sound.click(); sound.enabled = !sound.enabled; sound.click(); }
            #endif
          }
        }
      } else {
        if (altModeA || altModeB) { 
          #if ST4_HAND_CONTROL_FOCUSER == ON
            cmdSend(":FQ#",true);
          #endif
          altModeA = false;
          altModeB = false;
          sound.beep();
        }
    #endif

    if (axis1.isEnabled()) {
      // guide E/W
      GuideAction st4GuideActionAxis1 = GA_BREAK;
      static GuideAction lastSt4GuideActionAxis1 = GA_BREAK;
      if (st4Axis1Fwd.isDown() && st4Axis1Rev.isUp()) st4GuideActionAxis1 = GA_FORWARD;
      if (st4Axis1Rev.isDown() && st4Axis1Fwd.isUp()) st4GuideActionAxis1 = GA_REVERSE;

      if (st4GuideActionAxis1 != lastSt4GuideActionAxis1) {
        lastSt4GuideActionAxis1 = st4GuideActionAxis1;
        if (st4GuideActionAxis1 != GA_BREAK) {
          #if ST4_HAND_CONTROL == ON
            if (meridianFlipHome.paused) meridianFlipHome.resume = true; else
            if (gotoState == GS_GOTO) gotoStop(); else
          #endif
          {
            GuideRateSelect rateSelect = guideRateSelect;
            if (SEPARATE_PULSE_GUIDE_RATE == ON && ST4_HAND_CONTROL != ON) rateSelect = misc.pulseGuideRateSelect;
            guideStartAxis1(st4GuideActionAxis1, rateSelect, GUIDE_TIME_LIMIT*1000);
          }
        } else guideStopAxis1(GA_BREAK);
      }

      // guide N/S
      GuideAction st4GuideActionAxis2 = GA_BREAK;
      static GuideAction lastSt4GuideActionAxis2 = GA_BREAK;
      if (st4Axis2Fwd.isDown() && st4Axis2Rev.isUp()) st4GuideActionAxis2 = GA_FORWARD;
      if (st4Axis2Rev.isDown() && st4Axis2Fwd.isUp()) st4GuideActionAxis2 = GA_REVERSE;

      if (st4GuideActionAxis2 != lastSt4GuideActionAxis2) {
        lastSt4GuideActionAxis2 = st4GuideActionAxis2;
        if (st4GuideActionAxis2 != GA_BREAK) {
          #if ST4_HAND_CONTROL == ON
            if (meridianFlipHome.paused) meridianFlipHome.resume = true; else
            if (gotoState == GS_GOTO) gotoStop(); else
          #endif
          {
            GuideRateSelect rateSelect = guideRateSelect;
            if (SEPARATE_PULSE_GUIDE_RATE == ON && ST4_HAND_CONTROL != ON) rateSelect = misc.pulseGuideRateSelect;
            guideStartAxis2(st4GuideActionAxis2, rateSelect, GUIDE_TIME_LIMIT*1000);
          }
        } else guideStopAxis2(GA_BREAK);
      }

    }
    #if ST4_HAND_CONTROL == ON
    }
    #endif
  }
#endif

#endif
