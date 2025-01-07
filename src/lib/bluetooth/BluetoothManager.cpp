// bluetooth manager
#include "BluetoothManager.h"

#if SERIAL_BT_MODE == MASTER

#include "../nv/Nv.h"

bool BluetoothManager::init() {
  if (!active) {
    VLF("MSG: Bluetooth, init");

    #ifdef NV_BT_SETTINGS_BASE
      if (BluetoothSettingsSize < sizeof(BluetoothSettings)) { nv.initError = true; DL("ERR: BluetoothManager::init(), BluetoothSettingsSize error"); }

      if (!nv.hasValidKey() || nv.isNull(NV_BT_SETTINGS_BASE, sizeof(BluetoothSettings))) {
        VLF("MSG: Bluetooth, writing defaults to NV");
        nv.writeBytes(NV_BT_SETTINGS_BASE, &settings, sizeof(BluetoothSettings));
      }

      nv.readBytes(NV_BT_SETTINGS_BASE, &settings, sizeof(BluetoothSettings));
    #endif

    setStation(stationNumber);

    VF("MSG: Bluetooth, Master Pwd  = "); VL(settings.masterPassword);
    VF("MSG: Bluetooth, Station#     = "); VL(stationNumber);
    VF("MSG: Bluetooth, Station Name = "); VL(sta->host);
    VF("MSG: Bluetooth, Station MAC  = "); VL(sta->address);
    VF("MSG: Bluetooth, Station PIN  = "); VL(sta->pin);

    active = true;
  }

  return active;
}

void BluetoothManager::setStation(int number) {
  if (number >= 1 && number <= BluetoothStationCount) stationNumber = number;
  sta = &settings.station[stationNumber - 1];
}

void BluetoothManager::disconnect() {
  active = false;
  VLF("MSG: Bluetooth, disconnected");      
}

void BluetoothManager::writeSettings() {
  #ifdef NV_BT_SETTINGS_BASE
    VLF("MSG: BluetoothManager, writing settings to NV");
    nv.writeBytes(NV_BT_SETTINGS_BASE, &settings, sizeof(BluetoothSettings));
  #endif
}

BluetoothManager bluetoothManager;

#endif
