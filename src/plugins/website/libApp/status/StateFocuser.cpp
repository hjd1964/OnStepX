// update focuser state
#include "State.h"

#include "Status.h"
#include "../../../../lib/tasks/OnTask.h"
#include "../cmd/Cmd.h"
#include "../misc/Misc.h"
#include "../../locales/Locale.h"
#include "../../../../lib/convert/Convert.h"

void State::updateFocuser(bool now)
{
  if (!now && millis() - lastFocuserPageLoadTime > 2000) return;

  char temp[80];

  // identify active focuser
  if (onStep.command(":FA#", temp)) focuserSelected = atoi(temp); else focuserSelected = 0; Y;

  if (focuserSelected >= 1 && focuserSelected <= 6) {

    // Focuser/telescope temperature
    if (!onStep.command(":Ft#", temp)) strcpy(temp, "?"); else localeTemperature(temp);
    strncpyex(focuserTemperatureStr, temp, 16); Y;

    // Focuser backlash
    if (!onStep.command(":Fb#", temp)) strcpy(temp, "?");
    strncpyex(focuserBacklashStr, temp, 16); Y;

    // Focuser deadband
    if (!onStep.command(":Fd#", temp)) strcpy(temp, "?");
    strncpyex(focuserDeadbandStr, temp, 16); Y;

    // Focuser TCF enable
    focuserTcfEnable = onStep.commandBool(":Fc#"); Y;

    // Focuser TCF
    if (onStep.command(":FC#", temp))
    {
      char *conv_end;
      double tcfCoef = strtod(temp, &conv_end);
      if (&temp[0] != conv_end) {
        dtostrf(tcfCoef, 1, 4, temp);
      } else strcpy(temp, "?");
    } else strcpy(temp, "?");
    strncpyex(focuserTcfCoefStr, temp, 16); Y;

    // Focuser status
    if (onStep.command(":FT#", temp))
    {
      focuserSlewing = (bool)strchr(temp, 'M');
    } else {
      focuserSlewing = false;
    }
    Y;
 }
}