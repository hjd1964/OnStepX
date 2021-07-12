// ---------------------------------------------------------------------------------------------------
// Telescope status LED

#include "../Common.h"

#include "Telescope.h"
#include "../tasks/OnTask.h"
extern Tasks tasks;

void teleStatusFlash() {
  #if LED_STATUS != OFF && LED_STATUS_PIN != OFF
    static uint8_t cycle = 0;
    if (cycle++ > 16) cycle = 0;

    // show only the most severe error (in order)
    uint8_t flashes = 0;
    if (initError.nv)      flashes = 1; else
    if (initError.value)   flashes = 2; else
    if (initError.driver)  flashes = 3; else
    if (initError.tls)     flashes = 4; else
    if (initError.weather) flashes = 5;
  
    // everything is ok, turn on LED and exit
    if (flashes == 0) { digitalWriteEx(LED_STATUS_PIN, LED_STATUS_ON_STATE); return; }

    // flash the LED if there's an error
    if (cycle%2 == 0) { digitalWriteEx(LED_STATUS_PIN, !LED_STATUS_ON_STATE); } else { if (cycle/2 < flashes) digitalWriteEx(LED_STATUS_PIN, LED_STATUS_ON_STATE); }
  #endif
}

void Telescope::statusInit() {
  #if LED_STATUS != OFF && LED_STATUS_PIN != OFF
    pinModeEx(LED_STATUS_PIN, OUTPUT);
    VF("MSG: Telescope, start status LED task (rate 500ms priority 6)... ");
    if (tasks.add(500, 0, true, 6, teleStatusFlash, "staLed")) { VL("success"); } else { VL("FAILED!"); }
  #endif
}
