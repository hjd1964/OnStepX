// bluetooth manager
#pragma once

#include "../../Common.h"

#if SERIAL_BT_MODE == MASTER

#include "Bluetooth.defaults.h"

#ifndef ESP32
  #error "Configuration (Config.h): No Bluetooth support is present for this device"
#endif

typedef struct BluetoothStationSettings {
  char host[16];
  char address[18];
  char passkey[5];
} BluetoothStationSettings;

#ifndef BluetoothStationCount
  // number of bluetooth stations supported, between 1 and 6
  #define BluetoothStationCount 3
#endif
#define BluetoothSettingsSize (32 + BluetoothStationCount*39)
typedef struct BluetoothSettings {
  char masterPassword[32];
  BluetoothStationSettings station[BluetoothStationCount];
} BluetoothSettings;

class BluetoothManager {
  public:
    bool init();
    void disconnect();
    void setStation(int number);
    void readSettings();
    void writeSettings();

    BluetoothStationSettings *sta;

    BluetoothSettings settings = {
      PASSWORD_DEFAULT,

      {
        #if BluetoothStationCount > 0
          {STA1_BT_NAME, STA1_BT_ADDR, STA1_BT_PASSKEY},
        #endif
        #if BluetoothStationCount > 1
          {STA2_BT_NAME, STA2_BT_ADDR, STA2_BT_PASSKEY},
        #endif
        #if BluetoothStationCount > 2
          {STA3_BT_NAME, STA3_BT_ADDR, STA3_BT_PASSKEY},
        #endif
        #if BluetoothStationCount > 3
          {STA4_BT_NAME, STA4_BT_ADDR, STA4_BT_PASSKEY},
        #endif
        #if BluetoothStationCount > 4
          {STA5_BT_NAME, STA5_BT_ADDR, STA5_BT_PASSKEY},
        #endif
        #if BluetoothStationCount > 5
          {STA6_BT_NAME, STA6_BT_ADDR, STA6_BT_PASSKEY},
        #endif
      }
    };

    bool active = false;
    bool settingsReady = false;
    int stationNumber = 1;

  private:
    
};

extern BluetoothManager bluetoothManager;

#endif
