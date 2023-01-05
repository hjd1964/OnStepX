// -----------------------------------------------------------------------------------
// Constants
#pragma once

#define METRIC                       ON
#define IMPERIAL                     -200
#define LOCALE_DEFAULT               -201
#define AUTO_ON                      -202
#define AUTO_OFF                     -203
#define FWU                          -204

// unused ajax page update feature
#define AJAX_PAGE_UPDATE_RATE_FAST_MS 200     // fast update rate
#define AJAX_PAGE_UPDATE_FAST_SHED_MS 5000    // time before return to normal update rate

// The settings below are for initialization only, afterward they are stored and recalled from EEPROM and must
// be changed in the web interface OR with a reset (for initialization again) as described in the Config.h comments
#define TIMEOUT_WEB                  200
#define TIMEOUT_CMD                  200
