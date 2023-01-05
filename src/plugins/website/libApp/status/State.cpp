// scan OnStep state in the background
#include "State.h"

#include "../../../../lib/tasks/OnTask.h"
#include "../cmd/Cmd.h"

void pollState() { state.poll(); }
void pollStateSlow() { state.pollSlow(); }
void pollStateFast() { state.pollFast(); }

void State::init()
{
  VF("MSG: State, start polling task (rate "); V(STATE_POLLING_RATE_MS); VF("ms, priority 6)... ");
  if (tasks.add(STATE_POLLING_RATE_MS, 0, true, 7, pollState, "StaPoll")) { VLF("success"); } else { VLF("FAILED!"); }

  status.update();
}

void State::poll()
{
  status.update(true);
  if (status.mountFound == SD_TRUE) updateMount();
  if (status.focuserFound == SD_TRUE) updateFocuser();
  if (status.auxiliaryFound == SD_TRUE) updateAuxiliary();
  if (status.onStepFound) updateController();

  if (status.focuserFound == SD_TRUE && millis() - lastFocuserPageLoadTime < 2000) {
    char temp[80];
    if (!onStep.command(":FG#", temp)) strcpy(temp, "?"); else strcat(temp, " microns"); Y;
    strncpy(focuserPositionStr, temp, 20); focuserPositionStr[19] = 0; Y;
  }

  if (status.rotatorFound == SD_TRUE) updateRotator();
}

void formatDegreesStr(char *s)
{
  char *tail;

  // degrees part
  tail = strchr(s, '*');
  if (tail)
  {
    char head[80];
    tail[0] = 0;
    strcpy(head, s);
    strcat(head, "&deg;");

    tail++;
    strcat(head, tail);
    strcpy(s, head);
  } else return;

  // minutes part
  tail = strchr(s, ':');
  if (tail) // indicates there is a seconds part
  {
    tail[0] = '\'';
    strcat(s, "\"");
  }
  else // there is no seconds part
  {
    strcat(s, "\'");
  }
}

void formatHoursStr(char *s)
{
  char *tail;

  // hours part
  tail = strchr(s, ':');
  if (tail)
  {
    tail[0] = 'h';
  } else return;

  // minutes part
  tail = strchr(s, ':');
  if (tail) // indicates there is a seconds part
  {
    tail[0] = 'm';
    strcat(s, "s");
  }
  else // there is no seconds part
  {
    strcat(s, "m");
  }
}

State state;
