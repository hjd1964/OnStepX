// -----------------------------------------------------------------------------------
// Status from OnStep
#pragma once

enum SubsystemDetect {SD_UNKNOWN, SD_FALSE, SD_TRUE};

enum RateCompensation {RC_NONE, RC_REFR_RA, RC_REFR_BOTH, RC_FULL_RA, RC_FULL_BOTH, RC_UNKNOWN};

enum MountTypes {MT_UNKNOWN, MT_GEM, MT_FORK, MT_FORKALT, MT_ALTAZM};

enum Errors {
  ERR_NONE, ERR_MOTOR_FAULT, ERR_ALT_MIN, ERR_LIMIT_SENSE, ERR_DEC, ERR_AZM, 
  ERR_UNDER_POLE, ERR_MERIDIAN, ERR_SYNC, ERR_PARK, ERR_GOTO_SYNC, ERR_UNSPECIFIED,
  ERR_ALT_MAX, ERR_WEATHER_INIT, ERR_SITE_INIT, ERR_NV_INIT};

const char GuideRatesStr[][8] = {"1/4X", "1/2X", "1X", "2X", "4X", "8X", "20X", "48X", "1/2 Max", "Max"};

typedef struct Features {
  char name[11];
  int purpose;
  int value1;
  float value2;
  float value3;
  float value4;
} features;

#define PierSideNone     0
#define PierSideEast     1
#define PierSideWest     2
#define PierSideBest     3
#define PierSideFlipWE1  10
#define PierSideFlipWE2  11
#define PierSideFlipWE3  12
#define PierSideFlipEW1  20
#define PierSideFlipEW2  21
#define PierSideFlipEW3  22

class Status {
  public:
    bool update();

    inline bool getProduct(char product[]) { if (!mountFound) return false; else { strcpy(product, id); return true; } }
    inline bool getVersionStr(char version[]) { if (!mountFound) return false; else { strcpy(version, ver); return true; } }
    inline int  getVersionMajor() { return ver_maj; }
    inline int  getVersionMinor() { return ver_min; }
    inline char getVersionPatch() { return ver_patch; }

    bool getLastErrorMessage(char message[]);

    char configName[40] = "";

    bool atHome = false;
    bool homing = false;
    bool hasHomeSense = false;
    bool autoHome = false;
    bool waitingHome = false;
    bool pauseAtHome = false;

    bool tracking = false;
    bool inGoto = false;

    bool parked = false;
    bool parking = false;
    bool parkFail = false;

    bool pecEnabled = false;
    bool pecIgnore = false;
    bool pecReadyPlay = false;
    bool pecPlaying = false;
    bool pecReadyRec = false;
    bool pecRecorded = false;
    bool pecRecording = false;

    bool syncToEncodersOnly = false;

    bool ppsSync = false;

    int guideRate = 2;
    int guideRatePulse = 2;
    bool pulseGuiding = false;
    bool guiding = false;

    bool axisFault = false;

    bool buzzerEnabled = false;

    MountTypes mountType = MT_UNKNOWN;

    RateCompensation rateCompensation = RC_NONE;

    bool meridianFlips = true;
    bool autoMeridianFlips = false;

    byte pierSide = PierSideNone;

    bool aligning = false;
    int alignMaxStars = -1;
    int alignThisStar = -1;
    int alignLastStar = -1;

    Errors lastError = ERR_NONE;

    bool onStepFound = false;

    SubsystemDetect mountFound = SD_UNKNOWN;

    SubsystemDetect focuserFound = SD_UNKNOWN;
    int focuserCount = 0;
    bool focuserPresent[6] = {false,false,false,false,false,false};

    SubsystemDetect rotatorFound = SD_UNKNOWN;
    bool derotatorFound = false;

    SubsystemDetect auxiliaryFound = SD_UNKNOWN;

    features feature[8];

  private:
    void mountScan();
    void focuserScan();
    void rotatorScan();
    bool auxiliaryScan();

    char id[10] = "";
    char ver[10] = "";
    int  ver_maj = -1;
    int  ver_min = -1;
    char ver_patch = 0;
};

extern Status status;
