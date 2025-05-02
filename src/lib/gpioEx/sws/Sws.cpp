// -----------------------------------------------------------------------------------
// Smart Web Server GPIO support (optional when SWS encoders are not used)

#include "Sws.h"

#if defined(GPIO_DEVICE) && GPIO_DEVICE == SWS

#include "../../tasks/OnTask.h"
#include "../../../libApp/commands/ProcessCmds.h"

void swsGpioWrapper() { gpio.poll(); }

// check for SWS device
bool GpioSws::init() {
  if (found) return true;

  VF("MSG: GPIO, start SwsGpio monitor task (rate 1000ms priority 7)... ");
  if (tasks.add(1000, 0, true, 7, swsGpioWrapper, "SwsGpio")) { VLF("success"); } else { VLF("FAILED!"); return false; }

  found = true;
  return true;
}

// no command processing
bool GpioSws::command(char *reply, char *command, char *parameter, bool *supressFrame, bool *numericReply, CommandError *commandError) {
  if (command[0] == 'G') {
    if (command[1] == 'X' && parameter[2] == 0)  {
      // :GXGA#     Get Gpio presence
      //            Returns: 8# on success or 0 (error) if not present
      if (parameter[0] == 'G' && parameter[1] == 'A') {
        strcpy(reply, "8");

        active = true;
        lastActiveTimeMs = millis();
        *numericReply = false;
      } else

      // :GXGO#     Get Gpio Output mode or State
      //            Returns: Value
      if (parameter[0] == 'G' && parameter[1] == 'O') {
        for (int i = 0; i < 8; i++) {
          char newMode = 'X';
          if (mode[i] == INPUT) { if (virtualRead[i] == LOW) newMode = 'i'; else newMode = 'I'; } else
          if (mode[i] == INPUT_PULLUP) { if (virtualRead[i] == LOW) newMode = 'u'; else newMode = 'U'; } else
          if (mode[i] == OUTPUT) newMode = 'O';

          if (virtualMode[i] != newMode) {
            reply[i] = newMode;
            virtualMode[i] = newMode;
          } else {
            if (virtualMode[i] == 'O') {
              if (virtualWrite[i] == 0) reply[i] = '0'; else
              if (virtualWrite[i] == 1) reply[i] = '1'; else
              if (virtualWrite[i] >= 2) reply[i] = (char)((virtualWrite[i] - 2) + 128);
            } else reply[i] = newMode;
          }

          reply[i + 1] = 0;
        }

        active = true;
        lastActiveTimeMs = millis();
        *numericReply = false;
      } else return false;
    } else return false;
  } else

  if (command[0] == 'S') {
    if (command[1] == 'X') {
      if (parameter[2] != ',') { *commandError = CE_PARAM_FORM; return true; }

      // :SXG[n],[v]#   Set Gpio input [n]umber to [v]alue
      //              Return: 0 on failure
      //                      1 on success
      if (parameter[0] == 'G' && parameter[1] >= '0' && parameter[1] <= '7') {
        int i = parameter[1] - '0';
        int v = atol(&parameter[3]);
        if (v == 0 || v == 1) {
          virtualRead[i] = v;
        } else *commandError = CE_PARAM_RANGE;

        active = true;
        lastActiveTimeMs = millis();
      } else return false;
    } else return false;
  } else return false;

  return true;
}

// set GPIO pin (0 to 7) mode for INPUT, INPUT_PULLUP, or OUTPUT
void GpioSws::pinMode(int pin, int mode) {
  if (pin >= 0 && pin <= 7) {
    #ifdef INPUT_PULLDOWN
      if (mode == INPUT_PULLDOWN) mode = INPUT;
    #endif
    this->mode[pin] = mode;
  }
}

// one four channel SWS GPIO is supported, this gets the last set value
int GpioSws::digitalRead(int pin) {
  if (readyStage2 && pin >= 0 && pin <= 7) {
    if (mode[pin] == INPUT || mode[pin] == INPUT_PULLUP) {
      return virtualRead[pin];
    } else return state[pin]; 
  } else return 0;
}

// one four channel SWS GPIO is supported, this sets each output on or off
void GpioSws::digitalWrite(int pin, int value) {
  if (pin >= 0 && pin <= 7) {
    state[pin] = value;
    virtualWrite[pin] = value;
  } else return;
}

// one four channel SWS GPIO is supported
void GpioSws::analogWrite(int pin, int value) {
  value = (value*127)/ANALOG_WRITE_RANGE + 2;
  if (pin >= 0 && pin <= 7 && value >= 2 && value <= 129) {
    state[pin] = value;
    virtualWrite[pin] = value;
  } else return;
}

// monitor SWS presence
void GpioSws::poll() {
  if (lateInitError) return;

  // init wait for indication of SWS presence
  if (active && !readyStage1) { startTimeMs = millis(); readyStage1 = true; VLF("MSG: GpioSws, active"); }

  // init delay for synchronization
  if (readyStage1 && !readyStage2 && (long)(millis() - startTimeMs) > 2000) { readyStage2 = true; VLF("MSG: GpioSws, ready"); }

  // init timeout
  if (!readyStage1 && !readyStage2 && (long)(millis() - startTimeMs) > 60000) { lateInitError = true; DLF("ERR: GpioSws, connection failed"); }

  // running timeout
  if (readyStage2 && (long)(millis() - lastActiveTimeMs) > 5000) { startTimeMs = millis(); lastActiveTimeMs = millis(); active = false; readyStage1 = false; readyStage2 = false; DLF("WRN: GpioSws, connection restart"); }
}

GpioSws gpio;

#endif
