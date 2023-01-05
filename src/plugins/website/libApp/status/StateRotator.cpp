// update rotator state
#include "State.h"

#include "Status.h"
#include "../../../../lib/tasks/OnTask.h"
#include "../cmd/Cmd.h"
#include "../../locales/Locale.h"
#include "../../../../lib/convert/Convert.h"

void State::updateRotator(bool now)
{
  if (!now && millis() - lastRotatorPageLoadTime > 2000) return;

  char temp[80], temp1[80];

  // rotator position
  if (status.rotatorFound == SD_TRUE) {
    if (onStep.command(":rG#", temp1))
    {
      temp1[4] = 0;
      strcpy(temp, temp1);
      strcat(temp, "&deg;");
      strcat(temp, &temp1[5]);
      strcat(temp, "&#39;");
    } else strcpy(temp, "?");
    strncpyex(rotatorPositionStr, temp, 20); Y;

    if (onStep.command(":rT#", temp))
    {
      rotatorSlewing = (bool)strchr(temp, 'M');
      rotatorDerotate = (bool)strchr(temp, 'D');
      rotatorDerotateReverse = (bool)strchr(temp, 'R');
    } else {
      rotatorSlewing = false;
      rotatorDerotate = false;
      rotatorDerotateReverse = false;
    }
    Y;
  }
}