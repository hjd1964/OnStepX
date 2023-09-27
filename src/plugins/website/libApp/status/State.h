#pragma once

#include "../../../../Common.h"
#include "Status.h"

#define leftTri  "&#x25c4;"
#define rightTri "&#x25ba;"
#define upTri    "&#x25b2;"
#define downTri  "&#x25bc;"

typedef struct DriverOutputStatusEx {
  bool shortToGround;
  bool openLoad;
} DriverOutputStatusEx;

typedef struct DriverStatusEx {
  DriverOutputStatusEx outputA;
  DriverOutputStatusEx outputB;
  bool overTemperaturePreWarning;
  bool overTemperature;
  bool standstill;
  bool communicationFailure;
  bool fault;
  bool valid;
} DriverStatusEx;

// background ajax page update timing (in milliseconds, 50ms minimum)
#ifndef AJAX_PAGE_UPDATE_RATE_MS
#define AJAX_PAGE_UPDATE_RATE_MS      500     // normal ajax page update update rate
#endif

#ifndef STATE_POLLING_RATE_MS
#define STATE_POLLING_RATE_MS         500     // time between updates for most OnStep state information
#endif

class State {
  public:
    void init();
    void poll();
    void pollSlow();
    void pollFast();

    void updateController(bool now = false);
    void updateMount(bool now = false);
    void updateRotator(bool now = false);
    void updateFocuser(bool now = false);

    bool updateAuxiliary(bool all = false, bool now = false);
    inline void selectFeature(int f) { featureSelected = f; }
    inline char* featureName() { return status.feature[featureSelected].name; }
    inline int featurePurpose() { return status.feature[featureSelected].purpose; }
    inline int featureValue1() { return status.feature[featureSelected].value1; }
    inline float featureValue2() { return status.feature[featureSelected].value2; }
    inline float featureValue3() { return status.feature[featureSelected].value3; }
    inline float featureValue4() { return status.feature[featureSelected].value4; }

    void updateEncoders(bool now = false);

    unsigned long lastControllerPageLoadTime = 0;
    unsigned long lastMountPageLoadTime = 0;
    unsigned long lastAuxPageLoadTime = 0;
    unsigned long lastFocuserPageLoadTime = 0;
    unsigned long lastRotatorPageLoadTime = 0;

    char dateStr[10] = "?";
    char timeStr[10] = "?";
    char lastStr[10] = "?";
    double latitude = NAN;
    char latitudeStr[20] = "?";
    char longitudeStr[20] = "?";
    char siteTemperatureStr[16] = "?";
    char sitePressureStr[16] = "?";
    char siteHumidityStr[16] = "?";
    char siteDewPointStr[16] = "?";

    char indexAzmStr[20] = "?";
    char indexAltStr[20] = "?";
    char indexRaStr[20] = "?";
    char indexDecStr[20] = "?";
    char targetRaStr[20] = "?";
    char targetDecStr[20] = "?";

    char angleAxis1Str[20] = "?";
    char angleAxis2Str[20] = "?";
    char encAngleAxis1Str[20] = "?";
    char encAngleAxis2Str[20] = "?";

    char pierSideStr[10] = "?";
    char preferredPierSideChar = '?';
    char meridianFlipStr[10] = "?";

    char alignUdStr[16] = "?";
    char alignLrStr[16] = "?";
    char alignProgress[32] = "?";

    char parkStr[40] = "?";
    char trackStr[40] = "?";
    bool trackingSidereal = false;
    bool trackingLunar = false;
    bool trackingSolar = false;
    bool trackingKing = false;

    float slewSpeedNominal = NAN;
    float slewSpeedCurrent = NAN;
    char slewSpeedStr[16] = "?";

    DriverStatusEx driver[9];
    char driverStatusStr[9][40] = {"?","?","?","?","?","?","?","?","?"};

    char controllerTemperatureStr[16] = "?";
    char lastErrorStr[80] = "?";
    char workLoadStr[20] = "?";
    char signalStrengthStr[20] = "?";

    int featureSelected = 0;

    int focuserActive = 0;
    int focuserSelected = 0;
    char focuserPositionStr[20] = "?";
    bool focuserSlewing = false;
    char focuserTemperatureStr[16] = "?";
    char focuserBacklashStr[16] = "?";
    char focuserDeadbandStr[16] = "?";
    bool focuserTcfEnable = false;
    char focuserTcfCoefStr[16] = "?";
    int  focuserGotoRate = 3;
    char focuserSlewSpeedStr[16] = "?";

    char rotatorPositionStr[20] = "?";
    bool rotatorSlewing = false;
    bool rotatorDerotate = false;
    bool rotatorDerotateReverse = false;
    int rotatorGotoRate = 3;
    char rotateSlewSpeedStr[20] = "?";

   // DynamicJsonDocument *doc;

  private:
    void axisStatusUpdate();

    char vGpioMode[8] = {
      'X', 'X', 'X', 'X',
      'X', 'X', 'X', 'X'
    };

    int vGpioLastState[8] = {
      -1, -1, -1, -1,
      -1, -1, -1, -1
    };

    bool rotatorChecked = false;

    unsigned long lastPoll = 0;
};

void formatDegreesStr(char *s);
void formatHoursStr(char *s);

extern State state;
