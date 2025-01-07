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

#define BluetoothStationCount 3
#define BluetoothSettingsSize 149
typedef struct BluetoothSettings {
  char masterPassword[32];
  BluetoothStationSettings station[BluetoothStationCount];
} BluetoothSettings;

class BluetoothManager {
  public:
    bool init();
    void disconnect();
    void setStation(int number);
    void writeSettings();

    BluetoothStationSettings *sta;

    BluetoothSettings settings = {
      PASSWORD_DEFAULT,

      {
        {
          ONSTEP1_BT_NAME, ONSTEP1_BT_ADDR, ONSTEP1_BT_PASSKEY
        },

        {
          ONSTEP2_BT_NAME, ONSTEP2_BT_ADDR, ONSTEP2_BT_PASSKEY
        },

        {
          ONSTEP3_BT_NAME, ONSTEP3_BT_ADDR, ONSTEP3_BT_PASSKEY
        }

      }
    };

    bool active = false;
    int stationNumber = 1;

  private:
    
};

extern BluetoothManager bluetoothManager;

#endif
