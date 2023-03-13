// update rotator state
#include "State.h"

#include "Status.h"
#include "../cmd/Cmd.h"
#include "../../locales/Locale.h"
#include "../../../../lib/convert/Convert.h"

void State::updateRotator(bool now)
{
  if (!now && millis() - lastRotatorPageLoadTime > 2000) return;

  char temp[80], temp1[80];

  if (status.rotatorFound == SD_TRUE) {

    // rotator position
    if (onStep.command(":rG#", temp1))
    {
      temp1[4] = 0;
      strcpy(temp, temp1);
      strcat(temp, "&deg;");
      strcat(temp, &temp1[5]);
      strcat(temp, "&#39;");
    } else strcpy(temp, "?");
    strncpyex(rotatorPositionStr, temp, 20); delay(0);

    // rotator working slew rate
    if (status.getVersionMajor() >= 10)
    {
      if (onStep.command(":rW#", temp))
      {
        double s = atof(temp);
        if (s != 0.0) {
          sprintF(rotateSlewSpeedStr, "%0.1f&deg;/s", s);
        } else strcpy(rotateSlewSpeedStr, "?");
      } else strcpy(rotateSlewSpeedStr, "?");
    } else strcpy(rotateSlewSpeedStr, "?");

    // rotator status
    if (onStep.command(":rT#", temp))
    {
      rotatorSlewing = (bool)strchr(temp, 'M');
      rotatorDerotate = (bool)strchr(temp, 'D');
      rotatorDerotateReverse = (bool)strchr(temp, 'R');
      switch (temp[strlen(temp) - 1] - '0') {
        case 1: rotatorGotoRate = 1; break;
        case 2: rotatorGotoRate = 2; break;
        case 3: rotatorGotoRate = 3; break;
        case 4: rotatorGotoRate = 4; break;
        case 5: rotatorGotoRate = 5; break;
        default: rotatorGotoRate = 0; break;
      }
    } else {
      rotatorSlewing = false;
      rotatorDerotate = false;
      rotatorDerotateReverse = false;
      rotatorGotoRate = 0;
    }
    delay(0);
  }
}